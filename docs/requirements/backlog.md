# SmartGarden — Product Backlog

Priorisierung: [MUST] = MVP | [SHOULD] = v1.1 | [COULD] = Future | [WONT] = Out of Scope

---

## EPIC-01: Bewässerung

### US-001 [MUST] Automatische Bewässerung nach Bodenfeuchte
Als **Gärtner** möchte ich, dass mein Garten automatisch bewässert wird wenn
die Bodenfeuchte unter einen definierten Schwellenwert fällt,
damit meine Pflanzen optimal versorgt werden ohne dass ich vor Ort sein muss.

**Akzeptanzkriterien:**
- Given: Bodenfeuchtesensor meldet Wert < Schwellenwert
- When: Schwellenwert für mind. 10 Minuten unterschritten
- Then: Pumpe/Ventil öffnet für konfigurierte Dauer (1-60 min)

- Given: Pumpe läuft
- When: Bodenfeuchte > oberer Schwellenwert ODER maximale Laufzeit erreicht
- Then: Pumpe stoppt automatisch

---

### US-002 [MUST] Manuelle Bewässerung per App
Als **Gärtner** möchte ich die Bewässerung jederzeit manuell per App
starten und stoppen können,
damit ich auch ohne Automatik die Kontrolle behalte.

**Akzeptanzkriterien:**
- Given: Gärtner öffnet App
- When: Gärtner tippt "Bewässerung starten"
- Then: Pumpe startet innerhalb von 30 Sekunden und App zeigt Status "aktiv"

- Given: Pumpe läuft manuell
- When: Gärtner tippt "Stoppen" ODER maximale Sicherheitslaufzeit (2h) erreicht
- Then: Pumpe stoppt, App zeigt Bestätigung

---

### US-003 [SHOULD] Bewässerungsplan / Timer
Als **Gärtner** möchte ich feste Bewässerungszeiten definieren können
(z.B. täglich 6:00 Uhr morgens),
damit ich den Garten auch im Urlaub versorge ohne manuell eingreifen zu müssen.

**Akzeptanzkriterien:**
- Given: Gärtner konfiguriert Zeitplan in der App
- When: Definierte Uhrzeit erreicht
- Then: Bewässerung startet automatisch für konfigurierte Dauer

---

### US-004 [SHOULD] Bewässerungshistorie
Als **Gärtner** möchte ich sehen wann und wie lange bewässert wurde,
damit ich den Wasserverbrauch nachvollziehen kann.

---

## EPIC-02: Sensorik & Monitoring

### US-005 [MUST] Temperatur & Luftfeuchte anzeigen
Als **Gärtner** möchte ich aktuelle Temperatur und Luftfeuchtigkeit
in der App sehen,
damit ich den Zustand meines Gartens aus der Ferne beurteilen kann.

**Akzeptanzkriterien:**
- Given: Sensor-Node ist aktiv
- When: Gärtner öffnet App
- Then: Aktuelle Werte (max. 15 Minuten alt) werden angezeigt

---

### US-006 [MUST] Bodenfeuchte anzeigen
Als **Gärtner** möchte ich die aktuelle Bodenfeuchte in der App sehen,
damit ich den Bewässerungsbedarf einschätzen kann.

---

### US-007 [SHOULD] Historische Sensordaten & Diagramme
Als **Gärtner** möchte ich Sensordaten der letzten 30 Tage als Diagramm sehen,
damit ich Muster und Trends erkennen kann.

---

### US-008 [MUST] Batteriestand anzeigen (Garden Node)
Als **Gärtner** möchte ich den aktuellen Ladezustand des Garden-Node-Akkus
in der App sehen,
damit ich rechtzeitig eingreifen kann bevor das System ausfällt.

**Akzeptanzkriterien:**
- Given: Node sendet Telemetrie alle 15 Minuten
- When: Gärtner öffnet App
- Then: Batteriestand wird als Prozent (0–100%) und farbiger Balken angezeigt
  (≥30% grün, 15–29% gelb, <15% rot)

- Given: Akkuspannung gemessen via GPIO35 ADC (Spannungsteiler R3/R4 100kΩ)
- When: Spannung < 3,60V (≈25%)
- Then: App zeigt gelbes Warnsymbol 🟡 im Dashboard

- Given: Batteriestand < 15%
- When: Node sendet nächstes Paket
- Then: Push-Notification: „🌱 Garden Node: Batterie bei X% — Akku laden oder Solar prüfen"

---

## EPIC-03: Sicherheit & Alarm

### US-009 [MUST] Push-Alarm bei Bewegungserkennung
Als **Gärtner** möchte ich sofort eine Push-Benachrichtigung erhalten
wenn nachts Bewegung in meinem Garten erkannt wird,
damit ich bei einem Einbruch schnell reagieren kann.

