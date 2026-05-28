#pragma once

// ============================================================
// SmartGarden Guard — TTN OTAA Credentials
// ============================================================
// ANLEITUNG:
//   1. Diese Datei kopieren als "secrets.h" (selbes Verzeichnis)
//   2. Werte aus TTN Console eintragen (Application → Devices → Overview)
//   3. secrets.h ist in .gitignore — NIEMALS ins Git committen!
//
// TTN Console: https://console.cloud.thethings.network/
// ============================================================

// Device EUI — 8 Bytes, LSB first (in TTN Console auf "lsb" klicken)
static const u1_t PROGMEM DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Application EUI / JoinEUI — 8 Bytes, LSB first
static const u1_t PROGMEM APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Application Key — 16 Bytes, MSB first (in TTN Console auf "msb" klicken)
static const u1_t PROGMEM APPKEY[16] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
