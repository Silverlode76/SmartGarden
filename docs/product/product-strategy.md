# SmartGarden — Produktstrategie & Portfolio

> **Stand:** 2026-05  
> **Status:** Draft v0.1

---

## Vision

SmartGarden ist das erste **Off-Grid Smart Gardening Ökosystem** —
eine Familie spezialisierter Nodes, die überall dort funktionieren wo es
kein Strom, kein WLAN und keinen Wasseranschluss gibt.

---

## Kerninsight: Zwei verschiedene User, zwei Produkte

Die ursprüngliche Idee (alles in einem Node) scheitert an der **Physik**:
Bewässerung findet am Wasserfass/Beet statt, Einbruchschutz an der Laube/Tür.
Ein zentraler Node würde lange Kabel in alle Richtungen erfordern.

**Lösung:** Spezialisierte Nodes, die gemeinsam in einer App zusammenspielen.

---

## Produktportfolio

### 🌱 SmartGarden Irrigator

> *„Gieß automatisch — auch wenn du nicht da bist."*

| | |
|---|---|
| **Zielgruppe** | Schrebergärtner, Hobbygärtner mit Regentonne, Gewächshausbesitzer |
| **Kern-Problem** | Pflanzen gehen ein wenn man nicht da ist — manuelle Bewässerung vergessen |
| **Kern-Lösung** | Bodenfeuchte messen → automatisch pumpen → per App kontrollieren |
| **Hardware** | TTGO LoRa32 + kapazitiver Bodenfeuchtesensor + 12V Tauchpumpe + XL6009 Boost + Solar |
| **Kommunikation** | LoRaWAN 868 MHz (TTN) |
| **Stromversorgung** | 6W Solar + 2× 18650 — autark |
| **Zielpreis** | ~65€ |
| **BOM-Kosten** | ~38€ |
| **Bruttomarge** | ~42% |

**Warum kaufen Kunden das?**
- Gardena AquaBloom hat Pumpe, aber keine App und keine Sensorik
- MIYO hat App + Sensor, aber braucht Wasserhahn mit Druck (geht nicht am Fass!)
- SmartGarden Irrigator ist das **einzige System** das App + Sensor + Pumpe + kein Wasserhahn kombiniert

---

### 🔒 SmartGarden Guard

> *„Weißt du immer was in deiner Laube passiert."*

| | |
|---|---|
| **Zielgruppe** | Laubenbesitzer, Wochenendhaus, Jagdhütte, Ferienwohnung off-grid |
| **Kern-Problem** | Einbrüche in Gartenlauben nehmen zu — keine Alarmanlage ohne Strom/WLAN möglich |
| **Kern-Lösung** | Bewegung + Erschütterung erkennen → sofortiger Push-Alarm aufs Handy |
| **Hardware** | TTGO LoRa32 + AM312 PIR + SW-420 Vibrationssensor + BME280 + Solar |
| **Kommunikation** | LoRaWAN 868 MHz (TTN) |
| **Stromversorgung** | 6W Solar + 2× 18650 — autark, **Jahre ohne Wartung** |
| **Zielpreis** | ~45€ |
| **BOM-Kosten** | ~25€ |
| **Bruttomarge** | ~44% |

**Warum kaufen Kunden das?**
- Bestehende Alarmanlagen brauchen Strom + WLAN — off-grid unmöglich
- GSM-Alarmanlagen brauchen SIM-Karte + monatliche Kosten
- SmartGarden Guard nutzt das **Vereins-Gateway** (ein Gateway = alle Parzellen geschützt)

---

### 🌱🔒 SmartGarden Bundle

> *„Das Komplett-Set für den Kleingarten."*

| | |
|---|---|
| **Zielgruppe** | Schrebergärtner MIT Laube (~80% aller Parzellen) |
| **Inhalt** | 1× Irrigator + 1× Guard + gemeinsame App |
| **Zielpreis** | ~100€ (statt ~110€ einzeln) |
| **USP** | Eine App, ein Gateway, alles vernetzt |

---

## Positionierung im Wettbewerb

| Feature | MIYO LoRaWAN | Gardena AquaBloom | **Irrigator** | **Guard** | **Bundle** |
|---|---|---|---|---|---|
| Pumpe (kein Wasserhahn) | ❌ | ✅ | ✅ | — | ✅ |
| Solar / autark | ✅ | ✅ | ✅ | ✅ | ✅ |
| App / Fernzugriff | ✅ | ❌ | ✅ | ✅ | ✅ |
| Bodenfeuchtesensor | ✅ | ❌ | ✅ | — | ✅ |
| LoRaWAN (kein WLAN) | ✅ | ❌ | ✅ | ✅ | ✅ |
| Einbruchalarm / PIR | ❌ | ❌ | — | ✅ | ✅ |
| Vereins-Gateway | ❌ | ❌ | ✅ | ✅ | ✅ |
| Open Source | ❌ | ❌ | ✅ | ✅ | ✅ |
| **Preis** | ~250€ | ~80€ | **~65€** | **~45€** | **~100€** |

