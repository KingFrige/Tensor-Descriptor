# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

This is a C11 project using CMake as the build system. A wrapper script `build.sh` provides convenient build commands.

### Building

```bash
# Build only
./build.sh --build
# or
cd build && cmake .. && make

# Build and run all tests
./build.sh --all

# Show all build script options
./build.sh --help
```

### Build Outputs

- Static library: `build/libtensor_descriptor_lib.a`
- Test executables: `build/constraint_test`, `build/batch_constraint_test`

## Running Tests

### Run All Tests

```bash
./build.sh --test
```

### Run Individual Tests

```bash
# Single constraint test
./build/constraint_test

# Batch constraint test
./build/batch_constraint_test
```

### Memory Testing with Valgrind

```bash
./build.sh --valgrind
```

Generates reports in `build/valgrind-reports/`.

## Project Architecture

### Directory Structure

```
src/        - Core library implementation
tests/      - Test suite with utilities
build/      - CMake build directory (generated)
assets/     - Test data and JSON tensor definitions
docs/       - Documentation (ggml_tensor.md, cube.svg)
openspec/   - OpenSpec change management workflow
```

### Core Components

#### 1. Tensor Descriptor (`src/tensor_descriptor.h`)

Two-level tensor representation system:

**Architecture Tensor (External API):**
- `tensor_descriptor_t` - Public API structure with baseAddr, dimension[5], stride[5]
- 5D tensor layout: [byte, unit, slice, plane, cube]

**Microarchitecture Tensor (Internal):**
- `microarch_tensor_descriptor_t` - Hardware-level representation
- Contains byteNum, unitNum, sliceNum, planeNum, cubeNum dimensions
- Skip values for address calculation: unitSkip, sliceSkip, planeSkip, cubeSkip

#### 2. Constraint System

**User Constraints** (`tensor_constraints_t`):
- `maxByteNum`, `maxUnitNum`, `maxSliceNum`, `maxPlaneNum`, `maxCubeNum`
- `maxTotalBytes` - Total memory limit
- `enableBalance` - Enable dimension balancing (micro/arch dimension ratio optimization)

**Physical Limits** (`tensor_physical_limits_t`):
- Hardware constraints: maxPhysicalByteNum (default 64), maxPhysicalUnitNum (default 1024), etc.

#### 3. Conversion Pipeline

```
tensor_descriptor_convert()
    ↓
ggml_tensor_convert_with_constraints()
    ↓
microarch_constraints_convert()
    ↓
Compute effective limits (min of physical/constraints)
Apply dimension overflow logic (byte→unit→slice→plane→cube)
Calculate skip values (next_power_of_2 for unitSkip)
Validate against constraints
```

#### 4. Dimension Overflow Logic

When a dimension exceeds its effective limit, it overflows to the next dimension:

```c
// Example: byte overflow increases unit count
while (remainingBytes >= effectiveLimits.maxByte) {
    spillUnits++;
    remainingBytes -= effectiveLimits.maxByte;
}
```

#### 5. Skip Value Calculation

```c
unitSkip  = next_power_of_2(byteNum);
sliceSkip = unitNum * unitSkip;
planeSkip = sliceNum * sliceSkip;
cubeSkip  = planeNum * planeSkip;
```

### Test Utilities (`tests/utils.c`)

Helper functions for creating test tensors:
- `ggml_tensor_create_random()` - Random architecture tensor
- `microarch_tensor_create_random()` - Random microarchitecture tensor
- `microarch_tensor_traversal()` - Generate traversal pattern for verification

### Error Codes

| Code | Value | Meaning |
|------|-------|---------|
| `E_SUCCESS` | 0 | Success |
| `E_BYTE_NUM_EXCEEDED` | -1 | Byte count exceeds limit |
| `E_OVER_CONSTRAINED` | -2 | Constraints too restrictive |
| `E_INVALID_DIMENSION` | -3 | Invalid dimension values |
| `E_PHYSICAL_CONSTRAINT` | -4 | Physical limit violation |

## Development Guidelines

### C11 Standard
- Code must be C11 compliant
- No external library dependencies (including math.h)
- Bit operations replace math functions (e.g., `next_power_of_2()`)

### Build Requirements
- CMake 3.0+
- C11 compiler (gcc/clang)
- Valgrind (optional, for memory testing)

### File Organization
- All source in `src/`, headers in `src/`
- Tests in `tests/`, shared utilities in `tests/utils.c`
- No includes outside `src/` and `tests/`

### Testing Protocol
- Run `./build.sh --all` before committing
- Check valgrind output: `./build.sh --valgrind`
- All tests must pass with zero memory leaks
