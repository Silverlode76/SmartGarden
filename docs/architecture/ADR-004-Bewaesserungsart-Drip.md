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

## Referenzen

- `ADR-003-Pumpenauswahl.md` — Förderhöhe als kritisches Kriterium (aktualisiert)
- `hardware-overview.md` — BOM, Schaltplan, Energiebudget aktualisiert
- Gardena AquaBloom: [GARDENA Help Center — Förderhöhe](https://help.gardena.com/hc/de/articles/4450610568988)
