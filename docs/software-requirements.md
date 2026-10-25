# TMS Software Requirements

**Project:** UV26-TMS-V2  
**Status:** Draft requirements for team review — **not yet an approved safety specification**  
**Revision:** 0.1 — 2026-10-25

## 1. Scope

This document describes intended behavior for the Main STM32F407 and Satellite STM32L151 firmware. It is written to support design reviews and traceability to host-side and on-board tests. Specific limits and timing values must be agreed with Electronics and checked against applicable competition rules and the accumulator design before this document is baselined.

**Terminology:** *valid* = passes defined checks; *invalid* = fails measurement/protocol checks; *stale* = was valid but is older than the allowed age; *snapshot* = coherent set of measurements from a completed acquisition cycle. A status flag must accompany any data whose validity is uncertain.

## 2. Requirements and verification matrix

`SIL` means host-side logic testing; `HIL` means hardware-in-the-loop; `bench` means testing on actual boards. Requirement IDs are proposed identifiers, not claims of implementation.

| ID | Draft requirement | Intended verification |
|---|---|---|
| TMS-SWR-001 | The Satellite shall initialize required clocks, GPIO, ADC, and communication peripherals before declaring measurement data ready. | Bench startup/reset test |
| TMS-SWR-002 | The Satellite shall acquire all 23 populated thermistor channels using the configured MUX mapping. | SIL mapping + bench channel injection |
| TMS-SWR-003 | The acquisition module shall identify incomplete scans and shall not publish them as complete valid snapshots. | SIL state-machine + bench fault injection |
| TMS-SWR-004 | The Satellite shall apply a defined MUX settling and ADC sampling procedure before accepting each channel reading. | Bench timing/accuracy characterization |
| TMS-SWR-005 | ADC values shall be converted to documented electrical units using the configured ADC reference/resolution. | SIL conversion + bench reference test |
| TMS-SWR-006 | Measurements outside approved electrical bounds, or identified as sensor faults, shall be marked invalid. | SIL boundary tests + bench open/short tests |
| TMS-SWR-007 | The Satellite shall expose only coherent completed snapshots to the communications path. | SIL concurrency model + bench stress test |
| TMS-SWR-008 | The Main shall reject corrupted, malformed, or otherwise invalid Satellite messages. | SIL parser tests + bench corruption tests |
| TMS-SWR-009 | The Main shall identify missing or stale Satellite measurements after a specified deadline. | SIL fake-clock tests + bench disconnect |
| TMS-SWR-010 | The Main shall distinguish valid, invalid, and stale data in downstream reporting and fault decisions. | SIL status propagation + CAN bench capture |
| TMS-SWR-011 | Valid in-range thermistor voltages shall be converted to temperatures using the approved conversion characteristic. | Existing Main SIL + bench sensor tests |
| TMS-SWR-012 | An out-of-range or invalid temperature conversion shall return an explicit error/status and shall not be silently treated as valid. | Existing Main SIL + integration tests |
| TMS-SWR-013 | The Main shall detect an overtemperature condition according to approved thresholds and persistence policy. | SIL threshold tests + bench fault injection |
| TMS-SWR-014 | Loss of a Satellite shall be detected and represented as a communication fault after the approved timeout. | SIL timeout tests + bench unplug test |
| TMS-SWR-015 | Required CAN outputs shall use the approved message identifiers, payload formats, status conventions, and reporting periods. | Bench CAN analyzer |
| TMS-SWR-016 | Recoverable faults shall clear only under documented recovery conditions; any latched faults shall follow the approved reset policy. | SIL transition tests + bench recovery |
| TMS-SWR-017 | The firmware shall expose diagnostic information sufficient to distinguish invalid sensor data, stale data, and communication errors. | SIL + bench diagnostic inspection |
| TMS-SWR-018 | Every blocking wait or retry in a time-critical path shall have a documented finite bound or a justified hardware guarantee. | Code review + bench timing tests |
| TMS-SWR-019 | If a watchdog is enabled, it shall be serviced only when the defined progress/health conditions are satisfied. | Code review + bench fault injection |
| TMS-SWR-020 | On reset or startup, previously buffered values shall not be reported as current valid measurements until fresh data is acquired and validated. | SIL startup state + bench reset test |

## 3. Parameters requiring approval

| Parameter | Current status | Responsible review |
|---|---|---|
| Scan period / maximum scan duration | TBD | Software + Electronics |
| MUX settling delay and ADC sample time | TBD, measure on board | Electronics + Software |
| ADC reference and valid sensor voltage range | TBD, verify analog front end | Electronics |
| Temperature threshold(s) and persistence | TBD | Electronics + accumulator safety owner |
| Satellite response deadline / retry policy | TBD | Software + Electronics |
| Stale-data timeout | TBD | Software + Electronics |
| CAN IDs, payloads, report period | TBD | VCU/BMS interface owners |
| Fault latching, reset, and recovery policy | TBD | Safety review |
| Watchdog period and feed conditions | TBD | Software + Electronics |

**Do not substitute guessed numeric limits** for the TBD values. If a competition rule imposes a mandatory response or protection requirement, cite its exact edition and clause during review.

## 4. Fault behavior to design and test

| Event | Detection concept | Required decision before implementation |
|---|---|---|
| Open/short thermistor | ADC voltage outside approved envelope, with circuit-specific interpretation | Electrical thresholds, filtering, and status propagation |
| ADC or scan failure | ADC timeout, incomplete scan, or implausible conversion | Retry vs invalidation; when to report failure |
| Corrupted Satellite frame | Integrity and format checks | Reject/diagnose; whether to retry |
| Missing Satellite | No valid frame before deadline | Fault threshold, CAN indication, recovery |
| Stale but previously valid data | Elapsed time since last valid update | Max allowed age; how to report |
| Overtemperature | Approved conversion and threshold logic | Persistence, latching, reporting, recovery |
| MCU reset | Startup/reset-reason capture | Safe initial status and diagnostics |

## 5. Existing verification baseline

As last reported by the development team:

- Main temperature conversion: **45 individual checks passed** (anchors, interpolation, monotonicity, invalid inputs).
- Satellite MUX mapping: **30 individual checks passed** (23 mappings, uniqueness, unused position, invalid inputs).
- Both Main and Satellite target firmware produced successful debug builds.

These results support only the tested modules and buildability. **They do not verify** the full requirements matrix, end-to-end operation, or vehicle safety behavior.

## 6. Open architecture decisions

- Whether to replace FreeRTOS with a cooperative superloop after worst-case timing analysis.
- Whether ADC polling suffices or DMA measurably improves deadline compliance.
- Which communication events require interrupts and how shared snapshots are synchronized.
- Whether temperature conversion and fault policy execute on Satellite, Main, or both, and which module owns the authoritative status.

Document the rationale and test evidence when each decision is finalized in [Firmware Architecture](firmware-architecture.md).

## 7. Traceability maintenance

When implementing a requirement, add its production module/function, test case name, observed result, test date, hardware revision (if applicable), and commit hash. Keep draft/implemented/verified statuses distinct. Review this document after hardware, protocol, or competition-rule changes.
