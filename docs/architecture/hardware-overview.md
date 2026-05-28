# Hardware Architektur

## Systemübersicht

### Node-Typen

Das SmartGarden-System besteht aus zwei Node-Typen die kombiniert werden:

```
┌──────────────────────────────────────────────────────┐
│                    SENSOR-NODE                       │
│                                                      │
│  [6W 6V Solar] ─[CN3791 MPPT]─[2×18650 2P]          │
│                                      │               │
│  [BME280]  ────────────────┐         │               │
│  [Boden kapazitiv] ────────┤─[TTGO LoRa32]─────────► LoRaWAN
│  [AM312 PIR] ──────────────┤    │                    │
│  [SW-420] ─────────────────┘    │                    │
│                            [XL6009 12V]              │
│                                 │                    │
│                            [IRLZ44N]                 │
│                                 │                    │
│                            [Pumpe/Ventil 12V]        │
└──────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────┐
│                   SECURITY-NODE                      │
│  (vereinfachter Node ohne Pumpe)                     │
│                                                      │
│  [6W 6V Solar] ─[CN3791 MPPT]─[2×18650 2P]          │
│                                      │               │
│  [BME280] ─────────────────┐         │               │
│  [AM312 PIR] ──────────────┤─[TTGO LoRa32]─────────► LoRaWAN
│  [SW-420] ─────────────────┘                         │
└──────────────────────────────────────────────────────┘
```

---

## Schaltplan Sensor-Node (v0.1)

### 1. Ladekreis (Solar → Akku)

```
[6W 6V Solarpanel]
      +  ────────────[D1: SS14 Schottky]────────── VIN (CN3791)
                                                        │
                                          MPPT ──[100Ω]─┘
                                          PROG ──[1kΩ]── GND   (= 1A Ladestrom)
                                          VBAT ──────────────── Akku +
      -  ──────────────────────────────── GND (CN3791) ─────── Akku -
```

> Der **SS14 Schottky** verhindert Rückstrom vom Akku ins Panel bei Dunkelheit.
> Der **100Ω** Widerstand (VIN→MPPT) setzt den MPPT-Arbeitspunkt auf ~Vmpp des Panels.
> Der **1kΩ** PROG-Widerstand begrenzt den Ladestrom auf 1A (R = 1000 / I_charge).

### 2. Akkupack (2× 18650 parallel)

```
      CN3791 VBAT
            │
            ├──────── [18650 Zelle 1 +]
            ├──────── [18650 Zelle 2 +]   ← parallel, gleiche Zellen!
            │
      [DW01A + FS8205A]                   ← OVP / UVP / OCP Schutz
            │
      VBAT_OUT (3,7V nom.)
```

> Beide Zellen **müssen vor dem ersten Parallelschalten auf gleiche Spannung** gebracht werden
> (max. 50mV Differenz), sonst fließt Ausgleichsstrom. Gleicher Hersteller / Charge empfohlen.

### 3. Systemversorgung & Signale

```
      VBAT_OUT (3,7V)
            │
            ├──── TTGO LoRa32 [BAT-Pin]
            │          │
            │     [3,3V LDO onboard]
            │          │
            │          ├── BME280        SDA → GPIO21
            │          │                SCL → GPIO22
            │          │
            │          ├── Capacitive    AOUT → GPIO34 (ADC)
            │          │   Soil Sensor   VCC → 3,3V  (via GPIO33 schaltbar*)
            │          │
            │          ├── AM312 PIR     OUT  → GPIO13
            │          │                VCC → 3,3V
            │          │
            │          ├── SW-420        OUT  → GPIO14
            │          │                VCC → 3,3V
            │          │
            │          └── Akku-Messung  GPIO35 ← Spannungsteiler (100kΩ / 100kΩ)
            │                            von VBAT_OUT
            │
            └──── XL6009 Boost (3,7V → 12V)
                       │
                  [IRLZ44N Gate] ── 1kΩ ── GPIO12
                       │
                  [Pumpe / Magnetventil 12V]
                       │
                      GND
```

> *Der Bodenfeuchte-Sensor wird nur während der Messung mit Strom versorgt (GPIO33 HIGH),
> um elektrolytische Korrosion der Elektroden zu minimieren.

