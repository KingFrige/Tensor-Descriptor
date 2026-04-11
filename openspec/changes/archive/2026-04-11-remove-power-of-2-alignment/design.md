# Design: Remove Power-of-2 Alignment

## Goals

1. Simplify `microarch_constraints_convert()` by removing power-of-2 alignment logic
2. Remove `enablePowerOf2Skip` flag and `hasGap` field complexity
3. Ensure DMA hardware compatibility with non-power-of-2 skip values
4. Maintain backward compatibility where possible

## Architecture Changes

### Current Architecture (Before)

```
┌─────────────────────────────────────────────────────────────┐
│ tensor_descriptor_convert()                                  │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────┐  │
│  │ Route by enablePowerOf2Skip                           │  │
│  ├──────────────────┬───────────────────────────────────┤  │
│  │                  │                                   │  │
│  ▼                  ▼                                   │  │
│  ┌─────────────────┐  ┌──────────────────────────────┐  │  │
│  │ Direct Mode     │  │ Power-of-2 Mode              │  │  │
│  │ (enable=1)      │  │ (enable=0/default)           │  │  │
│  ├─────────────────┤  ├──────────────────────────────┤  │  │
│  │ unitSkip=byteNum│  │ unitSkip=next_power_of_2()   │  │  │
│  │ hasGap=0        │  │ hasGap calculated            │  │  │
│  └─────────────────┘  └──────────────────────────────┘  │  │
└─────────────────────────────────────────────────────────────┘
```

### New Architecture (After)

```
┌─────────────────────────────────────────────────────────────┐
│ microarch_dimension_distribute()                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ Dimension Distribution (Single Mode)                  │  │
│  ├──────────────────────────────────────────────────────┤  │
│  │ Phase 1: Initialize and normalize                     │  │
│  │ Phase 2: Expand for physical limits                   │  │
│  │ Phase 3: Balance dimensions (optional)                │  │
│  │ Phase 4: Validate constraints                         │  │
│  │ Phase 5: Calculate skips (always direct)              │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  Output: unitSkip = byteNum (always)                        │
│          hasGap = 0 (always, field to be removed)           │
└─────────────────────────────────────────────────────────────┘
```

## Detailed Design

### 1. Remove `enablePowerOf2Skip` Field

From both structures:
```c
// Remove from microarch_constraints_t
typedef struct {
    unsigned int maxByteNum;
    // ... other fields
    // REMOVE: unsigned int enablePowerOf2Skip;
} microarch_constraints_t;

// Remove from tensor_constraints_t  
typedef struct {
    unsigned int maxByteNum;
    // ... other fields
    // REMOVE: unsigned int enablePowerOf2Skip;
} tensor_constraints_t;
```

### 2. Simplify Skip Calculation

Replace conditional logic:
```c
// BEFORE
if (constraints && constraints->enablePowerOf2Skip == 1) {
    result->desc.unitSkip = result->desc.byteNum;
    result->hasGap = 0;
} else {
    result->desc.unitSkip = (int)next_power_of_2(result->desc.byteNum);
    unsigned int actualStride = next_power_of_2(dim[0]);
    if (dim[0] == 0) {
        result->hasGap = 0;
    } else if (dim[0] == 1) {
        result->hasGap = 1;
    } else {
        result->hasGap = (actualStride > dim[0]) ? 1 : 0;
    }
}

// AFTER
result->desc.unitSkip = result->desc.byteNum;
result->hasGap = 0;  // Always no gap
```

### 3. Remove or Deprecate `hasGap` Field

Option A: Remove completely
```c
// Remove from microarch_conversion_result_t
typedef struct {
    microarch_tensor_descriptor_t desc;
    // REMOVE: int hasGap;
    int errorCode;
    // ...
} microarch_conversion_result_t;
```

Option B: Keep but always set to 0 (for compatibility)
```c
result->hasGap = 0;  // Always 0, kept for API compatibility
```

### 4. Rename Function (Optional)

Better name reflecting actual purpose:
```c
// From
int microarch_constraints_convert(...);

// To
int microarch_dimension_distribute(...);
```

## Skip Calculation Formula

After simplification, all skips are calculated as:

```c
result->desc.unitSkip = result->desc.byteNum;
result->desc.sliceSkip = result->desc.unitNum * result->desc.unitSkip;
result->desc.planeSkip = result->desc.sliceNum * result->desc.sliceSkip;
result->desc.cubeSkip = result->desc.planeNum * result->desc.planeSkip;
```

No special cases, no conditional logic.

## Memory Impact

### Before (Power-of-2)
| Type | byteNum | unitSkip (aligned) | Overhead |
|------|---------|-------------------|----------|
| Q4_0 | 18 | 32 | 78% |
| Q8_0 | 34 | 64 | 88% |
| F32  | 4  | 4  | 0% |

### After (Direct)
| Type | byteNum | unitSkip | Overhead |
|------|---------|----------|----------|
| Q4_0 | 18 | 18 | 0% |
| Q8_0 | 34 | 34 | 0% |
| F32  | 4  | 4  | 0% |

## Test Updates Required

### Test Cases to Update

1. **constraint_test.c**
   - `test_has_gap_detection()`: Remove or update expectations
   - `test_direct_mapping_mode()`: Remove (no longer needed)
   - `test_power_of_2_mode_default()`: Update to expect direct mode

2. **batch_constraint_test.c**
   - Update expected hasGap values to 0

3. **ggml_tensor_traversal_test.c**
   - Update mapping verification to expect direct skip values

### Expected Test Changes

```c
// BEFORE: Test expected power-of-2
ASSERT(result.desc.unitSkip == 32, "unitSkip should be 32");
ASSERT(result.hasGap == 1, "hasGap should be 1");

// AFTER: Test expects direct
ASSERT(result.desc.unitSkip == 18, "unitSkip should be 18");
// hasGap check removed or always 0
```

## Implementation Order

1. Update header files (remove fields)
2. Simplify skip calculation in tensor_descriptor.c
3. Update all test files
4. Verify all tests pass
5. Update documentation

## Risks and Mitigations

| Risk | Mitigation |
|------|-----------|
| Hardware doesn't support non-power-of-2 | Verify DMA capabilities before deployment |
| Breaking existing code | Document breaking changes, provide migration guide |
| Test failures | Update test expectations systematically |

## Verification Checklist

- [ ] All constraint tests pass
- [ ] All batch tests pass  
- [ ] All GGML traversal tests pass
- [ ] Memory usage verified reduced
- [ ] No compiler warnings
- [ ] Code coverage maintained
