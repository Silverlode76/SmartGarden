# Wettbewerbsanalyse — SmartGarden

> **Dokument-Struktur:**
> - Teil 1: Guard Home — Sicherheit & Alarm (neu)
> - Teil 2: Garden Home — Bewässerung

---

# Teil 1 — SmartGarden Guard: Wettbewerbsanalyse

## Marktkontext

Einbrüche in Schrebergärten sind kein Randproblem: Der Gesamtverband der Deutschen
Versicherungswirtschaft schätzt ~100.000 Einbrüche/Jahr in Gartenlauben in Deutschland.
Der durchschnittliche Schaden: ~500–2.000€. Bestehende Lösungen haben alle mindestens
einen kritischen Nachteil für den Off-Grid-Einsatz.

---

## Wettbewerber Guard

### 1. GSM-Alarmanlagen (Microguard, China-Module)

**Produkte:** Microguard Reed-Kontakt Melder, diverse AliExpress/Amazon-GSM-Module

| Parameter | Wert |
|-----------|------|
| Preis | ~30–80€ Hardware |
| Laufzeit | ~1 Jahr (Akku, kein Solar) |
| Alarm-Kanal | SMS oder Anruf |
| App | ❌ keine |
| Monatliche Kosten | **~1–5€/Monat** (SIM-Karte Datentarif) |
| Solar | ❌ meistens nein |
| Technologie | 2G/4G GSM |

**Stärken:**
- ✅ Funktioniert überall mit Mobilfunknetz
- ✅ Günstig in der Anschaffung
- ✅ Kein Gateway nötig

**Schwächen:**
- ❌ **Laufende SIM-Kosten** (über 3 Jahre = 36–180€ zusätzlich)
- ❌ **Kein Solar** → jährlicher Batteriewechsel nötig
- ❌ **Nur SMS/Anruf** — kein Push, kein App-Verlauf
- ❌ **Keine Smart-Logik** (1 Sensor = 1 Alarm, viele Fehlalarme)
- ❌ **Kein Verlauf, kein Dashboard**

> **Fazit:** Günstige Einstiegslösung, aber laufende SIM-Kosten fressen Ersparnis.
> Kein App-Erlebnis. Nach 3 Jahren günstiger als SmartGarden nur auf dem Papier.

---

### 2. Daitem D22 (professionelle Funk-Alarmanlage)

| Parameter | Wert |
|-----------|------|
| Preis | ~300–800€ (inkl. Sensoren, ohne Installation) |
| Laufzeit | ~5 Jahre (Langzeitbatterien) |
| Alarm-Kanal | GSM-Modul optional (extra ~80€ + SIM) |
| App | rudimentär / über Zentrale |
| Monatliche Kosten | SIM-Karte + optional Notruf-Zentrale |
| Solar | ❌ |
| Installation | empfohlen durch Fachbetrieb |

**Stärken:**
- ✅ Professionelle Qualität, 5 Jahre Laufzeit
- ✅ 7 Sensortypen, Tamper-Schutz, Hundebellen-Abschreckung
- ✅ Tierimmun (kein Fehlalarm durch Katzen)

**Schwächen:**
- ❌ **Massiv überdimensioniert** für Schrebergarten (~400–800€)
- ❌ **GSM-Modul kostet extra** + laufende SIM-Kosten
- ❌ **Kein Solar** → nicht autark
- ❌ **Kein Open-Source-Konzept**
- ❌ Zielgruppe: Haus/Gewerbe, nicht Laube

> **Fazit:** Goldene Lösung für die falsche Zielgruppe. Niemand investiert 600€
> in den Schutz einer 2.000€-Laube.

---

### 3. VisorTech Solar-Alarmsirene (Pearl.de)

| Parameter | Wert |
|-----------|------|
| Preis | ~30–60€ |
| Laufzeit | Solar + Akku → autark |
| Alarm-Kanal | **Nur lokale Sirene (110 dB)** |
| App | ❌ keine |
| Monatliche Kosten | **0€** |
| Solar | ✅ |

**Stärken:**
- ✅ Solar → autark, keine Batteriewechsel
- ✅ Kein Abo, keine SIM
- ✅ Günstig
- ✅ Schreckt ab

**Schwächen:**
- ❌ **Kein Remote-Alert** — du weißt erst vom Einbruch wenn du vor Ort bist
- ❌ **Kein App, kein Push, kein Verlauf**
- ❌ Nur Abschreckung, keine Beweissicherung (kein Zeitstempel)
- ❌ Niemand hört die Sirene wenn die Laube leer ist

