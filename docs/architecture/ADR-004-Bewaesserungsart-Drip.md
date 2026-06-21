# ADR-004 — Bewässerungsart: Tropfbewässerung (Drip)

**Datum:** 2026  
**Status:** Entschieden  
**Autor:** Oliver Schmoll  
**Betrifft:** Hardware Garden Home Node — Aktorik, Pumpe, Boost-Converter

---

## Kontext

Nach ADR-003 (Pumpenauswahl) wurde klar, dass eine 10W/12V-Pumpe für direkte
Bewässerung einen XL6009 Boost-Converter (3,7V → 12V) erfordert — mit dem Risiko,
nahe am Strom-Limit zu arbeiten. Gleichzeitig wurde der **Gardena AquaBloom**
als relevanter Wettbewerber analysiert.

### Gardena AquaBloom Pumpen-Daten (Referenz)

| Parameter | Wert |
|-----------|------|
| Nennleistung | **1 W** |
| Nennspannung | **3,6 V** |
| Max. Förderhöhe | **4 m** |
| Max. Fördermenge | **10 L/h** |
| Betriebsart | Tropfbewässerung (Drip-Emitter) |

**Erkenntnis:** Gardena erreicht 4m Förderhöhe mit nur 1W — weil bei niedrigem
Durchfluss (10 L/h) die hydraulische Last minimal ist. Die Drip-Emitter begrenzen
den Durchfluss; die Pumpe arbeitet fast im Leerlauf.

---

## Entscheidung

**SmartGarden Garden Home verwendet Tropfbewässerung (Drip Irrigation).**

```
[Regentonne]
     │
[Drip-Pumpe 1–3W, 3,7–5V]  ← direkt aus LiPo (kein Boost!)
     │
[1-auf-8 Verteiler]
     │
[Mikro-Schlauch 4/6mm]
     ├──► [Drip-Emitter 0,5 L/h] → Pflanze 1
     ├──► [Drip-Emitter 1,0 L/h] → Pflanze 2
     ├──► [Drip-Emitter 0,5 L/h] → Pflanze 3
     └──► ...bis 8 Pflanzen pro Node
```

---

## Begründung

### Warum Drip und nicht direkte Bewässerung?

| Kriterium | Direkt (Schütten) | **Drip (gewählt)** |
|-----------|-------------------|---------------------|
| Pumpenleistung | 10–15W (12V) | **1–3W (3,7–5V)** |
| Boost-Converter | XL6009 nötig | **entfällt** |
| Schaltungskomplexität | höher | **einfacher** |
| BOM-Kosten Pumpe+Boost | ~14€ | **~7–10€** |
| Wassereffizienz | mittel (Überwässerung möglich) | **hoch (Wurzel-direkt)** |
| Pflanzenschonung | Blätter nass, Pilzgefahr | **Boden only, gesünder** |
| Autonomie ohne Solar | ~21 Tage | **~56 Tage** |
| Gardena-Kompatibilität | — | ✅ gleiche Drip-Heads nutzbar |
| Marktvalidierung | — | ✅ Gardena, MIYO, alle Profi-Systeme |

### Energetischer Vorteil

```
Direkt:  10W × 4 min/Tag     = 0,83 Wh/Tag (kurze Zyklen, hohe Leistung)
Drip:     1W × 60 min/Tag    = 1,00 Wh/Tag (lange Zyklen, niedrige Leistung)
```

Energieverbrauch ist ähnlich — aber Drip braucht **keinen Boost-Converter**,
spart Bauteilkosten und eliminiert einen potentiellen Fehlerpunkt.

---

## Konsequenzen

### Hardware — was sich ändert

| Komponente | Vorher | Nachher |
|------------|--------|---------|
| Pumpe | 12V 10W Tauchpumpe, ~12€ | **3–5V 1–3W Drip-Pumpe, ~5–8€** |
| Boost-Converter | XL6009, ~2€ | **entfällt → 0€** |
| Drip-Emitter | — | **8× à ~0,50€ = ~4€** |
| Drip-Schlauch | — | **~2€** |
| Verteiler | — | **~2€** |
| **Netto BOM-Änderung** | | **ca. +1–4€ mehr**, aber besseres Produkt |

### Schaltplan — vereinfacht

```
Alt:  VBAT → XL6009 (Boost 12V) → IRLZ44N → 12V Pumpe
Neu:  VBAT ─────────────────────→ IRLZ44N → 3,7V Drip-Pumpe
```

GPIO12 → 1kΩ → IRLZ44N Gate → Pump- → GND (Low-Side Switch, unverändert)

### Firmware — Implikation

Bewässerungsstrategie ändert sich:
- **Alt:** kurze Pulse (30 Sek. → 5 Min.)
- **Neu:** längere Zyklen (30 Min. – 2h), da Drip-Flow gering

