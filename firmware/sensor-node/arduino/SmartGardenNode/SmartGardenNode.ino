// ============================================================
// SmartGarden — Guard Node PoC v0.2
// Hardware : TTGO LoRa32 V2.1 (ESP32 + SX1276)
// Funktion : Deep Sleep + PIR Wakeup + Session-Persistenz
//            TTN OTAA-Join + LED per Downlink steuern
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
#define LED_PIN        25
#define OLED_SDA       21
#define OLED_SCL       22
#define OLED_RST       -1
#define OLED_WIDTH     128
#define OLED_HEIGHT    64
#define OLED_ADDR      0x3C
#define PIR_PIN        13       // HC-SR501 Bewegungssensor

// SX1276 SPI-Pinbelegung
const lmic_pinmap lmic_pins = {
    .nss  = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst  = 23,
    .dio  = { 26, 33, 32 },
};

// ── Timer-Intervall für regulären Status-Uplink ────────────
#define SLEEP_INTERVAL_US  (15ULL * 60 * 1000000)  // 15 Minuten

// ── RTC-Speicher (überlebt Deep Sleep) ────────────────────
// Session-Persistenz wurde entfernt: RTC-Restore führte zu
// Frame-Counter-/Kanalproblemen, bei denen Uplinks vom Network
// Server still verworfen wurden. Stabiler Ansatz: nach jedem
// Sleep frisch per OTAA joinen (Gateway ist in Reichweite,
// Join geht in <1s).
RTC_DATA_ATTR bool     rtcLedState     = false;
RTC_DATA_ATTR uint8_t  rtcAlarmCount   = 0;    // Anzahl Alarm-Uplinks seit letztem Sleep

// ── Max. Alarm-Uplinks vor erzwungenem Sleep ───────────────
#define MAX_ALARM_UPLINKS  5

// ── TEST-MODUS: Deep Sleep deaktiviert ─────────────────────
// Knoten bleibt wach und meldet jede PIR-Zustandsänderung sofort per Uplink.
// So lässt sich ALARM/KLAR-Übertragung nach TTN ohne Sleep-Zyklen testen.
#define TEST_NO_SLEEP  false

// ── Wakeup-Grund ───────────────────────────────────────────
enum WakeupReason { WAKEUP_RESET, WAKEUP_TIMER, WAKEUP_PIR };

WakeupReason getWakeupReason() {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_EXT0) return WAKEUP_PIR;
    if (cause == ESP_SLEEP_WAKEUP_TIMER) return WAKEUP_TIMER;
    return WAKEUP_RESET;
}

// ── Zustand ────────────────────────────────────────────────
static bool     ledState        = false;
static bool     motionAlert     = false;
static bool     motionLastState = false;
static uint32_t txCount         = 0;
static bool     txDone          = false;
static osjob_t  sendJob;

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RST);

// ── OLED ───────────────────────────────────────────────────
void updateOLED(const char* line1, const char* line2 = "", const char* line3 = "") {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("SmartGarden v0.2");
    display.drawLine(0, 10, 127, 10, WHITE);
    display.setCursor(0, 14); display.println(line1);
    display.setCursor(0, 26); display.println(line2);
    display.setCursor(0, 38); display.println(line3);
    display.setCursor(0, 54);
    display.printf("LED:%s  PIR:%s", ledState ? "ON" : "OFF", motionAlert ? "ALM" : "OK");
    display.display();
}

// ── TTN OTAA Callbacks ─────────────────────────────────────
void os_getArtEui(u1_t* buf) { memcpy_P(buf, APPEUI, 8); }
void os_getDevEui(u1_t* buf) { memcpy_P(buf, DEVEUI, 8); }
void os_getDevKey(u1_t* buf) { memcpy_P(buf, APPKEY, 16); }

// ── Sleep einleiten nach Warte-Timer ──────────────────────
void triggerSleep(osjob_t* j) {
    txDone = true;
}

// ── Uplink senden ──────────────────────────────────────────
void sendUplink(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) return;

    uint8_t payload[2];
    payload[0] = ledState    ? 0x01 : 0x00;
    payload[1] = motionAlert ? 0x01 : 0x00;

    LMIC_setTxData2(1, payload, sizeof(payload), 0);
    txCount++;
    Serial.printf("[TX] Uplink #%lu — LED=%s Motion=%s\n",
                  txCount, ledState ? "ON" : "OFF", motionAlert ? "ALARM" : "OK");
    updateOLED("Sende Uplink...",
               ledState ? "LED: AN" : "LED: AUS",
               motionAlert ? "BEWEGUNG!" : "Status OK");
}

