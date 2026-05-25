# KiCad Zeichenanleitung — Sensor-Node v0.1

Öffne `sensor-node.kicad_pro` in KiCad, dann den Schematic Editor.
Zeichne die Schaltung in den folgenden Blöcken (Sheets oder Bereiche auf einem Blatt).

---

## Block A — Ladekreis (oben links)

### Benötigte Symbole
| Ref | KiCad Symbol | Wert |
|---|---|---|
| J1 | `Connector:Conn_01x02_Pin` | Solar Panel 6V 6W |
| D1 | `Device:D_Schottky` | SS14 |
| C1 | `Device:C` | 10µF |
| U2 | `Device:IC_Generic` (6-Pin) | CN3791 |
| R1 | `Device:R` | 100Ω |
| R2 | `Device:R` | 1kΩ |
| C2 | `Device:C` | 10µF |
| J2 | `Connector:Conn_01x02_Pin` | Akku 2×18650 2P |

### CN3791 Pinbelegung (SOT-23-6)
```
Pin 1: PROG   → R2 (1kΩ) → GND
Pin 2: GND    → GND
Pin 3: MPPT   → R1 (100Ω) → VIN (nach Diode)
Pin 4: VIN    → Kathode D1 / C1+ / R1
Pin 5: VBAT   → Akku + / C2+
Pin 6: NC     → nicht verbinden
```

### Verbindungen Block A
```
J1 Pin1 (+) ──────────── Anode D1
J1 Pin2 (-) ──────────── GND
                         │
D1 Kathode ──┬────────── CN3791 VIN (Pin 4)
             │           │
             └── C1+ ─── CN3791 MPPT (Pin 3) ──[R1 100Ω]── CN3791 VIN
             C1- → GND
                         │
CN3791 PROG (Pin 1) ──[R2 1kΩ]── GND
CN3791 VBAT (Pin 5) ──┬── J2 Pin1 (Akku +)
                       └── C2+
C2- → GND
J2 Pin2 (Akku -) ────── GND
```

**Net-Labels setzen:**
- `SOLAR_IN` : zwischen J1(+) und D1 Anode
- `VIN_MPPT` : zwischen D1 Kathode und CN3791 VIN
- `VBAT_RAW` : zwischen CN3791 VBAT und Akku

---

## Block B — Akkuschutz (oben mitte)

### Benötigte Symbole
| Ref | KiCad Symbol | Wert |
|---|---|---|
| BT1 | `Device:Battery` | 2× 18650 2P |
| U3 | `Device:IC_Generic` (6-Pin) | DW01A |
| Q1 | `Device:Q_NMOS_GSD` (Dual) | FS8205A |

### DW01A Pinbelegung (SOT-23-6)
```
Pin 1: OD    → Gate Q1b (Entlade-MOSFET)
Pin 2: CS    → Source Q1 / Akku -  (Strommesspunkt)
Pin 3: GND   → B- (Akku Minus-Seite)
Pin 4: VCC   → B+ (Akku Plus)
Pin 5: OC    → Gate Q1a (Lade-MOSFET)
Pin 6: TD    → 0Ω zu CS oder offen (je nach Variante)
```

### Verbindungen Block B
```
VBAT_RAW ─────────── BT1 + ──────── DW01A VCC (Pin 4)
                      BT1 - ──────── DW01A GND (Pin 3)
                                     │
DW01A OD (Pin 1) ──────────────── Gate Q1b
DW01A OC (Pin 5) ──────────────── Gate Q1a
Q1 Source ─────────────────────── DW01A CS (Pin 2)
Q1 Drain ──────────────────────── VBAT_OUT (System GND)
```

**Net-Labels:**
- `VBAT_OUT` : geschützter Akku-Ausgang (3,7V nom.)
- `SYS_GND`  : Systemerde (nach Schutz-MOSFET)

---

## Block C — Systemversorgung & TTGO (mitte)

### Benötigte Symbole
| Ref | KiCad Symbol | Wert |
|---|---|---|
| U1 | `Connector:Conn_02x10_Odd_Even` | TTGO LoRa32 V2.1 |
| R3 | `Device:R` | 100kΩ |
| R4 | `Device:R` | 100kΩ |
| C3 | `Device:C` | 100nF |

> Für den TTGO einen Stecker (2×10 Pin Header) als Symbol verwenden.
> Beschriftung der Pins gemäß Pinbelegung unten.