Feuchtigkeitssensor (Capacitive Soil Sensor) steuert Zyklusende:
- Pumpe an → Boden-ADC überwachen → bei Ziel-Feuchte: Pumpe aus
- Oder: zeitbasiert (einfacher für MVP)

---

## Offene Punkte für Prototyp v0.2

- [ ] Geeignete 3–5V Drip-Pumpe mit ≥3m Förderhöhe beschaffen und testen
- [ ] Drip-Emitter (0,5 L/h und 1 L/h) auf Durchflussgenauigkeit prüfen
- [ ] Optimale Schlauchlänge und Verteiler-Konfiguration für 4–8 Pflanzen ermitteln
- [ ] Zeitbasiert vs. sensorbasierter Bewässerungszyklus evaluieren
- [ ] Schlauch-Dimensionierung: 4/6mm Mikro vs. 16mm Hauptleitung

---

## Update (Juni 2026) — Anwendungsfall präzisiert: Hochbeet statt Einzelpflanzen

Beim Hardware-Bring-up wurde klar, dass der konkrete Zielanwendungsfall ein
**Hochbeet von ca. 2×2m (4m²)** ist — nicht einzelne Topf-/Balkonpflanzen wie bei
Gardena AquaBloom. Das ist relevant, weil AquaBloom genau dafür in der
[Wettbewerbsanalyse](../research/competitive-analysis.md) als ungeeignet bewertet wird:
*"Ausgelegt für Balkon-/Topfpflanzen, nicht für Gartenbeet / größere Flächen."*

### Konsequenz für die Drip-Auslegung

Das ursprüngliche Bild (oben, "1-auf-8-Verteiler zu 8 Einzel-Emittern") passt zum
Balkonkasten-Maßstab, nicht zur Flächenbewässerung. Für ein Hochbeet ist stattdessen
ein **Tropfschlauch-Gitter** sinnvoller:

```
[Pumpe] → [Verteiler] → mehrere parallele Tropfschlauch-Linien quer über das Beet
                          (z.B. alle 30cm eine Reihe, integrierte Tropfpunkte alle 30cm)
```

**Auswirkung auf den Durchflussbedarf:** Mehr Tropfpunkte über die Fläche bedeuten
einen deutlich höheren Gesamtdurchfluss als die ursprünglich angenommenen 8 Einzel-
Emitter (4-8 L/h gesamt). Bei z.B. 7×7 Tropfpunkten (30cm-Raster auf 2×2m) mit je
1-2 L/h sind das schon **50-100+ L/h Gesamtbedarf**.

### Auswirkung auf die Pumpenwahl

Dies bestätigt, warum die in der Praxis getesteten kleinen Membran- (90-120 L/h) und
Peristaltikpumpen (3-6 L/h) für die **vollflächige** Hochbeet-Bewässerung eher knapp
bis ungeeignet sind, während die aktuell verbaute **12V-Tauchpumpe (216 L/h gemessen
bei 0,5m, siehe [hardware-overview.md](hardware-overview.md) Bring-up-Bericht)**
reichlich Reserve für ein Tropfschlauch-Gitter dieser Größenordnung bietet — ein
weiteres Argument, an der 12V-Tauchpumpe (statt einer kleineren 3,7-5V-Lösung)
festzuhalten.

### Offener Punkt
Konkretes Tropfschlauch-Layout für 2×2m (Rasterabstand, Gesamt-Tropfpunktzahl,
benötigter Betriebsdruck/Durchfluss) ist noch nicht final dimensioniert — siehe
"Offene Punkte für Prototyp v0.2" oben.

---

## Finale Entscheidung (Juni 2026): 12V-Tauchpumpe + Boost-Converter bleibt

Nach Abwägung aller Optionen (3,7-5V Drip-Pumpe ohne Boost, 12V Membranpumpe
selbstansaugend, 5V Peristaltikpumpe) bleibt SmartGarden bei der **12V-Tauchpumpe
+ Boost-Converter**, begründet durch:

- **Anwendungsfall ist größer als Balkonkasten** (2×2m Hochbeet, Tomaten) — kleine
  1-3W-Pumpen (Gardena-Klasse, 10 L/h) würden bei vollflächiger Tropfschlauch-
  Bewässerung an ihre Grenzen kommen bzw. unpraktikabel lange Laufzeiten benötigen
- **Reserve für Erweiterung** (größeres Beet, zweites Beet) ohne Pumpenwechsel
- **Bereits verifizierte, funktionierende Schaltung** (siehe Bring-up-Bericht in
  [hardware-overview.md](hardware-overview.md))
- Selbstansaugende Alternativen (Membran-/Peristaltikpumpe) lösen kein echtes
  Problem, da die Tauchpumpe ohnehin nicht ansaugen muss (sitzt im Wasser)

