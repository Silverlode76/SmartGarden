#pragma once

// ============================================================
// SmartGarden — TTN OTAA Credentials
// ============================================================
// ANLEITUNG:
//   1. Diese Datei kopieren als "secrets.h" (selbes Verzeichnis)
//   2. Werte aus TTN Console eintragen (Application → Devices → Overview)
//   3. secrets.h ist in .gitignore — niemals ins Git committen!
//
// TTN Console: https://console.cloud.thethings.network/
// ============================================================

// Device EUI — 8 Bytes, LSB first (in TTN Console auf "lsb" klicken)
// Beispiel TTN: 70 B3 D5 7E D0 06 00 01
static const u1_t PROGMEM DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Application EUI / JoinEUI — 8 Bytes, LSB first
// Beispiel TTN: 00 00 00 00 00 00 00 00
static const u1_t PROGMEM APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// Application Key — 16 Bytes, MSB first (in TTN Console auf "msb" klicken)
// Beispiel TTN: 2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C
static const u1_t PROGMEM APPKEY[16] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
