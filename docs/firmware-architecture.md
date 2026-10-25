# UV26-TMS-V2 — Firmware Architecture

**Project:** UVic Formula Racing Electric — Thermal Monitoring System (TMS)  
**Document status:** Working architecture proposal  
**Revision:** 0.1 — 2026-10-25  
**Audience:** TMS software developers, Electronics team, future Software Leads

> **Design principle — “Simplify, then add lightness.”** Choose the least complex architecture that demonstrably meets functional, timing, safety, and maintainability requirements. Complexity must justify itself through a measured or documented need. Simplicity never overrides required fault detection or safe behavior.

## 1. Purpose and scope

This document records the TMS firmware architecture, the confirmed hardware baseline, and open implementation decisions. It distinguishes **confirmed hardware**, **implemented and tested software**, and **proposed design choices**. It is not evidence of hardware validation or compliance with competition requirements.

The system comprises a Main board and multiple Satellite boards. Satellites acquire battery-module thermistor voltages; the Main aggregates and processes reported data and interfaces with the vehicle CAN network. The design must tolerate invalid measurements, communication loss, and MCU resets without silently presenting stale or invalid readings as current.

## 2. Hardware baseline

**Authoritative design reference:** Satellite schematic dated **2026-10-20**, supplied by Electronics (`Schematic PDF_[No Variations] (1).pdf`). Confirm pin assignments against the latest approved schematic and CubeMX `.ioc` before implementation.

| Item | Current hardware understanding | Confidence |
|---|---|---|
| Satellite MCU | STM32L151CBT6A | Confirmed in schematic |
| Thermistor inputs | 23 per Satellite | Confirmed in schematic |
| Analog selection | Three SN74HC4851-Q1 8:1 multiplexers with shared selection lines | Confirmed in schematic |
| Analog conversion | **Internal STM32L151 ADC**, not an ADS1256 | Confirmed by analog MUX outputs wired to MCU analog inputs |
| MUX outputs to MCU | `MUX01`, `MUX02`, `MUX03` routed to PA0, PA1, PA2 respectively | Confirmed in schematic; verify `.ioc` ADC channel configuration |
| Isolated communications | MCU SPI through LTC6820 transceiver | Confirmed in schematic |
| Main MCU | STM32F407 | Existing firmware/project baseline; verify against Main schematic |
| Vehicle interface | Main board CAN | Existing system architecture; verify Main schematic |

**Obsolete assumption:** Earlier documentation and diagrams refer to an **ADS1256 external SPI ADC**. That component is **not present in the October 2026 Satellite schematic**. Do not design the new acquisition driver around ADS1256 commands, timing, or SPI transactions.

The shared MUX address lines select the same numbered channel on each of the three multiplexers. An acquisition cycle can therefore iterate through **eight MUX selections**, reading up to three MCU ADC inputs per selection. The final selection includes an unused position: the third multiplexer has seven connected thermistors, for **23 total**. Actual sensor ordering is defined by the tested MUX mapping module, not by assuming sequential wiring.

## 3. Architectural objectives

1. **Correctness and safety first.** Invalid, incomplete, or stale data must have explicit status and must not be represented as valid current temperatures.
2. **Deterministic behavior.** Bound waiting times and document acquisition, communication, reporting, and fault-detection deadlines.
3. **Minimal moving parts.** Avoid schedulers, DMA, dynamic allocation, and abstraction layers unless requirements justify them.
4. **Clear ownership.** Each peripheral and mutable buffer has a defined owner; avoid uncontrolled cross-module globals.
5. **Portable core logic.** Keep conversion, mapping, packet validation, freshness checks, and fault policies independently testable on a host PC.
6. **Maintainability.** Prefer small modules with explicit interfaces, understandable failure paths, and repeatable build/test instructions.

## 4. Current implementation baseline

The following are **demonstrated by the latest reported local runs**, not a claim of on-target functionality:

| Component | Latest reported result |
|---|---|
| Main firmware (`tms_main.elf`) | Debug target build succeeded |
| Satellite firmware (`satellite.elf`) | Debug target build succeeded |
| Main temperature-voltage conversion SIL | 45 checks passed, 0 failed |
| Satellite MUX mapping SIL | 30 checks passed, 0 failed |

These tests validate portable logic only. They do **not** establish correct ADC configuration, analog settling, SPI/isoSPI timing, CAN operation, or fault response on the car.

