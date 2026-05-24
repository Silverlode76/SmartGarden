# Hardware Architektur

## Systemübersicht

### Node-Typen

Das SmartGarden-System besteht aus zwei Node-Typen die kombiniert werden:

```
┌─────────────────────────────────┐
│         SENSOR-NODE             │
│                                 │
│  [DHT22]──┐                     │
│  [Boden] ─┤─[ESP32+LoRa]──────► LoRaWAN
│  [Solar] ─┤    │                │
│  [LiOn]  ─┘    └──[Pumpe/Ventil]│
└─────────────────────────────────┘

┌─────────────────────────────────┐
│        SECURITY-NODE            │
│                                 │
│  [PIR]────┐                     │
│  [Erschüt]┤─[ESP32+LoRa]──────► LoRaWAN
│  [Solar] ─┤                     │
│  [LiOn]  ─┘                     │
└─────────────────────────────────┘
```

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
| Temp/Feuchte/Druck | **BME280** | ~4€ | Im Prototyp bewährt; präziser als DHT22, zusätzlich Luftdruck, I2C |
| ~~Temp/Feuchte~~ | ~~DHT22~~ | ~~3€~~ | ~~Ersetzt durch BME280~~ |
| Bodenfeuchte | Capacitive Sensor v1.2 | ~4€ | Kein Rost (resistive vermeiden!) |
| Bewegung | HC-SR501 PIR | ~2€ | Einstellbare Empfindlichkeit |
| Erschütterung | SW-420 | ~1€ | Für Türen/Fenster der Laube |

### Energieversorgung
| Komponente | Modell | Preis | Begründung |
|---|---|---|---|
| Solarpanel | 5W 12V Mono | ~12€ | Ausreichend für kontinuierlichen Betrieb |
| LiOn Akku | 18650 x2 (6000mAh) | ~10€ | 7+ Tage Autonomie |
| Laderegler | TP4056 + DW01 | ~2€ | Überlastschutz integriert |
| Step-Up | MT3608 3.3V | ~1€ | Stabile Versorgung MCU |
| MOSFET | IRLZ44N | ~1€ | Pumpensteuerung |

### Aktorik
| Komponente | Modell | Preis | Begründung |
|---|---|---|---|
| Pumpe | 12V Tauchpumpe 3W | ~8€ | Für Fass/Zisterne |
| Ventil | 12V Magnetventil | ~12€ | Für Wasseranschluss |

---

## Energiebudget (Schätzung)

### Verbrauch Sensor-Node
| Zustand | Strom | Dauer/Tag | Energie/Tag |
|---|---|---|---|
| Deep Sleep | 0.01 mA | 23.5h | 0.24 mWh |
| Messung + LoRa TX | 120 mA | 30 min | 60 mWh |
| Pumpe aktiv | 300 mA | 30 min | 150 mWh |
| **Gesamt** | | | **~210 mWh/Tag** |

### Solar-Ertrag (Deutschland, bewölkt)
- Solarpanel 5W, 2h effektive Sonnenstunden (bewölkt): ~10 Wh = 10.000 mWh
- Akku 6000 mAh @ 3.7V = 22.200 mWh
- **Autonomie ohne Sonne**: 22.200 / 210 = ~105 Tage ✅

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

- [x] ADR-001: MCU-Auswahl → ESP32 (TTGO/Heltec) statt STM32 — siehe [ADR-001](ADR-001-MCU-Auswahl.md)
- [x] ADR-002: Kommunikationsprotokoll → LoRaWAN (TTN) statt proprietär — siehe [ADR-001](ADR-001-MCU-Auswahl.md)
- [ ] Prototyp v0.0 Fritzing-Schaltplan archivieren in `hardware/schematics/v0.0-prototype-fritzing.png`
- [ ] Schaltplan v0.1 in KiCad (ESP32 + BME280 + Bodenfeuchte + Pumpe)
- [ ] Energiebudget mit Messungen am echten Node validieren
- [ ] Gehäuse-Konzept (IP65 Schutzklasse)
