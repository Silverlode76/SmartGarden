// SmartGarden — Lambda "smartgarden-read"
// Trigger : API Gateway HTTP API, GET /status?device_id=... (von der Flutter-App)
// Zweck   : Liest den zuletzt gespeicherten Status eines Geräts aus DynamoDB
//           und gibt ihn als JSON zurück.
//
// Siehe docs/architecture/ADR-005-AWS-Backend-Lernpfad.md für Architektur
// und Begründung (u.a. warum getrennt von "smartgarden-write").

import { DynamoDBClient, GetItemCommand } from "@aws-sdk/client-dynamodb";

const client = new DynamoDBClient({});

export const handler = async (event) => {
  try {
    const deviceId = event.queryStringParameters?.device_id;
    if (!deviceId) {
      return { statusCode: 400, body: JSON.stringify({ error: "Missing device_id query param" }) };
    }

    const result = await client.send(new GetItemCommand({
      TableName: "smartgarden-devices",
      Key: { device_id: { S: deviceId } },
    }));

    if (!result.Item) {
      return { statusCode: 404, body: JSON.stringify({ error: "Device not found" }) };
    }

    return {
      statusCode: 200,
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        device_id: result.Item.device_id.S,
        frm_payload: result.Item.frm_payload?.S,
        received_at: result.Item.received_at?.S,
      }),
    };
  } catch (err) {
    console.error(err);
    return { statusCode: 500, body: JSON.stringify({ error: err.message }) };
  }
};
