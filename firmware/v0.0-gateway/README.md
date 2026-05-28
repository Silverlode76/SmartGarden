# SmartGarden v0.0 — Gateway Firmware (Archiv)

**Hardware:** STM32F103C8T6 „Blue Pill" + SX1278 + ESP8266  
**Tool:** STM32CubeIDE 1.16.1  
**Datum:** 2024/2025  
**Status:** Archiviert — abgelöst durch TTGO LoRa32 + TTN (v0.1)

---

## Dateien

| Datei | Beschreibung |
|-------|-------------|
| `main.c` | Gateway Firmware (STM32 HAL, Credentials entfernt) |

Fritzing-Schaltplan und PCB-Dateien:  
→ [`../../hardware/schematics/v0.0-gateway/`](../../hardware/schematics/v0.0-gateway/)

---

## Was diese Firmware macht

```
STM32 USART3 (PB10/PB11, 115200 Baud)
        │
        ▼
ESP8266 AT-Kommandos:
  1. AT              → Verbindungstest
  2. AT+CWMODE=1     → Station Mode (WiFi-Client)
  3. AT+CWJAP=...    → Mit Home-WiFi verbinden
  4. AT+CIFSR        → IP-Adresse abfragen
  5. AT+CIPMUX=1     → Mehrfachverbindungen
  6. AT+CIPSERVER=1,80 → TCP-Server auf Port 80 starten
        │
        ▼
[Lokaler Webserver erreichbar über Browser im Heimnetz]
```

Nach der WiFi-Initialisierung blinkt die LED als Heartbeat.

> **Hinweis:** In dieser Version war der SX1278 LoRa-Empfang noch  
> nicht implementiert (TODO-Kommentare in `main.c`). Die WiFi-  
> Verbindung war der erste Meilenstein. LoRa-Empfang war geplant  
> für den nächsten Entwicklungsschritt.

---

## Warum die Credentials entfernt wurden

Das Original hatte WiFi-SSID und -Passwort als Hardcode im Code.  
**Lesson Learned für v0.1+:** Credentials gehören in eine  
separate `secrets.h` Datei die in `.gitignore` steht.

→ SmartGarden v0.1 macht das korrekt: `firmware/sensor-node/src/secrets.h`

---

## Entwicklungsumgebung

- STM32CubeIDE 1.16.1
- STM32F1xx HAL Drivers
- Target: STM32F103C8T6 (72 MHz, 64KB Flash, 20KB RAM)
- ESP8266 Firmware: AT-Kommando-Firmware (Standard ESP-AT)

---

## Lessons Learned → v0.1

| Problem v0.0 | Lösung v0.1 |
|-------------|-------------|
| WiFi-Credentials im Code | `secrets.h` + `.gitignore` |
| Lokaler Webserver (kein Cloud-Zugriff) | TTN → Cloud-Backend → App |
| AT-Kommandos komplex und fehleranfällig | ESP32 mit integriertem WiFi (kein AT-Modem) |
| Kein LoRa implementiert (TODO) | TTGO LoRa32: LoRa + WiFi in einem Chip |
| STM32 Toolchain aufwändig | PlatformIO + Arduino Framework |
