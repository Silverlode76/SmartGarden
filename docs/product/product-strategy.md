# SmartGarden — Produktstrategie & Portfolio

> **Stand:** 2026-05  
> **Status:** Draft v0.2

---

## Vision

SmartGarden ist ein **Off-Grid IoT Ökosystem** aus zwei unabhängigen Produktlinien,
die überall dort funktionieren wo es kein Strom, kein WLAN und keine Infrastruktur gibt.

```
SmartGarden
├── 🌱 Garden  — automatisch bewässern
└── 🔒 Guard   — sofort alarmieren
```

Beide Linien nutzen dieselbe App und dasselbe LoRaWAN-Netzwerk (TTN).
Die Hardware ist bewusst **spezialisiert** — kein Kompromiss-Gerät für alles.

---

---

# 🌱 Produktlinie: SmartGarden Garden

> *„Deine Pflanzen werden gegossen — auch wenn du nicht da bist."*

## Zielgruppe

| Segment | Beschreibung |
|---|---|
| Schrebergärtner | Parzelle mit Regentonne, kein Wasseranschluss |
| Hobbygärtner | Garten am Haus, Urlaub = Pflanzen tot |
| Gewächshausbesitzer | Konstante Feuchte wichtig, manuelle Kontrolle lästig |
| Balkongärtner | Keine Zeit zum täglichen Gießen |

## Kern-Problem

Pflanzen gehen ein wenn man nicht da ist.
Bestehende Lösungen versagen off-grid:
- **MIYO**: braucht Wasserhahn mit Druck → geht nicht am Fass
- **Gardena AquaBloom**: hat Pumpe, aber keine App, keine Sensorik

## Produkte

### Garden Home
*Standard-Bewässerung für eine Parzelle / einen Garten*

| | |
|---|---|
| **Preis** | ~65€ |
| **BOM** | ~38€ |
| **Einsatz** | 1 Bewässerungszone, 1 Bodenfeuchtesensor |

**ECU — Irrigator Node:**
```
[TTGO LoRa32 V2.1]
  GPIO12  → [R5 1kΩ] → [IRLZ44N Gate]  → Pumpensteuerung
  GPIO33  → Bodenfeuchte VCC (schaltbar)
  GPIO34  ← Bodenfeuchte AOUT (ADC)
  GPIO35  ← VBAT Spannungsteiler (ADC)
  GPIO21/22 → I2C (optional: BME280)
  
[XL6009 Boost]  3,7V → 12V  → [12V Tauchpumpe]
[TP4056 Modul]  Solar / USB  → [2× 18650]
```

**Sensor-Set:**
- Kapazitiver Bodenfeuchtesensor v1.2
- Optional: BME280 (Temp/Feuchte/Luftdruck)

**Bewässerungslogik:**
```
alle 15 Min:
  Bodenfeuchte messen
  wenn Feuchte < Schwellwert (z.B. 40%):
    Pumpe 30 Sek. an
  Daten per LoRaWAN senden
  Deep Sleep
```

---

### Garden Pro *(Phase 3)*
*Mehrere Zonen, mehrere Beete*

| | |
|---|---|
| **Preis** | ~120€ |
| **Einsatz** | bis zu 4 Bewässerungszonen, 4 Bodenfeuchtesensoren |
| **Neu** | Mehrkanal-MOSFET, größeres Solar-Panel (10W), größerer Akku |

---

## Garden — Markt & Zahlen

| Kennzahl | Wert |
|---|---|
| Schrebergarten-Parzellen DE | ~1,4 Mio. |
| Hobbygärten mit Regentonne (Schätzung) | ~2,0 Mio. |
| **TAM Deutschland** | **~3,4 Mio. Standorte** |
| Technikaffine Early Adopter (15%) | ~510.000 |
| Zahlungsbereit ~65€ (40%) | ~204.000 |
| **Realistisch Jahr 1–2 (3%)** | **~6.000 Einheiten** |

**Umsatzpotenzial Jahr 1–2:** ~390.000€ (6.000 × 65€)  
**Bruttomarge:** ~42% → ~164.000€

---

---

# 🔒 Produktlinie: SmartGarden Guard

> *„Du weißt sofort wenn jemand kommt — nicht erst beim nächsten Kontrollgang."*

## Das Wildkamera-Problem

Viele Besitzer installieren Wildkameras gegen Diebstahl:
- ✅ Foto vorhanden
- ❌ Benachrichtigung erst beim nächsten Kontrollgang
- ❌ Dieb ist längst weg

**Guard löst das eigentlich wichtigere Problem:**
Push-Alarm in dem Moment wo jemand eindringt — während man noch reagieren kann.

