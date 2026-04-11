# Microarchitecture Tensor Architecture

## Table of Contents

1. [Overview](#1-overview)
2. [Architecture Design](#2-architecture-design)
3. [Data Structure Interface](#3-data-structure-interface)
4. [Address Calculation Model](#4-address-calculation-model)
5. [Constraint System](#5-constraint-system)
6. [Usage Patterns](#6-usage-patterns)
7. [Appendix](#7-appendix)

---

## 1. Overview

### 1.1 Design Philosophy

The microarchitecture tensor system bridges the gap between high-level tensor abstractions and hardware DMA capabilities through a layered architecture:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Layered Architecture                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │  Layer 3: Application                                     │   │
│  │  - High-level tensor operations                           │   │
│  │  - Framework integrations (PyTorch, TensorFlow)           │   │
│  └───────────────────────────────────────────────────────────┘   │
│                           │                                       │
│                           ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │  Layer 2: Architecture Tensor (arch_tensor)               │   │
│  │  - 5D tensor layout: [byte, unit, slice, plane, cube]     │   │
│  │  - Stride-based addressing                                │   │
│  │  - Sub-tensor support                                     │   │
│  └───────────────────────────────────────────────────────────┘   │
│                           │                                       │
│                           ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │  Layer 1: Microarchitecture Tensor (microarch_tensor)   │   │
│  │  - DMA register mapping                                   │   │
│  │  - Skip-based addressing                                  │   │
│  │  - 4-level nested loop model                              │   │
│  └───────────────────────────────────────────────────────────┘   │
│                           │                                       │
│                           ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐   │
│  │  Layer 0: Hardware DMA                                    │   │
│  │  - Physical memory transfers                              │   │
│  │  - Hardware loop counters                                   │   │
│  │  - Address generation unit                                  │   │
│  └───────────────────────────────────────────────────────────┘   │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 Core Abstractions

The system is built around three fundamental abstractions:

**1. Dimension Hierarchy (5D Tensor)**

| Level | Name | Role | Hardware Mapping |
|-------|------|------|------------------|
| 0 | Byte | Smallest addressable unit | Transfer size |
| 1 | Unit | Collection of bytes | Inner loop counter |
| 2 | Slice | Collection of units | Loop counter |
| 3 | Plane | Collection of slices | Loop counter |
| 4 | Cube | Collection of planes | Outermost loop |

**2. Addressing Model (Skip-Based)**

Traditional stride-based addressing:
```
addr = base + Σ(index[i] × stride[i])
```

Microarchitecture skip-based addressing:
```
addr = base 
     + (u_cnt × unitSkip) 
     + (s_cnt × sliceSkip) 
     + (p_cnt × planeSkip) 
     + (c_cnt × cubeSkip)
```

**3. Constraint Model (Layered Limits)**

```
┌──────────────────────────────────────────────────────────────┐
│  Constraint Hierarchy                                        │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  Level 3: User Constraints (soft limits)                    │
│  ── Applied after physical limits                           │
│  ── Can be exceeded if necessary                            │
│  ── Expresses preferences, not hard requirements              │
│                                                              │
│  Level 2: Physical Limits (hard limits)                     │
│  ── Applied before user constraints                         │
│  ── Cannot be exceeded under any circumstances              │
│  ── Defined by hardware capabilities                        │
│                                                              │
│  Level 1: Hardcoded Defaults                                │
│  ── Baseline values (MAX_BYTE_NUM = 64, etc.)             │
│  ── Used when no other limits specified                   │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

---

## 2. Architecture Design

### 2.1 Component Overview

The microarchitecture tensor system consists of four main components:

```
┌─────────────────────────────────────────────────────────────────┐
│              Microarchitecture Tensor System                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌─────────────────────┐    ┌─────────────────────┐              │
│  │   Conversion        │    │   Constraint        │              │
│  │   Engine            │◄──►│   Resolver          │              │
│  │                     │    │                     │              │
│  │  - arch → microarch│    │  - Physical limits  │              │
│  │  - Skip calculation│    │  - User constraints │              │
│  │  - Overflow logic  │    │  - Conflict resolve │              │
│  └─────────────────────┘    └─────────────────────┘              │
│           │                            │                        │
│           ▼                            ▼                        │
│  ┌─────────────────────┐    ┌─────────────────────┐              │
│  │   Address           │    │   Dimension         │              │
│  │   Generator         │    │   Distributor       │              │
│  │                     │    │                     │              │
│  │  - Skip arithmetic  │    │  - 5D → 4D mapping  │              │
│  │  - Power-of-2 align │    │  - Overflow handling│              │
│  │  - Base + offset    │    │  - Balance logic    │              │
│  └─────────────────────┘    └─────────────────────┘              │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 Data Flow Architecture

The data flow through the system follows a pipeline pattern:

```
┌──────────────────────────────────────────────────────────────────────────┐
│                         Data Flow Pipeline                                  │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  Input                    Processing                    Output              │
│                                                                            │
│  ┌─────────────┐        ┌─────────────┐            ┌─────────────┐       │
│  │  arch_tensor│───────►│  Constraint │───────────►│ microarch   │       │
│  │             │        │  Resolution │            │ _tensor     │       │
│  │ 5D layout  │        │             │            │             │       │
│  └─────────────┘        └─────────────┘            └─────────────┘       │
│                              │                                             │
│                              ▼                                             │
│                       ┌─────────────┐                                      │
│                       │   Effective   │                                      │
│                       │    Limits     │                                      │
│                       │  (min of all) │                                      │
│                       └─────────────┘                                      │
│                              │                                             │
│                              ▼                                             │
│                       ┌─────────────┐                                      │
│                       │  Dimension  │                                      │
│                       │ Distribution│                                      │
│                       │  (5D → 4D)  │                                      │
│                       └─────────────┘                                      │
│                              │                                             │
│                              ▼                                             │
│                       ┌─────────────┐                                      │
│                       │    Skip     │                                      │
│                       │ Calculation │                                      │
│                       │ (power-of-2)│                                      │
│                       └─────────────┘                                      │
│                                                                            │
└──────────────────────────────────────────────────────────────────────────┘
```

### 2.3 Interface Boundaries

The system exposes three well-defined interfaces:

**Interface 1: Architecture Tensor (Input)**

```c
// 5D tensor descriptor
typedef struct {
    unsigned int baseAddr;
    unsigned int dimension[5];  // [byte, unit, slice, plane, cube]
    unsigned int stride[5];
} tensor_descriptor_t;
```

**Interface 2: Constraints (Configuration)**

```c
// User constraints (soft limits)
typedef struct {
    unsigned int maxByteNum, maxUnitNum, maxSliceNum;
    unsigned int maxPlaneNum, maxCubeNum, maxTotalBytes;
    unsigned int enableBalance;
} tensor_constraints_t;

// Physical limits (hard limits)
typedef struct {
    unsigned int maxPhysicalByteNum, maxPhysicalUnitNum;
    unsigned int maxPhysicalSliceNum, maxPhysicalPlaneNum;
    unsigned int maxPhysicalCubeNum;
} tensor_physical_limits_t;
```

**Interface 3: Microarchitecture Tensor (Output)**

```c
// 4-level DMA-ready descriptor
typedef struct {
    int baseAddr;
    int byteNum, unitNum, sliceNum, planeNum, cubeNum;
    int unitSkip, sliceSkip, planeSkip, cubeSkip;
} tensor_conversion_descriptor_t;
```

---

## 3. Data Structure Interface

### 3.1 Architecture Tensor Descriptor

The architecture tensor represents the high-level 5D tensor layout:

```
┌─────────────────────────────────────────────────────────────────┐
│           Architecture Tensor (5D Layout)                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  Dimension Mapping:                                               │
│                                                                   │
│  Index   Name    Description              Typical Range         │
│  ─────────────────────────────────────────────────────────────  │
│  [0]     Byte    Smallest unit              1-64 bytes          │
│  [1]     Unit    Group of bytes             0-511                │
│  [2]     Slice   Group of units             0-256                 │
│  [3]     Plane   Group of slices            0-2047                │
│  [4]     Cube    Group of planes          0-7                   │
│                                                                   │
│  Memory Layout:                                                   │
│                                                                   │
│  ┌─────────────────────────────────────────────────────────┐     │
│  │  Cube 0                                                 │     │
│  │  ┌─────────────────────────────────────────────────┐   │     │
│  │  │  Plane 0                                        │   │     │
│  │  │  ┌─────────────────────────────────────────┐   │   │     │
│  │  │  │  Slice 0                                │   │   │     │
│  │  │  │  ┌─────────────────────────────────┐   │   │   │     │
│  │  │  │  │  Unit 0   Unit 1   ...  Unit N  │   │   │   │     │
│  │  │  │  │  [bytes]  [bytes]       [bytes] │   │   │   │     │
│  │  │  │  └─────────────────────────────────┘   │   │   │     │
│  │  │  └─────────────────────────────────────────┘   │   │     │
│  │  └─────────────────────────────────────────────────┘   │     │
│  └─────────────────────────────────────────────────────────┘     │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 Microarchitecture Tensor Descriptor

The microarchitecture tensor is the DMA-ready representation:

```
┌─────────────────────────────────────────────────────────────────┐
│         Microarchitecture Tensor (DMA-Ready)                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  Structure:                                                       │
│                                                                   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  Base Address (baseAddr)                                 │   │
│  │  Starting memory address for transfer                    │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                    │
│                              ▼                                    │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  Dimensions (byteNum, unitNum, sliceNum, planeNum, cubeNum)│   │
│  │  Define the shape of the 4-level nested loop              │   │
│  └─────────────────────────────────────────────────────────┘   │
│                              │                                    │
│                              ▼                                    │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  Skips (unitSkip, sliceSkip, planeSkip, cubeSkip)         │   │
│  │  Address offsets between consecutive elements             │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                   │
│  4-Level Nested Loop Model:                                       │
│                                                                   │
│  ┌──────────────────────────────────────────────────────────┐    │
│  │  for cube in 0..cubeNum-1                                │    │
│  │    for plane in 0..planeNum-1                            │    │
│  │      for slice in 0..sliceNum-1                          │    │
│  │        for unit in 0..unitNum-1                          │    │
│  │          addr = baseAddr                                 │    │
│  │               + unit × unitSkip                          │    │
│  │               + slice × sliceSkip                        │    │
│  │               + plane × planeSkip                        │    │
│  │               + cube × cubeSkip                          │    │
│  │          transfer byteNum bytes at addr                │    │
│  └──────────────────────────────────────────────────────────┘    │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 3.3 Constraint Structures

The constraint system uses two complementary structures:

```
┌─────────────────────────────────────────────────────────────────┐
│              Constraint System Architecture                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌─────────────────────────────┐  ┌─────────────────────────────┐ │
│  │   User Constraints          │  │   Physical Limits           │ │
│  │   (Soft Limits)             │  │   (Hard Limits)             │ │
│  ├─────────────────────────────┤  ├─────────────────────────────┤ │
│  │                             │  │                             │ │
│  │ • maxByteNum                │  │ • maxPhysicalByteNum        │ │
│  │ • maxUnitNum                │  │ • maxPhysicalUnitNum        │ │
│  │ • maxSliceNum               │  │ • maxPhysicalSliceNum       │ │
│  │ • maxPlaneNum               │  │ • maxPhysicalPlaneNum       │ │
│  │ • maxCubeNum                │  │ • maxPhysicalCubeNum        │ │
│  │ • maxTotalBytes             │  │                             │ │
│  │ • enableBalance             │  │                             │ │
│  │                             │  │                             │ │
│  │ Characteristics:            │  │ Characteristics:            │ │
│  │ • Preferences, not rules    │  │ • Hardware boundaries       │ │
│  │ • Can be exceeded         │  │ • Cannot be exceeded        │ │
│  │ • Express intent          │  │ • Define capability         │ │
│  │                             │  │                             │ │
│  └─────────────────────────────┘  └─────────────────────────────┘ │
│              │                             │                      │
│              └─────────────┬───────────────┘                      │
│                            ▼                                       │
│                   ┌─────────────────┐                              │
│                   │  Constraint     │                              │
│                   │  Resolution     │                              │
│                   │                 │                              │
│                   │  effective =    │                              │
│                   │    min(physical,│                              │
│                   │        user)    │                              │
│                   └─────────────────┘                              │
│                            │                                       │
│                            ▼                                       │
│                   ┌─────────────────┐                              │
│                   │  Dimension      │                              │
│                   │  Distribution   │                              │
│                   └─────────────────┘                              │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## 4. Address Calculation Model

### 4.1 Physical Address Formula

The core address calculation follows a linear combination model:

```
address = baseAddr + Σ(level_counter × level_skip)

Expanded:
address = baseAddr
        + (unit_cnt × unitSkip)
        + (slice_cnt × sliceSkip)
        + (plane_cnt × planeSkip)
        + (cube_cnt × cubeSkip)
```

**Variables:**

| Symbol | Description | Range |
|--------|-------------|-------|
| `baseAddr` | Starting memory address | 0 to 4GB |
| `unit_cnt` | Unit loop counter | 0 to unitNum-1 |
| `slice_cnt` | Slice loop counter | 0 to sliceNum-1 |
| `plane_cnt` | Plane loop counter | 0 to planeNum-1 |
| `cube_cnt` | Cube loop counter | 0 to cubeNum-1 |
| `unitSkip` | Address offset between units | Power of 2 |
| `sliceSkip` | Address offset between slices | Calculated |
| `planeSkip` | Address offset between planes | Calculated |
| `cubeSkip` | Address offset between cubes | Calculated |

### 4.2 Skip Calculation Model

The skip values form a multiplicative hierarchy:

```
┌─────────────────────────────────────────────────────────────────┐
│                    Skip Hierarchy                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│   Level        Formula                    Dependency Chain      │
│  ───────────────────────────────────────────────────────────  │
│                                                                   │
│   unitSkip  = next_power_of_2(byteNum)                          │
│                      │                                            │
│                      ▼                                            │
│   sliceSkip = unitNum × unitSkip                                │
│                      │                                            │
│                      ▼                                            │
│   planeSkip = sliceNum × sliceSkip                              │
│                      │                                            │
│                      ▼                                            │
│   cubeSkip  = planeNum × planeSkip                              │
│                                                                   │
│   Total Size = byteNum × unitNum × sliceNum × planeNum × cubeNum│
│              = cubeSkip × cubeNum (when cubeSkip complete)      │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 4.3 Power-of-2 Alignment Principle

The hardware DMA requires power-of-2 alignment for performance:

```
┌─────────────────────────────────────────────────────────────────┐
│              Power-of-2 Alignment Requirement                     │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  Why Power-of-2?                                                  │
│  ───────────────                                                  │
│                                                                   │
│  1. Memory Bus Alignment                                          │
│     • Wide buses (64/128-bit) require aligned accesses            │
│     • Unaligned = multiple bus cycles                             │
│                                                                   │
│  2. Bank Interleaving                                             │
│     • DRAM uses address bits for bank selection                   │
│     • Power-of-2 ensures uniform bank distribution              │
│                                                                   │
│  3. Cache Line Alignment                                          │
│     • Cache lines are power-of-2 sized                          │
│     • Alignment prevents cache line splits                        │
│                                                                   │
│  4. Hardware Simplification                                       │
│     • Address calc: shift instead of multiply                     │
│     • addr = base + (index << log2(skip))                         │
│                                                                   │
│  Algorithm: next_power_of_2(x)                                    │
│  ───────────────────────────                                    │
│                                                                   │
│  Input:  x = byteNum (e.g., 50)                                  │
│  Output: smallest power of 2 >= x                               │
│                                                                   │
│  Process:                                                         │
│    if x == 0: return 1                                            │
│    x = x - 1                                                      │
│    x = x | (x >> 1)   // Propagate MSB to bit 1                 │
│    x = x | (x >> 2)   // Propagate to bits 2-3                  │
│    x = x | (x >> 4)   // Propagate to bits 4-7                  │
│    x = x | (x >> 8)   // Propagate to bits 8-15                 │
│    x = x | (x >> 16)  // Propagate to bits 16-31                │
│    return x + 1                                                   │
│                                                                   │
│  Examples:                                                      │
│    next_power_of_2(50)  = 64                                     │
│    next_power_of_2(64)  = 64                                     │
│    next_power_of_2(100) = 128                                    │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## 5. Constraint System

### 5.1 Constraint Hierarchy Model

The constraint system resolves limits through a layered precedence model:

```
┌─────────────────────────────────────────────────────────────────┐
│              Constraint Resolution Hierarchy                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  Layer 1: Hardcoded Defaults                                    │
│  ─────────────────────────                                      │
│  • MAX_BYTE_NUM  = 64                                           │
│  • DEFAULT_MAX_UNIT   = 1024                                      │
│  • DEFAULT_MAX_SLICE  = 1024                                      │
│  • DEFAULT_MAX_PLANE  = 1024                                      │
│  • DEFAULT_MAX_CUBE   = 1024                                      │
│                                                                   │
│         │                                                         │
│         ▼                                                         │
│  Layer 2: Physical Limits (hard constraints)                    │
│  ───────────────────────────────────────────                    │
│  • maxPhysicalByteNum  ≤ 64                                      │
│  • maxPhysicalUnitNum  ≤ 511                                     │
│  • maxPhysicalSliceNum ≤ 256                                     │
│  • maxPhysicalPlaneNum ≤ 2047                                    │
│  • maxPhysicalCubeNum  ≤ 7                                       │
│                                                                   │
│  effective_limit = min(default, physical_limit)                   │
│                                                                   │
│         │                                                         │
│         ▼                                                         │
│  Layer 3: User Constraints (soft constraints)                   │
│  ───────────────────────────────────────────                    │
│  • maxByteNum, maxUnitNum, etc.                                  │
│  • Expresses preferences                                        │
│  • Can be exceeded if necessary                                 │
│                                                                   │
│  effective_limit = min(effective_from_layer2, user_constraint)  │
│                                                                   │
│         │                                                         │
│         ▼                                                         │
│  Final: Effective Limits                                        │
│  ─────────────────────                                          │
│  Used for dimension distribution and overflow handling          │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 5.2 Dimension Distribution Model

The system maps 5D architecture tensors to 4D microarchitecture tensors:

```
┌─────────────────────────────────────────────────────────────────┐
│              Dimension Distribution Model                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  Architecture Tensor (5D)        Microarchitecture Tensor (4D)   │
│  ────────────────────────        ─────────────────────────────  │
│                                                                   │
│  dimension[0]: byteNum      ──►  byteNum (bytes per unit)        │
│  dimension[1]: unitNum      ──►  unitNum (units per slice)        │
│  dimension[2]: sliceNum     ──►  sliceNum (slices per plane)     │
│  dimension[3]: planeNum     ──►  planeNum (planes per cube)        │
│  dimension[4]: cubeNum      ──►  cubeNum (number of cubes)         │
│                                                                   │
│  Overflow Model:                                                  │
│  ───────────────                                                  │
│                                                                   │
│  When a dimension exceeds its effective limit, overflow propagates│
│  to the next higher dimension:                                    │
│                                                                   │
│      byteNum ──► unitNum ──► sliceNum ──► planeNum ──► cubeNum   │
│       (L0)        (L1)        (L2)         (L3)         (L4)       │
│                                                                   │
│  Overflow Formula:                                                │
│  ─────────────────                                                │
│                                                                   │
│  Given: input_dim, effective_max                                  │
│                                                                   │
│  If input_dim > effective_max:                                    │
│      spill = ceil((input_dim - effective_max) / effective_max)   │
│      next_dim = next_dim × (1 + spill)                           │
│      current_dim = effective_max                                 │
│                                                                   │
│  Example:                                                         │
│  ─────────                                                        │
│  Input: byteNum = 128, constraint: maxByteNum = 64                 │
│                                                                   │
│  Process:                                                         │
│  1. byteNum (128) > maxByte (64) ──► overflow triggered         │
│  2. spill = ceil((128 - 64) / 64) = 1                            │
│  3. unitNum = unitNum × (1 + 1) = unitNum × 2                   │
│  4. byteNum = 64 (clamped to max)                               │
│                                                                   │
│  Result: Total elements preserved through overflow distribution  │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

### 5.3 API Interface

The public API provides a single conversion function:

```c
/* ============================================================================
 * Public API
 * ============================================================================ */

/**
 * Convert architecture tensor to microarchitecture tensor
 *
 * @param desc        Input architecture tensor descriptor
 * @param constraints Optional user constraints (NULL for defaults)
 * @param limits      Optional physical limits (NULL for defaults)
 * @param result      Output conversion result
 *
 * @return E_SUCCESS (0) on success, error code otherwise:
 *         E_BYTE_NUM_EXCEEDED    (-1) Byte count exceeds limit
 *         E_OVER_CONSTRAINED     (-2) Constraints too restrictive
 *         E_INVALID_DIMENSION    (-3) Invalid dimension values
 *         E_PHYSICAL_CONSTRAINT  (-4) Physical limit violation
 */
int tensor_descriptor_convert(
    const tensor_descriptor_t* desc,
    const tensor_constraints_t* constraints,
    const tensor_physical_limits_t* limits,
    tensor_conversion_result_t* result
);
```

---

## 6. Usage Patterns

### 6.1 Pattern 1: Basic Tensor Configuration

For most use cases, provide the 5D dimensions and let the system calculate skips:

```
Input:  dimension = [64, 100, 10, 1, 1]  // 64B × 100 units × 10 slices

Process:
1. Validate dimensions against constraints
2. Calculate effective limits (min of defaults, physical, user)
3. Distribute dimensions (handle overflow if needed)
4. Calculate skips:
   - unitSkip  = next_power_of_2(64) = 64
   - sliceSkip = 100 × 64 = 6400
   - planeSkip = 10 × 6400 = 64000
   - cubeSkip  = 1 × 64000 = 64000

Output: Fully configured microarch_tensor ready for DMA
```

### 6.2 Pattern 2: Resource-Constrained Configuration

When hardware resources are limited, use constraints to limit dimensions:

```
Scenario: Embedded system with limited DMA buffer

Input:
  dimension = [64, 1000, 100, 1, 1]  // Large tensor
  constraints.maxUnitNum = 256        // Limit units per slice

Process:
1. effectiveMaxUnit = min(default(1024), constraint(256)) = 256
2. unitNum (1000) > effectiveMaxUnit (256) → overflow triggered
3. spill = ceil((1000 - 256) / 256) = 3
4. sliceNum = 100 × (1 + 3) = 400
5. unitNum = 256 (clamped)

Result: Tensor fits within DMA constraints through overflow
```

### 6.3 Pattern 3: Hardware-Specific Optimization

For maximum performance, align with hardware characteristics:

```
Scenario: Optimize for cache line size (64 bytes)

Input:
  byteNum = 50  // Natural data size

Process:
1. unitSkip = next_power_of_2(50) = 64
2. 14 bytes padding per unit (64 - 50)

Optimization Trade-off:
  • Cache-aligned: Better performance, memory waste
  • Non-aligned: Lower performance, memory efficient

Recommendation: Use enableBalance flag for automatic optimization
```

---

## 7. Appendix

### 7.1 Glossary

| Term | Definition |
|------|------------|
| **Architecture Tensor** | High-level 5D tensor representation used by software |
| **Microarchitecture Tensor** | Low-level 4D representation used by DMA hardware |
| **Dimension** | Size of a tensor along one axis (byteNum, unitNum, etc.) |
| **Skip** | Memory offset between consecutive elements at a level |
| **Overflow** | Process of redistributing excess elements to higher dimensions |
| **Power-of-2** | Alignment requirement for certain hardware operations |
| **Constraint** | Limit on dimension sizes (user preference or hardware limit) |

### 7.2 Formula Reference

| Formula | Description |
|---------|-------------|
| `unitSkip = next_power_of_2(byteNum)` | Calculate unit-level skip |
| `sliceSkip = unitNum × unitSkip` | Calculate slice-level skip |
| `planeSkip = sliceNum × sliceSkip` | Calculate plane-level skip |
| `cubeSkip = planeNum × planeSkip` | Calculate cube-level skip |
| `addr = base + Σ(cnt_i × skip_i)` | General address formula |
| `spill = ceil((dim - max) / max)` | Overflow spill calculation |
| `effective = min(physical, user)` | Constraint resolution |

### 7.3 Hardware Limit Reference

| Field | Hardware Maximum | Notes |
|-------|------------------|-------|
| byteNum | 64 | Register width limit |
| unitNum | 511 | 9-bit counter |
| sliceNum | 256 | 9-bit counter |
| planeNum | 2047 | 11-bit counter |
| cubeNum | 7 | 3-bit counter |
| Skip values | 4MB | 22-bit address field |

---

*Microarchitecture Tensor Architecture v1.0*