// ── Deep Sleep einleiten ───────────────────────────────────
void goToSleep() {
    if (TEST_NO_SLEEP) {
        Serial.println("[TEST] Sleep deaktiviert — bleibe wach (TEST_NO_SLEEP)");
        return;
    }

    rtcLedState = ledState;

    // Bug-Fix: PIR noch HIGH? Warten bis KLAR sonst kein Wakeup möglich!
    // EXT0 weckt nur bei steigender Flanke (0→1) — bei dauerhaft HIGH
    // würde der ESP32 nie wieder aufwachen.
    if (digitalRead(PIR_PIN) == HIGH) {
        Serial.println("[SLEEP] PIR noch HIGH — warte auf KLAR vor Sleep...");
        updateOLED("Warte auf KLAR", "PIR noch aktiv", "...");
        uint32_t waitStart = millis();
        while (digitalRead(PIR_PIN) == HIGH) {
            delay(500);
            // Sicherheits-Timeout: nach 5 Minuten trotzdem schlafen
            if (millis() - waitStart > 300000) {
                Serial.println("[SLEEP] Timeout — schlafen trotz PIR HIGH");
                break;
            }
        }
        delay(1000);  // kurz warten nach fallender Flanke
        Serial.println("[SLEEP] PIR KLAR — jetzt schlafen");
    }

    Serial.println("[SLEEP] Gehe in Deep Sleep...");
    Serial.printf("[SLEEP] Wakeup: PIR GPIO%d oder Timer %llu min\n",
                  PIR_PIN, SLEEP_INTERVAL_US / 60000000ULL);
    delay(100);

    display.clearDisplay();
    display.display();

    // Wakeup durch PIR (steigende Flanke) oder Timer
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIR_PIN, 1);
    esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_US);
    esp_deep_sleep_start();
}

// ── LMIC Event-Handler ─────────────────────────────────────
void onEvent(ev_t ev) {
    switch (ev) {
    case EV_JOINING:
        Serial.println("[TTN] OTAA Join...");
        updateOLED("Verbinde TTN...", "OTAA Join...", "Bitte warten");
        break;

    case EV_JOINED:
        Serial.println("[TTN] *** Join erfolgreich! ***");
        LMIC_setLinkCheckMode(1);  // Link Check AN — erkennt verlorene Verbindung
        LMIC_setAdrMode(0);
        updateOLED("TTN verbunden!", "OTAA OK", "Sende Uplink...");
        os_setCallback(&sendJob, sendUplink);
        break;

    case EV_JOIN_FAILED:
        Serial.println("[TTN] Join FEHLGESCHLAGEN");
        updateOLED("JOIN FAILED!", "Keys prüfen", "secrets.h ?");
        delay(2000);
        goToSleep();
        break;

    case EV_TXCOMPLETE:
        Serial.printf("[TTN] TX done (RXRSSI=%d)\n", LMIC.rssi);

        // Downlink empfangen?
        if (LMIC.dataLen > 0) {
            uint8_t cmd = LMIC.frame[LMIC.dataBeg];
            Serial.printf("[RX] Kommando: 0x%02X\n", cmd);
            switch (cmd) {
                case 0x00:
                    ledState = false;
                    digitalWrite(LED_PIN, LOW);
                    Serial.println("[CMD] LED → AUS");
                    updateOLED("Downlink!", "LED: AUS", "OK");
                    break;
                case 0x01:
                    ledState = true;
                    digitalWrite(LED_PIN, HIGH);
                    Serial.println("[CMD] LED → AN");
                    updateOLED("Downlink!", "LED: AN", "OK");
                    break;
                case 0x02:
                    ledState = !ledState;
                    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
                    Serial.printf("[CMD] LED → TOGGLE → %s\n", ledState ? "AN" : "AUS");
                    updateOLED("Downlink!", "TOGGLE", ledState ? "LED: AN" : "LED: AUS");
                    break;
            }
            delay(1000);
        }

        // PIR-Status neu auslesen — motionAlert war bisher nur der Stand
        // beim Boot und wurde nie aktualisiert (daher blieb "ALARM" hängen,
        // selbst wenn die Bewegung längst vorbei war).
        motionAlert = (digitalRead(PIR_PIN) == HIGH);
        Serial.printf("[PIR] Status vor Entscheidung: %s\n", motionAlert ? "ALARM" : "KLAR");

        // PIR noch aktiv UND maximale Alarm-Uplinks noch nicht erreicht?
        if (motionAlert && rtcAlarmCount < MAX_ALARM_UPLINKS) {
            rtcAlarmCount++;
            Serial.printf("[SLEEP] PIR aktiv — Alarm TX %d/%d, warte 30s\n",
                          rtcAlarmCount, MAX_ALARM_UPLINKS);
            updateOLED("PIR aktiv...",
                       String("Alarm " + String(rtcAlarmCount) + "/" + String(MAX_ALARM_UPLINKS)).c_str(),
                       "Warte 30s...");
            os_setTimedCallback(&sendJob,
                                os_getTime() + sec2osticks(30),
                                sendUplink);
        } else {
            // Kein Alarm ODER max. Uplinks erreicht → schlafen
            if (rtcAlarmCount >= MAX_ALARM_UPLINKS) {
                Serial.println("[SLEEP] Max. Alarm-Uplinks erreicht — erzwinge Sleep");
            } else {
                Serial.println("[SLEEP] PIR klar — warte 10s dann schlafen");
            }
            rtcAlarmCount = 0;  // Zähler zurücksetzen
            updateOLED("TX fertig", "Warte 10s...", "dann Deep Sleep");
            os_setTimedCallback(&sendJob,
                                os_getTime() + sec2osticks(10),
                                triggerSleep);
        }
        break;

    case EV_LINK_DEAD:
        // Keine Downlinks seit langer Zeit — beim nächsten Wakeup wird ohnehin neu gejoint
        Serial.println("[TTN] Link tot — schlafe, beim nächsten Wakeup neuer Join");
        updateOLED("Link Dead!", "Schlafe...", "");
        delay(1000);
        goToSleep();
        break;

    case EV_TXSTART:   Serial.println("[TTN] TX gestartet"); break;
    case EV_RXSTART:   Serial.println("[TTN] RX Fenster"); break;
    case EV_JOIN_TXCOMPLETE: Serial.println("[TTN] Join TX, warte..."); break;
    default: Serial.printf("[TTN] Event: %d\n", (unsigned)ev); break;
    }
}