```
Wildkamera:  Dieb kommt → Foto ✅ → Besitzer findet es TAGE SPÄTER ❌
Guard:       Dieb kommt → kein Foto → Push-Alarm SOFORT ✅ → Polizei kommt noch
Guard + Cam: Dieb kommt → Foto ✅  → Push-Alarm SOFORT ✅ → beste Lösung
```

## Zielgruppen & Einsatzgebiete

| Segment | Problem | Marktgröße DE |
|---|---|---|
| 🏡 Laubenbesitzer | Einbruch in Gartenlaube | ~1,12 Mio. |
| 🍇 Winzer | Wilddiebstahl ganze Ernte | ~20.000 Betriebe |
| 🌲 Waldbesitzer / Forstbetriebe | Holzdiebstahl | ~2 Mio. Waldbesitzer |
| 🏗️ Baustellen | Bagger, Generatoren, Werkzeug gestohlen | ~400.000 Baustellen/Jahr |
| 🚜 Landwirtschaft | Traktoren, Anhänger, Ernte | ~250.000 Betriebe |
| ⛺ Ferienhäuser / Jagdhütten | Einbruch in unbewohnte Gebäude | ~370.000 |

## Produkte

### Guard Home
*Einbruchschutz für Laube, Keller, Schuppen*

| | |
|---|---|
| **Preis** | ~45€ |
| **BOM** | ~25€ |
| **Einsatz** | 1 Gebäude, Tür/Fenster-Montage |

**ECU — Guard Node:**
```
[TTGO LoRa32 V2.1]
  GPIO13  ← AM312 PIR OUT
  GPIO14  ← SW-420 Vibration OUT
  GPIO21/22 → I2C BME280 (Temp/Feuchte)
  GPIO35  ← VBAT Spannungsteiler (ADC)

[TP4056 Modul]  Solar / USB  → [2× 18650]
Kein XL6009, kein MOSFET, kein Boost  → einfacher, günstiger
```

**Alarm-Logik (Fehlalarm-Prävention):**
```
Alarm NUR wenn:
  PIR auslöst UND SW-420 innerhalb 10 Sek. auslöst
  (AND-Logik = drastisch weniger Fehlalarme)

PIR allein (Tier, Ast) → kein Alarm, nur log
Vibration allein (Wind, Regen) → kein Alarm, nur log
Beide zusammen → Push-Alarm sofort

+ Nutzer definiert Aktivzeiten (z.B. 20:00–07:00)
+ PIR muss 2× innerhalb 10s auslösen (Bestätigungspuls)
```

---

### Guard Field
*Perimeter-Schutz für Weinberg, Holzlager, Forstwirtschaft*

| | |
|---|---|
| **Preis** | ~80€ |
| **BOM** | ~45€ |
| **Neu gegenüber Home** | Externer Antennenanschluss (längere Reichweite), größeres Solar (10W), IP67-Gehäuse, Kamera-Trigger-Ausgang |

**ECU — Guard Field Node:**
```
[TTGO LoRa32 V2.1]  mit externer LoRa-Antenne
  GPIO13  ← AM312 PIR #1
  GPIO14  ← SW-420 Vibration
  GPIO15  ← AM312 PIR #2  (zweite Richtung)
  GPIO12  → Optokoppler → Wildkamera Trigger  ← NEU
  GPIO21/22 → I2C BME280

[6W Solar + 4× 18650]  → längere Autonomie
[IP67 Gehäuse]
```

**Wildkamera-Integration:**
```
Guard Field erkennt Eindringling
  → GPIO12 HIGH (50ms Puls)
  → Optokoppler schließt Wildkamera-Shutter-Kontakt
  → Wildkamera macht Foto (lokal auf SD)
  → Guard sendet LoRaWAN-Alarm mit Zeitstempel
  → Nutzer sieht Alarm + kann zur Kamera gehen für Beweisfoto

Kompatibel mit: günstigen Wildkameras (~30–50€) mit ext. Trigger
```

---

### Guard Asset *(Phase 4)*
*Diebstahlschutz für bewegliche Objekte: Baumaschinen, Traktoren, Anhänger*

| | |
|---|---|
| **Preis** | ~95€ |
| **Neu** | GPS-Modul (NEO-6M), LTE-Backup wenn kein LoRa |
| **Logik** | Erschütterung → GPS-Position → Alarm + Live-Tracking |

**ECU — Asset Tracker Node:**
```
[ESP32 + SX1262 LoRa]  oder Heltec LoRa32
  + [NEO-6M GPS]  → Position
  + [ADXL345 Beschleunigungssensor]  → Bewegungserkennung
  + [SIM800L]  → GSM Fallback wenn kein TTN-Gateway in der Nähe

Eingebaut in wasserdichtes Gehäuse, magnetisch befestigt
```

