# Aufbauanleitung — Sensor-Node v0.1 (Lochraster)

> **Ziel:** Funktionsfähiger Sensor-Node auf Lochrasterplatine aufgebaut und getestet.  
> **Schwierigkeit:** Mittel (Löterfahrung empfohlen)  
> **Zeitaufwand:** ca. 3–4 Stunden

---

## Benötigtes Werkzeug

| Werkzeug | Hinweis |
|---|---|
| Lötkolben 25–40W | Temperatur regelbar, Feine Spitze |
| Lötzinn | 0,8mm, bleifrei (Sn99 oder Sn63Pb37) |
| Multimeter | Spannungs- und Widerstandsmessung |
| Lochrasterplatine | min. 100×160mm, Rastermaß 2,54mm |
| Seitenschneider | Bauteilbeine kürzen |
| Pinzette | SMD-Bauteile, feine Drähte |
| Lötflussmittel | Hilft bei sauberen Lötstellen |
| Drähte | 0,5mm Kupferdraht für Verbindungen |
| Helfende Hand / Halterung | Platine fixieren |
| Isopropanol + Wattestäbchen | Flussmittelreste reinigen |

---

## Sicherheitshinweise

> ⚠️ **LiIon-Akkus:** Nie kurzschließen, nie über 4,2V laden, nie unter 2,5V entladen.  
> ⚠️ **Polarität:** Vor dem Einschalten immer VCC/GND mit Multimeter prüfen.  
> ⚠️ **Löttemperatur:** Max. 350°C, kurzer Kontakt (< 3 Sek.) pro Lötstelle.  
> ⚠️ **Akku erst ganz am Ende** anschließen — nie während des Lötens.

---

## Aufbaureihenfolge (Block für Block)

Die Blöcke entsprechen dem KiCad-Schaltplan. **Immer testen bevor der nächste Block aufgebaut wird.**

---

## Block A — Ladekreis (CN3791 MPPT)

### Komponenten
| Ref | Bauteil | Wert |
|---|---|---|
| D1 | SS14 Schottky-Diode | 40V / 1A |
| R1 | Widerstand | 100Ω |
| R2 | Widerstand | 1kΩ |
| C1 | Kondensator | 10µF (Eingang) |
| C2 | Kondensator | 10µF (Ausgang) |
| U2 | CN3791 IC | SOT-23-6 |
| J1 | JST-PH 2-Pin | Solarpanel |
| J2 | JST-PH 2-Pin | Akku |

### Pinbelegung CN3791 (SOT-23-6)
```
Pin 1: VIN   — Solarpanel + (nach SS14)
Pin 2: GND   — Masse
Pin 3: MPPT  — R1 (100Ω) zu VIN
Pin 4: VBAT  — Akku +
Pin 5: PROG  — R2 (1kΩ) zu GND
Pin 6: NC    — nicht belegen
```

### Schaltplan Block A
```
[J1 Solar+] → [D1 SS14 Anode] → [D1 Kathode = VIN]
                                        │
                                   [C1 10µF] ← GND
                                        │
                              [CN3791 VIN (Pin1)]
                              [CN3791 MPPT (Pin3)] ←→ [R1 100Ω] ←→ [VIN]
                              [CN3791 PROG (Pin5)] → [R2 1kΩ] → GND
                              [CN3791 VBAT (Pin4)] → [C2 10µF] → GND
                                        │
                                  VBAT_RAW → [J2 Akku+]
[J1 Solar-] ──────────────────────────── GND
[J2 Akku-]  ──────────────────────────── GND
```

### Lötschritte
1. R1 (100Ω) einlöten
2. R2 (1kΩ) einlöten
3. C1 und C2 (10µF) einlöten — **Polarität beachten! (+) zu VIN/VBAT**
4. D1 (SS14) einlöten — **Kathode (Strich) zeigt zu CN3791 VIN**
5. CN3791 (SOT-23-6) einlöten — sehr kleine Pins, Lupe verwenden
6. J1 (Solar) und J2 (Akku) Stecker anlöten

### Test Block A ✅
**Ohne Akku, nur Solarpanel angeschlossen:**
```
Messung 1: VIN (nach D1) = Panelspannung − 0,3V (SS14 Spannungsabfall)
Messung 2: VBAT_RAW = 0V (kein Akku angeschlossen → normal)
Messung 3: MPPT-Pin = ca. VIN × 0,8 (MPPT-Arbeitspunkt)
```
**Mit Akku (voll geladen, 4,2V):**
```
Messung: VBAT_RAW = Akkuspannung (3,7–4,2V)
CN3791 CHRG-LED (falls vorhanden): leuchtet bei aktivem Laden
```

---

## Block B — Akkuschutz (DW01A + FS8205A)

### Komponenten
| Ref | Bauteil |
|---|---|
| U3 | DW01A (SOT-23-6) |
| Q1 | FS8205A (SOT-23-6) |

