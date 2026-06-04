# Firmware State Machine & Systemsequenz

## Guard Node v0.2 (Deep Sleep + PIR Wakeup)

---

## 1. Haupt-State Machine (Deep Sleep Zyklus)

```mermaid
stateDiagram-v2
    [*] --> BOOT : Power On

    BOOT --> CHECK_RTC : setup() aufgerufen\nWakeup-Grund ermitteln

    CHECK_RTC --> JOINING     : rtcJoined = false\n(Erster Boot oder Reset)
    CHECK_RTC --> TRANSMITTING : rtcJoined = true\n(Session aus RTC geladen)

    JOINING --> JOINING      : EV_JOIN_TXCOMPLETE\n(warte auf Accept, Duty Cycle)
    JOINING --> TRANSMITTING : EV_JOINED\n(Session gespeichert in RTC)
    JOINING --> DEEP_SLEEP   : EV_JOIN_FAILED\n(schlafen, später nochmal)

    TRANSMITTING --> RX_WINDOW : EV_TXSTART\n(Uplink gesendet)

    RX_WINDOW --> CMD_EXEC    : EV_TXCOMPLETE + Downlink\n(LED-Kommando empfangen)
    RX_WINDOW --> ALARM_CHECK : EV_TXCOMPLETE ohne Downlink

    CMD_EXEC --> ALARM_CHECK : Kommando ausgeführt

    ALARM_CHECK --> TRANSMITTING : PIR noch aktiv\nUND alarmCount < 5\n(warte 30s, dann nochmal)
    ALARM_CHECK --> WAIT_BEFORE_SLEEP : PIR klar\nODER alarmCount >= 5

    WAIT_BEFORE_SLEEP --> DEEP_SLEEP : 10s warten\n(für eventuelle Downlinks)

    DEEP_SLEEP --> PIR_WAIT  : goToSleep()\nPIR noch HIGH?

    PIR_WAIT --> PIR_WAIT    : PIR = HIGH\n(warten bis KLAR,\nmax. 5 Minuten)
    PIR_WAIT --> SLEEPING    : PIR = LOW\n(steigende Flanke möglich)

    SLEEPING --> BOOT        : PIR Wakeup\n(GPIO13 steigende Flanke)
    SLEEPING --> BOOT        : Timer Wakeup\n(15 Minuten)
```

---

## 2. Wakeup-Logik

```mermaid
flowchart TD
    A[ESP32 wacht auf] --> B{Wakeup-Grund?}

    B -->|EXT0 - PIR HIGH| C[WAKEUP_PIR\nmotionAlert = true]
    B -->|Timer| D[WAKEUP_TIMER\nPIR einmalig lesen]
    B -->|Kein Grund| E[WAKEUP_RESET\nNormaler Boot]

    D --> F{PIR aktuell HIGH?}
    F -->|Ja| C
    F -->|Nein| G[motionAlert = false]

    C --> H{rtcJoined?}
    G --> H
    E --> I[rtcJoined = false\nOTAA Join]

    H -->|Ja| J[Session aus RTC laden\nsofort Uplink senden]
    H -->|Nein| I
```

---

## 3. Alarm-Logik nach TX

```mermaid
flowchart TD
    A[EV_TXCOMPLETE] --> B{Downlink\nempfangen?}

    B -->|Ja| C[LED Kommando\nausführen]
    B -->|Nein| D{PIR aktiv?}
    C --> D

    D -->|Nein| E[alarmCount = 0\nWarte 10s]
    D -->|Ja| F{alarmCount\n< 5?}

    F -->|Ja| G[alarmCount++\nWarte 30s\ndann nochmal TX]
    F -->|Nein| H[alarmCount = 0\nMax erreicht\nWarte 10s]

    E --> I[triggerSleep\ntxDone = true]
    H --> I
    G --> J[sendUplink]

    I --> K[goToSleep]
```

---

## 4. Zustandsbeschreibung

