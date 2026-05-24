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
                    HOH AUTARKIE
                         |
          SmartGarden ●  |
                         |
KEIN ALARM ──────────────┼────────────── ALARM
                         |
              MIYO ●     |
                         |
                    NIEDRIG AUTARKIE
```

### Zielpreis SmartGarden
| Komponente | Zielpreis |
|---|---|
| Sensor + Security Node (ESP32 + LoRa + Sensoren) | ~60€ |
| Pumpe + Ventil | ~30€ |
| Solar + Akku Kit | ~40€ |
| **Gesamt Node** | **~130€** |
| App (kostenlos) + Cloud (Freemium) | 0€ / 3€ mtl. |

### Marktpotenzial Deutschland
- ~1 Mio. Schrebergartenparzellen in Deutschland
- Zielgruppe: ~15% technikaffin = ~150.000 potenzielle Kunden
- Durchschnittlicher Warenkorb: ~150€
- Adressierbarer Markt: ~22 Mio. €
