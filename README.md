# 🌱 SmartGarden

> **Off-Grid Smart Gardening Ökosystem** — Solar-betrieben, LoRaWAN-vernetzt.  
> Zwei spezialisierte Produkte: **Irrigator** (automatisch bewässern) + **Guard** (Laube überwachen).  
> Für jeden Ort ohne Strom, ohne WLAN und ohne Wasseranschluss.

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![KiCad](https://img.shields.io/badge/Hardware-KiCad%2010-blue.svg)](hardware/schematics/sensor-node/)
[![Platform](https://img.shields.io/badge/Platform-ESP32%20%2B%20LoRaWAN-orange.svg)]()
[![Status](https://img.shields.io/badge/Status-Prototyp%20v0.1-yellow.svg)]()

---

## Das Problem

Schrebergärten, Wochenendhütten, Jagdhütten, Ferienhäuser — überall dort wo es **Off-Grid** zugeht:

- ❌ Kein Stromanschluss
- ❌ Kein WLAN
- ❌ Kein Wasseranschluss mit Druck
- ✅ Wasserfässer / Regentonnen vorhanden

Bestehende Systeme scheitern hier:
- **MIYO** — braucht Wasserhahn mit Druck, funktioniert am Fass nicht
- **Gardena AquaBloom** — hat eine Pumpe, aber keine App, keine Sensorik, kein Alarm

## Die Lösung — Zwei spezialisierte Produkte

Ein Node für alles macht physisch keinen Sinn: Bewässerung ist am Wasserfass,
Alarm ist an der Laube — in entgegengesetzten Richtungen.
SmartGarden setzt auf **spezialisierte Nodes** in einer gemeinsamen App.

### 🌱 SmartGarden Irrigator — *für den Gärtner* (~65€)

```
[Wasserfass / Regentonne]
      |
[12V Tauchpumpe]  ← fördert Wasser aktiv, kein Wasserhahn nötig
      |
[6W Solar + 2×18650]  ← autark
      |
[TTGO LoRa32]  ← sendet ohne WLAN
      |--- [Bodenfeuchte kapazitiv] → Bewässerungsentscheidung
      |
[LoRaWAN Gateway TTN] → [App]  ← Bewässerung steuern
```

### 🔒 SmartGarden Guard — *für den Laubenbesitzer* (~45€)

```
[Gartenlaube / Tür / Fenster]
      |
[6W Solar + 2×18650]  ← autark, Jahre ohne Wartung
      |
[TTGO LoRa32]  ← sendet ohne WLAN
      |--- [AM312 PIR] → Bewegungserkennung
      |--- [SW-420]    → Erschütterungsalarm
      |--- [BME280]    → Temperatur / Feuchte
      |
[LoRaWAN Gateway TTN] → [App]  ← sofortiger Push-Alarm
```

### 🌱🔒 SmartGarden Bundle — *Irrigator + Guard* (~100€)

---

## Alleinstellungsmerkmale

| Feature | MIYO LoRaWAN | Gardena AquaBloom | **Irrigator** | **Guard** |
|---|---|---|---|---|
| Pumpe (kein Wasserhahn nötig) | ❌ | ✅ | ✅ | — |
| Solar / kein Strom | ✅ | ✅ | ✅ | ✅ |
| App / Fernzugriff | ✅ | ❌ | ✅ | ✅ |
| Bodenfeuchtesensor | ✅ | ❌ | ✅ | — |
| Ohne WLAN (LoRaWAN) | ✅ | ❌ | ✅ | ✅ |
| Einbruchalarm / PIR | ❌ | ❌ | — | ✅ |
| Vereins-Gateway | ❌ | ❌ | ✅ | ✅ |
| Open Source | ❌ | ❌ | ✅ | ✅ |
| **Preis** | ~250€ | ~80€ | **~65€** | **~45€** |

---

## Hardware-Architektur

### Technologie-Stack

| Komponente | Entscheidung | Begründung |
|---|---|---|
| **MCU + LoRa** | TTGO LoRa32 V2.1 (ESP32 + SX1276) | Integriert, kein manuelles LoRa-Löten |
| **Protokoll** | LoRaWAN 868 MHz, TTN, SF9 | Skalierbar, Vereins-Gateway möglich |
| **Laden** | CN3791 MPPT | Startet ab 4V, auch bei Bewölkung |
| **Akku** | 2× 18650 parallel (~5000mAh) | Kein HW-Aufwand, doppelter Puffer |
| **Boost** | XL6009 (3,7V → 12V) | Pumpenversorgung bei Bedarf |
| **Temp/Feuchte** | BME280 (I2C) | Bewährt, zusätzlich Luftdruck |
| **PIR** | AM312 (3,3V) | Direkt 3,3V-kompatibel |

### Schaltplan (KiCad 10)

Der Sensor-Node Schaltplan ist in 5 Blöcke aufgeteilt:

| Block | Funktion | Schlüsselbauteile |
|---|---|---|
| **A** | Ladekreis | CN3791 MPPT + SS14 + R_MPPT/R_PROG |
| **B** | Akkuschutz | DW01A + FS8205A (OVP/UVP/OCP) |
| **C** | Mikrocontroller | TTGO LoRa32 + VBAT-Spannungsteiler |
| **D** | Sensorik | BME280, Bodenfeuchte, AM312, SW-420 |
| **E** | Pumpensteuerung | XL6009 + IRLZ44N + 1N4007 |

📁 Schaltplan: [`hardware/schematics/sensor-node/`](hardware/schematics/sensor-node/)

### Energiebudget

| Saison | Verbrauch/Tag | Solar-Ertrag/Tag | Autonomie (ohne Sonne) |
|---|---|---|---|
| **Sommer** (mit Pumpe) | ~210 mWh | ~24.000 mWh | **~88 Tage** |
| **Winter** (nur Alarm) | ~60 mWh | ~3.000 mWh | **~308 Tage** |

---

## Projektstruktur

```
SmartGarden/
├── docs/
│   ├── architecture/
│   │   ├── hardware-overview.md      # Vollständige HW-Dokumentation
│   │   └── ADR-001-MCU-Auswahl.md   # Architecture Decision Record
│   ├── product/
│   │   └── product-strategy.md       # Portfolio: Irrigator + Guard + Bundle
│   ├── requirements/
│   │   └── agent-prompts.md          # KI-Agenten System Prompts
│   └── research/
│       └── competitive-analysis.md   # Wettbewerbsanalyse
├── hardware/
│   ├── bom/
│   │   └── sensor-node-bom.csv       # Stückliste ~50€/Node
│   ├── schematics/
│   │   └── sensor-node/              # KiCad 10 Schaltplan (ERC: 0 Errors)
│   ├── assembly/
│   │   └── sensor-node-aufbauanleitung.md  # Schritt-für-Schritt Löt-Guide
│   └── enclosure/                    # Gehäuse-Design (geplant)
├── firmware/
│   ├── sensor-node/                  # ESP32: Sensoren + LoRa (geplant)
│   └── shared/                       # Gemeinsame Libraries (geplant)
├── backend/                          # REST API + Regelwerk (geplant)
└── app/
    └── mobile/                       # Flutter App iOS + Android (geplant)
```

---

## Prototyp-Erkenntnisse (v0.0)

> Der erste Prototyp (STM32 + SX1276 auf Lochraster) lieferte wertvolle Erkenntnisse:

- **TP4056 → CN3791:** TP4056 versagte bei Bewölkung (min. 4,5V, kein MPPT). CN3791 lädt schon ab 4V mit echtem MPPT.
- **STM32 → ESP32:** TTGO LoRa32 eliminiert manuelles SX1276-Löten, vereinfacht Toolchain erheblich.
- **1× → 2× 18650 parallel:** Kein Hardware-Aufwand (gleiche Spannung), doppelte Kapazität als Sicherheitspuffer.

Details: [`docs/architecture/ADR-001-MCU-Auswahl.md`](docs/architecture/ADR-001-MCU-Auswahl.md)

---

## Roadmap

- [x] `v0.0` — Prototyp: STM32 + SX1276, proprietäres Protokoll *(Erkenntnisse dokumentiert)*
- [x] `v0.1` — Schaltplan: KiCad Sensor-Node, BOM, Aufbauanleitung
- [ ] `v0.2` — **Irrigator** Prototyp: Pumpe + Bodenfeuchte + LoRaWAN + Deep Sleep
- [ ] `v0.3` — **Guard** Prototyp: PIR + Vibration + Push-Alarm + LoRaWAN
- [ ] `v0.4` — App MVP: beide Nodes in einer Flutter UI
- [ ] `v0.5` — Feldtest: 3 Schrebergärten (Irrigator + Guard im Pilotbetrieb)
- [ ] `v1.0` — Markteinführung: Bundle + App Store + Amazon FBA

---

## KI-gestützte Entwicklung

Dieses Projekt wird mit spezialisierten KI-Agenten entwickelt:

- **Architekt-Agent** — Hardware, Komponenten, Energiebudget, ADRs
- **Requirements-Agent** — User Stories, Akzeptanzkriterien, MoSCoW
- **Business-PM-Agent** — ROI, Marktanalyse, Pricing, Go-to-Market

System Prompts: [`docs/requirements/agent-prompts.md`](docs/requirements/agent-prompts.md)  
Produktstrategie: [`docs/product/product-strategy.md`](docs/product/product-strategy.md)

---

## Lizenz

MIT — siehe [LICENSE](LICENSE)
