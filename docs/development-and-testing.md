# Development and Testing

This document describes how to build and run the host-side Software-in-the-Loop (SIL) tests for **UV26-TMS-V2**. Run all commands from the repository root unless stated otherwise.

## Purpose and scope

The SIL tests compile portable C modules for the development computer and check their behavior without STM32 hardware. They are distinct from building the Main and Satellite firmware for their respective microcontrollers. A passing SIL test does **not** demonstrate that ADC acquisition, analog multiplexers, ISO-SPI communication, or CAN communication work on hardware.

## Prerequisites

### Windows (VS Code / PowerShell)

- Open the `UV26-TMS-V2` repository folder in VS Code.
- Open **Terminal → New Terminal** and select **PowerShell**.
- Install CMake, Ninja, and a compatible native C compiler (such as GCC). Ensure these tools are available from the VS Code integrated terminal. MSYS2 is one option on Windows; installation paths and toolchain variants differ.
- Verify that the terminal is at the repository root.

Verify your tools:

```powershell
cmake --version
ninja --version
gcc --version
```

### Linux (VS Code terminal or shell)

Install CMake, Ninja, and a native C compiler such as GCC. On Fedora, for example:

```bash
sudo dnf install cmake ninja-build gcc
```

Open the repository root in VS Code or navigate there in your terminal. Verify the tools:

```bash
cmake --version
ninja --version
gcc --version
```

The commands in the following test sections work in **both PowerShell and Linux shells**. If a build directory was configured on another operating system or with another compiler, remove that directory and reconfigure it locally.

## Main: temperature conversion SIL tests

Configure, build, and run with verbose CTest output:

```sh
cmake -S tests/main_tests/temperature_tests -B tests/main_tests/temperature_tests/build -G Ninja
cmake --build tests/main_tests/temperature_tests/build
ctest --test-dir tests/main_tests/temperature_tests/build -V
```

**Last verified result:** 45 checks passed, 0 failed. Coverage includes LUT reference points, interpolation, monotonicity, and invalid-input handling.

## Satellite: MUX mapping SIL tests

Configure, build, and run with verbose CTest output:

```sh
cmake -S tests/satellite-tests -B tests/satellite-tests/build -G Ninja
cmake --build tests/satellite-tests/build
ctest --test-dir tests/satellite-tests/build -V
```

**Last verified result:** 30 checks passed, 0 failed. Coverage includes the 23 sensor mappings, mapping uniqueness/completeness, the unused multiplexer channel, and invalid inputs.

> **CTest terminology:** CTest may report one *test executable* or *CTest test* per suite, while that executable prints many individual checks. The 45 and 30 counts above refer to individual checks, not necessarily CTest's summary count.

## Running tests again

After the build directories have been configured, only the build and test commands are needed:

```sh
cmake --build tests/main_tests/temperature_tests/build
ctest --test-dir tests/main_tests/temperature_tests/build -V

cmake --build tests/satellite-tests/build
ctest --test-dir tests/satellite-tests/build -V
```

Use `-V` (or `--verbose`) to show test program output. If a test program does not print per-check messages, verbose mode cannot invent them.

## Troubleshooting

### CMake generator or compiler mismatch

CMake caches the compiler and generator in each build directory. If you move between computers or change toolchains, remove **only the affected test build directory** and configure again:

```powershell
Remove-Item -Recurse -Force tests/main_tests/temperature_tests/build
# Or, for the Satellite suite:
Remove-Item -Recurse -Force tests/satellite-tests/build
```

On Linux, use `rm -rf tests/main_tests/temperature_tests/build` or `rm -rf tests/satellite-tests/build` for the affected suite. Then rerun that suite's configure, build, and test commands above.

### `gcc`, `ninja`, or `cmake` is not recognized

Check the installation and `PATH` for your chosen toolchain. Configure your environment according to that toolchain’s instructions; no installation directory is assumed here.

### Tests pass but no individual `[PASS]` lines appear

CTest `-V` shows whatever the test executable writes to standard output. Check that you are on the intended Git branch and have rebuilt the latest source:

```sh
git status
git branch --show-current
git log -1 --oneline
```

### Confirm the current test results

The expected counts above reflect the last successful run, not a guarantee for future revisions. Update this document when tests are added, removed, or renamed.

## STM32 firmware builds

The SIL commands above build **native host test executables** (Windows or Linux), not STM32 `.elf` files. Main and Satellite target firmware use their own CMake presets and cross-compilation toolchain. Build those separately when checking target compilation; flashing and on-board verification require additional hardware testing.