> **Fazit:** Schreckt ab wenn jemand zuhört — im leeren Schrebergarten wirkungslos.
> Das eigentliche Kundenproblem ("Ich will sofort wissen wenn jemand da ist") löst es nicht.

---

### 4. YoLink (LoRa-basiert, proprietär)

| Parameter | Wert |
|-----------|------|
| Preis | Hub ~30$ + Sensor ~25–35$ |
| Technologie | Proprietäres LoRa (nicht LoRaWAN/TTN) |
| Alarm-Kanal | Push-App ✅ |
| App | ✅ gut |
| Monatliche Kosten | **0€** (Cloud kostenlos) |
| Hub-Anforderung | **Hub braucht WiFi/Internet** |
| EU-Verfügbarkeit | begrenzt (EC-Modelle, 868 MHz) |

**Stärken:**
- ✅ Gute App, echte Push-Notification
- ✅ Keine monatlichen Kosten
- ✅ LoRa-Reichweite (~400m)

**Schwächen:**
- ❌ **Hub muss zu Hause im WLAN hängen** → Schrebergarten ohne WLAN: kein Gateway
- ❌ **Proprietäres LoRa** → kein TTN, keine Community-Gateways nutzbar
- ❌ Kein Solar an Sensoren (Batterie 1–2 Jahre)
- ❌ In Deutschland/EU kaum im Handel, kein deutscher Support
- ❌ Hauptmarkt USA (915 MHz dominiert das Ökosystem)

> **Fazit:** Technisch näher an SmartGarden als alle anderen. Scheitert aber am
> Hub-Problem: der Hub braucht Internet am Standort. Für Schrebergärten ohne
> WLAN/Strom grundsätzlich ungeeignet — es sei denn der Nutzer baut seinen
> eigenen Gateway. Genau das ist SmartGardens Differenzierungspunkt.

---

### 5. Wildkamera mit 4G (Reolink, Browning, Victure)

| Parameter | Wert |
|-----------|------|
| Preis | ~80–200€ |
| Technologie | 4G/LTE SIM-Karte |
| Alarm-Kanal | Foto per App / SMS ✅ |
| App | ✅ mit Foto-Beweis |
| Monatliche Kosten | **~5–15€/Monat** (4G-Datentarif) |
| Solar | optional (Zusatzpanel ~30€) |
| Energieverbrauch | hoch (Kamera) → Solar oft nicht ausreichend |

**Stärken:**
- ✅ Foto/Video als Beweis → bestes Feature für Polizei
- ✅ Bekanntes Produkt, einfaches Setup
- ✅ 4G → funktioniert überall mit Mobilfunk

**Schwächen:**
- ❌ **Hohe laufende Kosten** (4G-SIM ~5–15€/Monat = 60–180€/Jahr)
- ❌ **Hoher Energieverbrauch** → Solar-Betrieb schwierig, häufige Akkuladung
- ❌ Kamera-Linse sichtbar → Einbrecher erkennen und zerstören/drehen sie
- ❌ Kein Bodenfeuchte/Temperatur-Sensor kombinierbar
- ❌ Datenschutzbedenken (Video-Footage in Cloud)

> **Fazit:** Stark für Beweissicherung, schwach für autonomen Dauerbetrieb.
> SmartGarden Guard ist kein Kamera-Ersatz — kann aber als Wildkamera-Trigger
> dienen (Guard Field v0.6: Schock-Kontakt löst Wildkamera aus).

---

## Guard — Wettbewerbsmatrix

| Feature | GSM-Alarm | Daitem D22 | VisorTech Solar | YoLink | Wildkamera 4G | **SmartGarden Guard** |
|---------|-----------|-----------|-----------------|--------|---------------|----------------------|
| Kein WiFi/WLAN nötig | ✅ | ✅ | ✅ | ❌ Hub! | ✅ | ✅ |
| Push-App Notification | ❌ SMS | teilweise | ❌ | ✅ | ✅ | ✅ |
| Keine monatl. Kosten | ❌ SIM | ❌ SIM | ✅ | ✅ | ❌ SIM | **✅** |
| Solar-autark | ❌ | ❌ | ✅ | ❌ | ❌/optional | **✅** |
| Kein eigener Hub nötig* | ✅ | ✅ | ✅ | ❌ | ✅ | **✅ (TTN)** |
| Smart AND-Logik | ❌ | teilweise | ❌ | ❌ | ❌ | **✅ PIR+Vibration** |
| App-Verlauf / Dashboard | ❌ | ❌ | ❌ | ✅ | ✅ | **✅** |
| Kombinierbar mit Bewässerung | ❌ | ❌ | ❌ | ❌ | ❌ | **✅** |
| Open Source / DIY | ❌ | ❌ | ❌ | ❌ | ❌ | **✅** |
| Preis (Hardware) | 30–80€ | 300–800€ | 30–60€ | ~60€ | 80–200€ | **~45€** |
| Laufende Kosten/Jahr | 12–60€ | 12–60€ | 0€ | 0€ | 60–180€ | **0€** |

