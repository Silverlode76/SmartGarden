# SmartGarden — Cloud Architektur

```mermaid
flowchart LR
    subgraph EDGE["⚡ Edge"]
        ESP["ESP32\nLILYGO T3 v1.6.1\n📡 SX1276 LoRa\n💧 Pumpe · Boden · Akku\nDeep Sleep 900s"]
    end

    subgraph LORA["📡 LoRaWAN EU868"]
        GW["Gateway\neigenes GW"]
        TTN["TTN v3\nNetwork Server"]
        GW --> TTN
    end

    subgraph AWS["☁️ AWS  eu-central-1"]
        WEBHOOK["λ ttn-webhook\nHTTP Integration"]
        DYNAMO[("DynamoDB\nsmartgarden-devices\ndevice_id · payload\nreceived_at · pump")]
        APIGW["API Gateway\nREST · HTTPS"]
        READ["λ smartgarden-read\nPayload dekodieren"]
        WATCHDOG["λ smartgarden-watchdog\nSilence > 1800s?"]
        EB["EventBridge\nrate(5 minutes)"]
        SNS["SNS\nSmartGardenHeartBeat"]

        WEBHOOK --> DYNAMO
        APIGW --> READ --> DYNAMO
        EB -->|alle 5min| WATCHDOG
        WATCHDOG -->|GetItem| DYNAMO
        WATCHDOG -->|Alarm| SNS
    end

    subgraph APP["📱 Flutter App"]
        MOBILE["Mobile App\n💧 Pumpstatus\n🌱 Bodenfeuchte\n🔋 Akku %\n❤️ HeartbeatBanner"]
        FB["Firebase\nRealtime DB"]
        MAIL["📧 E-Mail Alert\nschmoll.oliver@web.de"]
    end

    ESP -->|LoRa Uplink| GW
    TTN -->|HTTP Webhook| WEBHOOK
    MOBILE -->|HTTPS GET| APIGW
    DYNAMO -.->|JSON Response| MOBILE
    SNS -.->|E-Mail bei Silence| MAIL
    MOBILE <--> FB
```

## Datenfluss

| Pfad | Beschreibung |
|---|---|
| ESP32 → TTN → Lambda → DynamoDB | Uplink-Daten landen in der DB |
| Flutter → API Gateway → Lambda → DynamoDB | App liest aktuellen Status |
| EventBridge → Watchdog → DynamoDB | Alle 5min: Silence-Check |
| Watchdog → SNS → E-Mail | Alarm wenn Device > 30min still |

## TX-Intervalle

| Zustand | Intervall |
|---|---|
| Normal (Sleep) | 900s (15min) |
| Pumpe aktiv | 15s |
| Watchdog-Prüfung | 300s (5min) |
| Alarm-Schwelle | 1800s (30min) |
