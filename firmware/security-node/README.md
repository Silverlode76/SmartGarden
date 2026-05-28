# SmartGarden Guard — PoC v0.2

**Hardware:** TTGO LoRa32 V2.1 + PIR-Sensor (HC-SR501 oder AM312)  
**Stromversorgung:** 2× 18650 Li-Ion parallel (3,7V) — kein Solar für PoC  
**Protokoll:** LoRaWAN OTAA → TTN → ntfy.sh → Push-Notification  
**Ziel:** Bewegung erkannt → Push auf Handy in <30 Sekunden

---

## Hardware-Aufbau

### Verdrahtung

```
TTGO LoRa32 V2.1          PIR-Sensor (HC-SR501)
─────────────────          ─────────────────────
3V3  ──────────────────►  VCC   (⚠️ nur für AM312 — siehe Hinweis)
GND  ──────────────────►  GND
GPIO13 ◄────────────────  OUT   (Signalausgang)


Stromversorgung:
2× 18650 (parallel)  →  BAT-Pin am TTGO  (+ und -)
```

> ⚠️ **HC-SR501 braucht 5V, AM312 braucht 3,3V:**
>
> | Sensor | VCC | TTGO-Anschluss |
> |--------|-----|----------------|
> | **HC-SR501** (Arduino-Kit) | 5V | `5V`-Pin — nur wenn USB angeschlossen! |
> | **AM312** (empfohlen für Akku) | 3,3V | `3V3`-Pin |
>
> **Für den PoC (USB-Betrieb):** HC-SR501 an `5V`-Pin → funktioniert einwandfrei.  
> **Für Akku-Betrieb ohne USB:** AM312 verwenden (3,3V-kompatibel, ~2€).  
> Der `5V`-Pin des TTGO liefert nur Spannung wenn USB angeschlossen ist.

### Akku-Anschluss (2× 18650 parallel)

```
18650 Zelle 1 (+) ─┬─► TTGO BAT (+)
18650 Zelle 2 (+) ─┘

18650 Zelle 1 (−) ─┬─► TTGO BAT (−) / GND
18650 Zelle 2 (−) ─┘
```

> Beide Zellen **vor dem Parallelschalten auf gleiche Spannung** bringen  
> (max. 50 mV Differenz). Gleicher Hersteller/Charge empfohlen.

**Geschätzte Akkulaufzeit (PoC, kein Sleep):**
- 2× 18650 ≈ 5000 mAh
- TTGO Dauerbetrieb ≈ 60–80 mA (WiFi aus, nur LoRa)
- Laufzeit ≈ **62–83 Stunden** (~2,5–3,5 Tage)

---

## Software-Setup

### 1. PlatformIO-Projekt öffnen

```bash
cd firmware/security-node
```

Öffne den Ordner in VS Code mit PlatformIO-Extension.

### 2. secrets.h anlegen

```bash
cp src/secrets.example.h src/secrets.h
```

Die `secrets.h` bleibt leer bis Schritt 3 (TTN-Setup).

### 3. TTN-Gerät registrieren

**a) TTN-Konto anlegen (falls noch nicht vorhanden):**  
→ [https://console.cloud.thethings.network](https://console.cloud.thethings.network)  
→ Region: `eu1` wählen

**b) Application erstellen:**
- "Create application"
- Application ID: `smartgarden-guard`
- Name: `SmartGarden Guard`

**c) Device registrieren:**
- "Register end device"
- → "Enter end device specifics manually"
- Frequency plan: `Europe 863-870 MHz (SF9 for RX2)`
- LoRaWAN version: `LoRaWAN Specification 1.0.3`
- → "Generate" bei JoinEUI (AppEUI) und DevEUI
- → "Generate" bei AppKey
- End device ID: `guard-poc-01`
- → "Register end device"

**d) Credentials in secrets.h eintragen:**

In TTN Console: Device → Overview

| TTN-Feld | Format | In secrets.h |
|----------|--------|-------------|
| DevEUI | **lsb** anklicken → kopieren | `DEVEUI[]` |
| AppEUI / JoinEUI | **lsb** anklicken → kopieren | `APPEUI[]` |
| AppKey | **msb** anklicken → kopieren | `APPKEY[]` |

```cpp
// Beispiel secrets.h (eigene Werte eintragen):
static const u1_t PROGMEM DEVEUI[8]  = { 0x01, 0x02, 0x03, ... }; // lsb
static const u1_t PROGMEM APPEUI[8]  = { 0x00, 0x00, 0x00, ... }; // lsb
static const u1_t PROGMEM APPKEY[16] = { 0x2B, 0x7E, ... };        // msb
```

