# Microarchitecture Tensor Specification

## Overview

The microarchitecture tensor (`microarch_tensor`) is the hardware-facing representation of tensor data used by the DMA (Direct Memory Access) unit. It defines a 5-dimensional nested loop addressing model that maps directly to hardware DMA registers.

## Hardware Context

### DMA Unit Architecture (from tensor_engine_v1.5.html)

The DMA unit supports:
- **8 DMA channels** (dma Num = 8)
- **8 configurations per DMA** (each dma config = 8)
- **4-level nested loop addressing**: Unit → Slice → Plane → Cube

### Status Flags (Hardware Level)

| Status Flag | Hardware Effect |
|-------------|-----------------|
| `isFirstSlice` | ReRAM row init to first row of layer weight |
| `isLastSlice` | ReRAM jump to last row of layer weight |
| `isSliceDone` | iReg Reshape stall, clear iRegs, bankRow++, switch weight, mRegs update |
| `isPlaneDone` | iReg Reshape stall, clear iRegs, bankRow loop, jump to head |
| `isCubeDone` | iReg Reshape stall, clear iRegs, bankRow jump |
| `isDone` | Transfer complete |

## Data Structure

### C Structure (from tensor_descriptor.h)

```c
typedef struct {
    int baseAddr;      // Base address for DMA transfer
    int byteNum;       // Bytes per unit (1-64, typically)
    int unitNum;       // Number of units (0-511)
    int sliceNum;      // Number of slices (0-256)
    int planeNum;      // Number of planes (0-2047)
    int cubeNum;       // Number of cubes (0-7)
    int unitSkip;      // Offset between units (power-of-2 aligned)
    int sliceSkip;     // Offset between slices
    int planeSkip;     // Offset between planes
    int cubeSkip;      // Offset between cubes
} microarch_tensor_descriptor_t;
```

### Field Ranges (Hardware Limits)

| Field | Hardware Range | Default Max | Notes |
|-------|----------------|-------------|-------|
| `byteNum` | 1-64 | 64 | Must be power-of-2 for alignment |
| `unitNum` | 0-511 | 1024 | DMA loop counter |
| `sliceNum` | 0-256 | 1024 | DMA loop counter |
| `planeNum` | 0-2047 | 1024 | DMA loop counter |
| `cubeNum` | 0-7 | 1024 | DMA loop counter |
| `unitSkip` | 0-4MB | - | Must be power-of-2 |
| `sliceSkip` | 0-4MB | - | Calculated: unitNum × unitSkip |
| `planeSkip` | 0-4MB | - | Calculated: sliceNum × sliceSkip |
| `cubeSkip` | 0-4MB | - | Calculated: planeNum × planeSkip |

## Address Calculation

### Physical Address Formula

The DMA hardware computes the physical address using 4-level nested loops:

```
addr = base_addr 
     + (u_cnt × u_skip) 
     + (s_cnt × s_skip) 
     + (p_cnt × p_skip) 
     + (c_cnt × c_skip)
```

Where:
- `u_cnt`: Unit counter (0 to unitNum-1)
- `s_cnt`: Slice counter (0 to sliceNum-1)
- `p_cnt`: Plane counter (0 to planeNum-1)
- `c_cnt`: Cube counter (0 to cubeNum-1)

### Skip Calculation Algorithm

From `tensor_descriptor.c`:

```c
// Next power of 2 calculation (no math.h dependency)
static unsigned int next_power_of_2(unsigned int x) {
    if (x == 0) return 1;
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

// Skip calculations
unitSkip  = next_power_of_2(byteNum);
sliceSkip = unitNum * unitSkip;
planeSkip = sliceNum * sliceSkip;
cubeSkip  = planeNum * planeSkip;
```

### Why Power-of-2 Alignment?

From hardware specification (`tensor_engine_v1.5.html`):

> "unitSkip: only support normal skip" with range 0-4MB

The power-of-2 alignment requirement comes from:
1. **Memory bus width**: DMA transfers are optimized for aligned accesses
2. **Bank interleaving**: Hardware memory banks use address bits for interleaving
3. **Cache line alignment**: Ensures efficient cache utilization
4. **DMA engine design**: Hardware address generators use shift operations for speed

## Constraint System

### User Constraints (Requirements)

```c
typedef struct {
    unsigned int maxByteNum;
    unsigned int maxUnitNum;
    unsigned int maxSliceNum;
    unsigned int maxPlaneNum;
    unsigned int maxCubeNum;
    unsigned int maxTotalBytes;
    unsigned int enableBalance;  // NEW: balance micro/arch dimension ratio
} tensor_constraints_t;
```

### Physical Limits (Hardware)