*SmartGarden Guard benötigt TTN-Gateway in Reichweite — in Städten/Vororten meist vorhanden.
 In ländlichen Gebieten ist der SmartGarden-eigene Gateway erforderlich.

---

## Guard — Positionierung

```
                KEIN SOLAR / BATTERIE-BETRIEB
                          |
         GSM-Alarm ●      |      ● Daitem D22
                          |       (teuer, professionell)
KEIN PUSH ────────────────┼──────────────── PUSH APP
(nur SMS/Sirene)          |                (echter Alert)
                          |
    VisorTech Solar ●     |      ● YoLink (Hub braucht WiFi!)
                          |
                          |    SmartGarden Guard ●
                          |    (Solar + Push + kein Hub + 0€/Monat)
                SOLAR / AUTARK
```

**SmartGarden Guard ist die einzige Lösung die alle vier kombiniert:**
> Solar · Push-App · kein WiFi/Hub am Standort · keine monatlichen Kosten

---

## Guard — Kritische Annahmen zu validieren

| Annahme | Risiko | Test |
|---------|--------|------|
| TTN-Gateways in Schrebergärten erreichbar | Mittel | Feldtest: TTN-Coverage-Map für Top-10-Städte prüfen |
| LoRaWAN-Latenz (5–30 Sek.) akzeptabel | Niedrig | Nutzer wollen Benachrichtigung, keinen Echtzeit-Alarm |
| AND-Logik (PIR + Vibration) reduziert Fehlalarme ausreichend | Hoch | v0.2 Prototyp-Test: 1 Woche im Feld ohne Fehlalarm |
| Zahlungsbereitschaft ~45€ ohne Abo | Niedrig | Gardena ~80€ im selben Markt validiert Preispunkt |

---

# Teil 2 — SmartGarden Garden Home: Wettbewerbsanalyse

## Hauptwettbewerber: MIYO

### Stärken
- Ausgereiftes Produkt seit 2018
- Solar-betrieben, keine Batterien
- LoRaWAN-Variante verfügbar
- Gute App-UX, viele Nutzerbewertungen
- Evapotranspiration-Berechnung (intelligente Bewässerungsanpassung)

