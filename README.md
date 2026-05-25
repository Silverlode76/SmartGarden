# 🌱 SmartGarden

> **Off-Grid Smart Irrigation** — Solar-betrieben, LoRaWAN-vernetzt, mit Einbrucherkennung.  
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

## Die Lösung

SmartGarden ist das erste **Off-Grid Smart Irrigation System**:

```
[Wasserfass / Regentonne]
      |
[12V Tauchpumpe]  ← fördert Wasser aktiv, kein Wasserhahn nötig
      |
[6W Solar + 2×18650]  ← autark, kein Strom nötig
      |
[ESP32 + SX1276 LoRa]  ← sendet ohne WLAN
      |--- [BME280] Temperatur / Feuchte / Luftdruck
      |--- [Bodenfeuchte kapazitiv] Bewässerungsentscheidung
      |--- [AM312 PIR] Einbrucherkennung
      |--- [SW-420] Erschütterungsalarm
      |
[LoRaWAN Gateway TTN]  ← gemeinsam im Kleingartenverein nutzbar
      |
[Cloud Backend]
      |
[Smartphone App]  ← Bewässerung steuern + Alarm empfangen
```

---

## Alleinstellungsmerkmale

| Feature | MIYO LoRaWAN | Gardena AquaBloom | **SmartGarden** |
|---|---|---|---|
| Pumpe (kein Wasserhahn nötig) | ❌ | ✅ | ✅ |
| Solar / kein Strom | ✅ | ✅ | ✅ |
| App / Fernzugriff | ✅ | ❌ | ✅ |
| Bodenfeuchtesensor | ✅ | ❌ | ✅ |
| Ohne WLAN (LoRaWAN) | ✅ | ❌ | ✅ |
| Einbruchalarm / PIR | ❌ | ❌ | ✅ |
| Vereins-Gateway | ❌ | ❌ | ✅ |
| Open Source | ❌ | ❌ | ✅ |

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
- [ ] `v0.2` — Firmware: PlatformIO + LoRaWAN (MCCI LMIC) + Deep Sleep
- [ ] `v0.3` — Feldtest: 3 Schrebergärten im Pilotbetrieb
- [ ] `v1.0` — MVP: App Store Release + erstes Produktionslos

---

## KI-gestützte Entwicklung

Dieses Projekt wird mit spezialisierten KI-Agenten entwickelt:

- **Architekt-Agent** — Hardware, Komponenten, Energiebudget, ADRs
- **Requirements-Agent** — User Stories, Akzeptanzkriterien, MoSCoW

System Prompts: [`docs/requirements/agent-prompts.md`](docs/requirements/agent-prompts.md)

---

## Lizenz

MIT — siehe [LICENSE](LICENSE)
