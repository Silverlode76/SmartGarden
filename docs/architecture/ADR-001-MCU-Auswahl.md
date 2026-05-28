# ADR-001: MCU & LoRa Modul Auswahl

**Status:** Entschieden  
**Datum:** 2026-05-24  
**Autor:** Oliver Schmoll

---

## Kontext

Für den SmartGarden Sensor-Node wird ein Mikrocontroller benötigt, der:
- Sensoren ausliest (Temperatur, Feuchte, Boden)
- Eine Pumpe / ein Ventil steuert
- Per LoRa (868 MHz, EU) kommuniziert
- Im Deep-Sleep energieeffizient ist
- Solar-betrieben über längere Zeit autonom läuft

### Prototyp-Erfahrung (v0.0)

> **Hinweis:** Diese Beschreibung wurde ursprünglich aus dem Gedächtnis geschrieben.  
> Die exakte Stückliste (Fritzing-Export) ist dokumentiert in:  
> [`hardware/bom/v0.0-stm32-prototype-bom.md`](../../hardware/bom/v0.0-stm32-prototype-bom.md)  
> Dort sind auch die Abweichungen zu dieser ursprünglichen Beschreibung aufgeführt.

Ein erster Prototyp wurde gebaut mit:
- **MCU:** STM32F103C8T6 „Blue Pill" Board
- **LoRa:** SX1278 Breakout-Modul (**433 MHz** — nicht 868 MHz!)
- **Sensor:** GY-BME280 (Temperatur, Luftfeuchte, Luftdruck) via I2C
- **Energie:** 2× Solarzelle (1,5V / 1mA) → TP4056 → LiPo 110mAh 3,7V + Mini-360 Buck
- **Protokoll:** Eigenes LoRa-Protokoll auf 433 MHz (kein LoRaWAN, kein EU868)
- **Design:** Fritzing-Aufbau auf Lochrasterplatine

**Ergebnis Prototyp:**
- ✅ STM32 + SX1278 grundsätzlich funktionsfähig
- ✅ BME280 liefert gute Messwerte (besser als DHT22: zusätzlich Luftdruck, genauer)
- ⚠️ SX1278 arbeitet auf 433 MHz — **kein EU868 LoRaWAN möglich** mit diesem Chip
- ⚠️ Erheblicher Lötaufwand beim SX1278 Breakout auf Lochraster
- ⚠️ STM32 Toolchain aufwändig (STM32CubeIDE, HAL, manuelle SPI-Konfiguration)
- ⚠️ Eigenes Protokoll skaliert nicht — kein TTN, kein gemeinsamer Vereins-Gateway möglich
- ❌ Schwachlicht-Problem: Solarzellen 2× 1,5V/1mA = **3mW total** — zu schwach für TP4056
     → TP4056 benötigt min. ~4,5V Eingang, 3V reichen nicht
     → Kein MPPT → schlechter Wirkungsgrad bei diffusem Licht
     → LiPo 110mAh → ~22h Autonomie ohne Solar (viel zu wenig für Outdoor)

---

## Entscheidung

### MCU: Wechsel zu ESP32 für v0.1+

| Kriterium | STM32 (Prototyp) | ESP32 (Heltec / TTGO) | Gewichtung |
|---|---|---|---|
| Prototyp-Erfahrung | ✅ vorhanden | ❌ neu | mittel |
| Toolchain-Aufwand | ❌ hoch | ✅ Arduino/PlatformIO | hoch |
| LoRa integriert | ❌ separates Modul + Löten | ✅ onboard SX1276 | hoch |
| Deep Sleep | ✅ ~1 µA | ✅ ~10 µA (ausreichend) | niedrig* |
| OTA-Updates | ⚠️ aufwändig | ✅ built-in WiFi+OTA | hoch |
| Community & Libs | ⚠️ professionell, weniger Maker | ✅ riesig, viele Beispiele | mittel |
| Entwicklungsgeschwindigkeit | ❌ langsam | ✅ schnell | hoch |

