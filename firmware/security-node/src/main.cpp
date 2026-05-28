/* ============================================================
 * SmartGarden Guard — PoC v0.2
 * Hardware : TTGO LoRa32 V2.1 (ESP32 + SX1276)
 *            PIR-Sensor (HC-SR501 oder AM312)
 *            2× 18650 Li-Ion parallel (3,7V) am BAT-Pin
 *
 * Funktion:
 *   PIR erkennt Bewegung → LoRaWAN OTAA → TTN → ntfy.sh Push
 *
 * Pin-Belegung:
 *   GPIO13  ← PIR Signalausgang (OUT)
 *   GPIO25  → Onboard LED (Statusanzeige)
 *   GPIO18  → LoRa NSS  (intern, TTGO)
 *   GPIO23  → LoRa RST  (intern, TTGO)
 *   GPIO26  ← LoRa DIO0 (intern, TTGO)
 *   GPIO33  ← LoRa DIO1 (intern, TTGO)
 *   GPIO32  ← LoRa DIO2 (intern, TTGO)
 *
 * PoC-Einschränkungen (absichtlich vereinfacht):
 *   - Kein Deep Sleep → kontinuierlicher Betrieb, ~2-3 Tage auf 2×18650
 *   - Kein Watchdog → Neustart bei LMIC-Hänger nötig
 *   - Kein Downlink verarbeitet
 *   Deep Sleep + Watchdog kommen in v0.3
 * ============================================================ */

#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "secrets.h"

// ── Hardware ──────────────────────────────────────────────────
const lmic_pinmap lmic_pins = {
    .nss  = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst  = 23,
    .dio  = {26, 33, 32},
};

#define PIR_PIN   13    // HC-SR501 / AM312 Signalausgang
#define LED_PIN   25    // TTGO Onboard LED

// ── Konfiguration ─────────────────────────────────────────────
#define COOLDOWN_SEC   60     // Mindestabstand zw. zwei Alerts (Sekunden)
#define LORA_PORT       1     // LoRaWAN App Port
#define LORA_SF        DR_SF9 // Spreading Factor (SF9 = guter Kompromiss Reichweite/Airtime)

// ── State ─────────────────────────────────────────────────────
static volatile bool motionFlag  = false;  // Gesetzt vom PIR-Interrupt (ISR)
static bool          joined      = false;  // TTN OTAA Join abgeschlossen
static bool          txBusy      = false;  // LoRaWAN TX läuft gerade
static unsigned long lastSentMs  = 0;      // Zeitpunkt letzter Alert

// ── LoRaWAN Payload ───────────────────────────────────────────
// 2 Bytes:
//   Byte 0: Alert-Typ  — 0x01 = Bewegung (PIR)
//   Byte 1: Counter    — zählt hoch, erkennt verlorene Pakete in TTN
static uint8_t txPayload[2] = {0x01, 0x00};
static uint8_t alertCounter = 0;

static osjob_t sendJob;

// ── LMIC Callbacks ────────────────────────────────────────────
void os_getArtEui(u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

static void doSend(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("[LMIC] TX noch ausstehend — überspringe"));
        txBusy = false;
        return;
    }
    txPayload[1] = ++alertCounter;
    LMIC_setTxData2(LORA_PORT, txPayload, sizeof(txPayload), 0 /* unconfirmed */);
    Serial.printf("[LMIC] Paket #%d gesendet (Port %d)\n", alertCounter, LORA_PORT);
    digitalWrite(LED_PIN, HIGH);
}

