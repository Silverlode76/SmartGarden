# Wettbewerbsanalyse — SmartGarden

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