**Akzeptanzkriterien:**
- Given: PIR-Sensor erkennt Bewegung zwischen 22:00 und 06:00 Uhr
- When: Bewegung länger als 3 Sekunden
- Then: Push-Notification an alle registrierten Geräte innerhalb von 60 Sekunden

- Given: Alarm ausgelöst
- When: Gärtner öffnet App
- Then: Zeitstempel, Alarmtyp und (falls vorhanden) Foto sichtbar

---

### US-010 [MUST] Alarmzeiten konfigurieren
Als **Gärtner** möchte ich definieren in welchen Zeitfenstern der Alarm
aktiv ist,
damit ich keine Fehlalarme tagsüber bekomme wenn ich selbst im Garten bin.

---

### US-011 [SHOULD] Alarm stumm schalten
Als **Gärtner** möchte ich den Alarm temporär deaktivieren können
(z.B. für 2 Stunden),
damit ich ungestört im Garten arbeiten kann.

---

### US-012 [COULD] Foto bei Alarm (Kameramodul)
Als **Gärtner** möchte ich bei einem Alarm ein Foto des Eindringlings
in der App sehen,
damit ich entscheiden kann ob es ein Fehlalarm oder ein echter Einbruch ist.

---

## EPIC-04: App & Benachrichtigungen

### US-013 [MUST] App Login & Authentifizierung
Als **Gärtner** möchte ich mich sicher in der App anmelden,
damit nur ich Zugriff auf mein System habe.

---

### US-014 [MUST] Dashboard Übersicht
Als **Gärtner** möchte ich auf einem Blick alle wichtigen Werte sehen
(Sensoren, Pumpenstatus, letzter Alarm),
damit ich den Zustand meines Gartens schnell beurteilen kann.

---

### US-015 [SHOULD] Mehrere Geräte / Familienmitglieder
Als **Gärtner** möchte ich Familienmitglieder einladen
die ebenfalls die App nutzen können,
damit mehrere Personen den Garten überwachen können.

---

## EPIC-05: Vereins-Gateway & Mehrbenutzer

### US-016 [SHOULD] Gemeinsamer Gateway im Verein
Als **Vereinsadmin** möchte ich einen zentralen LoRaWAN-Gateway
für alle Vereinsmitglieder betreiben,
damit jedes Mitglied nur einen Node kaufen muss ohne eigenen Gateway.

---

### US-017 [COULD] Mitgliederverwaltung
Als **Vereinsadmin** möchte ich Mitglieder zum Gateway hinzufügen
und entfernen können,
damit ich die Kontrolle über das Vereinsnetz behalte.

---

## EPIC-06: Energie & Autarkie

### US-018 [MUST] Betrieb ohne Stromanschluss
Als **Gärtner** möchte ich das System ohne externe Stromversorgung betreiben,
damit ich es auch in Schrebergärten ohne Stromanschluss nutzen kann.

**Akzeptanzkriterien:**
- Given: Volll geladener Akku, bewölkter Tag (kein Solar)
- When: System läuft im Normalbetrieb
- Then: Autonomie von mind. 7 Tagen ohne Sonnenlicht

---

### US-019 [MUST] Solar-Nachladung (Garden Node)
Als **Gärtner** möchte ich dass der Akku automatisch per Solarpanel
nachgeladen wird,
damit das System dauerhaft ohne Wartung läuft.

---

### US-020 [MUST] Guard: Batteriestand mit Restlaufzeit
Als **Laubenbesitzer** möchte ich den Ladezustand meines Guard-Node-Akkus
als Prozentanzeige UND als geschätzte Restlaufzeit in der App sehen,
damit ich weiß wann ich das Gerät zum Laden anstecken muss.

> **Hintergrund:** Guard Home hat kein Solarpanel — nur 2× 18650 (~5000 mAh).
> Typische Laufzeit ~1–1,5 Jahre. Nutzer soll wie beim Rauchmelder
> rechtzeitig erinnert werden.

**Akzeptanzkriterien:**
- Given: Guard Node sendet Batteriedaten (GPIO35 ADC, Spannungsteiler)
- When: Nutzer öffnet App → Guard-Gerätekarte
- Then: App zeigt:
  - Prozentzahl (0–100%)
  - Farbbalken (grün / gelb / rot)
  - „Geschätzte Restlaufzeit: ~X Monate"
  - „Letztes Laden: vor X Monaten (Datum)"

- Given: Restlaufzeit < 3 Monate (ca. 25% Restkapazität)
- When: Node sendet nächstes Paket
- Then: App zeigt 🟡 Hinweis „Bald laden empfohlen"

