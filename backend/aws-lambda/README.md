# SmartGarden — AWS Lambda Funktionen (Lernpfad)

Quellcode für die beiden Lambda-Funktionen aus dem parallelen AWS-Backend-Lernpfad
(siehe [ADR-005](../../docs/architecture/ADR-005-AWS-Backend-Lernpfad.md) für
Architektur, Begründung und Sequenzdiagramm).

| Funktion | Trigger | Aufgabe |
|---|---|---|
| [`smartgarden-write/index.mjs`](smartgarden-write/index.mjs) | API Gateway `POST /uplink` (TTN-Webhook) | Schreibt Gerätestatus nach DynamoDB |
| [`smartgarden-read/index.mjs`](smartgarden-read/index.mjs) | API Gateway `GET /status?device_id=...` (Flutter-App) | Liest Gerätestatus aus DynamoDB |

## Deployment

Aktuell **manuell über die AWS Console** eingerichtet (Lernpfad, kein
Infrastructure-as-Code) — Code hier im Repo dient als Referenz/Backup, nicht als
automatisierte Deployment-Quelle.

Bei Änderungen: Code hier im Repo aktualisieren, **und** den Inhalt von
`index.mjs` manuell in den Lambda-Code-Editor in der AWS Console kopieren →
**Deploy** klicken, damit beide Stände synchron bleiben.

## Runtime & Abhängigkeiten

- **Runtime:** Node.js 20.x (oder neuer)
- **Dependencies:** `@aws-sdk/client-dynamodb` — ist in der Lambda-Node.js-Runtime
  bereits vorinstalliert, kein eigenes `npm install`/Layer nötig

## Konfiguration (AWS Console, Region `eu-central-1`)

| Ressource | Name |
|---|---|
| DynamoDB-Tabelle | `smartgarden-devices` (Partition Key: `device_id`, String) |
| API Gateway | `smartgarden-api` — `https://e3trf6ld2j.execute-api.eu-central-1.amazonaws.com` |
| IAM-Rollen | `smartgarden-write-role-*`, `smartgarden-read-role-*` (jeweils `AmazonDynamoDBFullAccess` — siehe ADR-005 "Offene Punkte" zur Verschärfung) |
| CORS | `Access-Control-Allow-Origin: *`, Methods `GET, POST, OPTIONS`, Headers `content-type` |
