# Setup Guide — Sensor Node (TTGO LoRa32 V2.1)

Inbetriebnahme des SmartGarden Sensor Node mit Arduino IDE und TTN.

## Voraussetzungen

- Arduino IDE 2.x
- ESP32 Board-Support: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
- Board: **Tools → Board → ESP32 Arduino → TTGO LoRa32-OLED**
- Libraries (über Library Manager):
  - `MCCI LoRaWAN LMIC library` ≥ 4.1.1
  - `Adafruit SSD1306` ≥ 2.5.7
  - `Adafruit GFX Library` ≥ 1.11.9

## LMIC konfigurieren

Die Datei `lmic_project_config.h` muss in den Library-Ordner kopiert werden:

```
C:\Users\<user>\Documents\Arduino\libraries\MCCI_LoRaWAN_LMIC_library\project_config\lmic_project_config.h
```

Inhalt:
```cpp
#define CFG_eu868 1
#define CFG_sx1276_radio 1
#define LMIC_USE_INTERRUPTS
```

Ohne diese Datei kompiliert der Code nicht (fehlende Symbole wie `DR_SF12`, `BAND_CENTI`).

## TTN Device anlegen

1. Account auf [console.thethingsnetwork.org](https://console.thethingsnetwork.org) — Region **eu1**
2. Application anlegen (z.B. `smartgardenollie`)
3. End Device registrieren:
   - Frequency plan: **Europe 863-870 MHz (SF9 for RX2)**
   - LoRaWAN version: **1.0.3**
   - Regional Parameters: **RP001 Regional Parameters 1.0.3 revision A**
   - Activation: **OTAA**
   - JoinEUI: `0000000000000000`
   - DevEUI + AppKey: **Generate**

## secrets.h anlegen

`secrets.h` aus `secrets.example.h` kopieren und TTN-Keys eintragen.

**Wichtig: Byte-Reihenfolge**

| Key     | TTN-Format | secrets.h  |
|---------|-----------|------------|
| DevEUI  | MSB       | **LSB** (umkehren) |
| AppEUI  | MSB       | **LSB** (umkehren) |
| AppKey  | MSB       | MSB (so lassen) |

Beispiel für DevEUI `70 B3 D5 7E D0 07 7C 23`:
```cpp
static const u1_t PROGMEM DEVEUI[8] = { 0x23, 0x7C, 0x07, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
```

## Bekannte Probleme

### OLED_RST Pin 16 → Watchdog Reset
Pin 16 ist auf dem TTGO LoRa32 V2.1 intern belegt. `OLED_RST` muss `-1` sein:
```cpp
#define OLED_RST  -1
```

### Duty Cycle beim Join
EU868 limitiert Sendehäufigkeit auf 1%. Nach mehreren Join-Versuchen muss das Board bis zu 5 Minuten warten. Nicht resetten — warten bis `EV_JOINED` erscheint.

## Downlink-Kommandos

Via TTN Console → Messaging → Schedule downlink (FPort 1, Payload type: Bytes):

| Payload | Aktion       |
|---------|-------------|
| `00`    | LED aus     |
| `01`    | LED an      |
| `02`    | LED toggeln |

Downlink wird beim nächsten Uplink-Zyklus (alle 60s) zugestellt. **Confirmed downlink deaktivieren** — spart Duty Cycle.

## Serielle Ausgabe (115200 Baud)

```
=== SmartGarden PoC v0.1 ===
[LMIC] Starte OTAA Join...
[TTN] Verbinde mit TTN (OTAA)...
[TTN] *** OTAA Join erfolgreich! ***
[TX] Uplink #1 — LED=OFF
[TTN] TX abgeschlossen (RXRSSI=-49)
[RX] Downlink empfangen: 1 Byte(s) auf Port 1
[CMD] LED → TOGGLE → AN
```
