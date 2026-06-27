// ============================================================
// SmartGarden Irrigator — Feuchtegesteuerte Bewässerung (PoC)
// Hardware : LILYGO T3_V1.6.1 (ESP32)
//            IRLZ44N Low-Side-Switch an GPIO12 (500Ω Gate-R, 10kΩ Pulldown)
//            YWBL-WH Mini-Membranpumpe (3-3,7V, direkt am Akku, kein Boost)
//            Kapazitiver Bodenfeuchtesensor: VCC → GPIO4, AOUT → GPIO34
//            (GPIO33 laut Doku-Plan ist auf diesem Board nicht herausgeführt)
// Funktion : Pumpe läuft nur, wenn der Boden zu trocken ist. Hysterese
//            verhindert Flackern am Übergang. Max. Laufzeit als Notbremse.
//
// Kalibrierung (Juni 2026, eigener Sensor/Topf):
//   trocken (Luft) ≈ 1671   |   nass (Wasser) ≈ 667
// ============================================================

#define PUMP_GATE_PIN      12

#define SOIL_PWR_PIN       4     // Sensor-VCC, schaltbar
#define SOIL_ADC_PIN       34    // Sensor-AOUT

// Hysterese-Schwellen zwischen den kalibrierten Werten (667 trocken...1671 nass)
// Pumpe startet erst, wenn deutlich zu trocken; stoppt erst, wenn deutlich feucht genug
#define SOIL_THRESHOLD_DRY   1400   // ab hier: Pumpe AN
#define SOIL_THRESHOLD_WET   950    // ab hier: Pumpe AUS

#define CHECK_INTERVAL_SEC   10     // wie oft Feuchte geprüft wird
#define MAX_PUMP_RUNTIME_SEC 120    // Notbremse: nie länger ununterbrochen pumpen

static bool pumpRunning = false;
static unsigned long pumpStartMs = 0;

int readSoilMoisture() {
    // Sensor nur kurz vor der Messung mit Strom versorgen,
    // verhindert elektrolytische Korrosion der Elektroden
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

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== SmartGarden Irrigator — Feuchtegesteuerte Bewässerung ===");

    // GPIO12 ist ESP32-Strapping-Pin (MTDI) — explizit LOW setzen,
    // bevor irgendetwas anderes passiert (Pumpe darf beim Boot nicht laufen)
    pinMode(PUMP_GATE_PIN, OUTPUT);
    digitalWrite(PUMP_GATE_PIN, LOW);

    pinMode(SOIL_PWR_PIN, OUTPUT);
    digitalWrite(SOIL_PWR_PIN, LOW);
}

void loop() {
    int soil = readSoilMoisture();
    Serial.printf("[SOIL] ADC-Rohwert: %d\n", soil);

    // Notbremse: max. Laufzeit erreicht → immer abschalten, unabhängig vom Feuchtewert
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

    delay(CHECK_INTERVAL_SEC * 1000UL);
}
