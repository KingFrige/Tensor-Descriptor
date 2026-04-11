# Proposal: Remove Power-of-2 Alignment

## Why This Change?

Currently, `microarch_constraints_convert()` has dual-mode behavior controlled by `enablePowerOf2Skip` flag:
- Power-of-2 mode: `unitSkip = next_power_of_2(byteNum)`
- Direct mode: `unitSkip = byteNum`

This creates unnecessary complexity:
1. **Single flag controls two different concerns**: routing (which function to call) and internal behavior (how to calculate skip)
2. **Code coupling**: Dimension distribution logic is mixed with alignment logic
3. **Memory waste**: Power-of-2 alignment causes ~44% memory overhead for Q4_0, ~47% for Q8_0
4. **Hardware now supports non-power-of-2**: DMA controller can handle arbitrary skip values

## What Changes?

### High-Level Changes

1. **Remove `enablePowerOf2Skip` flag** from constraints structures
2. **Simplify `microarch_constraints_convert()`**: Always use direct byteNum mapping
3. **Remove `hasGap` field** or set it always to 0
4. **Rename function** to better reflect its purpose
5. **Update all test cases** to expect direct mapping behavior

### Before vs After

```c
// BEFORE: Dual-mode complexity
if (constraints && constraints->enablePowerOf2Skip == 1) {
    result->desc.unitSkip = result->desc.byteNum;
    result->hasGap = 0;
} else {
    result->desc.unitSkip = next_power_of_2(result->desc.byteNum);
    result->hasGap = (actualStride > dim[0]) ? 1 : 0;
}

// AFTER: Simplified direct mapping
result->desc.unitSkip = result->desc.byteNum;
result->hasGap = 0;  // Always no gap
```

## Capabilities

- **dimension-distribution**: Clean dimension distribution without alignment concerns
- **memory-optimization**: Remove memory overhead from power-of-2 padding

## Impact

### Positive Impact
- **Simpler code**: Remove conditional branches and dual-mode logic
- **Better memory efficiency**: No wasted space from alignment
- **Clearer API**: One function = one responsibility
- **Easier testing**: Single code path to test

### Breaking Changes
- **Behavior change**: Default behavior changes from power-of-2 to direct
- **API change**: `enablePowerOf2Skip` field removed
- **Test updates**: All tests expecting power-of-2 behavior need update

## Rollback Plan

If issues discovered:
1. Revert to previous commit
2. Power-of-2 alignment can be added back as a separate preprocessing step if needed

## Success Criteria

- [ ] All existing tests pass with updated expectations
- [ ] Memory usage reduced for quantized types
- [ ] Code complexity metrics improved (cyclomatic complexity, LOC)
- [ ] No performance regression
