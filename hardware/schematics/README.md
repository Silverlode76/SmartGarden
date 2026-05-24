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
| Akku | 6SP 061225 LiPo 110mAh 3.7V | Zu klein für Produktion → 18650 geplant |

### Erkenntnisse aus v0.0

- ✅ STM32 + SX1276 auf 868 MHz grundsätzlich funktionsfähig
- ✅ BME280 liefert gute Messwerte
- ⚠️ SX1276 Breakout auf Lochraster: erheblicher Lötaufwand
- ⚠️ 110mAh LiPo: zu gering für autonomen Betrieb
- ⚠️ Eigenes LoRa-Protokoll: nicht kompatibel mit TTN/Vereins-Gateway

→ Für v0.1: Wechsel auf TTGO LoRa32 (ESP32 + SX1276 integriert), LoRaWAN, 2× 18650

## v0.1 — Erster offizieller Prototyp (KiCad)

*In Planung*
