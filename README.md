# UV26-TMS-V2
The firmware for version 2 of the thermal monitoring system, an STM32-based, main-satellite architecture circuit inside the tractive battery system of UV26.

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