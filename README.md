# 🌱 SmartGarden

> **Off-Grid Smart Irrigation** — Solar-betrieben, LoRaWAN-vernetzt, mit Einbrucherkennung.
> Für jeden Ort ohne Strom, ohne WLAN und ohne Wasseranschluss.

## Warum SmartGarden?

Schrebergärten, Wochenendhütten, Jagdhütten, Ferienhäuser — überall dort wo es **Off-Grid** zugeht:

- ❌ Kein Stromanschluss
- ❌ Kein WLAN
- ❌ Kein Wasseranschluss mit Druck
- ✅ Wasserfässer / Regentonnen

Bestehende Systeme versagen hier:
- **MIYO** braucht einen Wasserhahn mit Druck — funktioniert am Fass nicht
- **Gardena AquaBloom** hat eine Pumpe, aber keine App, keine Sensorik, kein Alarm

SmartGarden ist das erste **Off-Grid Smart Irrigation System**: Eine 12V Tauchpumpe fördert Wasser aktiv aus dem Fass, LoRaWAN überträgt Daten ohne WLAN, Solar versorgt alles — und die App informiert dich über Bewässerung und Einbruch, egal wo du bist.

## Alleinstellungsmerkmale

| Feature | MIYO LoRaWAN | SmartGarden |
|---|---|---|
| Solar / batterielos | ✅ | ✅ |
| Ohne WLAN nutzbar | ✅ | ✅ |
| **Pumpe für Regentonne / Fass** | ❌ | ✅ |
| Einbruchalarm per App | ❌ | ✅ |
| PIR-Bewegungserkennung | ❌ | ✅ |
| Zielgruppe Schrebergarten | teilweise | direkt |
| Vereins-Gateway (geteilt) | ❌ | ✅ |

## Systemübersicht

```
[Wasserfass / Regentonne]
      |
[12V Tauchpumpe]  <-- fördert Wasser aktiv, kein Wasseranschluss nötig
      |
      +---------------------------+
      |                           |
[Solar + 2×18650]                 |
      |                           |
[ESP32 + LoRa-Modul]              |
      |--- [BME280 Temp/Feuchte/Druck]
      |--- [Bodenfeuchtesensor kapazitiv]
      |--- [AM312 PIR-Bewegungsmelder]
      |--- [SW-420 Erschütterung]
      |--- [Pumpensteuerung MOSFET] ---+
      |
[LoRaWAN Gateway]  <-- gemeinsam im Verein nutzbar
      |
[Cloud Backend]
      |
[Smartphone App]  <-- Bewässerung steuern + Alarm empfangen
```

## Projektstruktur

```
SmartGarden/
├── docs/
│   ├── architecture/     # HW-Blockdiagramme, Energiebudget, Pinbelegung
│   ├── requirements/     # User Stories, MoSCoW-Priorisierung
│   └── research/         # Wettbewerbsanalyse, Marktrecherche
├── hardware/
│   ├── bom/              # Bill of Materials (Stückliste)
│   ├── schematics/       # Schaltpläne (KiCad)
│   └── enclosure/        # Gehäuse-Design
├── firmware/
│   ├── sensor-node/      # ESP32: Sensoren + LoRa
│   ├── security-node/    # ESP32: PIR + Alarm
│   └── shared/           # Gemeinsame Libraries
├── backend/
│   ├── api/              # REST API / MQTT Bridge
│   └── rules-engine/     # Bewässerungslogik, Alarmregeln
└── app/
    └── mobile/           # Flutter App (iOS + Android)
```

## Roadmap

- [ ] `v0.1` — Proof of Concept: Sensor → LoRa → Gateway → App
- [ ] `v0.2` — Prototype: Bewässerung + Alarm integriert
- [ ] `v0.3` — Feldtest: 3 Schrebergärten im Pilotbetrieb
- [ ] `v1.0` — MVP: App Store Release + erstes Produktionslos

## KI-Agenten

Dieses Projekt wird mit spezialisierten KI-Agenten entwickelt:

- **Architekt-Agent** — Hardware, Komponenten, Energiebudget
- **Requirements-Agent** — User Stories, Akzeptanzkriterien, MoSCoW

System Prompts: siehe [`docs/requirements/agent-prompts.md`](docs/requirements/agent-prompts.md)

## Lizenz

MIT