## 5. Proposed execution model — decision pending

**Proposal:** Evaluate a **bare-metal cooperative superloop** on Main and Satellite rather than retaining FreeRTOS. This is **not yet an approved implementation decision**. Review existing RTOS dependencies and timing constraints first.

A cooperative loop repeatedly advances short, bounded application state machines:

```c
int main(void)
{
    hardware_init();
    application_init();

    for (;;) {
        communication_process();
        acquisition_process();   /* Satellite */
        fault_monitor_process();
        diagnostics_process();
    }
}
```

This is **conceptual pseudocode**, not a proposed common `main.c` for both boards. Main and Satellite have different responsibilities. Each processing function should return promptly, avoid unbounded waits, and use explicit state rather than implicit scheduling assumptions. Timers may provide deadlines; interrupts should perform minimal work and hand off events to the loop.

**Criteria before removing FreeRTOS:**

- Identify every existing task, interrupt, semaphore, queue, and timing dependency.
- Establish required scan, communication-response, CAN-reporting, and fault-detection deadlines from system requirements and rules.
- Measure or bound worst-case execution time and blocking latency.
- Demonstrate that communication remains responsive throughout acquisition.
- Verify recovery from peripheral timeout, interrupted transfer, and MCU reset.

If the cooperative model cannot meet those requirements cleanly, retaining an RTOS or introducing selective interrupt-driven operations remains an option.

## 6. Satellite acquisition — proposed design

### 6.1 Acquisition pipeline

```text
Thermistors (23)
    -> three 8:1 analog multiplexers
    -> STM32L151 internal ADC inputs (PA0/PA1/PA2)
    -> raw readings / voltages
    -> tested physical-to-logical sensor mapping
    -> completed acquisition snapshot
    -> communication packet to Main
```

### 6.2 Proposed state machine

```text
SELECT_MUX_CHANNEL
       |
       v
WAIT_FOR_ANALOG_SETTLING
       |
       v
SAMPLE_VALID_ADC_INPUTS
       |
       v
STORE_MAPPED_RESULTS
       |
       +-- more MUX selections --> SELECT_MUX_CHANNEL
       |
       v
VALIDATE_SCAN_COMPLETENESS
       |
       v
PUBLISH_COMPLETE_SNAPSHOT
       |
       v
SELECT_MUX_CHANNEL
```

At each of eight selection values, sample the valid ADC inputs. Skip the unused MUX 2/channel 7 combination. Handle ADC timeout, out-of-range values, and incomplete scans explicitly. The mapping module determines which physical MUX/channel corresponds to each logical sensor index.

**Open hardware questions:** analog settling time after switching the MUX, ADC sampling time/resolution, reference voltage and calibration, expected voltage range, sensor open/short behavior, and whether the three ADC inputs require any additional analog precautions.

### 6.3 Polling versus DMA

**Initial proposal:** use ADC polling with **bounded timeouts**, and measure a complete 23-sensor scan. DMA is **not required by the hardware schematic**. Consider DMA or interrupt-driven acquisition only if polling demonstrably violates timing or responsiveness requirements. DMA is compatible with bare-metal firmware; choosing not to use FreeRTOS does not preclude it.

## 7. Communication and shared-data safety

A bare-metal design still has concurrency wherever interrupt handlers, DMA, or peripheral hardware access shared memory. Removing FreeRTOS removes task-level preemption, **not all races**.

**Proposed ownership model:**

- Acquisition code exclusively updates a **working scan buffer**.
- Communication code only reads a **completed, published snapshot**.
- Publication is synchronized with any ISR or DMA reader (for example, a short critical section around a pointer/index handoff, with buffer lifetime guaranteed).
- Every published snapshot carries validity and freshness metadata (e.g., sequence number and/or acquisition time).
- An incomplete scan is never published as a fully valid scan.

The exact snapshot and transfer protocol remains **undecided**. In particular, verify whether Main initiates each scan or polls the most recent completed Satellite snapshot, and whether the Satellite must service a master-initiated SPI transaction at any time.

For all SPI/isoSPI and CAN interactions, define transfer timeouts, failure counters, retry limits, stale-data policy, and safe recovery. Do not perform substantial packet processing inside ISRs.

## 8. Main board responsibilities

The Main board is expected to:

1. Receive and validate Satellite packets (including integrity, source, and freshness).
2. Convert validated voltages to temperatures using the existing portable conversion module, subject to the agreed data contract.
3. Track per-Satellite online/offline and last-valid information.
4. Evaluate temperature and communication faults according to defined requirements.
5. Report temperatures, status, and fault information over vehicle CAN.
6. Preserve diagnostic counters and reset/failure context where practical.

**Pending design choice:** Decide whether Satellite sends raw ADC counts, calibrated voltages, or temperatures. Existing host-tested temperature conversion resides in the Main software baseline; the wire format and responsibility split must be specified before integrating the protocol.

## 9. Reliability and failure behavior

The implementation must define and test behavior for:

- ADC conversion timeout or peripheral failure.
- Thermistor disconnected, shorted, or outside valid voltage range.
- Partial or inconsistent scan.
- Corrupt, missing, repeated, or out-of-order communication packet.
- Stale data after Satellite reset or communication loss.
- Overtemperature and recovery/hysteresis where required.
- CAN transmission failure or bus-off conditions.
- MCU reset, initialization failure, and watchdog behavior.

**Watchdog proposal:** use a hardware watchdog once the software has a well-defined progress/health criterion. Never feed it unconditionally if the application is stuck or failing critical work.

Specific thresholds, latencies, and fault actions are **not set by this document**; derive them from the software requirements, electronics constraints, and applicable competition rules.

## 10. Module boundaries — proposed

| Module | Portable host-testable logic | Hardware-specific adapter |
|---|---|---|
| MUX mapping | `(mux, channel) -> sensor index` | GPIO selection lines |
| Acquisition | Scan progression, completion, timeout policy | ADC initialization, conversion, voltage scaling |
| Temperature | Voltage-to-temperature LUT and validity | None required |
| Protocol | Encode/decode, CRC/integrity, sequence validation | SPI/isoSPI transport |
| Fault monitoring | State transitions, freshness, error classification | Timers, watchdog, CAN reporting |
| Diagnostics | Counters and event semantics | Reset-cause registers, optional debug output |

Do not create an abstraction solely because it appears in this table; add boundaries when they simplify testing, ownership, or hardware replacement.

## 11. Decision register

| ID | Decision / assumption | Status | Evidence or next action |
|---|---|---|---|
| ARCH-001 | Use internal STM32L151 ADC, not ADS1256 | **Confirmed hardware** | October 20 Satellite schematic |
| ARCH-002 | Three MUXes select 23 sensors via shared address lines | **Confirmed hardware** | October 20 Satellite schematic; mapping SIL |
| ARCH-003 | Portable temperature and MUX mapping modules | **Implemented/tested** | 45 Main + 30 Satellite SIL checks |
| ARCH-004 | Remove FreeRTOS on both boards | **Proposed** | Audit dependencies and deadlines |
| ARCH-005 | Poll ADC initially; no DMA initially | **Proposed** | Benchmark scan and response times |
| ARCH-006 | Publish only complete snapshots | **Proposed** | Design atomic handoff and protocol |
| ARCH-007 | Satellite scans continuously | **Undecided** | Confirm Main/Satellite request model |
| ARCH-008 | Raw counts vs voltage vs temperature on wire | **Undecided** | Agree data contract and units |
| ARCH-009 | Fault thresholds and recovery rules | **Undecided** | Trace to requirements and EV rules |

## 12. Next implementation steps

1. Compare the latest Satellite schematic with the authoritative Satellite `.ioc` (PA0–PA2 ADC inputs, MUX GPIOs, SPI, clocks, interrupts).
2. Confirm timing requirements and communication master/slave behavior with Electronics.
3. Write a small, portable acquisition state machine and SIL tests (selection order, all 23 indices, unused channel, timeout, incomplete scan).
4. Implement the STM32 ADC/GPIO adapter and measure scan time on hardware.
5. Specify the Satellite–Main packet contract and atomic snapshot handoff.
6. Integrate Main receive/validation and add fault-injection SIL tests.
7. Revisit FreeRTOS/DMA only with measured evidence.

---

### Document maintenance

Update the **decision register** whenever an architectural choice becomes approved or changes. Record the reason, supporting measurement/requirement, and relevant PR or issue. Do not describe a proposed architecture as deployed firmware until it is implemented and verified.