---

## Zielgruppen & Segmente

### Primär: Schrebergärtner Deutschland

| Kennzahl | Wert |
|---|---|
| Schrebergarten-Parzellen DE | ~1,4 Mio. |
| Parzellen mit Laube (~80%) | ~1,12 Mio. |
| Technikaffine Early Adopter (15%) | ~168.000 |
| Zahlungsbereit (40% davon) | ~67.000 |

### Sekundär: Off-Grid Hütten & Häuser

| Segment | Anzahl DE |
|---|---|
| Wochenendhütten | ~200.000 |
| Jagdhütten | ~50.000 |
| Ferienhäuser off-grid | ~120.000 |
| **Zusatz-TAM** | **~370.000** |

---

## Go-to-Market Strategie

### Phase 1 — DIY & Community (2026)
- Open Source auf GitHub → Maker-Community baut nach
- YouTube / Blogs: „Ich habe meinen Schrebergarten smart gemacht"
- Reddit: r/DIYIOT, r/homeautomation, dt. Maker-Foren
- Ziel: 100 aufgebaute Nodes in der Community

### Phase 2 — Kleingartenvereine (2026–2027)
- Ein Verein = 1 Gateway → deckt alle Parzellen
- Bundle-Deal: Verein kauft Gateway + 10 Nodes zum Einführungspreis
- Virales Wachstum: 1 begeisterter Gärtner → 5 Nachbarn kaufen auch
- Ziel: 10 Pilotvereine, 500 Nodes

### Phase 3 — Amazon / Retail (2027)
- FBA (Fulfillment by Amazon): geringer Logistikaufwand
- Einzelhandel: Hornbach, toom Baumarkt (Gartenabteilung)
- Ziel: 5.000 Nodes/Jahr

### Phase 4 — OEM / Lizenz (2028+)
- Hardware-Design lizenzieren an Gartengerätehersteller
- White-Label für Kleingärtner-Verbände
- Ziel: Lizenzeinnahmen ohne eigene Fertigung

---

## Produktentwicklungs-Roadmap

```
v0.2  (2026 Q2)  →  Irrigator Prototyp
                      Pumpe + Bodenfeuchte + LoRaWAN + Deep Sleep

v0.3  (2026 Q3)  →  Guard Prototyp
                      PIR + Vibration + Push-Alarm + LoRaWAN

v0.4  (2026 Q3)  →  App MVP
                      Flutter: beide Nodes in einer UI, Push-Notifications

v0.5  (2026 Q4)  →  Feldtest
                      3 Schrebergärten Pilotbetrieb (Irrigator + Guard)

v1.0  (2027 Q1)  →  Markteinführung
                      Bundle: App Store + Amazon FBA
```

---

## Kritische Annahmen

1. **LoRaWAN-Abdeckung:** TTN-Gateway muss im/nahe dem Verein verfügbar sein
   oder Verein kauft eigenes Gateway (~80€) → Vereinsmodell trägt das
2. **Zahlungsbereitschaft:** Schrebergärtner zahlen ~65€ für Bewässerungsautomatik
   → Vergleich: Gardena AquaBloom ~80€ ohne App/Sensor → plausibel
3. **Pumpe funktioniert mit Regentonne:** Ansaughöhe, Förderhöhe, Wasserqualität
   → im Feldtest zu validieren (v0.3)
4. **LoRaWAN-Reichweite reicht für Verein:** SF9, 868 MHz, ~1–2km Reichweite
   → ein Gateway pro Verein (typisch 2–5 ha Fläche) sollte reichen

---

## Risiken

| Risiko | Wahrscheinlichkeit | Impact | Mitigation |
|---|---|---|---|
| LoRaWAN-Coverage fehlt | Mittel | Hoch | Vereins-Gateway-Paket anbieten |
| Pumpe versagt im Feldtest | Niedrig | Hoch | 2 Pumpenmodelle testen (v0.3) |
| Gardena kopiert Konzept | Mittel | Mittel | Open Source = Community-Moat |
| Preis zu hoch für Zielgruppe | Niedrig | Hoch | Bundle-Rabatt, Ratenzahlung |
| Akku-Lebensdauer < 1 Saison | Niedrig | Hoch | Energiebudget bereits berechnet: 88 Tage ohne Solar |
