# 🌱 SmartGarden

> Autarkes IoT-System für Schrebergärten — Solar-betrieben, LoRaWAN-vernetzt, mit Einbrucherkennung.

## Warum SmartGarden?

Bestehende Systeme wie MIYO benötigen WLAN und einen Stromanschluss — beides fehlt in den meisten Schrebergärten. SmartGarden ist von Grund auf für diese Umgebung gebaut.

## Alleinstellungsmerkmale

| Feature | MIYO LoRaWAN | SmartGarden |
|---|---|---|
| Solar / batterielos | ✅ | ✅ |
| Ohne WLAN nutzbar | ✅ | ✅ |
| Einbruchalarm per App | ❌ | ✅ |
| PIR-Bewegungserkennung | ❌ | ✅ |
| Zielgruppe Schrebergarten | teilweise | direkt |
| Vereins-Gateway (geteilt) | ❌ | ✅ |

## Systemübersicht

```
[Solar + LiOn]
      |
[ESP32 + LoRa-Modul]
      |--- [Temp/Feuchte-Sensor]
      |--- [Bodenfeuchtesensor]
      |--- [PIR-Bewegungsmelder]
      |--- [12V Pumpe / Ventil]
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
