// ============================================================
// SmartGarden — Garden Home PoC v0.1
// Hardware : TTGO LoRa32 V2.1 (ESP32 + SX1276)
// Funktion : TTN OTAA-Join + LED per Downlink steuern
//            + LED-Status per Uplink zurückmelden
// Author   : Oliver Schmoll, 2026
// ============================================================

#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "secrets.h"   // DEVEUI, APPEUI, APPKEY — nicht ins Git!

// ── Hardware-Pins TTGO LoRa32 V2.1 ────────────────────────
#define LED_PIN        25       // Onboard LED
#define OLED_SDA       21
#define OLED_SCL       22
#define OLED_RST       -1   // kein Reset-Pin am TTGO LoRa32 V2.1
#define OLED_WIDTH     128
#define OLED_HEIGHT    64
#define OLED_ADDR      0x3C

// SX1276 SPI-Pinbelegung
const lmic_pinmap lmic_pins = {
    .nss  = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst  = 23,
    .dio  = { 26, 33, 32 },
};

// ── Uplink-Intervall (PoC: 60s, Produktion: 900s = 15min) ──
#define TX_INTERVAL_SEC  60

// ── PIR Bewegungssensor ────────────────────────────────────
#define PIR_PIN  13

// ── Zustand ────────────────────────────────────────────────
static bool     ledState        = false;
static bool     motionAlert     = false;  // aktueller PIR-Zustand
static bool     motionLastState = false;  // vorheriger PIR-Zustand
static uint32_t txCount         = 0;
static bool     joined          = false;
static osjob_t  sendJob;

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RST);

// ── OLED-Hilfsfunktion ─────────────────────────────────────
void updateOLED(const char* line1, const char* line2 = "", const char* line3 = "") {
    display.clearDisplay();
    display.setTextColor(WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("SmartGarden PoC v0.1");
    display.drawLine(0, 10, 127, 10, WHITE);

    display.setTextSize(1);
    display.setCursor(0, 14);
    display.println(line1);
    display.setCursor(0, 26);
    display.println(line2);
    display.setCursor(0, 38);
    display.println(line3);

    // Status-Zeile unten
    display.setCursor(0, 54);
    display.printf("LED:%s  TX#%lu", ledState ? "ON " : "OFF", txCount);

    display.display();
}

// ── TTN OTAA Callbacks ─────────────────────────────────────
// LMIC erwartet diese drei Funktionen — Werte kommen aus secrets.h

void os_getArtEui(u1_t* buf) {
    memcpy_P(buf, APPEUI, 8);
}

void os_getDevEui(u1_t* buf) {
    memcpy_P(buf, DEVEUI, 8);
}

void os_getDevKey(u1_t* buf) {
    memcpy_P(buf, APPKEY, 16);
}

// ── Uplink senden ──────────────────────────────────────────
// Payload: 1 Byte LED-Status (0x00 = aus, 0x01 = an)
void sendUplink(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("[TX] Noch beschäftigt, warte...");
        return;
    }

    uint8_t payload[2];
    payload[0] = ledState    ? 0x01 : 0x00;
    payload[1] = motionAlert ? 0x01 : 0x00;

    LMIC_setTxData2(1, payload, sizeof(payload), 0);

    txCount++;
    Serial.printf("[TX] Uplink #%lu — LED=%s Motion=%s\n",
                  txCount, ledState ? "ON" : "OFF", motionAlert ? "ALARM" : "OK");
    updateOLED("Sende Uplink...", ledState ? "LED: AN" : "LED: AUS",
               motionAlert ? "BEWEGUNG!" : String("TX #" + String(txCount)).c_str());
}