### Pinbelegung DW01A
```
Pin 1: OD  — FS8205A Gate
Pin 2: CS  — FS8205A Source + VBAT_OUT−
Pin 3: GND — Masse
Pin 4: VCC — VBAT_RAW (+)
Pin 5: OC  — FS8205A Gate (wie Pin 1)
Pin 6: TD  — NC
```

### Pinbelegung FS8205A (Q1)
```
Gate   — DW01A OD + OC (Pin 1 + Pin 5)
Source — DW01A CS (Pin 2) + VBAT_OUT−
Drain  — GND
```

### Lötschritte
1. DW01A (U3) einlöten
2. FS8205A (Q1) einlöten
3. Verbindungen: OD+OC → Gate, CS → Source, GND → Drain

### Test Block B ✅
```
Messung: VBAT_OUT+ = VBAT_RAW (3,7–4,2V) — Plusleitung direkt durchgeschaltet
Messung: VBAT_OUT− = GND + FS8205A Kanalwiderstand (~0,03V bei 1A Last)
→ VBAT_OUT ≈ VBAT_RAW (minimaler Spannungsabfall)
```

---

## Block C — TTGO LoRa32 V2.1

### Komponenten
| Ref | Bauteil |
|---|---|
| A1 | TTGO LoRa32 V2.1 Modul |
| R3 | 100kΩ (Spannungsteiler oben) |
| R4 | 100kΩ (Spannungsteiler unten) |

### Anschluss TTGO
```
TTGO BAT-Pin   → VBAT_OUT (3,7V vom Akkupack)
TTGO GND       → GND
```

> **Achtung:** Das TTGO-Modul hat einen integrierten 3,3V LDO.  
> Alle Sensoren (BME280, AM312, SW-420) werden von diesem 3,3V Rail versorgt.

### Spannungsteiler VBAT_ADC
```
VBAT_OUT → [R3 100kΩ] → Knotenpunkt → [R4 100kΩ] → GND
                               │
                           GPIO35 (ADC)
```
> Teilt 4,2V auf max. 2,1V für den ADC-Eingang des ESP32 (max. 3,3V).

### GPIO-Belegung (Drähte zum TTGO anlöten)
| GPIO | Funktion | Richtung |
|---|---|---|
| GPIO12 | PUMP_GATE → MOSFET R5 | OUT |
| GPIO13 | PIR_OUT ← AM312 | IN |
| GPIO14 | VIB_OUT ← SW-420 | IN |
| GPIO21 | I2C_SDA ↔ BME280 | I2C |
| GPIO22 | I2C_SCL ↔ BME280 | I2C |
| GPIO33 | SOIL_PWR → Sensor VCC | OUT |
| GPIO34 | SOIL_ADC ← Sensor AOUT | IN (ADC) |
| GPIO35 | VBAT_ADC ← Spannungsteiler | IN (ADC) |

### Lötschritte
1. R3 und R4 (100kΩ) als Spannungsteiler einlöten
2. TTGO-Modul auf Lochraster setzen (Pinleisten anlöten oder Buchsenleisten verwenden)
3. BAT-Pin und GND verdrahten
4. GPIO-Drähte anlöten (kurze, farbcodierte Drähte empfehlen sich)

### Test Block C ✅
```
TTGO einschalten (BAT-Pin an VBAT_OUT):
→ Blaue LED leuchtet kurz auf
→ Serieller Monitor (115200 Baud) zeigt Boot-Meldung
→ Spannungsteiler: GPIO35 messen = ca. VBAT_OUT / 2 (1,85V bei 3,7V Akku)
```

---

## Block D — Sensoren

### BME280 (I2C Temperatur/Feuchte/Druck)
```
VCC  → 3,3V (TTGO onboard)
GND  → GND
SDA  → GPIO21
SCL  → GPIO22
```
> I2C-Adresse: 0x76 (SDO→GND) oder 0x77 (SDO→VCC)

### Bodenfeuchte-Sensor kapazitiv v1.2
```
VCC  → GPIO33 (schaltbare Stromversorgung)
GND  → GND
AOUT → GPIO34 (ADC)
```
> Sensor nur während Messung mit Strom versorgen → GPIO33 kurz HIGH setzen,  
> messen, dann wieder LOW. Verhindert elektrolytische Korrosion.

### AM312 PIR (Bewegung)
```
VCC  → 3,3V
GND  → GND
OUT  → GPIO13
```
> Erfassungswinkel: ~100°, Reichweite: ~3m, Ansprechzeit: ~1s

### SW-420 (Erschütterung/Vibration)
```
VCC  → 3,3V
GND  → GND
OUT  → GPIO14
```
> An Türrahmen / Fensterrahmen der Gartenlaube montieren.

