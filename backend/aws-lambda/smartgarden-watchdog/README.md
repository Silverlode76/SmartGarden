# smartgarden-watchdog

Prüft alle 5 Minuten ob registrierte Nodes noch Uplinks senden.  
Alarm via SNS (E-Mail) wenn ein Device länger als `TIMEOUT_SEC` still ist.

## Architektur

```
EventBridge Scheduler (alle 5min)
        │
        ▼
Lambda: smartgarden-watchdog
        │
        ├── DynamoDB GetItem → received_at prüfen
        │
        └── SNS Publish → E-Mail Alarm
```

## AWS Setup (einmalig)

### 1. Lambda deployen

```bash
zip function.zip index.mjs
aws lambda create-function \
  --function-name smartgarden-watchdog \
  --runtime nodejs22.x \
  --handler index.handler \
  --zip-file fileb://function.zip \
  --role arn:aws:iam::ACCOUNT_ID:role/smartgarden-watchdog-role
```

### 2. IAM Role

Die Lambda-Role braucht folgende Permissions:

```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "dynamodb:GetItem",
      "Resource": "arn:aws:dynamodb:eu-central-1:ACCOUNT_ID:table/smartgarden-devices"
    },
    {
      "Effect": "Allow",
      "Action": "sns:Publish",
      "Resource": "arn:aws:sns:eu-central-1:ACCOUNT_ID:smartgarden-alerts"
    },
    {
      "Effect": "Allow",
      "Action": ["logs:CreateLogGroup","logs:CreateLogStream","logs:PutLogEvents"],
      "Resource": "arn:aws:logs:*:*:*"
    }
  ]
}
```

### 3. SNS Topic anlegen

```bash
aws sns create-topic --name smartgarden-alerts

# E-Mail Subscription
aws sns subscribe \
  --topic-arn arn:aws:sns:eu-central-1:ACCOUNT_ID:smartgarden-alerts \
  --protocol email \
  --notification-endpoint deine@email.de
```

### 4. Umgebungsvariablen setzen

| Variable | Beispiel | Beschreibung |
|---|---|---|
| `ALERT_TOPIC_ARN` | `arn:aws:sns:eu-central-1:123:smartgarden-alerts` | SNS Topic ARN |
| `TIMEOUT_SEC` | `1800` | Sekunden bis Alarm (default 30 min) |
| `DEVICE_IDS` | `smartgarden-irrigator` | Komma-separiert, mehrere möglich |

### 5. EventBridge Scheduler

```bash
aws scheduler create-schedule \
  --name smartgarden-watchdog-5min \
  --schedule-expression "rate(5 minutes)" \
  --target '{"Arn":"arn:aws:lambda:eu-central-1:ACCOUNT_ID:function:smartgarden-watchdog","RoleArn":"arn:aws:iam::ACCOUNT_ID:role/eventbridge-scheduler-role"}' \
  --flexible-time-window '{"Mode":"OFF"}'
```

## Warum separate Lambda? (Cloud-Architektur-Begründung)

| | smartgarden-read | smartgarden-watchdog |
|---|---|---|
| Trigger | API Gateway (on-demand) | EventBridge (zeitgesteuert) |
| Aufrufer | Flutter-App | AWS intern |
| IAM | nur `dynamodb:GetItem` | + `sns:Publish` |
| Fehlerverhalten | HTTP 500 → App zeigt Fehler | CloudWatch Alarm → kein User-Impact |
| Skalierung | mit App-Traffic | konstant alle 5min |