// ── LMIC Event-Handler ─────────────────────────────────────
void onEvent(ev_t ev) {
    switch (ev) {

    case EV_JOINING:
        Serial.println("[TTN] Verbinde mit TTN (OTAA)...");
        updateOLED("Verbinde TTN...", "OTAA Join...", "Bitte warten");
        break;

    case EV_JOINED:
        Serial.println("[TTN] *** OTAA Join erfolgreich! ***");
        joined = true;
        // ADR aktivieren, Link-Check deaktivieren (spart Duty Cycle)
        LMIC_setLinkCheckMode(0);
        updateOLED("TTN verbunden!", "OTAA OK", "Sende ersten TX...");
        // Sofort ersten Uplink senden
        os_setCallback(&sendJob, sendUplink);
        break;

    case EV_JOIN_FAILED:
        Serial.println("[TTN] OTAA Join FEHLGESCHLAGEN — prüfe Keys!");
        updateOLED("JOIN FAILED!", "Keys prüfen:", "secrets.h korrekt?");
        break;

    case EV_TXCOMPLETE:
        Serial.printf("[TTN] TX abgeschlossen (RXRSSI=%d)\n", LMIC.rssi);

        // Downlink empfangen? (RX1 oder RX2)
        if (LMIC.dataLen > 0) {
            Serial.printf("[RX] Downlink empfangen: %d Byte(s) auf Port %d\n",
                          LMIC.dataLen, LMIC.frame[LMIC.dataBeg - 1]);

            // Payload-Auswertung (1 Byte Kommando)
            // 0x00 = LED aus
            // 0x01 = LED an
            // 0x02 = LED toggeln
            if (LMIC.dataLen >= 1) {
                uint8_t cmd = LMIC.frame[LMIC.dataBeg];
                Serial.printf("[RX] Kommando: 0x%02X\n", cmd);

                switch (cmd) {
                    case 0x00:
                        ledState = false;
                        digitalWrite(LED_PIN, LOW);
                        Serial.println("[CMD] LED → AUS");
                        updateOLED("Downlink RX!", "Kommando: LED AUS", "OK");
                        break;
                    case 0x01:
                        ledState = true;
                        digitalWrite(LED_PIN, HIGH);
                        Serial.println("[CMD] LED → AN");
                        updateOLED("Downlink RX!", "Kommando: LED AN", "OK");
                        break;
                    case 0x02:
                        ledState = !ledState;
                        digitalWrite(LED_PIN, ledState ? HIGH : LOW);
                        Serial.printf("[CMD] LED → TOGGLE → %s\n", ledState ? "AN" : "AUS");
                        updateOLED("Downlink RX!", "Kommando: TOGGLE",
                                   ledState ? "LED jetzt: AN" : "LED jetzt: AUS");
                        break;
                    default:
                        Serial.printf("[CMD] Unbekanntes Kommando: 0x%02X\n", cmd);
                        break;
                }
            }
        } else {
            // Kein Downlink — normaler Status-Update
            char rssi_buf[32];
            snprintf(rssi_buf, sizeof(rssi_buf), "RSSI: %d dBm", LMIC.rssi);
            updateOLED("TX OK", rssi_buf, String("Nächster TX: " + String(TX_INTERVAL_SEC) + "s").c_str());
        }

        // Nächsten Uplink einplanen
        os_setTimedCallback(&sendJob,
                            os_getTime() + sec2osticks(TX_INTERVAL_SEC),
                            sendUplink);
        break;

    case EV_TXSTART:
        Serial.println("[TTN] TX gestartet");
        break;

    case EV_RXSTART:
        Serial.println("[TTN] RX Fenster geöffnet");
        break;

    case EV_JOIN_TXCOMPLETE:
        Serial.println("[TTN] Join TX abgeschlossen, warte auf Antwort...");
        break;

    case EV_RESET:
        Serial.println("[TTN] Stack Reset");
        break;

    case EV_LINK_DEAD:
        Serial.println("[TTN] Link tot — kein Downlink seit langer Zeit");
        updateOLED("Link Dead!", "Kein Downlink", "Läuft weiter...");
        break;

    default:
        Serial.printf("[TTN] Event: %d\n", (unsigned)ev);
        break;
    }
}

// ── Setup ──────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n=== SmartGarden PoC v0.1 ===");
    Serial.println("Hardware: TTGO LoRa32 V2.1");

    // LED
    pinMode(LED_PIN, OUTPUT);

    // PIR
    pinMode(PIR_PIN, INPUT);
    digitalWrite(LED_PIN, LOW);

    // OLED init
    Wire.begin(OLED_SDA, OLED_SCL);
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("[OLED] Kein Display gefunden — weiter ohne OLED");
    } else {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(0, 10);
        display.println("Smart");
        display.println("Garden");
        display.setTextSize(1);
        display.setCursor(0, 50);
        display.println("PoC v0.1 — Starte...");
        display.display();
        delay(1500);
    }

    updateOLED("Initialisiere...", "LMIC Setup", "EU868 / OTAA");

    // LMIC init
    os_init();
    LMIC_reset();

    // EU868: alle 8 Standard-Kanäle aktivieren
    // (TTN nutzt 8 Kanäle: 868.1, 868.3, 868.5 + 5 weitere)
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);

    // Datenrate für ersten Join: SF9 (robuster als SF7, weniger Übertragungen)
    LMIC_setDrTxpow(DR_SF9, 14);

    // ADR (Adaptive Data Rate) — für Outdoor-Tests aktivieren
    LMIC_setAdrMode(1);

    Serial.println("[LMIC] Starte OTAA Join...");
    LMIC_startJoining();
}

// ── Loop ───────────────────────────────────────────────────
void loop() {
    os_runloop_once();

    // PIR Polling — Zustandsänderung erkennen und sofort senden
    if (joined) {
        bool pirNow = digitalRead(PIR_PIN) == HIGH;
        if (pirNow != motionLastState) {
            motionLastState = pirNow;
            motionAlert     = pirNow;
            Serial.printf("[PIR] Zustand: %s\n", pirNow ? "ALARM" : "KLAR");
            if (!(LMIC.opmode & OP_TXRXPEND)) {
                os_setCallback(&sendJob, sendUplink);
            }
        }
    }
}