### 4. Pin-Belegung TTGO LoRa32 (Zusammenfassung)

| GPIO | Funktion | Typ |
|---|---|---|
| 21 | BME280 SDA | I2C |
| 22 | BME280 SCL | I2C |
| 34 | Bodenfeuchte ADC | Analog IN |
| 33 | Bodenfeuchte Power | Digital OUT |
| 13 | AM312 PIR Signal | Digital IN |
| 14 | SW-420 Erschütterung | Digital IN |
| 12 | MOSFET Gate (Pumpe) | Digital OUT |
| 35 | Akku-Spannung ADC | Analog IN |
| 5  | LoRa NSS (intern) | SPI (onboard) |
| 18 | LoRa SCK (intern) | SPI (onboard) |
| 19 | LoRa MISO (intern) | SPI (onboard) |
| 27 | LoRa MOSI (intern) | SPI (onboard) |
| 26 | LoRa RST (intern) | Digital (onboard) |
| 25 | LoRa DIO0 (intern) | Digital (onboard) |

---

## Komponenten (vorläufige BOM)

### Mikrocontroller
| Komponente | Modell | Preis | Begründung |
|---|---|---|---|
| MCU + LoRa | TTGO LoRa32 V2.1 | ~15€ | ESP32 + SX1276 integriert, kein manuelles Löten des LoRa-Moduls |
| Alternativ | Heltec WiFi LoRa 32 V3 | ~18€ | Display integriert, einfacheres Debugging |

> **Prototyp-Erfahrung (v0.0):** STM32 + SX1276 Breakout auf Lochraster funktionierte auf 868 MHz,
> jedoch mit erheblichem Lötaufwand und aufwändiger Toolchain. Wechsel zu ESP32 mit integriertem
> SX1276 für schnellere Entwicklung. Siehe [ADR-001](ADR-001-MCU-Auswahl.md).

### Sensorik
| Komponente | Modell | Preis | Begründung |
|---|---|---|---|
| Temp/Feuchte/Druck | BME280 (GY-BME280) | ~4€ | Im Prototyp bewährt; I2C, zusätzlich Luftdruck |
| Bodenfeuchte | Capacitive Sensor v1.2 | ~4€ | Kein Rost (resistive vermeiden!) |
| Bewegung | **AM312 PIR** | ~2€ | 3,3V kompatibel → kein 5V-Boost nötig; Ersatz für HC-SR501 |
| Erschütterung | SW-420 | ~1€ | Für Türen/Fenster der Laube, 3,3V kompatibel |

### Energieversorgung
| Komponente | Modell | Preis | Begründung |
|---|---|---|---|
| Solarpanel | 6W 6V Mono | ~10€ | Vmp ~6V passt direkt zu CN3791 (max. 7V Vin) |
| LiOn Akku | 2× 18650 parallel (2P, ~5000mAh) | ~10€ | Doppelter Puffer, **kein HW-Aufwand** da gleiche Spannung wie 1× |
| MPPT-Laderegler | **CN3791** | ~3€ | MPPT für 1S LiIon, Vin 4–7V, lädt ab ~4V Panelspannung |
| Schutz-IC | DW01A + FS8205A | ~1€ | OVP / UVP / OCP Schutz für Akkupack |
| Schottky-Diode | SS14 | <1€ | Rückstromschutz Panel → CN3791 |
| Boost-Converter | XL6009 (3,7V → 12V) | ~2€ | Pumpenversorgung, nur bei Bedarf aktiv; bei 10W Pumpe: I_out ≈ 830mA → I_in ≈ 3,4A, noch im XL6009-Limit (4A) |
| MOSFET | IRLZ44N | ~1€ | Pumpensteuerung, Logic-Level-kompatibel mit 3,3V GPIO |
| Widerstand MPPT | 100Ω | <1€ | Setzt MPPT-Arbeitspunkt des CN3791 |
| Widerstand PROG | 1kΩ | <1€ | Begrenzt Ladestrom auf 1A |

### Aktorik

> **Wichtig:** Ein Schrebergarten hat keinen Wasseranschluss mit Druck — nur Wasserfässer
> und Regentonnen. Das Wasser muss aktiv gepumpt werden. Die Tauchpumpe ist deshalb
> **kein optionales Feature**, sondern der Kern-Mechanismus des Produkts.
> Systeme wie MIYO, die nur Magnetventile unterstützen, sind für diesen Use-Case
> grundsätzlich ungeeignet.