### Test Block D ✅
**I2C Scanner (Arduino-Sketch):**
```cpp
#include <Wire.h>
void setup() {
  Wire.begin(21, 22); // SDA, SCL
  Serial.begin(115200);
}
void loop() {
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0)
      Serial.printf("I2C Gerät gefunden: 0x%02X\n", addr);
  }
  delay(5000);
}
```
**Erwartete Ausgabe:** `I2C Gerät gefunden: 0x76` (BME280)

---

## Block E — Pumpensteuerung (12V)

### Komponenten
| Ref | Bauteil |
|---|---|
| U5 | XL6009 Boost-Modul (3,7V → 12V) |
| R5 | 1kΩ Gate-Widerstand |
| Q2 | IRLZ44N N-MOSFET |
| D2 | 1N4007 Freilaufdiode |
| J8 | Pump-Anschluss |

### Schaltplan Block E
```
VBAT_OUT → [XL6009 VIN] → [XL6009 VOUT = 12V_BOOST]
                                     │
                              [J8 Pin 1 Pumpe+]
                                     │
                               [12V Pumpe]
                                     │
                              [J8 Pin 2 Pumpe−]
                                     │
                         [D2 Anode] ─┤
                         [D2 Kathode] → 12V_BOOST
                                     │
                               [Q2 Drain]
                               [Q2 Source] → GND
                               [Q2 Gate] ← [R5 1kΩ] ← GPIO12
```

### XL6009 Ausgangsspannung einstellen
> Das XL6009-Modul hat einen **Trimmer** zur Spannungseinstellung.
1. XL6009 an VBAT (3,7V) anschließen, **ohne Last**
2. Multimeter an VOUT anlegen
3. Trimmer drehen bis **12,0V** angezeigt werden
4. Einstellung prüfen unter Last (Pumpe angeschlossen)

### Lötschritte
1. R5 (1kΩ) einlöten
2. Q2 (IRLZ44N, TO-220) einlöten — **Kühlkörper empfohlen bei >1A Pumpenstrom**
3. D2 (1N4007) einlöten — **Kathode (Strich) zur 12V_BOOST Seite**
4. XL6009-Modul verdrahten
5. J8 (Pump-Stecker) anlöten

### Test Block E ✅
```
Schritt 1: XL6009 auf 12V einstellen (ohne Pumpe, mit Multimeter)
Schritt 2: GPIO12 = LOW → Multimeter an J8 Pins → 0V (Pumpe aus)
Schritt 3: GPIO12 = HIGH → Multimeter an J8 Pins → 12V (Pumpe ein)
Schritt 4: Pumpe anschließen → läuft an wenn GPIO12 = HIGH
```

---

## Abschlusstest (Gesamtsystem)

### Checkliste vor dem ersten Einschalten
- [ ] VCC/GND Kurzschluss prüfen: Multimeter im Widerstandsmodus zwischen VBAT+ und GND → muss > 100Ω zeigen
- [ ] CN3791 Polarität korrekt (VIN, VBAT, GND)
- [ ] TTGO BAT-Pin korrekt (3,7V, nicht 5V!)
- [ ] Solarpanel-Polarität prüfen (+ und −)
- [ ] Akku-Zellen auf gleiche Spannung gebracht (max. 50mV Differenz)

### Spannungen im Betrieb
| Messpunkt | Erwarteter Wert |
|---|---|
| Solar VIN (bei Sonne) | 5,5–6,5V |
| VBAT_RAW | 3,5–4,2V |
| VBAT_OUT | ≈ VBAT_RAW (−0,05V) |
| 3,3V Rail (TTGO intern) | 3,27–3,35V |
| 12V_BOOST | 11,8–12,2V |
| GPIO35 ADC (bei 3,7V Akku) | ~1,85V |

---

## Typische Fehler & Lösungen

| Problem | Ursache | Lösung |
|---|---|---|
| TTGO startet nicht | BAT-Pin < 3,0V oder Verpolung | Akkuspannung messen, Polarität prüfen |
| CN3791 lädt nicht | Panel < 4V oder TP4056 statt CN3791 | Panelspannung messen, IC prüfen |
| BME280 nicht gefunden | SDA/SCL vertauscht oder I2C-Adresse falsch | I2C-Scanner, Drähte prüfen |
| Pumpe läuft nicht | XL6009 Spannung falsch oder Q2 defekt | 12V prüfen, Gate-Spannung messen |
| Bodenfeuchte liest immer 0 | GPIO33 bleibt LOW | Firmware: GPIO33 HIGH vor Messung |
| PIR löst ständig aus | AM312 zu nah an Wärmequelle | Sensor repositionieren |

---

## Nächste Schritte nach erfolgreichem Test

- [ ] Energieverbrauch messen (Deep Sleep, TX, Pumpe aktiv)
- [ ] LoRaWAN-Verbindung zu TTN testen
- [ ] Gehäuse dimensionieren (IP65, Kabeleinführungen)
- [ ] Firmware: Bewässerungslogik + Alarmfunktion implementieren
