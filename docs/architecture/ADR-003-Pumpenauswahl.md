# ADR-003 — Pumpenauswahl: Förderhöhe als kritisches Kriterium

**Datum:** 2026  
**Status:** Entschieden  
**Autor:** Oliver Schmoll  
**Betrifft:** Hardware — Garden Home Node (Sensor-Node mit Bewässerung)

---

## Kontext

Im Prototyp-Test wurde versucht, Wasser aus einem Eimer (ca. 1m tiefer) in ein
Testbecken zu pumpen. Die verwendete **günstige 12V Tauchpumpe (3–5W, ~8€)** konnte
das Wasser **nicht fördern** — der Wasserdurchfluss war null oder minimal.

### Ursache: Pumpenkennlinie wird ignoriert

Jede Pumpe hat eine **Head-Flow-Kennlinie**: Je mehr Höhe überwunden werden muss
(= Gegendruck), desto weniger Wasser fließt. Am **Stillstandspunkt** (max. Förderhöhe)
fließt kein Wasser.

```
Förderhöhe
(m)
 3,0m │● ← Stillstandsförderhöhe (Nennwert im Datenblatt — Durchfluss = 0)
      │  ╲
 2,0m │   ╲
      │    ╲
 1,5m │─────╲──────── SmartGarden Betriebspunkt (Regentonne + Schlauch)
      │      ╲  → benötigt: min. 200 L/h hier
 1,0m │       ╲
      │        ╲
 0,0m │_________●── L/h (max. Durchfluss bei 0 Gegendruck)
```

**Billigpumpen 3–5W** haben typisch:
- Stillstandsförderhöhe: ~1,5m (steht im Datenblatt als „max. head")
- Bei 1m Gegendruck: ~50 L/h oder weniger
- Bei 1,2m Gegendruck: bereits nahe null

### Betriebspunkt SmartGarden

```
Pumpe (Boden der Regentonne)
   │
   │ +0,80m  Regentonne 80cm hoch → Wasser über Rand
   │ +0,40m  Höhenunterschied Tonnenrand → Pflanzenbeet
   │ +0,10m  Reibungsverluste Schlauch (3–5m, 8mm Innendurchmesser)
   ▼
Drip-Auslass
= 1,30m Gesamtgegendruck (worst case: 1,5m bei großem Höhenunterschied)
```

Eine Pumpe mit 1,5m Stillstandsförderhöhe hat **bei 1,3–1,5m praktisch keinen nutzbaren Durchfluss**.

---

## Entscheidung

**Mindestspezifikation für die SmartGarden-Pumpe:**

| Parameter | Minimum | Empfohlen |
|-----------|---------|-----------|
| Stillstandsförderhöhe (max. head) | 3m | 4–5m |
| Durchfluss bei 1,5m Gegendruck | ≥ 200 L/h | ≥ 400 L/h |
| Leistung | 10W | 10–15W |
| Spannung | 12V DC | 12V DC |
| Bauform | Tauchpumpe (submersible) | Tauchpumpe |
| Preis | ~12€ | ~12–18€ |

**Verworfene Alternative — 3–5W Pumpe:**
- Zu geringe Förderhöhe für den Anwendungsfall
- Falscher Eindruck durch „max. head"-Angabe (= Nulldurchfluss!)

---

## Auswirkung auf Boost-Converter (XL6009)

Alte Spezifikation (3–5W) → neue Spezifikation (10W) verändert den Strombedarf:

| | 3–5W Pumpe (alt) | 10W Pumpe (neu) |
|--|--|--|
| Strom bei 12V | ~250–420 mA | ~830 mA |
| Akku-Strom (3,7V, η=80%) | ~950 mA–1,6A | ~3,4 A |
| XL6009 Max. Input | 4A | 4A |
| Bewertung | unkritisch | **knapp im Limit** |

Der **XL6009 ist bei 10W noch verwendbar** (I_in ≈ 3,4A < 4A Limit), aber ohne
Sicherheitspuffer. Bei einer 15W-Pumpe oder langen Kabelwegen (Spannungsabfall)
wäre ein Wechsel auf **XL6019** (5A) oder ein dedizierter Boost-Converter nötig.

**Entscheidung:** XL6009 bleibt für v0.2-Prototyp. Vor Serienfreigabe Messung
am echten Aufbau (Temperatur des XL6009 nach 5 min Pumpenbetrieb prüfen).

---

## Auswirkung auf Energiebudget

Realistischer Bewässerungszyklus: **2× täglich à 2 Minuten** (= 4 min/Tag).

| Berechnung | Wert |
|------------|------|
| Pumpenleistung | 10W |
| Boost-Verluste (η=80%) | 12,5W Aufnahme aus Akku |
| Betrieb 4 min/Tag | 12,5W × 0,067h = **0,83 Wh/Tag** |
| Betrieb 20 min/Tag (Worst Case) | 12,5W × 0,33h = **4,2 Wh/Tag** |
| Autonomie ohne Solar (2× 18650, 18,5 Wh) | **4–22 Tage** (je nach Pumpdauer) |

Mit Solar (6W Panel, 4h Sonne/Tag = 24 Wh/Tag) ist das Energiebudget
auch im Worst Case **komfortabel positiv**.

---

## Kaufempfehlung (Prototyp v0.2)

**Suchbegriffe:** „12V DC Submersible Water Pump 10W 800L/H 3M Head"

Datenblatt-Pflichtangaben beim Kauf:
- „Max. Head" oder „Förderhöhe max." **≥ 3m**
- „Max. Flow" **≥ 600 L/h** (sichert nutzbaren Durchfluss bei 1,5m)
- Betriebsspannung: **12V DC**

Bezugsquellen:
- AliExpress: ~8–12€ (Vorlaufzeit 2–4 Wochen, Datenblatt anfordern)
- Amazon DE: ~12–18€ (schnellere Verfügbarkeit, oft mit Kennlinie)
- Berrybase / Reichelt: selten im Sortiment

---

## Lessons Learned

> Beim ersten Prototyp wurde die Pumpe **nur nach Watt und Preis** ausgewählt —
> nicht nach der Förderhöhe. Die Förderhöhe ist das **einzige kaufentscheidende
> Kriterium** für diesen Anwendungsfall.
>
> Analog: Eine Pumpe mit 1,5m max. head für 1,3m Betriebsgegendruck zu kaufen
> ist wie einen Motor zu kaufen, der genau bei der Betriebsdrehzahl seine
> Nennleistung erreicht — in der Praxis funktioniert er nicht.

---

## Referenzen

- `hardware-overview.md` — Aktorik-Tabelle, Energiebudget aktualisiert
- `ADR-001-MCU-Auswahl.md` — MCU-Entscheidung
- `ADR-002-Custom-PCB-Produktionspfad.md` — Produktionspfad
