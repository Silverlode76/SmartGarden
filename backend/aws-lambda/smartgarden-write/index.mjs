// SmartGarden — Lambda "smartgarden-write"
// Trigger : API Gateway HTTP API, POST /uplink (von TTN-Webhook aufgerufen)
// Zweck   : Nimmt eine TTN-Uplink-Webhook-Payload entgegen und schreibt den
//           aktuellen Status (device_id, frm_payload, received_at) als
//           einen Datensatz pro Gerät in DynamoDB.
//
// Siehe docs/architecture/ADR-005-AWS-Backend-Lernpfad.md für Architektur
// und Begründung (u.a. warum getrennt von "smartgarden-read").

import { DynamoDBClient, PutItemCommand } from "@aws-sdk/client-dynamodb";

const client = new DynamoDBClient({});

export const handler = async (event) => {
  try {
    const body = JSON.parse(event.body);
    const deviceId = body.end_device_ids?.device_id;
    if (!deviceId) {
      return { statusCode: 400, body: "Missing device_id" };
    }

    const frmPayload = body.uplink_message?.frm_payload || "";
    const receivedAt = body.received_at || new Date().toISOString();

    await client.send(new PutItemCommand({
      TableName: "smartgarden-devices",
      Item: {
        device_id: { S: deviceId },
        frm_payload: { S: frmPayload },
        received_at: { S: receivedAt },
        raw: { S: JSON.stringify(body) },
      },
    }));

    return { statusCode: 200, body: "OK" };
  } catch (err) {
    console.error(err);
    return { statusCode: 500, body: "Error: " + err.message };
  }
};
