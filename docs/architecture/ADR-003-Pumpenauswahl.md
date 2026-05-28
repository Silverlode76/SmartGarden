# ADR-003 — Pumpenauswahl: Förderhöhe als kritisches Kriterium

**Datum:** 2026  
**Status:** Entschieden — aktualisiert nach ADR-004 (Drip-System)  
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

**Update durch ADR-004 (Drip-System):** XL6009 entfällt vollständig.
Die Drip-Pumpe läuft direkt auf 3,7–5V LiPo-Spannung. Kein 12V-Boost nötig.
Damit entfällt auch die Grenzbetrachtung — der Stromkreis wird deutlich einfacher.

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

## Kaufempfehlung (Prototyp v0.2) — aktualisiert für Drip-System

**Suchbegriffe:** „3V 5V Mini Submersible Pump 3m head" oder „DC 3.7V drip irrigation pump"

Referenz: Gardena AquaBloom Pumpe (1W / 3,6V / 4m Förderhöhe / 10 L/h) — beweist
dass 1W-Pumpen bei niedrigem Drip-Durchfluss 4m Förderhöhe erreichen können.

Datenblatt-Pflichtangaben beim Kauf:
- „Max. Head" oder „Förderhöhe max." **≥ 3m**
- „Max. Flow" **≥ 10 L/h** (reicht für Drip; mehr ist besser)
- Betriebsspannung: **3–5V DC**
- Leistung: **1–3W**

Bezugsquellen:
- AliExpress: ~3–6€ (Vorlaufzeit beachten, Datenblatt anfordern)
- Amazon DE: ~5–10€
- Berrybase: gelegentlich im Sortiment (Qualität besser dokumentiert)

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