*\*Energiebudget zeigt 105 Tage Autonomie mit Solar — Deep-Sleep-Unterschied irrelevant*

**Empfehlung:** TTGO LoRa32 V2.1 (~15€) oder Heltec WiFi LoRa 32 V3 (~18€)  
Beide haben **ESP32 + SX1276 onboard** — kein manuelles Löten des LoRa-Moduls nötig.

### LoRa-Protokoll: Wechsel zu LoRaWAN

| | Eigenes Protokoll (Prototyp) | LoRaWAN (TTN) |
|---|---|---|
| Gateway nötig | Eigener Gateway zwingend | TTN-Community-Gateways nutzbar |
| Vereins-Gateway | ❌ schwer umsetzbar | ✅ Standard, einfach teilbar |
| Reichweite / SF | Manuell verwalten | Automatisch (ADR) |
| Sicherheit | Selbst implementieren | AES-128 built-in |
| Skalierung | ❌ | ✅ |

**Empfehlung:** LoRaWAN über The Things Network (TTN) — kostenlos, EU868, direkt auf Vereins-Gateway-Anforderung ausgelegt.

### Sensor: BME280 statt DHT22

Der Prototyp hat gezeigt, dass der **BME280** dem DHT22 überlegen ist:

| | DHT22 | BME280 |
|---|---|---|
| Interface | 1-Wire | I2C / SPI |
| Messgrößen | Temp + Feuchte | Temp + Feuchte + **Luftdruck** |
| Genauigkeit Temp | ±0.5°C | ±0.5°C |
| Genauigkeit Feuchte | ±2–5% | ±3% |
| Preis | ~3€ | ~3–5€ |
| Fazit | — | ✅ bevorzugt |

**Entscheidung:** BME280 als Standard-Sensor in allen Nodes.

### Akku & Ladekette: Komplettes Redesign

Der Prototyp zeigte zwei kombinierte Probleme beim Laden bei Schwachlicht:

**Batterie im Prototyp:** 1× 18650 (~2500mAh) → ~44 Tage Autonomie ohne Sonne.
Die Kapazität war **nicht** die Ursache des Schwachlicht-Problems.

**Eigentliche Ursache — Ladekette:**
- TP4056 benötigt min. ~4,5V Eingang → kleine Panels fallen bei Bewölkung darunter → kein Laden
- Kein MPPT → schlechter Wirkungsgrad bei diffusem Licht
- Solarpanels im Prototyp zu klein dimensioniert

**Lösung v0.1:**
- CN3791 MPPT-Laderegler (startet ab ~4V, maximiert Ladeleistung auch bei Schwachlicht)
- 6W 6V Solarpanel (ausreichend Strom auch bei Bewölkung)
- Optional: 2× 18650 parallel (~5000mAh) für mehr Sicherheitspuffer (~88 Tage)

```
[6W 6V Solarpanel]
       |
  [CN3791 MPPT]  ← effizienter Schwachlichtbetrieb
       |
[2× 18650 parallel]  ← 6000mAh Puffer
       |
  [DW01 Schutz]
       |
  [MT3608 3.3V]
       |
   [ESP32 Node]
```

---

## Konsequenzen

- `hardware-overview.md` wird auf ESP32 + BME280 aktualisiert
- Firmware-Basis: PlatformIO + Arduino Framework + MCCI LoRaWAN LMIC Library
- SX1276 wird nicht mehr separat gelötet — Breakout-Board via TTGO/Heltec
- Eigenes Protokoll wird nicht weiterentwickelt
- Prototyp-Schaltplan (Fritzing) wird in `hardware/schematics/` archiviert als `v0.0-prototype-fritzing.png`

---

## Verworfene Alternativen

- **STM32 + separater SX1276:** Funktioniert (belegt), aber Lötaufwand und Toolchain-Komplexität sprechen dagegen für Rapid Prototyping
- **STM32 + eigenes Protokoll:** Skaliert nicht, kein TTN/Vereins-Gateway möglich
- **Arduino Uno/Nano:** Zu wenig RAM/Flash, kein Deep Sleep