> ⚠️ Guard Asset ist ein anderes Produkt-Paradigma (B2B, Versicherung, Fleet).
> Separates Projekt-Backlog empfohlen.

---

## Guard — Markt & Zahlen

| Segment | TAM | SAM (technikaffin) |
|---|---|---|
| Laubenbesitzer | 1,12 Mio. | ~168.000 |
| Winzer + Landwirtschaft | 270.000 Betriebe | ~40.000 |
| Baustellen (B2B) | 400.000/Jahr | ~20.000 |
| Waldbesitzer + Jagd | 2,4 Mio. | ~50.000 |
| **Guard gesamt TAM** | **~4,2 Mio.** | **~278.000** |

**Umsatzpotenzial Guard Home Jahr 1–2:**  
5.000 Einheiten × 45€ = **225.000€** / Marge ~44% = **99.000€**

---

---

# Gemeinsame Infrastruktur

## App (eine App für beide Linien)

```
SmartGarden App
├── Meine Geräte
│   ├── 🌱 Garden Home — Parzelle Nord
│   │     Feuchte: 42% · Pumpe: aus · Akku: 87%
│   └── 🔒 Guard Home — Laube
│         Status: Aktiv · Letzte Bewegung: gestern 22:14
├── Alarme
│   └── 🔴 22:14 — Bewegung + Erschütterung (Laube)
└── Einstellungen
      Aktivzeiten · Schwellwerte · Benachrichtigungen
```

## LoRaWAN Gateway (Vereinsmodell)

```
1 Gateway (~80€) im Kleingartenverein
  → deckt alle Parzellen (2–5 ha Radius)
  → alle Mitglieder nutzen denselben Gateway
  → Verein kauft Gateway, Mitglieder kaufen Nodes
  → Virales Wachstum: 1 Pilot → ganzer Verein
```

---

# Roadmap

| Version | Zeitraum | Inhalt |
|---|---|---|
| `v0.2` | 2026 Q2 | Garden Home Prototyp: Pumpe + Feuchte + LoRaWAN |
| `v0.3` | 2026 Q3 | Guard Home Prototyp: PIR + Vibration + AND-Logik |
| `v0.4` | 2026 Q3 | App MVP: beide Nodes, Push-Alarm |
| `v0.5` | 2026 Q4 | Feldtest: 3 Schrebergärten (Garden + Guard) |
| `v0.6` | 2027 Q1 | Guard Field: externe Antenne + Wildkamera-Trigger |
| `v1.0` | 2027 Q2 | Markteinführung: Bundle + Amazon FBA |
| `v1.5` | 2027 Q4 | Garden Pro: Mehrkanal-Bewässerung |
| `v2.0` | 2028 | Guard Asset: GPS + GSM für Baumaschinen/Traktoren |

---

# Marktpotenzial — Erweiterte Analyse

## Europäischer Markt

### Schrebergärten & Off-Grid Standorte Europa

| Land | Schrebergärten | Off-Grid Hütten (Schätzung) | Gesamt |
|---|---|---|---|
| 🇩🇪 Deutschland | ~1.400.000 | ~370.000 | **~1.770.000** |
| 🇬🇧 United Kingdom | ~330.000 | ~200.000 | **~530.000** |
| 🇫🇷 Frankreich | ~200.000 | ~300.000 | **~500.000** |
| 🇳🇱 Niederlande | ~250.000 | ~80.000 | **~330.000** |
| 🇦🇹 Österreich | ~100.000 | ~120.000 | **~220.000** |
| 🇨🇭 Schweiz | ~70.000 | ~100.000 | **~170.000** |
| 🇸🇪🇩🇰🇧🇪 Weitere | ~400.000 | ~300.000 | **~700.000** |
| **Europa gesamt** | **~2.750.000** | **~1.470.000** | **~4.220.000** |

### TAM Europa (Hardware)

| Produkt | Standorte | Preis | TAM |
|---|---|---|---|
| Garden Home | ~4,2 Mio. | 65€ | **~273 Mio. €** |
| Guard Home | ~4,2 Mio. | 45€ | **~189 Mio. €** |
| Guard Field (B2B) | ~5 Mio. Betriebe | 80€ | **~400 Mio. €** |
| **Europa Hardware TAM** | | | **~862 Mio. €** |

### TAM SaaS-Komponente (Cloud Abo 3€/Monat)

