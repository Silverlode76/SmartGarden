# ADR-002 — Custom PCB für Serienproduktion

**Status:** Accepted  
**Datum:** 2026-05  
**Entscheider:** Oliver Schmoll

---

## Kontext

Der TTGO LoRa32 V2.1 (ESP32 + SX1276 integriert) kostet im Einzelbezug ~21€.
Damit ist ein profitabler Verkauf des Guard Home (~45€ Ziel) oder Garden Home
(~65€ Ziel) in der Serienproduktion nicht möglich.

**Problemrechnung Guard Home mit TTGO:**

| Position | Kosten |
|---|---|
| TTGO LoRa32 (Einzelbezug) | ~21€ |
| Sensoren + Akku + Gehäuse | ~26€ |
| **BOM gesamt** | **~47€** |
| Zielpreis Guard Home | ~45€ |
| **Marge** | **negativ** |

Das TTGO ist ein **Entwicklungsboard** — es enthält OLED, USB-C-Schnittstelle,
Spannungsregler und weitere Komponenten die im Endprodukt nicht benötigt werden.
Der Aufpreis gegenüber Einzelkomponenten beträgt ~15€ pro Node.

---

## Entscheidung

**Dreistufiger Hardware-Entwicklungspfad:**

### Phase 1 — PoC (Proof of Concept)
- **Hardware:** TTGO LoRa32 V2.1 auf Lochraster
- **Ziel:** Konzept validieren — funktioniert Pumpe + Bodenfeuchte + LoRaWAN?
- **Stückzahl:** 1–5 Nodes
- **Preis:** ~89–99€ (Maker-Preis, Community akzeptiert das)
- **KiCad-Schaltplan** dient als Referenz und Dokumentation

### Phase 2 — Pilotproduktion
- **Hardware:** TTGO in Bulk direkt bei LILYGO (~12€ statt 21€)
- **Ziel:** Markt validieren — kaufen echte Nutzer das Produkt?
- **Stückzahl:** 100–500 Nodes
- **Preis:** ~65–79€

### Phase 3 — Serienproduktion
- **Hardware:** Custom PCB (KiCad) mit Einzelkomponenten
- **Hauptkomponenten:**
  - ESP32-WROOM-32E Modul: ~2,50€
  - SX1262 LoRa Transceiver (LLCC68 oder SX1262): ~3,00€
  - CN3791 MPPT (Garden) oder TP4056 (Guard): ~0,80€
  - Restliche Bauteile wie Schaltplan v0.1
- **Fertigung:** JLCPCB oder PCBWay inkl. SMD-Bestückung
- **BOM Guard Home:** ~18€ / **BOM Garden Home:** ~24€
- **Stückzahl:** 1.000+ Nodes
- **Preis:** ~45–65€

---

## Custom PCB Entwicklungsprozess

```
Phase 1 Schaltplan (KiCad, fertig) ──→ Phase 3 PCB-Layout (KiCad PCB Editor)
                                              │
                                        Gerber-Dateien exportieren
                                              │
                                        JLCPCB hochladen
                                        + BOM für PCBA (PCB Assembly)
                                              │
                                        Prototyp-Platinen bestellen
                                        (5 Stück ~50–80€ inkl. Bestückung)
                                              │
                                        Test & Validierung
                                              │
                                        Serienbestellung
```

### Wichtige Designentscheidungen für Custom PCB

| Thema | Entscheidung | Begründung |
|---|---|---|
| MCU | ESP32-WROOM-32E (Modul, kein bare chip) | FCC/CE vorzertifiziert, einfacheres PCB-Design |
| LoRa | SX1262 statt SX1276 | Neuere Generation, besser, günstiger |
| Formfaktor | 50×70mm (Guard) / 70×90mm (Garden) | IP65-Gehäuse Standard-Größen |
| Fertigung | JLCPCB + LCSC BOM | Gleicher Konzern, nahtlose Integration |
| Bestückung | PCBA (vollständig bestückt) | Kein manuelles SMD-Löten in Kleinserie |

---

## Konsequenzen

**Positiv:**
- BOM sinkt von ~47€ auf ~18€ (Guard) → Profitabler Verkauf möglich
- Kein OLED, kein USB-Hub, keine unnötigen Komponenten → weniger Ausfallpunkte
- Eigenes PCB-Design = vollständige Kontrolle über alle Bauteile
- KiCad-Schaltplan (v0.1) ist bereits Basis für PCB-Layout

**Negativ / Risiken:**
- PCB-Layout erfordert zusätzliche Entwicklungszeit (~40–80h)
- Erst ab ~100 Stück wirtschaftlich (Setup-Kosten Fertigung)
- CE-Zertifizierung erforderlich für EU-Verkauf (~2.000–5.000€ einmalig)
- SX1262 statt SX1276: Firmware-Anpassung LMIC Library nötig

**Meilenstein:**
- Phase 3 startet erst nach erfolgreichem Feldtest (v0.5)
- Keine Custom PCB bevor PoC und Pilotphase Funktionalität bestätigt haben

---

## Alternativen die verworfen wurden

| Alternative | Warum verworfen |
|---|---|
| TTGO dauerhaft verwenden | BOM zu hoch, kein profitabler Verkauf möglich |
| Bare ESP32 Chip (kein Modul) | Komplexeres PCB-Design, eigene HF-Zertifizierung nötig |
| RAK Wireless WisBlock | Teurer als TTGO, proprietäres Ökosystem |
| Nordic nRF9160 (LTE-M) | Monatliche SIM-Kosten, kein LoRaWAN |
| STM32 + SX1262 | Bewährt im Prototyp v0.0 als zu komplex verworfen |