**Spannungs-Prozent-Mapping (18650):**
| Spannung | Prozent | Status |
|---|---|---|
| 4,20V | 100% | 🟢 |
| 3,80V | ~60% | 🟢 |
| 3,60V | ~25% | 🟡 |
| 3,50V | ~10% | 🔴 |
| 3,30V | ~2%  | 🚨 |

---

### US-021 [MUST] Guard: Push-Alarm bei niedrigem Batteriestand
Als **Laubenbesitzer** möchte ich eine Push-Benachrichtigung erhalten
wenn der Guard-Akku einen kritischen Ladestand erreicht,
damit ich das Gerät rechtzeitig aufladen kann bevor der Einbruchschutz ausfällt.

**Akzeptanzkriterien:**
- Given: Batteriestand fällt unter 25%
- When: Node sendet nächste Telemetrie
- Then: Einmalige Push-Notification: „🔋 Guard Laube: Batterie bei 25% — Bald laden"

- Given: Batteriestand fällt unter 10%
- When: Node sendet nächste Telemetrie
- Then: Tägliche Push-Notification: „🔴 Guard Laube: Batterie kritisch (X%)! Bitte USB-C anschließen"

- Given: Batteriestand fällt unter 3%
- When: Node sendet letzte Telemetrie
- Then: Sofortige Push-Notification: „🚨 Guard Laube: Batterie fast leer — System schaltet bald ab!"
  + Sonder-LoRaWAN-Payload mit Flag `low_battery_critical: true`

**Firmware-Verhalten:**
- Bei < 5%: Node sendet alle 5 Minuten statt alle 15 Minuten
  (damit letzte Warnung sicher ankommt bevor Abschaltung)

---

### US-022 [MUST] Guard: Node-Offline-Erkennung
Als **Laubenbesitzer** möchte ich benachrichtigt werden wenn mein Guard-Node
keine Daten mehr sendet,
damit ich unterscheiden kann ob die Batterie leer ist oder das Gerät
manipuliert / gestohlen wurde.

> **Kritischer Sicherheitsaspekt:** Ein leerer Akku und ein gestohlener
> Node sehen aus Sicht der App gleich aus — kein Signal.
> Durch den letzten bekannten Batteriestand kann die App eine
> fundierte Einschätzung geben.

**Akzeptanzkriterien:**
- Given: Guard Node hat zuletzt vor > 30 Minuten gesendet
- When: Backend erkennt fehlende Heartbeats
- Then: Push-Notification mit Kontext:

  **Fall A — Batterie war niedrig:**
  „⚫ Guard Laube: Keine Verbindung seit 30 Min.
   Letzter Akkustand: 8% → Wahrscheinlich Batterie leer"

  **Fall B — Batterie war voll:**
  „🚨 Guard Laube: Keine Verbindung seit 30 Min.
   Letzter Akkustand: 87% → Mögliche Manipulation! Bitte prüfen."

- Given: Node kommt nach Offline-Phase wieder online
- When: Erste Telemetrie empfangen
- Then: Push-Notification: „✅ Guard Laube: Wieder online — Akkustand: X%"

---

### US-023 [SHOULD] Guard: Ladehistorie & Wartungserinnerung
Als **Laubenbesitzer** möchte ich sehen wann ich meinen Guard-Node
zuletzt geladen habe und wann das nächste Laden empfohlen wird,
damit ich die Wartung wie einen Rauchmelder-Batteriewechsel einplanen kann.

**Akzeptanzkriterien:**
- Given: Backend erkennt Akkustand-Anstieg > 30% innerhalb von 2 Stunden
  (= Ladevorgang erkannt)
- When: Ladevorgang abgeschlossen (Spannung > 4,1V)
- Then: Backend speichert Zeitstempel als „Letztes Laden"

- Given: Nutzer öffnet Gerätedetails in App
- When: Guard Node ausgewählt
- Then: App zeigt:
  - „Letztes Laden: Oktober 2026 (vor 4 Monaten)"
  - „Nächstes Laden empfohlen: März 2027 (in 3 Monaten)"
  - Basis: aktuelle Entladerate (mAh/Tag) aus letzten 30 Tagen

- Given: Empfohlenes Ladedatum < 30 Tage entfernt
- When: Nutzer öffnet App
- Then: Hinweis-Banner: „🔔 Guard Laube: Laden empfohlen bis März 2027"

---

## Nicht im Scope (WONT)

- **W-001**: Integration mit Smart Home Systemen (Alexa, Google Home) — zu komplex für MVP
- **W-002**: Wettervorhersage-basierte Bewässerung — MIYO-Feature, später möglich
- **W-003**: Videoüberwachung / Livestream — zu hoher Datenverbrauch für LoRa
