# Hardware–Software Interface

**Project:** UV26-TMS-V2  
**Status:** Working draft; pin-level sign-off required  
**Revision:** 0.1 — 2026-10-25

## 1. Purpose and authority

This document identifies the interfaces firmware must implement and the checks needed before hardware integration. Its current Satellite baseline is the Electronics schematic dated **2026-10-20** (`Schematic PDF_[No Variations] (1).pdf`). Verify against the **latest approved schematic, PCB revision, and STM32CubeMX `.ioc`** before implementation. A schematic connection alone does not confirm the current `.ioc` configuration or successful hardware operation.

## 2. System overview

- **Main:** STM32F407; coordinates Satellite measurements and interfaces with the vehicle over CAN.
- **Satellites:** STM32L151CBT6A; six are planned, each monitoring 23 thermistor positions.
- **Main–Satellite link:** isolated SPI using LTC6820 transceiver circuitry. Master/slave details, framing, timing, and chain topology require confirmation against the approved communication design.

## 3. Satellite analog acquisition

| Signal / component | Confirmed connection or function | Firmware implication |
|---|---|---|
| U3: SN74HC4851-Q1 | 8:1 analog MUX, output `MUX01` | ADC input PA0 / ADC_IN0 |
| U4: SN74HC4851-Q1 | 8:1 analog MUX, output `MUX02` | ADC input PA1 / ADC_IN1 |
| U5: SN74HC4851-Q1 | 8:1 analog MUX, output `MUX03` | ADC input PA2 / ADC_IN2 |
| `MUX1`, `MUX2`, `MUX3` | Shared MUX address/select controls | Configure GPIO outputs; verify exact MCU pin assignments |
| `MUXE` | Shared MUX inhibit/enable control | Verify polarity, reset default, and MCU GPIO assignment |
| Thermistor network | 23 inputs; 10 kΩ pull-ups to 3V3 in supplied schematic | Check ADC range, sensor electrical model, and open/short thresholds |
| ADC peripheral | STM32L151 internal ADC | Three **channels on one ADC**, not three independent ADCs |

The three MUXes share their address signals. A candidate acquisition strategy is to iterate address values 0–7 and read PA0, PA1, and PA2 at each step, skipping the unpopulated final channel of U5. **The exact logical sensor ordering is not sequential by assumption**: use the existing `mux_mapping.c` mapping and validate it against the PCB/schematic net labels.

### Previously tested logical mapping — verify on hardware

| MUX | Channel 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 0 | 2 | 1 | 0 | 3 | 6 | 5 | 4 | 7 |
| 1 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 |
| 2 | 17 | 18 | 19 | 16 | 20 | 21 | 22 | unused |

These are **logical indices from the tested mapping module**, not independently verified physical pin-to-cell assignments. Confirm indexing conventions (zero- versus one-based) with Electronics.

### ADC implementation checks

1. Confirm PA0/PA1/PA2 analog mode and ADC channel configuration in the Satellite `.ioc`.
2. Confirm GPIO pin numbers for `MUX1`/`MUX2`/`MUX3`/`MUXE`, drive polarity, and reset states.
3. Select an ADC sample time compatible with MUX on-resistance, sensor divider impedance, and ADC sampling capacitance.
4. Measure or conservatively bound settling after each MUX address change; account for channel-to-channel switching.
5. Define ADC reference/calibration assumptions and voltage conversion formula using actual configured resolution and reference.
6. Confirm thermistor voltage limits, fault thresholds, and whether a disconnected sensor produces a reliably detectable voltage.
7. Confirm the conversion path from measured Satellite voltage to the Main temperature LUT; do not assume the prior LUT's voltage range matches this revised analog front end.

**Obsolete design:** Do not use ADS1256 SPI commands or ADS1256-specific conversion assumptions; the October 2026 Satellite uses the STM32 internal ADC.

## 4. Isolated SPI communications

The Satellite MCU connects through an **LTC6820**. The implementation must define and verify:

- MCU SPI instance and exact pins; clock polarity/phase, bit order, and baud rate.
- Main/Satellite transaction initiation, chip-select or wakeup handling, and response deadline.
- Packet layout, units, field widths, byte order, and integrity checking.
- How a coherent completed sensor snapshot is made available during an incoming transfer.
- Timeout, invalid frame, missed response, and link-recovery behavior.

**Open decision:** Whether a polling-only loop can meet SPI slave-response timing, or an interrupt/peripheral-assisted path is required. No RTOS or DMA choice is finalized by this document.

## 5. Main interfaces

| Interface | Purpose | Verification needed |
|---|---|---|
| STM32F407 ↔ isolated SPI interface | Request/receive Satellite measurements | MCU peripheral/pin map, topology, timings, packet protocol |
| STM32F407 ↔ CAN | Publish temperature and diagnostic information | CAN peripheral/pins, message IDs, payload layout, cycle times, error handling |
| Main application ↔ temperature conversion | Convert validated voltages to temperature | Units, LUT applicability, range/error behavior |
| Main application ↔ fault logic | Distinguish valid, invalid, stale, and faulted data | Thresholds, deadlines, latching/recovery policy |

Main-board pin-level assignments and CAN identifiers are **not asserted here** without an approved Main schematic and interface specification.

## 6. Configuration verification checklist

- [ ] Satellite MCU variant and clock tree match approved PCB.
- [ ] PA0/PA1/PA2 ADC channels and analog configuration verified.
- [ ] MUX address/enable GPIO pins and polarity verified.
- [ ] MUX mapping compared against all 23 schematic nets and PCB labels.
- [ ] ADC sample time, reference, and settling characterized on hardware.
- [ ] LTC6820 MCU SPI pins/mode/timing verified.
- [ ] Main CAN and isolated SPI pins verified against Main schematic.
- [ ] Communication protocol and error handling documented and tested.
- [ ] Hardware revision, `.ioc` revision, and firmware commit recorded for test results.

## 7. Change control

Record schematic/PCB changes here and update relevant `.ioc` files, mappings, test vectors, and requirements together. Treat an unverified mapping or timing parameter as an open item rather than an implementation fact.