// ── Setup ──────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(300);

    WakeupReason reason = getWakeupReason();

    Serial.println("\n=== SmartGarden Guard v0.2 ===");
    Serial.printf("Wakeup: %s\n",
        reason == WAKEUP_PIR   ? "PIR ALARM" :
        reason == WAKEUP_TIMER ? "Timer" : "Reset/Boot");

    // Hardware init
    pinMode(LED_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);

    // LED-Status aus RTC wiederherstellen
    ledState = rtcLedState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);

    // Bewegung: PIR-Wakeup = Alarm, oder PIR gerade aktiv beim Timer-Wakeup
    motionAlert = (reason == WAKEUP_PIR) || (digitalRead(PIR_PIN) == HIGH);
    Serial.printf("[PIR] Status beim Start: %s\n", motionAlert ? "ALARM" : "KLAR");

    // OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(0, 10);
        display.println("Smart");
        display.println("Guard");
        display.setTextSize(1);
        display.setCursor(0, 50);
        display.printf("%s", motionAlert ? "ALARM!" : "Status Update");
        display.display();
        delay(1000);
    }

    updateOLED(
        motionAlert ? "BEWEGUNG!" : "Status Update",
        "Join TTN...",
        "Sende Uplink..."
    );

    // LMIC init
    os_init();
    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 20 / 100);  // RX-Fenster 20% breiter

    // EU868 Kanäle immer setzen (gilt für OTAA und RTC-Session)
    LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);

    // Immer frisch joinen — stabiler als RTC-Session-Restore (siehe Tests)
    Serial.println("[LMIC] Starte OTAA Join...");
    LMIC_setDrTxpow(DR_SF9, 14);
    LMIC_startJoining();
}

// ── Loop ───────────────────────────────────────────────────
void loop() {
    os_runloop_once();

    if (TEST_NO_SLEEP) {
        // PIR-Zustandsänderung sofort melden (kein Deep-Sleep-Wakeup zur Erkennung)
        bool pirNow = digitalRead(PIR_PIN) == HIGH;
        if (pirNow != motionLastState) {
            motionLastState = pirNow;
            motionAlert     = pirNow;
            Serial.printf("[PIR] Zustandswechsel: %s\n", pirNow ? "ALARM" : "KLAR");
            if (!(LMIC.opmode & OP_TXRXPEND)) {
                os_setCallback(&sendJob, sendUplink);
            }
        }
        return;
    }

    // Nach TX → schlafen
    if (txDone) {
        txDone = false;
        delay(500);
        goToSleep();
    }
}