### Akzeptierter Kostenfaktor
Der Boost-Converter (3,7V→12V) bleibt als zusätzliche BOM-Position bestehen
(~3-5€ für ein ausreichend dimensioniertes Modul, z.B. XL6019 5A) — das ist der
bewusst in Kauf genommene Mehrpreis gegenüber der ursprünglich in ADR-004 geplanten
boost-losen Lösung. Sollte in der BOM (`hardware/bom/sensor-node-bom.csv` bzw.
[hardware-overview.md](hardware-overview.md) Komponententabelle) entsprechend ergänzt
werden, inkl. des in einem früheren Gespräch identifizierten fehlenden **Inline-
Filters** vor dem Drip-Verteiler (Scheiben-/Siebfilter, ~3-5€) gegen Verstopfung
durch Regentonnenwasser.

### Nächster Schritt
PoC mit 12V-Tauchpumpe + neuem, ausreichend dimensioniertem Boost-Modul aufbauen
und **im Feld testen** (statt nur am Labornetzteil). Felderkenntnisse fließen in
die weitere Bewertung/Dimensionierung ein.

---

## Revision (Juni 2026): PoC wird doch mit kleiner Membranpumpe aufgebaut

Nach der finalen Entscheidung oben wurde zusätzlich eine **YWBL-WH Mini-Membranpumpe**
getestet — mit Erfolg: Sie ist **selbstansaugend und läuft direkt am Akku (3-3,7V),
ganz ohne Boost-Converter.**

### Datenblatt YWBL-WH

| Parameter | Wert |
|---|---|
| Pumpentyp | Membran |
| Spannung | laut Listing "1 Volt" (vermutlich Übersetzungs-/Listingfehler — Praxistest zeigt Betrieb bei 3-3,7V) |
| Leistung | 3W |
| Max. Durchflussmenge | **1,2 L/min = 72 L/h** |
| Max. Hubhöhe | **1,5 m** |
| Gewicht | 59,5 g |

### Entscheidung
Auf Wunsch wird der PoC jetzt mit dieser Pumpe statt der 12V-Tauchpumpe aufgebaut.
Damit kehrt SmartGarden faktisch zur ursprünglichen ADR-004-Idee zurück
(boost-lose Drip-Pumpe direkt am Akku), nur mit konkretem, verifiziertem Produkt.

### ⚠️ Zu prüfender Vorbehalt — Förderhöhen-Marge
**Max. Hubhöhe 1,5m entspricht exakt dem oberen Ende des in ADR-003 ermittelten
Betriebspunkts (1,3-1,5m Gesamtgegendruck Tonne→Beet).** Genau das war die in
ADR-003 dokumentierte Lektion aus dem allerersten Fehlkauf: **am Stillstandspunkt
(max. Förderhöhe) ist der Durchfluss praktisch null** — die 72 L/h gelten nur bei
0m Gegendruck, nicht bei 1,3-1,5m. Vor dem endgültigen Einbau unbedingt den
**tatsächlichen Durchfluss bei der realen Betriebshöhe (1,3-1,5m) messen**
(gleiche Methode wie beim 12V-Pumpentest: Messbecher + Stoppuhr) — nicht nur
den freien Durchfluss. Falls der Durchfluss bei dieser Höhe zu stark einbricht,
bleibt die 12V-Tauchpumpe (siehe finale Entscheidung oben) die Rückfalloption.

### Vorteil gegenüber der 12V-Lösung (falls Förderhöhen-Test besteht)
- Kein Boost-Converter nötig → eine Fehlerquelle/Kostenposition weniger
- Geringerer Energieverbrauch (3W vs. 12V-Pumpe + Boost-Verluste)
- Selbstansaugend → flexiblere Montage (Pumpe kann trocken neben der Tonne stehen)

### Erster Praxistest (Juni 2026) — Teilbestätigung
Pumpe fördert Wasser über **50cm Höhenunterschied durch einen 1,5m Schlauch
ohne Probleme** (qualitative Beobachtung, noch kein gemessener L/h-Wert bei
dieser Höhe). **Kein Boost-Converter nötig** — Pumpe läuft direkt am Akku.

**Aber:** 50cm ist noch nicht der volle Betriebspunkt (1,3-1,5m laut ADR-003).
Der Vorbehalt oben ist damit teilweise, aber nicht vollständig ausgeräumt —
offen bleibt der Test bei **voller Betriebshöhe (1,3-1,5m)** mit Durchfluss-
Messung (Messbecher + Stoppuhr), bevor die Pumpe endgültig den Boost-Ansatz
ersetzt.

---

## Referenzen

- `ADR-003-Pumpenauswahl.md` — Förderhöhe als kritisches Kriterium (aktualisiert)
- `hardware-overview.md` — BOM, Schaltplan, Energiebudget aktualisiert
- Gardena AquaBloom: [GARDENA Help Center — Förderhöhe](https://help.gardena.com/hc/de/articles/4450610568988)