```c
typedef struct {
    unsigned int maxPhysicalByteNum;   // Default: 64
    unsigned int maxPhysicalUnitNum;   // Default: 1024
    unsigned int maxPhysicalSliceNum;  // Default: 1024
    unsigned int maxPhysicalPlaneNum; // Default: 1024
    unsigned int maxPhysicalCubeNum;   // Default: 1024
} tensor_physical_limits_t;
```

### Constraint Resolution Algorithm

From `tensor_descriptor.c`:

```c
static void compute_effective_limits_internal(
    const microarch_physical_limits_t* limits,
    const microarch_constraints_t* constraints,
    unsigned int* effMaxByte,
    unsigned int* effMaxUnit,
    unsigned int* effMaxSlice,
    unsigned int* effMaxPlane,
    unsigned int* effMaxCube
) {
    // 1. Start with hardcoded maximums
    *effMaxByte = MAX_BYTE_NUM;      // 64
    *effMaxUnit = DEFAULT_MAX_UNIT;  // 1024
    *effMaxSlice = DEFAULT_MAX_SLICE; // 1024
    *effMaxPlane = DEFAULT_MAX_PLANE; // 1024
    *effMaxCube = DEFAULT_MAX_CUBE;   // 1024
    
    // 2. Apply physical limits (if provided)
    if (limits) {
        *effMaxByte = limits->maxPhysicalByteNum > 0 ? 
            limits->maxPhysicalByteNum : *effMaxByte;
        // ... similar for other dimensions
    }
    
    // 3. Apply user constraints (if provided) - OVERRIDES physical limits
    if (constraints) {
        if (constraints->maxByteNum > 0 && constraints->maxByteNum < *effMaxByte) 
            *effMaxByte = constraints->maxByteNum;
        // ... similar for other dimensions
    }
}
```

Key insight: **Constraints override physical limits** - if user requests smaller limits than hardware maximum, user constraints win.

### Dimension Overflow Logic

When input dimensions exceed effective limits, automatic overflow occurs:

```
Overflow Direction (lowest to highest):
byteNum → unitNum → sliceNum → planeNum → cubeNum

Algorithm:
1. If byteNum > maxByte:
   - spillUnits = ceil((byteNum - maxByte) / maxByte)
   - unitNum *= (1 + spillUnits)
   - byteNum = maxByte

2. Redistribute across dimensions while respecting limits:
   - Minimize dimension count (use lower dimensions first)
   - Respect effective limits at each level
   - Ensure total element count is preserved
```

## Usage Examples

### Example 1: Basic Tensor Configuration

```c
// Configure a tensor with:
// - 64 bytes per unit
// - 100 units, 10 slices, 5 planes, 2 cubes
// - Let hardware calculate skips automatically

tensor_descriptor_t arch_desc = {
    .baseAddr = 0x10000,
    .dimension = {64, 100, 10, 5, 2},  // [byte, unit, slice, plane, cube]
    .stride = {64, 6400, 640000, 3200000, 16000000}
};

tensor_constraints_t constraints = {
    .maxByteNum = 64,
    .maxUnitNum = 512,
    .enableBalance = 1  // Enable dimension balancing
};

tensor_physical_limits_t limits = {
    .maxPhysicalByteNum = 64,
    .maxPhysicalUnitNum = 1024
};

tensor_conversion_result_t result;
int ret = tensor_descriptor_convert(&arch_desc, &constraints, &limits, &result);

// Result should show:
// - byteNum = 64
// - unitNum = 100
// - unitSkip = 64 (next_power_of_2(64))
// - sliceSkip = 6400 (100 * 64)
```

### Example 2: Dimension Overflow Handling

```c
// Configure a tensor with byteNum > maxByte
// This will trigger dimension overflow

tensor_descriptor_t arch_desc = {
    .baseAddr = 0x20000,
    .dimension = {128, 50, 1, 1, 1},  // byteNum=128 > maxByte=64
    .stride = {128, 6400, 6400, 6400, 6400}
};

tensor_constraints_t constraints = {
    .maxByteNum = 64  // Limit to 64 bytes per unit
};

tensor_conversion_result_t result;
int ret = tensor_descriptor_convert(&arch_desc, &constraints, NULL, &result);

// Result should show overflow handling:
// - byteNum = 64 (limited to max)
// - unitNum = 100 (50 * 2, overflow increased unit count)
// - Error code: E_SUCCESS (overflow handled automatically)
```

## Related Documentation

- `tensor_engine_v1.5.html` - Hardware DMA unit specification
- `tensor_streaming_accelerator_architecture.html` - Data structure description
- `assets/model_spec.md` - Functional logic and addressing model
- `src/tensor_descriptor.h` - C structure definitions
- `src/tensor_descriptor.c` - Implementation and algorithms

## References

1. DMA Unit Specification v1.5, Section 4.8
2. Tensor Streaming Accelerator Architecture, Section 3.6.1
3. Microarchitecture Model Specification
4. C Implementation: `src/tensor_descriptor.c`
