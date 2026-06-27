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
│                            [IRLZ44N]                 │
│                                 │                    │
│                         [Drip-Pumpe 3,7–5V]         │
│                         [+ Drip-Emitter]             │
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
            │          │   Soil Sensor   VCC → 3,3V  (via GPIO33 schaltbar*, auf LILYGO T3_V1.6.1: GPIO4 — siehe Hinweis unten)
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
            └──── [IRLZ44N Gate] ── 1kΩ ── GPIO12
                       │ (Low-Side Switch)
                  [Drip-Pumpe 3,7–5V]  ← VBAT_OUT direkt (kein Boost nötig)
                       │
                      GND

> **Boost-Converter entfällt:** Die Drip-Pumpe läuft direkt auf 3,7–5V LiPo-Spannung.
> Der XL6009 (3,7V → 12V) wird nicht mehr benötigt. Falls die gewählte Pumpe 5V
> benötigt, genügt ein kleiner 5V-Boost (z.B. MT3608, ~1€) statt des XL6009.
```

> *Der Bodenfeuchte-Sensor wird nur während der Messung mit Strom versorgt (GPIO33 HIGH),
> um elektrolytische Korrosion der Elektroden zu minimieren.
>
> **Board-Hinweis (Juni 2026):** Auf dem tatsächlich verbauten **LILYGO T3_V1.6.1**
> (siehe [Pinout](../../hardware/schematics/lilygo-t3/lilygo-t3-v1.6.1-pinout.png))
> ist **GPIO33 nicht herausgeführt**. Für den Irrigator-Knoten wird stattdessen
> **GPIO4** für die schaltbare Sensor-Stromversorgung verwendet (siehe
> `firmware/irrigator-node/`).

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
| Boost-Converter | ~~XL6009~~ **entfällt** | ~~2€~~ **0€** | Drip-Pumpe läuft direkt auf 3,7V — kein 12V-Boost nötig. Bei 5V-Pumpe: MT3608 (~1€) |
| MOSFET | IRLZ44N | ~1€ | Pumpensteuerung, Logic-Level-kompatibel mit 3,3V GPIO; Low-Side Switch |
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
| Pumpe | DC Tauchpumpe **1–3W, 3,7–5V, min. 3m Förderhöhe** | ~5–8€ | **PRIMÄR** | Drip-System: niedriger Durchfluss (10–50 L/h), hoher Druck reicht; kein Boost-Converter nötig → ADR-003, ADR-004 |
| Drip-Emitter | 0,5 L/h oder 1 L/h (je nach Pflanze) | ~0,50€/Stk. | **PRIMÄR** | Regulieren den Durchfluss pro Pflanze; keine Überwässerung |
| Drip-Schlauch | Mikro-Schlauch 4/6mm | ~2€ | **PRIMÄR** | Verteilt Wasser von Pumpe zu den Emittern |
| Verteiler | 1-auf-8 Drip-Verteiler | ~2€ | **PRIMÄR** | Verzweigung zu bis zu 8 Pflanzen (erweiterbar) |
| Ventil | 12V Magnetventil | ~12€ | Optional | Nur für Parzellen mit Vereins-Wasseranschluss; benötigt dann XL6009 zurück |

> **Tatsächlich verbaute Pumpe (Prototyp v0.2, Stand Juni 2026):** Abweichend von der
> ADR-004-Planung (3,7–5V Drip-Pumpe ohne Boost) wird aktuell eine
> **QWORK 12V Tauchwasserpumpe, 840 L/h, 5m max. Förderhöhe** verbaut (3 Adern:
> braun = +, blau = −, gelb = Gehäuseerdung, nicht angeschlossen). Damit ist
> **wieder ein Boost-Converter (3,7V→12V) nötig** — siehe ADR-003 und den
> Bring-up-Bericht oben. Gemessener Durchfluss bei 0,5m Förderhöhe: ~216 L/h.

> ⚠️ **Förderhöhe bleibt kritisch.** Auch bei Drip-Systemen muss die Pumpe Wasser über den
> Tonnenrand heben (~1–1,5m). Min. 3m Stillstandsförderhöhe erforderlich.
> **Vorteil Drip:** Bei 10–50 L/h Durchfluss wird die Kennlinie sehr flach — selbst kleine
> 1–3W Pumpen schaffen bei niedrigem Durchfluss 3–4m Förderhöhe (validiert: Gardena AquaBloom
> 1W / 3,6V / 4m / 10 L/h). Suchbegriff: „3V 5V Mini Submersible Pump 3m head drip".

---

## Energiebudget (Schätzung)

### Verbrauch Sensor-Node

> **Analyse Gardena AquaBloom:** Bei Drip-Systemen ist der **Controller-Standby (24h)**
> oft größer als die Pump-Energie. Gardena AquaBloom L: ~0,86 Wh/Tag Standby vs. ~0,75 Wh/Tag Pumpe.
> Für SmartGarden ist der Standby stark abhängig davon ob TTGO-Board oder Custom-PCB verwendet wird.

#### ⚠️ TTGO-Board vs. Custom PCB — kritischer Unterschied beim Standby

| Komponente | TTGO LoRa32 (Prototyp) | Custom PCB (Phase 3) |
|---|---|---|
| ESP32-Chip Deep Sleep | ~10 µA | ~10 µA |
| AMS1117 LDO (Quiescent) | ~5.000 µA ⚠️ | entfällt (effizienter LDO) |
| CP2104 USB-UART Chip | ~500 µA ⚠️ | entfällt |
| SX1276 Sleep Mode | ~200 nA | ~200 nA |
| **Board-Gesamt Deep Sleep** | **~5.500 µA ≈ 5,5 mA** | **~50 µA ≈ 0,05 mA** |

> Das TTGO-Dev-Board verbraucht im Deep Sleep ~5,5 mA statt der oft zitierten 0,01 mA —
> hauptsächlich durch den AMS1117-LDO und den USB-UART-Chip.
> Auf Custom PCB mit abgeschaltetem USB-Chip und effizientem LDO (z.B. MCP1700):
> echter Deep-Sleep-Strom ~50 µA. Faktor 100 Unterschied.

#### Verbrauch Sensor-Node

| Zustand | Strom (Akku-Ebene) | Dauer/Tag | TTGO Energie/Tag | Custom PCB |
|---|---|---|---|---|
| Deep Sleep | **5,5 mA** (TTGO) / **0,05 mA** (PCB) | ~22,5h | **~460 mWh** | **~4 mWh** |
| Messung + LoRa TX | 120 mA | ~24 min (96× alle 15min, je 15 Sek.) | ~180 mWh | ~180 mWh |
| Pumpe aktiv (1W Drip, 3,7V direkt) | ~270 mA | ~60 min | ~270 mWh | ~270 mWh |
| Pumpe aktiv — Worst Case (3W, 2h) | ~810 mA | 120 min | ~1.620 mWh | ~1.620 mWh |
| **Gesamt realistisch (1W Pumpe)** | | | **~910 mWh/Tag** | **~454 mWh/Tag** |
| **Gesamt Worst Case (3W, 2h)** | | | **~2.260 mWh/Tag** | **~1.870 mWh/Tag** |

### Solar-Ertrag & Saisonbetrachtung (Deutschland)

| | Sommer (TTGO) | Sommer (Custom PCB) | Winter (kein Pumpen) |
|---|---|---|---|
| Verbrauch/Tag | ~910 mWh | ~454 mWh | ~464 mWh (TTGO) / ~184 mWh (PCB) |
| Effektive Sonnenstunden | 4–6h | 4–6h | 0,5–1h |
| Ertrag 6W-Panel/Tag | ~24.000 mWh | ~24.000 mWh | ~3.000 mWh |
| Tagesbilanz | **+23.090 mWh** ✅ | **+23.546 mWh** ✅ | **+2.536–2.816 mWh** ✅ |
| **Autonomie ohne Sonne** (18.500 mWh) | **~20 Tage** | **~41 Tage** | **~40–100 Tage** |

> **TTGO-Prototyp vs. Custom PCB:** Der AMS1117-LDO auf dem TTGO-Board kostet ~460 mWh/Tag —
> das ist die dominante Verbrauchsquelle, größer als Pumpe (270 mWh) und LoRa TX (180 mWh) zusammen.
> Auf Custom PCB mit MCP1700-LDO (2 µA Quiescent): Standby sinkt auf ~4 mWh/Tag → Faktor 115 besser.
> **Erkenntnis (identisch wie Gardena):** Der Standby-Verbrauch dominiert das Energiebudget.

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

## PoC Verdrahtung — TTGO LoRa32 V2.1 (Inbetriebnahme v0.1)

Aktuelle Verdrahtung des laufenden Prototyps (Stand: Juni 2026):

> **Board-Variante:** Verwendetes Modul ist konkret ein **LILYGO T3_V1.6.1** (WiFi+Bluetooth LoRa Board,
> SX1276 868/915MHz). Vollständiges Pinout-Diagramm: [`hardware/schematics/lilygo-t3/lilygo-t3-v1.6.1-pinout.png`](../../hardware/schematics/lilygo-t3/lilygo-t3-v1.6.1-pinout.png).
> Pin-Belegung deckt sich mit den in diesem Dokument verwendeten GPIOs (12, 13, 14, 21, 22, 34, 35);
> LoRa-SPI intern: MOSI=27, SCLK=5, CS=18, MISO=19, RST=23, DIO0=26.

### Angeschlossene Komponenten

```
TTGO LoRa32 V2.1
│
├── OLED (onboard, I2C)
│   ├── SDA → GPIO 21
│   └── SCL → GPIO 22
│
├── HC-SR501 PIR Bewegungsmelder
│   ├── VCC → 5V (USB-Pin des TTGO!)  ⚠️ nicht 3.3V!
│   ├── GND → GND
│   ├── OUT → GPIO 13
│   ├── Jumper: L (Repeating Trigger)
│   ├── Poti links: Empfindlichkeit (~mittig)
│   └── Poti rechts: Haltezeit (~kurz, für Tests)
│
└── BME280 (geplant, noch nicht angeschlossen)
    ├── VCC → 3.3V
    ├── GND → GND
    ├── SDA → GPIO 21  ← gleicher I2C-Bus wie OLED (Adresse 0x76/0x77)
    └── SCL → GPIO 22  ← gleicher I2C-Bus wie OLED
