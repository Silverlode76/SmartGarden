// SmartGarden — Lambda "smartgarden-watchdog"
//
// Trigger : EventBridge Scheduler — alle 5 Minuten
// Zweck   : Prüft für jede überwachte Device-ID ob der letzte Uplink
//           nicht älter als TIMEOUT_SEC Sekunden ist.
//           Falls ja → SNS-Alarm (E-Mail / Push).
//
// AWS-Ressourcen die diese Lambda benötigt (IAM Role):
//   - dynamodb:GetItem      auf Tabelle "smartgarden-devices"
//   - sns:Publish           auf SNS Topic ALERT_TOPIC_ARN
//   - logs:CreateLogGroup / logs:CreateLogStream / logs:PutLogEvents
//
// Umgebungsvariablen (Lambda Environment):
//   ALERT_TOPIC_ARN   ARN des SNS Topics für Alarme
//   TIMEOUT_SEC       Maximale Sekunden seit letztem Uplink (default: 1800)
//   DEVICE_IDS        Komma-separierte Liste der zu überwachenden Device-IDs
//                     z.B. "smartgarden-irrigator,smartgarden-sensor-01"

import { DynamoDBClient, GetItemCommand } from "@aws-sdk/client-dynamodb";
import { SNSClient, PublishCommand }       from "@aws-sdk/client-sns";

const dynamo = new DynamoDBClient({});
const sns    = new SNSClient({});

const TABLE_NAME  = "smartgarden-devices";
const TOPIC_ARN   = process.env.ALERT_TOPIC_ARN;
const TIMEOUT_SEC = parseInt(process.env.TIMEOUT_SEC ?? "1800", 10);
const DEVICE_IDS  = (process.env.DEVICE_IDS ?? "smartgarden-irrigator")
                      .split(",")
                      .map(id => id.trim())
                      .filter(Boolean);

export const handler = async () => {
  const now    = Date.now();
  const alerts = [];

  for (const deviceId of DEVICE_IDS) {
    const result = await dynamo.send(new GetItemCommand({
      TableName: TABLE_NAME,
      Key: { device_id: { S: deviceId } },
    }));

    if (!result.Item) {
      // Device noch nie gesehen → Alarm
      alerts.push({ deviceId, reason: "Kein Eintrag in DynamoDB — Device hat noch nie gesendet" });
      continue;
    }

    const receivedAt = result.Item.received_at?.S;
    if (!receivedAt) {
      alerts.push({ deviceId, reason: "received_at fehlt im DynamoDB-Eintrag" });
      continue;
    }

    const lastSeenMs  = new Date(receivedAt).getTime();
    const silenceSec  = Math.round((now - lastSeenMs) / 1000);

    console.log(`[${deviceId}] Letzter Uplink vor ${silenceSec}s (Limit: ${TIMEOUT_SEC}s)`);

    if (silenceSec > TIMEOUT_SEC) {
      alerts.push({
        deviceId,
        reason:     `Kein Uplink seit ${silenceSec}s (Limit: ${TIMEOUT_SEC}s)`,
        lastSeen:   receivedAt,
        silenceSec,
      });
    }
  }

  if (alerts.length === 0) {
    console.log("Alle Devices OK.");
    return { status: "ok" };
  }

  // SNS-Alarm senden
  const message = alerts.map(a =>
    `⚠️ SmartGarden Watchdog\nDevice: ${a.deviceId}\nProblem: ${a.reason}${
      a.lastSeen ? `\nLetzter Uplink: ${a.lastSeen}` : ""
    }`
  ).join("\n\n");

  await sns.send(new PublishCommand({
    TopicArn: TOPIC_ARN,
    Subject:  `SmartGarden Alarm — ${alerts.length} Device(s) still`,
    Message:  message,
  }));

  console.warn(`Alarm gesendet für: ${alerts.map(a => a.deviceId).join(", ")}`);
  return { status: "alert", alerts };
};
