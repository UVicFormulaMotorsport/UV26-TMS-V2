# UV26-TMS-V2
The firmware for version 2 of the thermal monitoring system, an STM32-based, main-satellite architecture circuit inside the tractive battery system of UV26.

## Host-Based Unit Testing (SIL)

The TMS firmware supports host-based Software-in-the-Loop (SIL) testing, allowing developers to compile and execute selected production firmware modules directly on their computers without requiring STM32 hardware.

The same production C source files are compiled for both the host computer and the STM32 target. This allows application logic to be tested independently of hardware-dependent components.

Currently supported:
- Temperature lookup-table conversion and linear interpolation
- Native Windows testing using MSYS2 GCC
- Native Linux testing using GCC (pending verification)

### Windows Setup

**Prerequisites**

Install the following:

- [MSYS2](https://www.msys2.org/) with the UCRT64 GCC toolchain
- CMake
- Ninja

Install GCC and Ninja from an MSYS2 UCRT64 terminal:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja
```

The expected installation directory is:

```text
C:\msys64\ucrt64\bin
```

**Configure, build, and run tests**

Open PowerShell in the repository root.

First, ensure the MSYS2 compiler takes precedence over potentially conflicting STM32CubeCLT runtime libraries:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
```

Navigate to the temperature test directory:

```powershell
cd tests/main_tests/temperature_tests
```

Configure CMake

```powershell
cmake -S . -B build -G Ninja `
    -DCMAKE_C_COMPILER="C:/msys64/ucrt64/bin/gcc.exe" `
    -DCMAKE_MAKE_PROGRAM="C:/msys64/ucrt64/bin/ninja.exe"
```

Build the tests:

```powershell
cmake --build build
```

Execute:

```powershell
ctest --test-dir build --verbose
```

CTest reports each named conversion check and the final failure count. Its exit code is `0` when all checks pass and nonzero if a check fails.

**Troubleshooting**

If GCC fails unexpectedly, verify which compiler and runtime libraries Windows resolves:

```powershell
where.exe gcc
where.exe libwinpthread-1.dll
```

The first matching paths should point to `C:\msys64\ucrt64\bin`.

If CMake reports a compiler or generator mismatch, delete the test directory's `build` folder and configure again.

### Linux Setup (Fedora)

**Prerequisites**

Install GCC, CMake, and Ninja:

```bash
sudo dnf install gcc cmake ninja-build
```

Verify installation:

```bash
gcc --version
cmake --version
ninja --version
```

**Configure, build, and run tests**

From the repository root:

```bash
cd tests/main_tests/temperature_tests
```

Configure CMake:

```bash
cmake -S . -B build -G Ninja
```

Build:

```bash
cmake --build build
```

Execute:

```bash
ctest --test-dir build --verbose
```

CTest reports each named conversion check and the final failure count. Its exit code is `0` when all checks pass and nonzero if a check fails.

Unlike Windows, Linux generally requires no additional compiler path configuration.

**Note:** The Linux commands have not yet been verified on the team's development environment.

### Re-running Tests After Code Changes

Once CMake has been configured, there is no need to repeat the configuration step for ordinary source changes.

Windows:

```powershell
cmake --build build
ctest --test-dir build --verbose
```

Linux:

```bash
cmake --build build && ctest --test-dir build --verbose
```

Run these commands from `tests/main_tests/temperature_tests`.

### STM32 Firmware Build

Host tests do not replace target compilation.

The STM32 firmware is built separately using the ARM embedded GCC toolchain and the firmware's CMake presets.

From `firmware/main`:

```bash
cmake --preset Debug
cmake --build --preset Debug
```

This produces the STM32 firmware executable (`tms_main.elf`) without requiring a connected board.

The STM32 build requires an installed ARM cross-compiler and correctly configured toolchain paths. The commands above have been verified on Windows; Linux target-build setup remains to be verified.

### Project Structure

```text
UV26-TMS-V2/
├── firmware/
│   └── main/
│       ├── app/
│       │   └── temperature/
│       │       ├── temperature.c
│       │       └── temperature.h
│       └── CMakeLists.txt
│
└── tests/
    └── main_tests/
        └── temperature_tests/
            ├── CMakeLists.txt
            └── test_temperature.c
```

The host test executable compiles the same `temperature.c` source used by the STM32 firmware.

### Development Guidelines

- Keep hardware-independent application logic separate from STM32 HAL and peripheral drivers.
- Add host unit tests when introducing or modifying portable firmware logic.
- Ensure tests pass before submitting a pull request.
- Verify that the STM32 firmware still builds successfully.
- Do not commit generated build directories or compiled binaries.

The long-term goal is to expand host-based testing to communication protocols, CRC validation, fault detection, sensor validity, and CAN message generation.

## Host-Based Unit Testing (SIL)

The TMS firmware supports host-based Software-in-the-Loop (SIL) testing, allowing developers to compile and execute selected production firmware modules directly on their computers without requiring STM32 hardware.

The same production C source files are compiled for both the host computer and the STM32 target. This allows application logic to be tested independently of hardware-dependent components.

Currently supported:
- Temperature lookup-table conversion and linear interpolation
- Native Windows testing using MSYS2 GCC
- Native Linux testing using GCC (pending verification)

### Windows Setup

**Prerequisites**

Install the following:

- [MSYS2](https://www.msys2.org/) with the UCRT64 GCC toolchain
- CMake
- Ninja

Install GCC and Ninja from an MSYS2 UCRT64 terminal:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja
```

The expected installation directory is:

```text
C:\msys64\ucrt64\bin
```

**Configure, build, and run tests**

Open PowerShell in the repository root.

First, ensure the MSYS2 compiler takes precedence over potentially conflicting STM32CubeCLT runtime libraries:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
```

Navigate to the temperature test directory:

```powershell
cd tests/main_tests/temperature_tests
```

Configure CMake

```powershell
cmake -S . -B build -G Ninja `
    -DCMAKE_C_COMPILER="C:/msys64/ucrt64/bin/gcc.exe" `
    -DCMAKE_MAKE_PROGRAM="C:/msys64/ucrt64/bin/ninja.exe"
```

Build the tests:

```powershell
cmake --build build
```

Execute:

```powershell
ctest --test-dir build --verbose
```

CTest reports each named conversion check and the final failure count. Its exit code is `0` when all checks pass and nonzero if a check fails.

**Troubleshooting**

If GCC fails unexpectedly, verify which compiler and runtime libraries Windows resolves:

```powershell
where.exe gcc
where.exe libwinpthread-1.dll
```

The first matching paths should point to `C:\msys64\ucrt64\bin`.

If CMake reports a compiler or generator mismatch, delete the test directory's `build` folder and configure again.

### Linux Setup (Fedora)

**Prerequisites**

Install GCC, CMake, and Ninja:

```bash
sudo dnf install gcc cmake ninja-build
```

Verify installation:

```bash
gcc --version
cmake --version
ninja --version
```

**Configure, build, and run tests**

From the repository root:

```bash
cd tests/main_tests/temperature_tests
```

Configure CMake:

```bash
cmake -S . -B build -G Ninja
```

Build:

```bash
cmake --build build
```

Execute:

```bash
ctest --test-dir build --verbose
```

CTest reports each named conversion check and the final failure count. Its exit code is `0` when all checks pass and nonzero if a check fails.

Unlike Windows, Linux generally requires no additional compiler path configuration.

**Note:** The Linux commands have not yet been verified on the team's development environment.

### Re-running Tests After Code Changes

Once CMake has been configured, there is no need to repeat the configuration step for ordinary source changes.

Windows:

```powershell
cmake --build build
ctest --test-dir build --verbose
```

Linux:

```bash
cmake --build build && ctest --test-dir build --verbose
```

Run these commands from `tests/main_tests/temperature_tests`.

### STM32 Firmware Build

Host tests do not replace target compilation.

The STM32 firmware is built separately using the ARM embedded GCC toolchain and the firmware's CMake presets.

From `firmware/main`:

```bash
cmake --preset Debug
cmake --build --preset Debug
```

This produces the STM32 firmware executable (`tms_main.elf`) without requiring a connected board.

The STM32 build requires an installed ARM cross-compiler and correctly configured toolchain paths. The commands above have been verified on Windows; Linux target-build setup remains to be verified.

### Project Structure

```text
UV26-TMS-V2/
├── firmware/
│   └── main/
│       ├── app/
│       │   └── temperature/
│       │       ├── temperature.c
│       │       └── temperature.h
│       └── CMakeLists.txt
│
└── tests/
    └── main_tests/
        └── temperature_tests/
            ├── CMakeLists.txt
            └── test_temperature.c
```

The host test executable compiles the same `temperature.c` source used by the STM32 firmware.

### Development Guidelines

- Keep hardware-independent application logic separate from STM32 HAL and peripheral drivers.
- Add host unit tests when introducing or modifying portable firmware logic.
- Ensure tests pass before submitting a pull request.
- Verify that the STM32 firmware still builds successfully.
- Do not commit generated build directories or compiled binaries.

The long-term goal is to expand host-based testing to communication protocols, CRC validation, fault detection, sensor validity, and CAN message generation.

## Running Host Unit Tests (Windows)

Host unit tests compile and execute production firmware logic natively on Windows using MSYS2 UCRT64 GCC, without requiring STM32 hardware.

### Prerequisites

- CMake
- Ninja
- MSYS2 UCRT64 GCC (`C:\msys64\ucrt64\bin\gcc.exe`)

### Build and run temperature tests

From the repository root, open a PowerShell terminal:

```powershell
cd tests/main_tests/temperature_tests

cmake -S . -B build -G Ninja `
    -DCMAKE_C_COMPILER="C:/msys64/ucrt64/bin/gcc.exe" `
    -DCMAKE_MAKE_PROGRAM="C:/msys64/ucrt64/bin/ninja.exe"

cmake --build build

.\build\test_temperature.exe

$LASTEXITCODE
```

A successful test returns exit code `0`. A failed assertion terminates the test with a nonzero exit code.

**Note:** On Windows, MSYS2 UCRT64's `bin` directory must appear before conflicting STM32CubeCLT directories in `PATH`. If necessary, run:

```powershell
$env:Path = "C:\msys64\ucrt64\bin;" + $env:Path
```

### Verify STM32 firmware build

From the repository root:

```powershell
cd firmware/main

cmake --preset Debug
cmake --build --preset Debug --clean-first
```

This cross-compiles the production firmware for the STM32F407 using the ARM toolchain. It does not require a connected board.

**Both builds compile the same production `temperature.c` source file.**