```

### I2C Bus Belegung

| Gerät | Adresse | GPIO SDA | GPIO SCL |
|-------|---------|----------|----------|
| OLED SSD1306 | `0x3C` | 21 | 22 |
| BME280 | `0x76` oder `0x77` | 21 | 22 |

Kein Konflikt — I2C ist ein Bus, mehrere Geräte parallel möglich.

### Wichtige Hinweise HC-SR501

- **5V Versorgung:** HC-SR501 benötigt 5V (nicht 3.3V!) — am TTGO den 5V-Pin verwenden
- **Signal ist 3.3V-kompatibel:** OUT-Pin gibt 3.3V aus → direkt an ESP32 GPIO
- **30s Kalibrierungszeit** nach dem Einschalten — keine Fehlalarme in dieser Phase
- **Jumper L** (Repeating Trigger) für kontinuierliche Erkennung empfohlen
- **AM312** wäre die bessere Wahl für den Produktiveinsatz (3.3V, kleiner)

### Pin-Belegung aktueller PoC

| GPIO | Funktion | Angeschlossen |
|------|----------|---------------|
| 21 | I2C SDA | OLED + (BME280 geplant) |
| 22 | I2C SCL | OLED + (BME280 geplant) |
| 13 | PIR Signal | HC-SR501 OUT |
| 25 | Onboard LED | (intern) |
| 18 | LoRa NSS | (intern) |
| 23 | LoRa RST | (intern) |
| 26 | LoRa DIO0 | (intern) |
| 33 | LoRa DIO1 | (intern) |
| 32 | LoRa DIO2 | (intern) |

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

## Pumpen-Bring-up — Erkenntnisse (Juni 2026)

Erster Hardware-Bring-up des Pumpenkreises (LILYGO T3_V1.6.1 + IRLZ44N + Boost-Converter
+ QWORK 12V Tauchpumpe, 840 L/h, 3 Adern braun/blau/gelb).

### Ergebnis

| Komponente | Status | Erkenntnis |
|---|---|---|
| **Pumpe** | ✅ I.O. | Läuft an echten 12V (Labornetzteil) einwandfrei; Durchfluss bei 0,5m Förderhöhe gemessen: **~216 L/h** (4× 5s-Schübe = 1,2L) |
| **MOSFET-Schaltung** (IRLZ44N, 500Ω Gate-R, 10kΩ Pulldown, 1N4007 Freilauf) | ✅ I.O. | Schaltet zuverlässig über GPIO12 (LilyGo), auch bei 3,3V Gate-Pegel volle Durchsteuerung |
| **Firmware** (GPIO12 Toggle, `firmware/irrigator-node/`) | ✅ I.O. | Pumpe schaltet synchron zum Sketch |
| **Boost-Converter (XL6009-Modul #1)** | ❌ defekt | Vout = Vin unabhängig vom Trimmer, auch mit Last (LED) — Regler schaltet nicht |
| **Boost-Converter (XL6009-Modul #2)** | ⚠️ unterdimensioniert | Liefert 12V im Leerlauf, bricht aber unter Pumpenlast auf ~4V ein (Strombegrenzung erreicht, hörbares Spulenpfeifen) |
| **12V-Labornetzteil (Referenz)** | ✅ I.O. | Bricht unter Last nur auf ~11V ein (normaler Innenwiderstand) — bestätigt: Pumpe selbst ist nicht das Problem |

### Offener Punkt
Das aktuelle Boost-Modul muss **ersetzt werden** durch ein für höheren Dauerstrom
ausgelegtes Modul (z.B. **XL6019, 5A** — bereits in [ADR-003](ADR-003-Pumpenauswahl.md)
als Upgrade-Pfad genannt — oder ein explizit für "3A @ 12V" beworbenes Modul).
Erst danach ist der Akkubetrieb (statt Labornetzteil) sinnvoll testbar.

### Noch nicht gemessen
Durchfluss bei 1,0m und 1,5m Förderhöhe (Betriebspunkt laut ADR-003) — fehlender
passender Schlauch am Messtag. Förderhöhe/Durchfluss wird vorläufig als **I.O.**
eingestuft (216 L/h bei 0,5m ist weit über dem Drip-Bedarf von 10-50 L/h).

### Test: selbstansaugende Mini-Pumpe als Alternative zur Tauchpumpe
Im Zuge der in der [Wettbewerbsanalyse](../research/competitive-analysis.md) notierten
offenen Designfrage (Tauchpumpe vs. trocken verbaute Pumpe mit Ansaugschlauch, wie bei
AquaBloom/Onherm) wurde eine vorhandene **"Mini Wasserpumpe 3V" (Aquarium/Refugium)**
trocken aufgestellt und versucht, Wasser über einen Schlauch anzusaugen.

**Ergebnis: ❌ Pumpe kann nicht ansaugen.**

**Erklärung:** Diese günstigen DC-Mini-Pumpen sind **Kreiselpumpen (Zentrifugalpumpen)**
mit offenem Flügelrad — sie benötigen bereits Wasser im Pumpengehäuse, um einen
Förderdruck aufzubauen. Luft im Schlauch/Gehäuse verhindert das Ansaugen komplett
(kein Selbstansaug-Effekt). Die kommerziellen Trocken-Gehäuse-Systeme (AquaBloom, Onherm)
setzen vermutlich auf **Membran- oder Peristaltikpumpen**, die im Gegensatz zu
Kreiselpumpen selbstansaugend sind — das ist aufwändiger/teurer als eine einfache
Tauchpumpe.

**Vorläufiges Fazit:** Für SmartGarden v0.2 bleibt die **12V-Tauchpumpe die pragmatischste
Lösung** — sie braucht keine Selbstansaugfähigkeit, da sie direkt im Wasser sitzt.
Der Wechsel zu einer trockenen Pumpe wäre nur mit einer dedizierten Membran-/
Peristaltikpumpe sinnvoll, nicht mit einer einfachen Kreiselpumpe — das ist ein
größerer Hardware-Schritt und bleibt vorerst zurückgestellt.

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
- [x] ADR-003: Pumpenauswahl → min. 3m Förderhöhe, 1–3W Drip-Pumpe (nicht 10W/12V)
- [x] ADR-004: Bewässerungsart → Tropfbewässerung (Drip); XL6009 Boost-Converter entfällt

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
