// ============================================================
// SmartGarden Irrigator — Pumpentest (Hardware-Bring-up)
// Hardware : LILYGO T3_V1.6.1 (ESP32)
//            IRLZ44N Low-Side-Switch an GPIO12 (500Ω Gate-R, 10kΩ Pulldown)
//            XL6009 Boost 3,7V→12V, QWORK 12V Tauchpumpe
// Funktion : GPIO12 alle PUMP_INTERVAL_SEC Sekunden ein/aus schalten
//            — kein LoRa, nur zum Verifizieren der Pumpenschaltung
// ============================================================

#include <Arduino.h>

#define PUMP_GATE_PIN      12
#define PUMP_ON_SEC        5     // Pumpe an
#define PUMP_OFF_SEC       5     // Pumpe aus

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== SmartGarden Irrigator — Pumpentest ===");

    // GPIO12 ist ESP32-Strapping-Pin (MTDI) — explizit LOW setzen,
    // bevor irgendetwas anderes passiert (Pumpe darf beim Boot nicht laufen)
    pinMode(PUMP_GATE_PIN, OUTPUT);
    digitalWrite(PUMP_GATE_PIN, LOW);
    Serial.println("[PUMP] GPIO12 = LOW (Pumpe AUS)");
}

void loop() {
    Serial.printf("[PUMP] AN für %ds...\n", PUMP_ON_SEC);
    digitalWrite(PUMP_GATE_PIN, HIGH);
    delay(PUMP_ON_SEC * 1000UL);

    Serial.printf("[PUMP] AUS für %ds...\n", PUMP_OFF_SEC);
    digitalWrite(PUMP_GATE_PIN, LOW);
    delay(PUMP_OFF_SEC * 1000UL);
}