| Zustand | Beschreibung | OLED-Anzeige |
|---------|-------------|--------------|
| `BOOT` | setup() aufgerufen, Hardware init | "SmartGarden Guard v0.2" |
| `CHECK_RTC` | Wakeup-Grund prüfen, RTC-Daten laden | — |
| `JOINING` | OTAA Join-Request, warte auf Accept | "Join TTN..." |
| `TRANSMITTING` | Uplink wird gesendet | "Sende Uplink..." |
| `RX_WINDOW` | RX1/RX2 Fenster offen (5-6s nach TX) | — |
| `CMD_EXEC` | LED-Kommando ausführen | "Downlink! LED: AN/AUS" |
| `ALARM_CHECK` | PIR-Status prüfen, alarmCount auswerten | — |
| `WAIT_BEFORE_SLEEP` | 10s warten für eventuelle Downlinks | "Warte 10s... Deep Sleep" |
| `PIR_WAIT` | Warten bis PIR=LOW vor Sleep | "Warte auf KLAR" |
| `SLEEPING` | Deep Sleep (~0.15mA) | Display aus |

---

## 5. RTC-Speicher (überlebt Deep Sleep)

| Variable | Typ | Inhalt |
|----------|-----|--------|
| `rtcJoined` | bool | Session gültig? |
| `rtcNwkSKey` | uint8_t[16] | Network Session Key |
| `rtcAppSKey` | uint8_t[16] | Application Session Key |
| `rtcDevAddr` | uint32_t | Temporäre TTN-Adresse |
| `rtcSeqnoUp` | uint32_t | Uplink Frame Counter |
| `rtcSeqnoDn` | uint32_t | Downlink Frame Counter |
| `rtcLedState` | bool | LED an/aus |
| `rtcAlarmCount` | uint8_t | Anzahl Alarm-Uplinks (max. 5) |

> **Wichtig:** `rtcSeqnoUp` und `rtcSeqnoDn` müssen gespeichert werden — TTN verwirft Pakete mit zu niedrigem Counter als Replay-Angriff.

---

## 6. Energiebudget Guard v0.2

| Zustand | Strom | Dauer | Energie/Wakeup |
|---------|-------|-------|----------------|
| Deep Sleep | ~0.15 mA | ~15 min | ~0.56 mAh |
| Boot + LMIC init | ~80 mA | ~0.5s | ~0.01 mAh |
| TX Uplink | ~120 mA | ~0.2s | ~0.007 mAh |
| RX Fenster | ~12 mA | ~1s | ~0.003 mAh |
| **Gesamt pro Zyklus** | | | **~0.58 mAh** |
| **Tagesverbrauch** | | 96 Zyklen | **~55 mAh/Tag** |
| **2× 18650 (5000 mAh)** | | | **~90 Tage** |

> Bei PIR-Alarm: bis zu 5 Uplinks × 30s = max. 2.5 Minuten aktiv → ca. 5 mAh pro Alarm-Ereignis.

---

## 7. Payload-Format

### Uplink (Node → TTN, FPort 1)

| Byte | Wert | Bedeutung |
|------|------|-----------|
| 0 | `0x00` | LED aus |
| 0 | `0x01` | LED an |
| 1 | `0x00` | Kein Alarm |
| 1 | `0x01` | Bewegung erkannt |

### Downlink (TTN → Node, FPort 1)

| Byte | Wert | Bedeutung |
|------|------|-----------|
| 0 | `0x00` | LED ausschalten |
| 0 | `0x01` | LED einschalten |
| 0 | `0x02` | LED toggeln |

---

## 8. Bekannte Einschränkungen

| Problem | Ursache | Status |
|---------|---------|--------|
| Join dauert 1-5 Min | EU868 Duty Cycle + Gateway-Timing | ✅ durch RTC-Session gelöst (nach erstem Join) |
| PIR HIGH → kein Wakeup | EXT0 nur bei steigender Flanke | ✅ gefixt: warten bis PIR=LOW vor Sleep |
| Endless Alarm-Loop | PIR dauerhaft aktiv | ✅ gefixt: max. 5 Alarm-Uplinks |
| SF10 nach ADR | ADR passt Rate an | ✅ ADR deaktiviert (festes SF9) |
| Downlink sporadisch | Gateway Duty Cycle / Timing | ⚠️ LoRaWAN by design (best-effort) |