### TTGO LoRa32 V2.1 relevante Pins
```
BAT    ← VBAT_OUT  (3,7V Akku direkt)
GND    ← SYS_GND
3V3    → interne 3,3V Schiene (Ausgang, nicht einspeisen!)
GPIO21 → BME280 SDA
GPIO22 → BME280 SCL
GPIO34 → Soil ADC (Eingang)
GPIO33 → Soil VCC (schaltbare 3,3V)
GPIO13 → AM312 PIR Signal
GPIO14 → SW-420 Signal
GPIO12 → MOSFET Gate (Pumpe)
GPIO35 → Akku-ADC (Spannungsteiler)
```

### Spannungsteiler Akku-Messung
```
VBAT_OUT ──[R3 100kΩ]──┬──[R4 100kΩ]── SYS_GND
                        │
                     GPIO35 (TTGO)
                     (max 3,3V → bei 4,2V Akku: 4,2×100/(100+100) = 2,1V ✅)
```

---

## Block D — Sensoren (rechts oben)

### Benötigte Symbole
| Ref | KiCad Symbol | Wert |
|---|---|---|
| U5 | `Sensor:BME280` oder `Device:IC_Generic` | BME280 |
| U6 | `Connector:Conn_01x03_Pin` | AM312 PIR |
| SW1| `Connector:Conn_01x03_Pin` | SW-420 |
| S1 | `Connector:Conn_01x03_Pin` | Capacitive Soil |

### Verbindungen Block D
```
3V3 (TTGO) ─── BME280 VCC
SYS_GND    ─── BME280 GND
GPIO21     ─── BME280 SDA
GPIO22     ─── BME280 SCL
               BME280 SDO → GND  (I2C Adresse 0x76)

3V3 (TTGO) ─── AM312 VCC
SYS_GND    ─── AM312 GND
GPIO13     ─── AM312 OUT

3V3 (TTGO) ─── SW-420 VCC
SYS_GND    ─── SW-420 GND
GPIO14     ─── SW-420 OUT

GPIO33     ─── Soil VCC  (schaltbar!)
SYS_GND    ─── Soil GND
GPIO34     ─── Soil AOUT
```

---

## Block E — Pumpensteuerung 12V (rechts unten)

### Benötigte Symbole
| Ref | KiCad Symbol | Wert |
|---|---|---|
| U4 | `Connector:Conn_01x02_Pin` | XL6009 Boost Modul |
| Q2 | `Device:Q_NMOS_GSD` | IRLZ44N |
| R5 | `Device:R` | 1kΩ |
| D2 | `Device:D` | 1N4007 (Freilaufdiode) |
| J3 | `Connector:Conn_01x02_Pin` | Pumpe / Ventil |

### Verbindungen Block E
```
VBAT_OUT ──────────────── XL6009 Modul VIN+
SYS_GND  ──────────────── XL6009 Modul VIN-
XL6009 VOUT+ (12V) ────── J3 Pin1 (Pumpe +)
                           J3 Pin2 (Pumpe -)  ─── Drain Q2
GPIO12 ──[R5 1kΩ]──────── Gate Q2
SYS_GND ───────────────── Source Q2

(Freilaufdiode D2 parallel zur Pumpe: Anode an Drain Q2, Kathode an 12V)
```

**Net-Labels:**
- `VCC_12V` : XL6009 Ausgang (nur aktiv wenn GPIO12 HIGH)
- `PUMP_GND` : Source Q2

---

## Power-Flags (KiCad Pflicht)

KiCad meldet ERC-Fehler ohne Power-Flags. Folgende setzen:
- `PWR_FLAG` an `VBAT_OUT`
- `PWR_FLAG` an `3V3` (vom TTGO)
- `PWR_FLAG` an `VCC_12V`
- `PWR_FLAG` an `GND` / `SYS_GND`

---

## ERC-Checkliste nach dem Zeichnen

- [ ] Alle Nets haben mindestens 2 Verbindungen
- [ ] Keine unverbundenen Pins (außer NC)
- [ ] Power-Flags gesetzt
- [ ] CN3791 NC-Pin explizit als "unconnected" markiert
- [ ] Alle Referenzen annotiert (Tools → Annotate)
- [ ] BOM aus KiCad exportieren und mit `hardware/bom/sensor-node-bom.csv` abgleichen
