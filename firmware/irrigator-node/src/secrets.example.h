#pragma once

// ============================================================
// SmartGarden Irrigator — TTN ABP Credentials
// ============================================================
// ANLEITUNG:
//   1. Diese Datei kopieren als "secrets.h" (selbes Verzeichnis)
//   2. In TTN Console: Device → General Settings → Activation Mode → ABP
//      Dann "Reset Frame Counters" aktivieren (PoC-Modus)
//   3. Werte aus TTN Console eintragen:
//      - Device Address  → DEVADDR  (MSB first, 4 Bytes)
//      - Network Session Key → NWKSKEY (MSB first, 16 Bytes)
//      - App Session Key     → APPSKEY (MSB first, 16 Bytes)
//   4. secrets.h ist in .gitignore — NIEMALS ins Git committen!
//
// TTN Console: https://console.cloud.thethings.network/
// ============================================================

// Device Address — 4 Bytes, MSB first
static const u4_t DEVADDR = 0x00000000;

// Network Session Key — 16 Bytes, MSB first
static const u1_t PROGMEM NWKSKEY[16] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

// App Session Key — 16 Bytes, MSB first
static const u1_t PROGMEM APPSKEY[16] = {
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};