### Schwächen
- **Keine Einbrucherkennung / Alarm** ← unser USP
- **Setzt Wasseranschluss mit Druck voraus** ← unser USP
  MIYO nutzt ausschließlich Magnetventile (3/4" Gewindeanschluss) —
  funktioniert nur mit fließendem Wasser. Regentonnen / Zisternen (ohne Druck)
  werden nicht unterstützt. Für viele Schrebergärten ohne eigenen Wasseranschluss
  damit **grundsätzlich ungeeignet**.
- Cube-Serie benötigt WLAN (nur LoRaWAN-Variante ohne)
- Kein Konzept für Vereins-Gateway
- Hoher Preis (Starter-Set ca. 200-300€)
- Kein Community/Open-Source Ansatz

### Preisstruktur MIYO (ca.)
| Produkt | Preis |
|---|---|
| Starter Set (Cube + 1 Sensor + 1 Ventil) | ~250€ |
| Erweiterung (1 Sensor + 1 Ventil) | ~120€ |
| LoRaWAN Gateway | ~150€ |

---

## Weitere Wettbewerber

### ⚠️ Gardena AquaBloom — nächster Konkurrent
**Der relevanteste Wettbewerber nach MIYO**, da er wie SmartGarden Solar + Pumpe + Fass kombiniert.

**Funktionsweise:**
- 3-in-1 Haupteinheit: integrierte Pumpe + Steuerung + Solarpanel
- Saugt Wasser direkt aus Regentonne / Eimer / Reservoir
- Tropfbewässerung: bis zu 20–30 Pflanzen (Drip-Heads 0,5 l/h oder 2 l/h)
- 14 voreingestellte Bewässerungsprogramme — Auswahl per **Drehregler**
- AA-Akkus / Li-Ion als Pufferspeicher

**Stärken:**
- ✅ Pumpe integriert → kein Wasseranschluss nötig
- ✅ Solar-betrieben → kein Strom nötig
- ✅ Einfache Bedienung (Plug & Play)
- ✅ Etablierte Marke (Gardena / Husqvarna Group)

**Schwächen / Lücken:**
- ❌ **Keine App, keine Konnektivität** → kein Fernzugriff, keine Push-Nachrichten
- ❌ **Keine Bodenfeuchtesensorik** → starres Programm, nicht adaptiv
- ❌ **Kein Alarm / Sicherheit** → keine PIR-Erkennung
- ❌ **Kein LoRaWAN** → funktioniert nur lokal, keine Überwachung aus der Ferne
- ❌ Ausgelegt für Balkon-/Topfpflanzen, **nicht für Gartenbeet / größere Flächen**
- ❌ Kein Vereins-Gateway-Konzept

**Preisrahmen:** ~60–100€ (Standard) / ~120–150€ (L-Set)

> **Fazit:** Der AquaBloom beweist, dass ein Markt für Off-Grid-Bewässerung existiert.
> Er löst das Hardware-Problem, lässt aber die **Smart-Lücke** komplett offen —
> kein Fernzugriff, keine Sensorik, kein Alarm. Genau dort setzt SmartGarden an:
> **Off-Grid Smart Irrigation** — die intelligente Weiterentwicklung des AquaBloom-Konzepts.

> **Offene Designfrage (für v2):** AquaBloom und ähnliche Solar-Bewässerungssets (siehe
> Produktfotos im Handel) bauen die Pumpe **trocken ins Gehäuse** ein und saugen das
> Wasser per Schlauch aus der Tonne, statt eine Tauchpumpe direkt ins Wasser zu legen.
> SmartGarden Prototyp v0.2 nutzt aktuell eine **Tauchpumpe** (einfacher, kein
> Ansaug-/Rückschlagventil-Problem, aber Pumpenkopf dauerhaft im Wasser → Verschleiß/
> Verschmutzung). Trade-off für eine spätere Iteration evaluieren:
> Tauchpumpe (aktuell) vs. selbstansaugende Pumpe im trockenen Gehäuse (Marktstandard
> bei AquaBloom & Co.) — siehe [hardware-overview.md](../architecture/hardware-overview.md)
> Bring-up-Bericht für den aktuellen Hardware-Stand.

---

### Onherm Solar Bewässerungssystem (Amazon, Generic-Hersteller)

**Modell:** BSV-IC015, 15m Tropfschlauch — Beispiel für die "dumme" Einstiegsklasse
ohne jede Sensorik/Konnektivität.

| Parameter | Wert |
|---|---|
| Preis | ~25€ (UVP 29,99€, Angebot -17%) |
| Akku | 2200mAh, Solar-Nachladung |
| Steuerung | Nur Timer, keine App, kein Bodenfeuchtesensor |
| Verkaufszahlen | 200+ /Monat (Amazon-Anzeige) |

**Stärken:**
- ✅ Sehr günstig, Plug & Play
- ✅ Wasserentnahme direkt aus Eimer/Tonne ohne Wasserhahn

**Schwächen:**
- ❌ Keine App, kein Fernzugriff, keine Sensorik — reiner Zeitschalt-Timer
- ❌ Keine Bodenfeuchte-Anpassung, kein Alarm

> **Fazit:** Bestätigt die Preisvalidierung — selbst ganz ohne Smart-Funktionen
> verkauft sich Off-Grid-Bewässerung bei ~25€ gut. Untermauert, dass SmartGardens
> Zielpreis (~85-95€) für deutlich mehr Funktionsumfang (App, Sensorik, LoRaWAN,
> Alarm) marktfähig ist.

> **Architektur-Bestätigung:** Auch dieses System (wie AquaBloom) nutzt **keine
> Tauchpumpe** — die Pumpe sitzt trocken im Gehäuse und saugt Wasser per Schlauch
> aus dem Behälter an (IN/OUT-Anschluss, siehe Produktbild). Zusätzlich ist ein
> **Anti-Siphon-Ventil** zwischen Pumpenausgang und Pflanze verbaut, das Rückfluss
> von Wasser bei ausgeschalteter Pumpe verhindert (Höhenunterschied Tonne↔Beet).
> Damit bestätigen **mehrere** Marktprodukte unabhängig voneinander den
> Trocken-Pumpen-Ansatz als Standard — verstärkt die offene Designfrage oben:
> SmartGardens Tauchpumpen-Wahl ist die Ausnahme, nicht die Regel am Markt.

---

### Gardena Smart System
- Benötigt WLAN + Steckdose → nicht für Schrebergarten geeignet
- Kein LoRa, keine Solar-Option
- Kein Alarm

### Netro Sprite
- WiFi-basiert → nicht geeignet
- Günstig aber keine Autarkie

### Homematic IP
- Komplexes Smart-Home-System
- Nicht auf Garten spezialisiert
- Benötigt Hub + Strom

---

## Positionierung SmartGarden

```
                    HOHE AUTARKIE
                         |
          SmartGarden ●  |   ← Solar + Pumpe (kein Wasseranschluss, kein Strom nötig)
                         |
KEIN ALARM ──────────────┼────────────── ALARM
                         |
              MIYO ●     |   ← Solar, aber Wasseranschluss mit Druck zwingend nötig
                         |
                    NIEDRIGE AUTARKIE
```

### Tatsächliche Zielgruppen im Vergleich

**MIYO LoRaWAN — echte Zielgruppe:**
- Landwirte / Winzer mit großen Flächen ohne WiFi-Abdeckung
- Große Privatgärten weit vom Haus (Wasseranschluss vorhanden, aber kein WLAN)
- Gewächshäuser auf dem Grundstück ohne WLAN-Reichweite
- → Immer mit **Wasseranschluss unter Druck**

**SmartGarden — Zielgruppen (alle ohne Strom & ohne Wasseranschluss):**
| Zielgruppe | Beschreibung |
|---|---|
| 🌱 Schrebergarten | Parzelle im Kleingartenverein, nur Regentonne, kein Strom |
| 🏚️ Wochenendhütte | Hütte / Datsche ohne Versorgungsanschlüsse |
| 🏕️ Jagdhütte | Abgelegenes Revier, keine Infrastruktur |
| 🏔️ Ferienhütte / Alm | Remote-Lage, Solar als einzige Energiequelle |
| 🌿 Vereinsgelände | Sportverein, Camping, Kleintierhaltung ohne Anschlüsse |
| 🌾 Kleingarten / Nutzgarten | Garten abseits des Wohngebäudes |

**Gemeinsamer Nenner aller SmartGarden-Zielgruppen:**
> **Off-Grid-Orte** — Wasserfass vorhanden, aber kein Strom, kein WLAN, kein Wasseranschluss.
> Diese Zielgruppe wird von keinem bestehenden Smart-Irrigation-System am Markt adressiert.

### USP-Vergleich (Kernpunkte)
| Feature | MIYO LoRaWAN | Gardena AquaBloom | SmartGarden |
|---|---|---|---|
| Pumpe (kein Wasserhahn nötig) | ❌ | ✅ | ✅ |
| Solar / kein Strom | ✅ | ✅ | ✅ |
| App / Fernzugriff | ✅ | ❌ | ✅ |
| Bodenfeuchtesensor | ✅ | ❌ | ✅ |
| Ohne WLAN (LoRaWAN) | ✅ | ❌ | ✅ |
| Einbruchalarm / PIR | ❌ | ❌ | ✅ |
| Adaptiv (Sensor-gesteuert) | ✅ | ❌ | ✅ |
| Vereins-Gateway | ❌ | ❌ | ✅ |
| Open Source | ❌ | ❌ | ✅ |

### Marktpotenzial Deutschland (aktualisiert)
| Zielgruppe | Geschätzte Anzahl | Quelle |
|---|---|---|
| Schrebergarten-Parzellen | ~1.000.000 | Bundesverband Kleingarten |
| Wochenendhäuser / Datschen | ~300.000 | Schätzung |
| Jagdhütten | ~320.000 | DJV |
| Ferienhütten / Almhütten | ~150.000 | Schätzung |
| **Gesamt adressierbarer Markt** | **~1.770.000** | |
| Technikaffine Zielgruppe (15%) | **~265.000** | |
| Ø Warenkorb ~90€ | **~24 Mio. €** | |

### Zielpreis SmartGarden
| Komponente | Zielpreis |
|---|---|
| Sensor + Security Node (ESP32 + LoRa + Sensoren) | ~50€ |
| Pumpe (Regentonne) oder Magnetventil (Wasseranschluss) | ~10–20€ |
| Solar + Akku Kit (6W Panel + 2×18650 + CN3791) | ~25€ |
| **Gesamt Node** | **~85–95€** |
| App (kostenlos) + Cloud (Freemium) | 0€ / 3€ mtl. |

### Marktpotenzial Deutschland
- ~1 Mio. Schrebergartenparzellen in Deutschland
- Zielgruppe: ~15% technikaffin = ~150.000 potenzielle Kunden
- Durchschnittlicher Warenkorb: ~150€
- Adressierbarer Markt: ~22 Mio. €

---

## Garden Home — Kritische Annahmen zu validieren

Analog zur Guard-Tabelle oben — für Garden Home bisher nicht erfasst.

### Preisanker zur Einordnung
| Klasse | Preis | Beispiel |
|---|---|---|
| No-Name "dumm" (Timer, kein Branding) | **~20-30€** | Onherm BSV-IC015 (~25€) — geschätzte Bauteilkosten nur wenige Euro |
| Marke "dumm" (Timer, Markenaufschlag) | ~25-100€ | Gardena AquaBloom (Standard/L) |
| Marke "smart" (App, Sensorik) | ~250€ | MIYO |
| **SmartGarden Zielpreis** | **~85-95€** | ~3-4x Aufschlag gegenüber No-Name-Baseline |

| Annahme | Risiko | Test |
|---------|--------|------|
| Kunden zahlen Aufpreis für Fernüberwachung der Bewässerung (TTN/WiFi-Daten: Tankstand, Pumpenstatus, Bodenfeuchte) gegenüber einem reinen Timer-System (No-Name ~20-30€, Marke ~25-100€) | **Hoch** — bisher nur Annahme, nicht validiert. MIYO (~250€) beweist Zahlungsbereitschaft für *ein* Smart-Segment, aber Preisdelta zur Konnektivität allein ist unbekannt, und der ~3-4x-Aufschlag gegenüber der No-Name-Baseline (20-30€) ist beträchtlich | Einfache Preis-Anker-Umfrage im eigenen Kleingartenverein: zwei Varianten (Basis-Timer ~25€ vs. Basis+Konnektivität ~85-95€) mit der tatsächlichen Preisdifferenz vorlegen, Kaufbereitschaft abfragen. Alternativ: zwei Landingpage-Varianten mit unterschiedlichem Preis testen (Conversion-Vergleich) |
| Zahlungsbereitschaft besonders hoch bei **abwesenden** Nutzern (Wochenendhütte, Jagdhütte) vs. Nutzern, die täglich vor Ort sind | Mittel | Befragung nach Nutzungsmuster (Besuchsfrequenz) getrennt auswerten — Hypothese: Fernüberwachung ist für Vielbesucher wenig wert, für Wenigbesucher hoch |
| 2×2m-Hochbeet/Tomaten-Anwendungsfall ist repräsentativ für die Zielgruppe, nicht Einzelfall | Mittel | Kurzumfrage im Kleingartenverein: Beetgröße, angebaute Pflanzenarten, aktuelle Bewässerungsmethode abfragen |
| Preisaufschlag durch "Intelligenz" ist hoch und konkret kalkulierbar, nicht nur abstrakt — ESP32-Node (~50€ laut Zielpreis-Tabelle oben) statt einfachem Timer-Chip (~5-10€ bei Onherm/Gardena-Klasse) **plus** ggf. eigener LoRa-Gateway (~100-150€), falls kein TTN-Gateway in Reichweite | **Hoch** — der reale Kostenaufschlag (Hardware + ggf. Gateway) ist um ein Vielfaches höher als bei Guard (dort nur 1 Node, kein zusätzliches Bewässerungs-Equipment). Ungeklärt: deckt die Zahlungsbereitschaft diesen Mehrpreis, oder ist Garden Home nur rentabel, wenn ein TTN-Gateway bereits vorhanden ist (z.B. weil der Kunde schon einen Guard-Node hat)? | Klar zwischen zwei Szenarien trennen: (1) Kunde hat schon TTN-Coverage/eigenes Gateway (z.B. durch Guard) → niedrigerer Mehrpreis nötig, (2) Kunde braucht Gateway komplett neu → Preisaufschlag muss Gateway-Kosten mit einkalkulieren. Bundle-Pricing (Guard+Garden Home zusammen, gemeinsames Gateway) als Test-Variante in der Umfrage mit aufnehmen |
