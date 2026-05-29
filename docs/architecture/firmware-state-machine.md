# Firmware State Machine & Systemsequenz — Sensor Node v0.1

## 1. State Machine (LMIC + PIR)

Der Node durchläuft folgende Zustände:

```mermaid
stateDiagram-v2
    [*] --> INIT : Power On / Reset

    INIT --> JOINING : os_init() + LMIC_startJoining()

    JOINING --> JOINING    : EV_JOIN_TXCOMPLETE\n(warte auf Antwort, Duty Cycle)
    JOINING --> IDLE       : EV_JOINED\n(OTAA erfolgreich, erster Uplink sofort)
    JOINING --> JOIN_FAILED : EV_JOIN_FAILED\n(Keys prüfen)

    JOIN_FAILED --> JOINING : LMIC_startJoining()\n(automatischer Retry)

    IDLE --> TRANSMITTING  : Timer (60s) oder PIR-Zustandsänderung

    TRANSMITTING --> RX_WINDOW : EV_TXSTART\n(Paket gesendet)
    RX_WINDOW --> IDLE         : EV_TXCOMPLETE ohne Downlink\n(nächster TX in 60s)
    RX_WINDOW --> CMD_EXEC     : EV_TXCOMPLETE mit Downlink\n(Kommando empfangen)

    CMD_EXEC --> IDLE : LED-Kommando ausgeführt\n(0x00/0x01/0x02)\nnächster TX in 60s

    IDLE --> IDLE : PIR polling (loop)\nkeine Änderung

    note right of TRANSMITTING
        Payload 2 Bytes:
        [0] LED-Status (0x00/0x01)
        [1] Motion (0x00/0x01)
    end note

    note right of RX_WINDOW
        RX1: 5s nach TX
        RX2: 6s nach TX (SF9, 869.525 MHz)
        Downlink kann ausbleiben
        (Duty Cycle Gateway)
    end note
```

### Zustandsbeschreibung

| Zustand | Beschreibung | OLED-Anzeige |
|---------|-------------|--------------|
| `INIT` | Hardware-Init, LMIC-Setup, EU868-Kanäle | "Initialisiere..." |
| `JOINING` | OTAA Join-Request wird gesendet, warte auf Accept | "Verbinde TTN..." |
| `JOIN_FAILED` | Join fehlgeschlagen (falsche Keys) | "JOIN FAILED!" |
| `IDLE` | Verbunden, warte auf Timer oder PIR-Event | "TX OK / RSSI: x dBm" |
| `TRANSMITTING` | Uplink wird über LoRa gesendet | "Sende Uplink..." |
| `RX_WINDOW` | Empfangsfenster offen (RX1/RX2) | — |
| `CMD_EXEC` | Downlink-Kommando wird ausgeführt | "Downlink RX!" |

### PIR-Logik (Polling im Loop)

```
loop() jeden Tick:
  pirNow = digitalRead(PIR_PIN)
  if pirNow != motionLastState:
    motionLastState = pirNow
    motionAlert = pirNow
    if NOT TXRXPEND:
      → sofortiger Uplink (überspringt 60s-Timer)
```

---

## 2. Sequenzdiagramm — Uplink & Downlink

```mermaid
sequenceDiagram
    participant Node as TTGO Node
    participant GW as LoRa Gateway
    participant TTN as TTN (eu1)
    participant App as Flutter App

    Note over Node: Boot / Reset

    Node->>GW: Join Request (OTAA, SF9, 868.5 MHz)
    GW->>TTN: Forward Join Request
    TTN->>GW: Join Accept
    GW->>Node: Join Accept (RX1, 5s delay)
    Note over Node: EV_JOINED → joined=true

    loop alle 60s (oder PIR-Event)
        Node->>GW: Uplink [LED, Motion] FPort=1 SF7
        GW->>TTN: Forward Uplink
        TTN->>App: MQTT Publish\nv3/smartgardenollie@ttn/devices/.../up
        Note over App: frm_payload dekodieren\nLED & Motion Status anzeigen

        alt Downlink in Queue
            TTN->>GW: Schedule Downlink (RX1, 5s)
            GW->>Node: Downlink [0x00/0x01/0x02] FPort=1
            Note over Node: EV_TXCOMPLETE\nKommando ausführen
        else kein Downlink
            Note over Node: EV_TXCOMPLETE\nnächster TX in 60s
        end
    end
```

### Downlink-Pfad (Flutter → Node)

```mermaid
sequenceDiagram
    participant App as Flutter App
    participant TTN as TTN (eu1)
    participant GW as LoRa Gateway
    participant Node as TTGO Node

    App->>TTN: MQTT Publish\nv3/.../down/push\nfrm_payload: base64([0x02])
    Note over TTN: Downlink in Queue\n(wartet auf nächsten Uplink)

    Node->>GW: Uplink (regulär, 60s Zyklus)
    GW->>TTN: Forward Uplink
    TTN->>GW: Downlink anhängen (RX1 Delay: 5s)
    GW->>Node: Downlink übertragen
    Note over Node: 0x02 → LED Toggle
    Node->>GW: nächster Uplink mit\naktualisiertem LED-Status
    GW->>TTN: Forward Uplink
    TTN->>App: MQTT Update\n(LED-Status aktualisiert)
```

---

## 3. Payload-Format

### Uplink (Node → TTN, FPort 1)

| Byte | Wert | Bedeutung |
|------|------|-----------|
| 0    | `0x00` | LED aus |
| 0    | `0x01` | LED an |
| 1    | `0x00` | Kein Bewegungsalarm |
| 1    | `0x01` | Bewegung erkannt (PIR HIGH) |

### Downlink (TTN → Node, FPort 1)

| Byte | Wert | Bedeutung |
|------|------|-----------|
| 0    | `0x00` | LED ausschalten |
| 0    | `0x01` | LED einschalten |
| 0    | `0x02` | LED toggeln |

---

## 4. Timing-Übersicht

| Parameter | Wert | Hinweis |
|-----------|------|---------|
| TX-Intervall | 60s (PoC) / 900s (Produktion) | Duty Cycle: ~0.08% |
| Join SF | SF9 | Robuster als SF7 für ersten Join |
| Data SF | SF7BW125 | ADR passt automatisch an |
| RX1 Delay | 5s | TTN Standard |
| Downlink-Zustellung | 1–3 TX-Zyklen | Gateway Duty Cycle abhängig |