| Komponente | Modell | Preis | Typ | Begründung |
|---|---|---|---|---|
| Pumpe | 12V Tauchpumpe **10W, min. 3m Förderhöhe** | ~12€ | **PRIMÄR** | Fördert Wasser aktiv aus Regentonne/Fass; Förderhöhe ist das kritische Kriterium → siehe ADR-003 |
| Ventil | 12V Magnetventil | ~12€ | Optional | Nur für Parzellen mit Vereins-Wasseranschluss |

> ⚠️ **Förderhöhe ist nicht optional.** Eine billige 3–5W Tauchpumpe hat typisch ~1,5m **Stillstandsförderhöhe** —
> bei 1m Gegendruck (Regentonne 80cm + Schlauch) fließt praktisch kein Wasser.
> Mindestvorgabe: **3m max. Förderhöhe** (damit bei 1,5m Betriebspunkt noch ≥200 L/h fließen).
> Prototyp-Erfahrung: Pumpe versagte bereits bei 1m Höhenunterschied Eimer → Becken.
> Kennzahl beim Kauf prüfen: „max. head" oder „Förderhöhe max." im Datenblatt, nicht den Nennstrom.

---

## Energiebudget (Schätzung)

### Verbrauch Sensor-Node

> **Hinweis Pumpe:** Strom- und Energiewerte für die Pumpe wurden mit der korrekten 10W-Spezifikation
> neu berechnet (vorher: 3–5W ohne Förderhöhe). Realistischer Bewässerungszyklus: 2× täglich à 2 min
> (nicht 30 min). Worst-Case-Budget (z.B. intensiver Bewässerungstag): 4× à 5 min = 20 min.

| Zustand | Strom (Akku-Ebene) | Dauer/Tag | Energie/Tag |
|---|---|---|---|
| Deep Sleep | 0,01 mA | 23,5h | 0,24 mWh |
| Messung + LoRa TX | 120 mA | 30 min | 60 mWh |
| Pumpe aktiv (10W, η Boost 80%) | ~3.400 mA | **4 min** (realistisch) | **~830 mWh** |
| Pumpe aktiv — Worst Case | ~3.400 mA | 20 min | ~1.130 mWh |
| **Gesamt (realistisch)** | | | **~890 mWh/Tag** |
| **Gesamt (Worst Case)** | | | **~1.190 mWh/Tag** |

### Solar-Ertrag & Saisonbetrachtung (Deutschland)

| | Sommer | Winter |
|---|---|---|
| Verbrauch/Tag | ~890 mWh (realistisch, mit Pumpe) | ~60 mWh (nur Alarm + Sensor, keine Bewässerung) |
| Effektive Sonnenstunden | 4–6h | 0,5–1h |
| Ertrag 6W-Panel/Tag | ~24.000 mWh (4h) | ~3.000 mWh (0,5h) |
| Tagesbilanz | +23.110 mWh ✅ | +2.940 mWh ✅ |
| **Autonomie ohne Sonne** (2× 18650, ~18.500 mWh) | **~21 Tage** | **~308 Tage** |

> **Erkenntnis aus Prototyp-Experimenten:** Der Engpass im Winter war nicht die Panelgröße,
> sondern der **TP4056 ohne MPPT**, der bei diffusem Licht und sinkender Panelspannung
> (<4,5V) den Ladevorgang komplett abbrach. Mit **CN3791 MPPT** startet das Laden
> bereits ab ~4V und das 6W-Panel reicht auch im deutschen Winter.
>
> Ein größeres Panel (10W+) verbessert die Sicherheitsmarge, ist aber für den
> Normalbetrieb nicht zwingend erforderlich.

### Prototyp-Erfahrung: Schwachlicht-Problem (v0.0)

Im ersten Prototyp wurde der Node bei schwacher Sonne nicht ausreichend geladen.
Verwendet wurde **1× 18650 (~2500mAh, ~44 Tage Autonomie)** — die Batteriekapazität war
also nicht das Problem. Die Ursache lag in der **Ladekette**:

