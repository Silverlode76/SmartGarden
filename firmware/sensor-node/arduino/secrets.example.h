// TTN Zugangsdaten — NIEMALS ins Git!
// Kopiere diese Datei als secrets.h und trage deine TTN-Keys ein.
// Keys findest du in TTN Console → Application → End Device → Overview

// DevEUI: aus TTN Console → LSB (umgekehrt) eintragen
static const u1_t PROGMEM DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// AppEUI / JoinEUI: alle Nullen → LSB (bleibt gleich)
static const u1_t PROGMEM APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// AppKey: aus TTN Console → MSB (so lassen wie angezeigt)
static const u1_t PROGMEM APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