| Szenario | Aktive Nodes | MRR | ARR |
|---|---|---|---|
| Konservativ (0,5% Penetration) | ~21.000 | ~63.000€ | **~756.000€** |
| Realistisch (2% Penetration) | ~84.000 | ~252.000€ | **~3,0 Mio. €** |
| Optimistisch (5% Penetration) | ~211.000 | ~633.000€ | **~7,6 Mio. €** |

---

## 3-Jahres-Umsatzprojektion

| Jahr | Nodes verkauft | Ø Preis | Hardware-Umsatz | SaaS ARR | **Gesamt** |
|---|---|---|---|---|---|
| 2026 (Jahr 1) | 500 | 60€ | 30.000€ | 5.400€ | **~35.000€** |
| 2027 (Jahr 2) | 5.000 | 58€ | 290.000€ | 54.000€ | **~344.000€** |
| 2028 (Jahr 3) | 20.000 | 55€ | 1.100.000€ | 216.000€ | **~1,3 Mio. €** |

> **Annahmen:** 30% Nodes wechseln zu Pro-Abo (3€/Monat), Preisreduktion durch Skalierung,
> Vertrieb: Jahr 1 Community, Jahr 2 Amazon DE + AT + CH, Jahr 3 EU + B2B Guard Field.

### Break-Even-Analyse

```
Entwicklungskosten (Schätzung):
  Zeit: ~400h × 80€/h (Opportunitätskosten) = 32.000€
  Material Prototypen (×5):                  =  1.500€
  App-Entwicklung (Flutter, extern):          = 15.000€
  Gesamt Invest:                             = ~48.500€

Break-Even:
  Marge ~42% × Ø 60€ = ~25€ Deckungsbeitrag pro Node
  48.500€ / 25€ = ~1.940 Nodes

→ Break-Even bei ca. 2.000 verkauften Nodes
→ Erreichbar Ende Jahr 2 (2027)
```

---

## Vergleichbare Unternehmen & Exits

| Unternehmen | Kategorie | Bewertung / Exit | Relevanz |
|---|---|---|---|
| **Netatmo** | Smart Home IoT (Wetter, Kamera, Alarm) | ~70 Mio. € (Legrand, 2018) | Nische IoT Consumer wird aufgekauft |
| **Gardena** | Bewässerung | Teil von Husqvarna, >800 Mio. € Umsatz | Direkter Markt — großes etabliertes Segment |
| **MIYO** | LoRaWAN Bewässerung | Startup, ~5 Mio. € Finanzierung | Direkter Wettbewerber, validiert Markt |
| **Pixie Systems** | IoT Asset Tracking | ~50 Mio. $ (Zebra Technologies, 2021) | Guard Asset Analogie |
| **Ring** | Alarm + Kamera Consumer | ~1 Mrd. $ (Amazon, 2018) | Guard-Markt validiert auf großer Skala |

> **Kernaussage:** Der IoT-Consumer-Security und Irrigation-Markt ist real, wächst,
> und wird von strategischen Käufern (Gardena/Husqvarna, Ring/Amazon, Legrand) aktiv konsolidiert.
> SmartGarden adressiert das einzige unbesetzte Segment: **off-grid + pump + alarm**.

---

## Wettbewerbsmatrix (erweitert)

```
                    hoher Preis
                         │
              Ring ●     │      Gardena Smart
              (Alarm)    │      System ●
                         │
kein LoRa ───────────────┼─────────────── LoRaWAN
                         │
    Gardena   ●          │      MIYO ●
    AquaBloom            │      (braucht Wasserhahn)
                         │
                    niedriger Preis
                         
SmartGarden ● (rechts unten: LoRaWAN + günstig)
  → einzige Lösung mit Pumpe + LoRaWAN + Alarm + <100€
```

---

# Kritische Annahmen

| # | Annahme | Risiko | Validierung |
|---|---|---|---|
| 1 | Nutzer zahlen ~65€ für Bewässerung | Mittel | Prototyp + 5 Nutzerinterviews |
| 2 | AND-Logik reduziert Fehlalarme ausreichend | Hoch | Feldtest v0.3 |
| 3 | LoRaWAN-Reichweite reicht für Weinberg/Baustelle | Mittel | Reichweitentest mit ext. Antenne |
| 4 | Wildkamera-Trigger funktioniert mit gängigen Modellen | Niedrig | Labortest mit 3 Kameramodellen |
| 5 | Vereine kaufen Gateway als Gemeinschaft | Mittel | 2 Pilotvereine ansprechen |
| 6 | Break-Even bei 2.000 Nodes erreichbar bis Ende 2027 | Mittel | Traction aus Community-Phase validiert |
