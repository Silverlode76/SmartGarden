# Schematics

## v0.0 — Erster Prototyp (Fritzing / Lochraster)

**Datei:** `v0.0-prototype-fritzing.png`  
*(Bitte den Fritzing-Screenshot hier ablegen)*

### Komponenten des Prototyps

| Komponente | Modell | Anmerkung |
|---|---|---|
| MCU | STM32 (Nano-Formfaktor) | Debug-UART: PB10 (TX) / PB11 (RX) |
| LoRa | SX1276 Breakout-Modul | 868 MHz, eigenes Protokoll |
| Sensor | BME280 (GY-BME/P280) | I2C |
| Solar | 2× Solarpanel | ~6V in Reihe/parallel |
| Laderegler | MPPT-Modul (grün) | 6V → 7V |
| Akku-Lader | TP4056 (blau, mit USB) | |
| Akku | 18650 3.7V (~2500mAh) | Fritzing-Label "110mAh" war Platzhalter — realer Akku war 18650 |

### Erkenntnisse aus v0.0

- ✅ STM32 + SX1276 auf 868 MHz grundsätzlich funktionsfähig
- ✅ BME280 liefert gute Messwerte
- ⚠️ SX1276 Breakout auf Lochraster: erheblicher Lötaufwand
- ❌ Schwachlicht-Problem: Node wurde nicht ausreichend geladen (Akku war 18650 — nicht die Ursache!)
- ❌ Ursache: TP4056 braucht min. ~4,5V, kleine Panels fallen bei Bewölkung darunter → Ladestopp
- ❌ Kein MPPT: schlechter Wirkungsgrad bei diffusem Licht
- ⚠️ Eigenes LoRa-Protokoll: nicht kompatibel mit TTN/Vereins-Gateway

→ Für v0.1: Wechsel auf TTGO LoRa32 (ESP32 + SX1276 integriert), LoRaWAN, 2× 18650

## v0.1 — Erster offizieller Prototyp (KiCad)

*In Planung*
