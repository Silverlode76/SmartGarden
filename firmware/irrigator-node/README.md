# SmartGarden Irrigator — PoC v0.2 (LoRaWAN)

**Hardware:** LILYGO T3_V1.6.1 + IRLZ44N (Pumpe) + kapazitiver Bodenfeuchtesensor
**Pumpe:** YWBL-WH Mini-Membranpumpe, 3-3,7V, direkt am Akku (kein Boost-Converter)
**Protokoll:** LoRaWAN OTAA → TTN
**Ziel:** Boden zu trocken → Pumpe an, automatisch wieder aus bei ausreichender Feuchte,
Status per LoRaWAN gemeldet

---

## Hardware-Aufbau

```
LILYGO T3_V1.6.1            IRLZ44N MOSFET
─────────────────            ───────────────
GPIO12 ──[500Ω]────────────►  Gate
                               Drain ──► Pumpe blau (−)
                               Source ─► GND
GND ──[10kΩ Pulldown]──────►  Gate (verhindert Pumpenstart beim Boot,
                                      GPIO12 ist ESP32-Strapping-Pin)

Pumpe braun (+) ──────────────► VBAT (Akku, direkt — kein Boost nötig)

Bodenfeuchtesensor (kapazitiv)
───────────────────────────────
VCC  → GPIO4   (schaltbar, nicht GPIO33 — auf diesem Board intern für LoRa belegt)
GND  → GND
AOUT → GPIO34  (ADC)
```

## Kalibrierung (eigener Sensor, Juni 2026)

| Zustand | ADC-Rohwert |
|---|---|
| Trocken (Luft) | ~1671 |
| Nass (Wasser) | ~667 |

Schwellenwerte mit Hysterese: **Pumpe AN ab 1400, AUS ab 950** — bei abweichendem
Sensor/Topf eigene Werte ermitteln (siehe `firmware/irrigator-node/arduino/IrrigatorPumpTest/`
für ein reines Kalibrier-Sketch ohne LoRaWAN).

---

## Software-Setup

### 1. PlatformIO-Projekt öffnen

```bash
cd firmware/irrigator-node
```

### 2. secrets.h anlegen

```bash
cp src/secrets.example.h src/secrets.h
```

### 3. TTN-Anwendung & Gerät registrieren

**a) Application anlegen:**
→ [console.cloud.thethings.network](https://console.cloud.thethings.network) — Region `eu1`
→ "Create application", z.B. ID `smartgarden-irrigator`

**b) Device registrieren:**
- "Register end device" → "Enter end device specifics manually"
- Frequency plan: `Europe 863-870 MHz (SF9 for RX2)`
- LoRaWAN version: `LoRaWAN Specification 1.0.3`
- Regional Parameters: `RP001 Regional Parameters 1.0.3 revision A`
- Activation: `OTAA`
- JoinEUI/DevEUI/AppKey: "Generate"
- End device ID: z.B. `irrigator-poc-01`

**c) Credentials in secrets.h eintragen** (Device → Overview):

| TTN-Feld | Format | In secrets.h |
|----------|--------|-------------|
| DevEUI | **lsb** | `DEVEUI[]` |
| AppEUI / JoinEUI | **lsb** | `APPEUI[]` |
| AppKey | **msb** | `APPKEY[]` |

### 4. Firmware flashen

```bash
pio run --target upload
pio device monitor
```

Erwartete Ausgabe:
```
=== SmartGarden Irrigator PoC v0.2 (LoRaWAN) ===
[LMIC] Starte OTAA Join...
[TTN] Verbinde mit TTN (OTAA)...
[TTN] *** OTAA Join erfolgreich! ***
[SOIL] ADC-Rohwert: 1532
[SOIL] Zu trocken — starte Pumpe
[PUMP] AN
[TX] Uplink — Pumpe=AN Boden=1532
[TTN] TX abgeschlossen (RSSI=-52)
```

---

## Payload Formatter (TTN Console → Payload Formatters → Uplink → Custom Javascript)

```javascript
function decodeUplink(input) {
  var pumpOn = input.bytes[0] === 0x01;
  var soil   = (input.bytes[1] << 8) | input.bytes[2];
  return {
    data: {
      pump_on: pumpOn,
      soil_raw: soil,
    },
    warnings: [],
    errors: []
  };
}
```

---

## Bekannte PoC-Einschränkungen

| Einschränkung | Auswirkung | Geplante Lösung (v0.3) |
|---------------|------------|------------------------|
| Kein Deep Sleep | Kontinuierlicher Betrieb, hoher Akkuverbrauch | Deep Sleep zwischen Messzyklen, analog Guard v0.2 |
| Feste Schwellenwerte im Code | Keine Fernkonfiguration | Schwellenwerte per Downlink konfigurierbar machen |
| Sensor-Kalibrierung manuell im Code | Andere Sensoren/Böden brauchen Neukalibrierung | Auto-Kalibrierung oder Konfig-Workflow in App |

---

## Verzeichnisstruktur

```
firmware/irrigator-node/
├── platformio.ini
├── README.md                              ← diese Datei
├── src/
│   ├── main.cpp                           ← LoRaWAN-Version (PlatformIO)
│   ├── secrets.example.h                  ← Vorlage (committed)
│   └── secrets.h                          ← deine Credentials (in .gitignore!)
└── arduino/IrrigatorPumpTest/
    └── IrrigatorPumpTest.ino              ← Kalibrier-/Testsketch ohne LoRaWAN (Arduino IDE)
```