| Problem | Prototyp v0.0 | Lösung v0.1 |
|---|---|---|
| Mindestspannung Laderegler | TP4056 braucht min. ~4,5V — kleine Panels fallen bei Bewölkung darunter → **kein Laden** | CN3791 MPPT startet schon ab ~4V und optimiert die Entnahme |
| Kein MPPT | TP4056 arbeitet ohne Maximum-Power-Point-Tracking — schlechter Wirkungsgrad bei diffusem Licht | CN3791 mit echtem **MPPT** — holt auch bei 20% Sonnenstrahlung das Maximum |
| Solarpanels zu klein | Zwei kleine Panels mit unbekannter Leistung — Strom zu gering für zuverlässiges Laden | 6W 6V Mono-Panel — ausreichend Strom auch bei Bewölkung |

**Energievergleich 1× vs. 2× 18650 (parallel, 2P):**

| | 1× 18650 (~2500mAh) | **2× 18650 2P (~5000mAh)** |
|---|---|---|
| Kapazität | 9.250 mWh | **18.500 mWh** |
| Spannung | 3,7V | **3,7V (identisch!)** |
| Laderegler | CN3791 | **CN3791 (gleicher!)** |
| HW-Änderung nötig | — | **❌ nein — nur parallel schalten** |
| Autonomie Sommer (ohne Sonne) | ~44 Tage | ~88 Tage |
| Autonomie Winter (ohne Sonne) | ~154 Tage | ~308 Tage |

**Fazit:** 2× 18650 **parallel (2P)** erfordert keine Hardware-Änderung am Laderegler —
der CN3791 sieht nur eine "größere" Batterie mit identischer Spannung. Empfohlen als
Sicherheitspuffer für lange Schlechtwetterperioden im Winter.

---

## LoRaWAN Konfiguration

| Parameter | Wert |
|---|---|
| Frequenzband | 868 MHz (EU863-870) |
| Spreading Factor | SF9 (Kompromiss Reichweite/Energie) |
| Sendezyklus | alle 15 Minuten |
| Payload | ~20 Bytes (Temp, Feuchte, Boden, Akku, Alarm-Flag) |
| Netzwerk | The Things Network (TTN) — kostenlos |

---

## Nächste Schritte

### ✅ Entschieden
- [x] ADR-001: MCU-Auswahl → ESP32 (TTGO LoRa32) statt STM32
- [x] ADR-002: Protokoll → LoRaWAN / TTN statt proprietär
- [x] Sensor → BME280 statt DHT22
- [x] PIR → AM312 (3,3V) statt HC-SR501 (5V)
- [x] Laderegler → CN3791 MPPT statt TP4056
- [x] Akku → 2× 18650 parallel (2P), kein HW-Aufwand
- [x] Ladekreis-Architektur definiert (SS14 + CN3791 + DW01A + XL6009)
- [x] ADR-003: Pumpenauswahl → min. 3m Förderhöhe, 10W (nicht 3–5W)

### 🔧 Hardware (v0.1)
- [ ] Prototyp v0.0 Fritzing-Schaltplan archivieren → `hardware/schematics/v0.0-prototype-fritzing.png`
- [ ] Schaltplan v0.1 in KiCad zeichnen (Ladekreis + TTGO + Sensoren + Pumpe)
- [ ] KiCad Schaltplan reviewen & ERC-Check durchführen
- [ ] Lochraster-Aufbau v0.1 (basierend auf neuem Schaltplan)
- [ ] CN3791 Ladekreis mit Solarpanel + Multimeter validieren
- [ ] Energiebudget mit Messungen am echten Node validieren (Deep Sleep, TX, Pumpe)

### 📐 Gehäuse
- [ ] Anforderungen definieren (IP65, Montageart, Kabeleinführungen)
- [ ] Gehäuse-Konzept & Maße (für Lochraster-Platine + 2× 18650 + Panel-Halterung)

### 💻 Firmware
- [ ] PlatformIO-Projekt anlegen (ESP32 + MCCI LMIC LoRaWAN)
- [ ] Deep-Sleep-Zyklus implementieren (15-Minuten-Intervall)
- [ ] BME280 + Bodenfeuchte auslesen
- [ ] LoRaWAN Payload kodieren & an TTN senden
- [ ] Pumpensteuerung per Downlink
