# UV26-TMS-V2

Firmware for the second-generation **Thermal Monitoring System (TMS)** developed by **UVic Formula Racing** for the UV26 electric vehicle.

The TMS uses a Main–Satellite architecture to monitor accumulator thermistors. Each Satellite acquires up to 23 thermistor measurements and communicates with the Main over an isolated SPI link. The Main processes incoming measurements and interfaces with the vehicle over CAN.

> **Project status:** Active development. Successful firmware builds and host-side tests do **not** imply completed hardware integration or validated vehicle safety behavior.

## Hardware overview

| Board | MCU | Role |
|---|---|---|
| Main | STM32F407 | Collect Satellite data, process/report temperatures, interface with vehicle CAN |
| Satellite | STM32L151CBT6A | Select thermistors using three analog multiplexers, acquire voltages with the MCU's internal ADC, communicate through LTC6820 |

The October 2026 Satellite schematic uses **three SN74HC4851-Q1 multiplexers** and the **STM32 internal ADC**. The ADS1256 external ADC referenced in older project materials is **not** part of this revised Satellite design.

## Repository areas

- `firmware/main/` — Main STM32 firmware
- `firmware/satellite/` — Satellite STM32 firmware
- `tests/main_tests/temperature_tests/` — temperature conversion SIL tests
- `tests/satellite-tests/` — MUX mapping SIL tests
- `docs/` — architecture, interfaces, requirements, and development guides

## Quick start: host-side tests

Install CMake, Ninja, and a working native C compiler (such as GCC), and run from the repository root in PowerShell or a Linux shell:

```sh
cmake -S tests/main_tests/temperature_tests -B tests/main_tests/temperature_tests/build -G Ninja
cmake --build tests/main_tests/temperature_tests/build
ctest --test-dir tests/main_tests/temperature_tests/build -V

cmake -S tests/satellite-tests -B tests/satellite-tests/build -G Ninja
cmake --build tests/satellite-tests/build
ctest --test-dir tests/satellite-tests/build -V
```

The latest reported successful runs covered **45 Main checks** and **30 Satellite checks**. These test portable logic, not on-board ADC, SPI, or CAN operation. See [Development and Testing](docs/development-and-testing.md) for Windows/Linux prerequisites and troubleshooting.

## Documentation

| Document | Purpose |
|---|---|
| [Firmware Architecture](docs/firmware-architecture.md) | Design principles, implementation baseline, proposed execution model, module boundaries |
| [Development and Testing](docs/development-and-testing.md) | Windows/Linux SIL build and test procedures |
| [Hardware–Software Interface](docs/hardware-software-interface.md) | Board signals, ADC/MUX interface, communication boundaries, configuration checks |
| [Software Requirements](docs/software-requirements.md) | Draft traceable behavior requirements and verification plans |

## Engineering principle

**Simplify, then add lightness.** Prefer the simplest firmware architecture that can be shown to satisfy timing, functional, safety, and maintainability requirements. FreeRTOS removal, DMA avoidance, and a cooperative superloop are **proposals under evaluation**, not confirmed implementation decisions.

## Contributing

Keep hardware-specific code separate from portable application logic where practical. Add or update host-side tests for portable behavior changes. Verify `.ioc` settings against the approved schematic before relying on MCU pin assignments. Document changes to communication formats, timing assumptions, and fault behavior.
