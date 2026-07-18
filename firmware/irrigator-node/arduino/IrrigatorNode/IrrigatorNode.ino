// ============================================================
// SmartGarden Irrigator — PoC v0.4 mit LoRaWAN + Deep Sleep
// Hardware : LILYGO T3_V1.6.1 (ESP32 + SX1276)
//            IRLZ44N Low-Side-Switch an GPIO12 (500Ω Gate-R, 10kΩ Pulldown)
//            YWBL-WH Mini-Membranpumpe (3-3,7V, direkt am Akku, kein Boost)
//            Kapazitiver Bodenfeuchtesensor: VCC → GPIO4, AOUT → GPIO34
//            Batteriespannung: GPIO35 (Spannungsteiler 5,5kΩ/5,5kΩ → Vbat/2)
//            (GPIO33 ist auf diesem Board intern für LoRa DIO1 belegt)
//
// Funktion : Boot → OTAA-Join → messen → senden → Deep Sleep → repeat
//            Pumpe läuft → kein Sleep, alle 15s prüfen (Notbremse!)
//
// Kalibrierung (Juni 2026):
//   trocken (Luft) ≈ 1671   |   nass (Wasser) ≈ 667
//
// Payload: 5 Bytes
//   Byte 0:   Pumpenstatus (0x00 = aus, 0x01 = an)
//   Byte 1-2: Bodenfeuchte-ADC-Rohwert (16-bit, big-endian)
//   Byte 3-4: Batteriespannung in mV (16-bit, big-endian)
// ============================================================

#include <Arduino.h>
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
#define SOIL_PWR_PIN       4
#define SOIL_ADC_PIN       34
#define BAT_ADC_PIN        35
#define BAT_DIVIDER_RATIO  2

// ── Bewässerungslogik ──────────────────────────────────────────
#define SOIL_THRESHOLD_DRY    1400
#define SOIL_THRESHOLD_WET    950
#define MAX_PUMP_RUNTIME_SEC  120

// ── LoRaWAN ───────────────────────────────────────────────────
#define LORA_PORT          1
#define TX_INTERVAL_SEC    55    // Debugging — für Produktion: 1800 (30 min)

// ── RTC-RAM: Pump-Zustand überlebt Deep Sleep ─────────────────
RTC_DATA_ATTR static bool rtcPumpOn = false;

static bool          joined      = false;
static bool          pumpRunning = false;
static unsigned long pumpStartMs = 0;
static osjob_t       sendJob;

// ── LMIC OTAA Callbacks ───────────────────────────────────────
void os_getArtEui(u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

// ── Sensoren ──────────────────────────────────────────────────
int readBatteryMv() {
    const int samples = 8;
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogReadMilliVolts(BAT_ADC_PIN);
        delay(5);
    }
    int vbat_mv = (sum / samples) * BAT_DIVIDER_RATIO;
    Serial.printf("[BAT] Vbat: %d mV\n", vbat_mv);
    return vbat_mv;
}

int readSoilMoisture() {
    digitalWrite(SOIL_PWR_PIN, HIGH);
    delay(50);
    int raw = analogRead(SOIL_ADC_PIN);
    digitalWrite(SOIL_PWR_PIN, LOW);
    return raw;
}

// ── Pumpensteuerung ───────────────────────────────────────────
void setPump(bool on) {
    if (on == pumpRunning) return;
    digitalWrite(PUMP_GATE_PIN, on ? HIGH : LOW);
    pumpRunning = on;
    rtcPumpOn   = on;
    pumpStartMs = millis();
    Serial.printf("[PUMP] %s\n", on ? "AN" : "AUS");
}

int updateIrrigation() {
    int soil = readSoilMoisture();
    Serial.printf("[SOIL] ADC-Rohwert: %d\n", soil);

    if (pumpRunning && (millis() - pumpStartMs > MAX_PUMP_RUNTIME_SEC * 1000UL)) {
        Serial.println("[PUMP] Max. Laufzeit — Sicherheitsabschaltung");
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
void sendUplink(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("[TX] Noch beschäftigt, warte...");
        os_setCallback(&sendJob, sendUplink);
        return;
    }

    int soil  = updateIrrigation();
    int batMv = readBatteryMv();

    uint8_t payload[5];
    payload[0] = pumpRunning ? 0x01 : 0x00;
    payload[1] = (soil  >> 8) & 0xFF;
    payload[2] = soil         & 0xFF;
    payload[3] = (batMv >> 8) & 0xFF;
    payload[4] = batMv        & 0xFF;

    LMIC_setTxData2(LORA_PORT, payload, sizeof(payload), 0);
    Serial.printf("[TX] Uplink — Pumpe=%s Boden=%d Akku=%dmV\n",
                  pumpRunning ? "AN" : "AUS", soil, batMv);
}

// ── LMIC Event-Handler ─────────────────────────────────────────
void onEvent(ev_t ev) {
    switch (ev) {
        case EV_JOINING:
            Serial.println("[TTN] OTAA Join...");
            break;

        case EV_JOINED:
            Serial.println("[TTN] Join erfolgreich");
            joined = true;
            LMIC_setLinkCheckMode(0);
            os_setCallback(&sendJob, sendUplink);
            break;

        case EV_JOIN_FAILED:
            Serial.println("[TTN] Join FEHLGESCHLAGEN — schlafe und versuche erneut");
            Serial.flush();
            esp_sleep_enable_timer_wakeup((uint64_t)TX_INTERVAL_SEC * 1000000ULL);
            esp_deep_sleep_start();
            break;

        case EV_TXCOMPLETE:
            Serial.printf("[TTN] TX abgeschlossen (RSSI=%d)\n", LMIC.rssi);
            if (LMIC.dataLen > 0)
                Serial.printf("[RX] Downlink: %d Byte(s)\n", LMIC.dataLen);

            if (pumpRunning) {
                Serial.println("[PUMP] Läuft — kein Sleep, nächste Prüfung in 15s");
                os_setTimedCallback(&sendJob,
                                    os_getTime() + sec2osticks(15),
                                    sendUplink);
            } else {
                Serial.printf("[SLEEP] %ds Deep Sleep\n", TX_INTERVAL_SEC);
                Serial.flush();
                esp_sleep_enable_timer_wakeup((uint64_t)TX_INTERVAL_SEC * 1000000ULL);
                esp_deep_sleep_start();
            }
            break;

        case EV_LINK_DEAD:
            Serial.println("[TTN] Link tot — schlafe");
            Serial.flush();
            esp_sleep_enable_timer_wakeup((uint64_t)TX_INTERVAL_SEC * 1000000ULL);
            esp_deep_sleep_start();
            break;

        default:
            Serial.printf("[TTN] Event: %d\n", (unsigned)ev);
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== SmartGarden Irrigator PoC v0.4 ===");

    // GPIO12 sofort LOW — Pumpe darf beim Boot nicht laufen (Strapping-Pin)
    pinMode(PUMP_GATE_PIN, OUTPUT);
    digitalWrite(PUMP_GATE_PIN, LOW);
    pinMode(SOIL_PWR_PIN, OUTPUT);
    digitalWrite(SOIL_PWR_PIN, LOW);

    // Pump-Zustand aus RTC-RAM wiederherstellen
    pumpRunning = rtcPumpOn;
    if (pumpRunning) {
        digitalWrite(PUMP_GATE_PIN, HIGH);
        pumpStartMs = millis();
        Serial.println("[PUMP] Zustand wiederhergestellt: AN");
    }

    os_init();
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 10 / 100);

    Serial.println("[TTN] Starte OTAA Join...");
    LMIC_startJoining();
}

void loop() {
    os_runloop_once();
}
