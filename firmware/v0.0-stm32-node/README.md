# SmartGarden v0.0 — STM32 LoRa Node Firmware (Archiv)

**Hardware:** STM32F103C8T6 „Blue Pill" + SX1278 (läuft auf 868 MHz)  
**Tool:** STM32CubeIDE 1.16.1  
**LoRa-Library:** Sslman Motlaq (pilot.motlaq@gmail.com, [@S_Motlaq](https://t.me/S_Motlaq))  
**Datum:** 2024/2025  
**Status:** Archiviert — abgelöst durch TTGO LoRa32 + TTN (v0.1)  
**Getestete Reichweite:** ~3 km (Feldtest bestätigt)

---

## Dateien

| Datei | Beschreibung |
|-------|-------------|
| `Core/Src/main.c` | Hauptprogramm — Master/Slave Logik, State Machine |
| `Core/Src/LoRa.c` | LoRa HAL SPI-Stack (SX1278) |
| `Core/Src/BME280_STM32.c` | BME280 Sensor-Treiber (Temp/Feuchte/Druck) |
| `Core/Src/UartRingbuffer_multi.c` | UART Ring-Buffer (Multi-UART) |
| `Core/Inc/LoRa.h` | LoRa Typedefs, Register-Defines, Funktionsdeklarationen |
| `Core/Inc/BME280_STM32.h` | BME280 Header |
| `Core/Inc/UartRingbuffer_multi.h` | Ring-Buffer Header |
| `Core/Inc/main.h` | STM32-Peripherie-Definitionen, Pin-Mapping |

Fritzing-Schaltplan und Steckplatinen-Layout:  
→ [`../../hardware/schematics/v0.0-stm32-slave/`](../../hardware/schematics/v0.0-stm32-slave/)

---

## Hardware-Konfiguration

| Pin | Funktion |
|-----|----------|
| PA4 (NSS) | SX1278 Chip Select |
| PA5 (SCK) | SX1278 SPI Clock |
| PA6 (MISO) | SX1278 SPI MISO |
| PA7 (MOSI) | SX1278 SPI MOSI |
| PA1 (DIO0) | SX1278 IRQ — RxDone / TxDone (EXTI) |
| RST | SX1278 Reset |
| PB6 (SCL) | BME280 I2C Clock |
| PB7 (SDA) | BME280 I2C Data |
| PB10 (TX) | UART Debug / ESP32-Kommunikation |
| PB11 (RX) | UART Debug / ESP32-Kommunikation |

---

## LoRa-Konfiguration

```c
#define Frequency           868       // MHz (SX1278 läuft auf 868, obwohl Modul-Aufdruck "433")
#define Spreading_Factor    SF_7
#define Bandwidth           BW_125KHz
#define CRCRate             CR_4_5
#define Power               POWER_20db  // 20 dBm
#define Preamble            8
```

> **Hinweis zu 433 MHz vs. 868 MHz:** Das Modul trägt den Aufdruck „SX1278" (433 MHz),  
> lief aber mit `#define Frequency 868`. Wahrscheinlich ein AliExpress-Modul mit  
> SX1276-Chip (137–1020 MHz) unter falschem Label. Bestätigt: ~3 km Reichweite auf 868 MHz.

---

## MASTER / SLAVE — Compile-Time-Schalter

```c
#define MASTER  0   // Gateway-Node
#define SLAVE   1   // Outdoor-Sensor-Node
```

Beide Rollen liegen im **gleichen Quellcode**. Vor dem Flashen `MASTER` oder `SLAVE`
auf `1` setzen (das andere auf `0`).

### Master (Gateway-Node)

- Sendet alle 15 Sekunden `"REQ\r\n"` an den Slave
- Empfängt UUID-Antwort und sendet LED-Kommando
- Leitet empfangene Daten per UART an ESP32/ESP8266 weiter
- Bleibt dauerhaft aktiv (keine Sleep-Modi)

### Slave (Outdoor-Sensor-Node)

- Wartet auf `"REQ\r\n"` vom Master (Interrupt-getrieben, DIO0 EXTI)
- Antwortet mit UUID, empfängt LED-Kommando
- Steuert LED, liest BME280-Sensor, sendet Status + Temperatur
- Geht nach jeder Übertragung in **STOP-Mode** (Low-Power):

```c
HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
SystemClock_Config();  // Takt nach STOP-Mode wiederherstellen
```

---

## Kommunikationsprotokoll (proprietär, 868 MHz)

```
Master → Slave: "REQ\r\n"
Slave → Master: "UUID=AAAA02\r\n"
Master → Slave: "CMD;LED=0\r\n"  oder  "CMD;LED=1\r\n"  (counter % 2)
Slave → Master: "UUID=AAAA02;LED=1,Temp=23.45\r\n"
```

Die LED wechselt automatisch alle 15 Sekunden (Master sendet `counter % 2`).

### DB-Kommando: Master → ESP32 via UART

```c
sprintf(db_command,
    "WRITE;SLAVE=0;UUID=%s;COUNT=%i;LED_STAT=%i;LED_CMD=%s\r\n",
    UUID_SLAVE, counter, Led_stat, sTemp);
Uart_sendstring(db_command, wifi_uart);
```

Beispiel: `WRITE;SLAVE=0;UUID=AAAA02;COUNT=42;LED_STAT=1;LED_CMD=23.45`

---

## State Machine (LoRaState)

```
Master:
  REQUEST_SLAVE_DATA  →  (sendet REQ)
  RECEIVE_SLAVE_DATA  →  (wartet auf UUID via DIO0-IRQ)
  SEND_DB             →  (leitet UUID-Bestätigung an ESP32 weiter)
  WRITE_SLAVE_DATA    →  (sendet CMD;LED=x)
  SEND_DB             →  (leitet LED+Temp an ESP32 weiter)
  [zurück zu REQUEST_SLAVE_DATA nach 15s Delay]

Slave:
  REC                 →  (wartet auf REQ via DIO0-IRQ)
  SEND_UUID           →  (sendet UUID=AAAA02)
  REC                 →  (wartet auf CMD;LED=x via DIO0-IRQ)
  SEND_LED_STAT       →  (sendet UUID;LED;Temp, dann STOP-Mode)
```

---

## DIO0 Interrupt-Handler

Interrupt-getriebener Empfang via `HAL_GPIO_EXTI_Callback`:

```c
uint8_t irqFlags = LoRa_read(&Dev01.myLoRa, 0x12);  // IRQ_FLAGS Register
if (irqFlags & 0x08)  // TxDone (Bit 3)
if (irqFlags & 0x40)  // RxDone (Bit 6)
```

Nach dem Verarbeiten: `LoRa_write(&Dev01.myLoRa, 0x12, 0xFF);` (alle IRQ-Flags löschen)

---

## LoRa-Library (Sslman Motlaq)

HAL SPI-basierter LoRa-Stack für STM32. Direktes Register-Mapping auf SX1278/SX1276.

Wichtigste Funktionen:
```c
LoRa_Device newLoRaDevice(frequency, sf, bw, cr, preamble, power, ocp);
LoRa_init(&myLoRa);
LoRa_startReceiving(&myLoRa);
LoRa_transmit(&myLoRa, data, length, timeout);
LoRa_receive(&myLoRa, buffer, length);
LoRa_read(&myLoRa, register);
LoRa_write(&myLoRa, register, value);
```

---

## Lessons Learned → v0.1

| Problem v0.0 | Lösung v0.1 |
|-------------|-------------|
| Proprietäres Protokoll, kein TTN | LoRaWAN + MCCI LMIC Library |
| MASTER/SLAVE Compile-Schalter | Separate Firmware-Projekte |
| SX1278 (433-Label, 868-Betrieb) | SX1276 (EU868, klar spezifiziert) |
| STM32 CubeIDE + HAL komplex | PlatformIO + Arduino Framework |
| Kein Over-The-Air-Update | TTN + OTA geplant |
| Sleep: STOP-Mode, kein Duty Cycle | LoRaWAN Class A Duty Cycle nativ |
| UART-Bridge STM32→ESP32 | TTGO: ESP32 direkt mit SX1276 |
| 110 mAh Akku (~22h Autonomie) | 2× 18650 (~42 Tage Autonomie) |

---

## Entwicklungsumgebung

- STM32CubeIDE 1.16.1
- STM32F1xx HAL Drivers
- Target: STM32F103C8T6 (72 MHz, 8 MHz HSE × PLL ×9, 64 KB Flash, 20 KB RAM)