### 4. Firmware flashen

```bash
pio run --target upload
pio device monitor
```

Erwartete Serial-Ausgabe nach Start:
```
╔══════════════════════════════════════╗
║   SmartGarden Guard PoC — v0.2       ║
╚══════════════════════════════════════╝
[PIR] Interrupt aktiviert (RISING)
[TTN] OTAA Join gestartet — warte auf Gateway...
[TTN] OTAA Join wird versucht...
[TTN] ✓ Joined! Netzwerk-Session aktiv.
[PIR] Warte auf Bewegung...
```

> **Kein Join?** → TTN-Coverage prüfen: [TTN Mapper](https://ttnmapper.org) für deinen Standort.  
> LoRa-Reichweite 2–5 km → oft reicht ein Gateway im Umkreis.

---

## Push-Notification einrichten (ntfy.sh)

Für den PoC verwenden wir **ntfy.sh** — kostenlos, kein Backend nötig.

### Schritt 1: Eigenes Topic wählen

Wähle einen **einzigartigen Topic-Namen**, z.B.:  
`smartgarden-guard-oliver2026`  
(öffentlich → kein Login nötig, aber auch für andere sichtbar wenn sie den Namen kennen)

### Schritt 2: ntfy App installieren

- Android: [ntfy im Play Store](https://play.google.com/store/apps/details?id=io.heckel.ntfy)
- iOS: [ntfy im App Store](https://apps.apple.com/app/ntfy/id1625396347)

App öffnen → **"+" → Topic eingeben:** `smartgarden-guard-oliver2026`

### Schritt 3: TTN Webhook konfigurieren

In TTN Console: **Application → Integrations → Webhooks → Add Webhook**

| Feld | Wert |
|------|------|
| Webhook ID | `ntfy-push` |
| Webhook format | `JSON` |
| Base URL | `https://ntfy.sh/smartgarden-guard-oliver2026` |

**Additional headers:**

| Header-Name | Wert |
|-------------|------|
| `Title` | `🚨 SmartGarden Guard` |
| `Priority` | `urgent` |
| `Tags` | `rotating_light,de` |

**Enabled event types:** ✅ Uplink message

→ **"Add webhook"**

### Schritt 4: Payload Formatter einrichten (optional, für lesbare Notification)

In TTN Console: **Application → Payload Formatters → Uplink → Custom Javascript**

```javascript
function decodeUplink(input) {
  if (input.bytes[0] === 0x01) {
    return {
      data: {
        alert: "Bewegung erkannt!",
        counter: input.bytes[1],
        rssi: input.rx_metadata ? input.rx_metadata[0].rssi : null
      },
      warnings: [],
      errors: []
    };
  }
  return { data: { raw: input.bytes }, errors: [] };
}
```

---

## Testen

1. Hand vor den PIR-Sensor halten
2. Serial Monitor: `🚨 BEWEGUNG ERKANNT — sende LoRaWAN Alert`
3. TTN Console: Application → Live Data → Uplink erscheint
4. Handy: ntfy-Notification erscheint (typisch 5–20 Sekunden nach PIR)

**Erwarteter Ablauf im Serial Monitor:**
```
🚨 ═══════════════════════════════════ 🚨
   BEWEGUNG ERKANNT — sende LoRaWAN Alert
🚨 ═══════════════════════════════════ 🚨

[LMIC] Paket #1 gesendet (Port 1)
[TTN] TX beginnt...
[TTN] ✓ TX abgeschlossen
```

**Cooldown:** Nach einem Alert 60 Sekunden Pause (verhindert Alarm-Flut).

---

## Bekannte PoC-Einschränkungen

| Einschränkung | Auswirkung | Geplante Lösung (v0.3) |
|---------------|------------|------------------------|
| Kein Deep Sleep | 2–3 Tage Akkulaufzeit (statt Wochen) | Ext0-Wakeup auf PIR-GPIO |
| HC-SR501 braucht 5V | Nur mit USB-Anschluss | AM312 (3,3V) im Produktdesign |
| Kein Watchdog | Hänger möglich | Watchdog-Timer in v0.3 |
| ntfy.sh (öffentlich) | Nicht für Produktion | Flutter App mit FCM in v0.4 |
| Unconfirmed Uplink | Paket-Verlust möglich | Confirmed TX in kritischem Alarm |

---

## Verzeichnisstruktur

```
firmware/security-node/
├── platformio.ini
├── README.md               ← diese Datei
└── src/
    ├── main.cpp
    ├── secrets.example.h   ← Vorlage (committed)
    └── secrets.h           ← deine Credentials (in .gitignore!)
```
