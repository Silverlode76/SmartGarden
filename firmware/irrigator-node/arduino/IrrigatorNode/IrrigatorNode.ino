// ============================================================
// SmartGarden Irrigator — PoC v0.2 mit LoRaWAN (Arduino IDE)
// Hardware : LILYGO T3_V1.6.1 (ESP32 + SX1276)
//            IRLZ44N Low-Side-Switch an GPIO12 (500Ω Gate-R, 10kΩ Pulldown)
//            YWBL-WH Mini-Membranpumpe (3-3,7V, direkt am Akku, kein Boost)
//            Kapazitiver Bodenfeuchtesensor: VCC → GPIO4, AOUT → GPIO34
//            (GPIO33 ist auf diesem Board intern für LoRa DIO1 belegt,
//             daher nicht als freier Header-Pin für den Sensor nutzbar)
//
// Funktion : Bodenfeuchte messen → Pumpe feuchtegesteuert schalten
//            (Hysterese + Notbremse) → Status per LoRaWAN an TTN senden
//
// Kalibrierung (Juni 2026, eigener Sensor/Topf):
//   trocken (Luft) ≈ 1671   |   nass (Wasser) ≈ 667
//
// WICHTIG vor dem Compilieren:
//   1. "MCCI LoRaWAN LMIC library" über Werkzeuge → Bibliotheken verwalten installieren
//   2. lmic_project_config.h (liegt eine Ebene höher in firmware/irrigator-node/)
//      kopieren nach:
//      ~/Documents/Arduino/libraries/MCCI_LoRaWAN_LMIC_library/project_config/lmic_project_config.h
//      (ohne diese Datei: Compile-Fehler wegen fehlender Symbole wie DR_SF12)
//   3. secrets.example.h (im gleichen Ordner wie dieses Sketch) kopieren als
//      "secrets.h" und mit den eigenen TTN-Keys befüllen
// ============================================================

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "secrets.h"   // DEVEUI, APPEUI, APPKEY — nicht ins Git!

// ── Hardware ──────────────────────────────────────────────────
const lmic_pinmap lmic_pins = {
    .nss  = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst  = 23,
    .dio  = {26, 33, 32},
};

#define PUMP_GATE_PIN      12
#define SOIL_PWR_PIN       4     // Sensor-VCC, schaltbar
#define SOIL_ADC_PIN       34    // Sensor-AOUT

// ── Bewässerungslogik (Hysterese zwischen kalibrierten Werten) ─
#define SOIL_THRESHOLD_DRY    1400   // ab hier: Pumpe AN
#define SOIL_THRESHOLD_WET    950    // ab hier: Pumpe AUS
#define MAX_PUMP_RUNTIME_SEC  120    // Notbremse

// ── LoRaWAN ───────────────────────────────────────────────────
#define LORA_PORT          1
#define TX_INTERVAL_SEC    60        // PoC: 60s, Produktion: 900s (15min)

static bool          joined      = false;
static bool          pumpRunning = false;
static unsigned long pumpStartMs = 0;
static osjob_t        sendJob;

// ── LMIC OTAA Callbacks ───────────────────────────────────────
void os_getArtEui(u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

// ── Bodenfeuchte messen ───────────────────────────────────────
int readSoilMoisture() {
    digitalWrite(SOIL_PWR_PIN, HIGH);
    delay(50);                       // Einschwingzeit
    int raw = analogRead(SOIL_ADC_PIN);
    digitalWrite(SOIL_PWR_PIN, LOW);
    return raw;
}

void setPump(bool on) {
    if (on == pumpRunning) return;
    digitalWrite(PUMP_GATE_PIN, on ? HIGH : LOW);
    pumpRunning = on;
    pumpStartMs = millis();
    Serial.printf("[PUMP] %s\n", on ? "AN" : "AUS");
}

// ── Bewässerungsentscheidung (wird vor jedem Uplink getroffen) ─
int updateIrrigation() {
    int soil = readSoilMoisture();
    Serial.printf("[SOIL] ADC-Rohwert: %d\n", soil);

    if (pumpRunning && (millis() - pumpStartMs > MAX_PUMP_RUNTIME_SEC * 1000UL)) {
        Serial.println("[PUMP] Max. Laufzeit erreicht — Sicherheitsabschaltung");
        setPump(false);
    } else if (!pumpRunning && soil >= SOIL_THRESHOLD_DRY) {
        Serial.println("[SOIL] Zu trocken — starte Pumpe");
        setPump(true);
    } else if (pumpRunning && soil <= SOIL_THRESHOLD_WET) {
        Serial.println("[SOIL] Feucht genug — stoppe Pumpe");
        setPump(false);
    }
    return soil;
}

// ── Uplink senden ──────────────────────────────────────────────
// Payload: 3 Bytes
//   Byte 0:   Pumpenstatus (0x00 = aus, 0x01 = an)
//   Byte 1-2: Bodenfeuchte-ADC-Rohwert (16-bit, big-endian)
void sendUplink(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("[TX] Noch beschäftigt, warte...");
        return;
    }

    int soil = updateIrrigation();

    uint8_t payload[3];
    payload[0] = pumpRunning ? 0x01 : 0x00;
    payload[1] = (soil >> 8) & 0xFF;
    payload[2] = soil & 0xFF;

    LMIC_setTxData2(LORA_PORT, payload, sizeof(payload), 0 /* unconfirmed */);
    Serial.printf("[TX] Uplink — Pumpe=%s Boden=%d\n", pumpRunning ? "AN" : "AUS", soil);
}

// ── LMIC Event-Handler ─────────────────────────────────────────
void onEvent(ev_t ev) {
    switch (ev) {
        case EV_JOINING:
            Serial.println("[TTN] Verbinde mit TTN (OTAA)...");
            break;

        case EV_JOINED:
            Serial.println("[TTN] *** OTAA Join erfolgreich! ***");
            joined = true;
            LMIC_setLinkCheckMode(0);
            os_setCallback(&sendJob, sendUplink);
            break;

        case EV_JOIN_FAILED:
            Serial.println("[TTN] OTAA Join FEHLGESCHLAGEN — prüfe Keys!");
            break;

        case EV_TXCOMPLETE:
            Serial.printf("[TTN] TX abgeschlossen (RSSI=%d)\n", LMIC.rssi);
            if (LMIC.dataLen > 0) {
                Serial.printf("[RX] Downlink empfangen: %d Byte(s)\n", LMIC.dataLen);
            }
            // Nächsten Uplink einplanen
            os_setTimedCallback(&sendJob,
                                os_getTime() + sec2osticks(TX_INTERVAL_SEC),
                                sendUplink);
            break;

        case EV_LINK_DEAD:
            Serial.println("[TTN] Link tot — kein Gateway erreichbar");
            break;

        default:
            Serial.printf("[TTN] Event: %d\n", (unsigned)ev);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== SmartGarden Irrigator PoC v0.2 (LoRaWAN) ===");

    // GPIO12 ist ESP32-Strapping-Pin (MTDI) — explizit LOW setzen,
    // bevor irgendetwas anderes passiert (Pumpe darf beim Boot nicht laufen)
    pinMode(PUMP_GATE_PIN, OUTPUT);
    digitalWrite(PUMP_GATE_PIN, LOW);

    pinMode(SOIL_PWR_PIN, OUTPUT);
    digitalWrite(SOIL_PWR_PIN, LOW);

    os_init();
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 10 / 100);  // RX-Fenster breiter für ESP32-Timing

    Serial.println("[LMIC] Starte OTAA Join...");
    LMIC_startJoining();
}

void loop() {
    os_runloop_once();
}