void onEvent(ev_t ev) {
    switch (ev) {

        case EV_JOINING:
            Serial.println(F("[TTN] OTAA Join wird versucht..."));
            break;

        case EV_JOINED:
            Serial.println(F("[TTN] ✓ Joined! Netzwerk-Session aktiv."));
            LMIC_setLinkCheckMode(0);          // Airtime sparen
            LMIC_setDrTxpow(LORA_SF, 14);     // SF9 @ 14 dBm
            joined = true;
            // Falls Bewegung bereits vor dem Join erkannt wurde:
            if (motionFlag) {
                motionFlag = false;
                txBusy     = true;
                lastSentMs = millis();
                os_setCallback(&sendJob, doSend);
            }
            break;

        case EV_JOIN_FAILED:
            Serial.println(F("[TTN] ✗ Join FAILED — Neuversuch in 60s..."));
            // LMIC versucht automatisch erneut (exponential backoff)
            break;

        case EV_REJOIN_FAILED:
            Serial.println(F("[TTN] ✗ ReJoin FAILED"));
            break;

        case EV_TXSTART:
            Serial.println(F("[TTN] TX beginnt..."));
            break;

        case EV_TXCOMPLETE:
            txBusy = false;
            digitalWrite(LED_PIN, LOW);
            Serial.print(F("[TTN] ✓ TX abgeschlossen"));
            if (LMIC.txrxFlags & TXRX_ACK)
                Serial.print(F(" (ACK)"));
            if (LMIC.dataLen)
                Serial.printf(" — Downlink: %d Bytes", LMIC.dataLen);
            Serial.println();
            break;

        case EV_RXCOMPLETE:
            Serial.println(F("[TTN] RX complete"));
            break;

        case EV_LINK_DEAD:
            Serial.println(F("[TTN] Link dead — kein Gateway erreichbar"));
            break;

        default:
            Serial.printf("[LMIC] Event: %u\n", (unsigned)ev);
            break;
    }
}

// ── PIR Interrupt Service Routine ─────────────────────────────
void IRAM_ATTR onPIR() {
    motionFlag = true;   // Nur Flag setzen — keine Serial.print() im ISR!
}

// ── Setup ─────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println(F("╔══════════════════════════════════════╗"));
    Serial.println(F("║   SmartGarden Guard PoC — v0.2       ║"));
    Serial.println(F("║   TTGO LoRa32 + PIR → TTN → Push     ║"));
    Serial.println(F("╚══════════════════════════════════════╝"));
    Serial.printf( "  PIR-Pin : GPIO%d\n", PIR_PIN);
    Serial.printf( "  LED-Pin : GPIO%d\n", LED_PIN);
    Serial.printf( "  Cooldown: %d Sekunden\n", COOLDOWN_SEC);
    Serial.println();

    // LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // PIR — Interrupt auf RISING (LOW→HIGH = Bewegung erkannt)
    pinMode(PIR_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIR_PIN), onPIR, RISING);
    Serial.println(F("[PIR] Interrupt aktiviert (RISING)"));

    // LMIC initialisieren
    os_init();
    LMIC_reset();

    // 5% Clock-Toleranz für den Quarz des TTGO (typisch bei günstigen Boards)
    LMIC_setClockError(MAX_CLOCK_ERROR * 5 / 100);

    // OTAA Join starten
    LMIC_startJoining();
    Serial.println(F("[TTN] OTAA Join gestartet — warte auf Gateway..."));
    Serial.println(F("[PIR] Warte auf Bewegung..."));
    Serial.println();
}

// ── Loop ──────────────────────────────────────────────────────
void loop() {
    os_runloop_once();  // LMIC State Machine

    // PIR-Flag prüfen (gesetzt vom ISR)
    if (motionFlag) {
        unsigned long now = millis();
        unsigned long elapsed = now - lastSentMs;

        if (!joined) {
            // Noch nicht eingeloggt — Flag bleibt, wird nach Join gesendet
            Serial.println(F("[PIR] Bewegung erkannt — warte auf TTN Join..."));

        } else if (txBusy) {
            // TX läuft noch — verwerfen (nächste Bewegung triggert neu)
            Serial.println(F("[PIR] Bewegung erkannt — TX läuft, überspringe"));
            motionFlag = false;

        } else if (elapsed < (COOLDOWN_SEC * 1000UL)) {
            // Cooldown aktiv
            unsigned long remaining = (COOLDOWN_SEC * 1000UL) - elapsed;
            Serial.printf("[PIR] Bewegung erkannt — Cooldown noch %lus\n",
                          remaining / 1000);
            motionFlag = false;

        } else {
            // Alles OK → Alert senden
            Serial.println();
            Serial.println(F("🚨 ═══════════════════════════════════ 🚨"));
            Serial.println(F("   BEWEGUNG ERKANNT — sende LoRaWAN Alert"));
            Serial.println(F("🚨 ═══════════════════════════════════ 🚨"));
            Serial.println();

            motionFlag = false;
            txBusy     = true;
            lastSentMs = now;
            os_setCallback(&sendJob, doSend);
        }
    }
}
