## 1. Update Header File

- [x] 1.1 Add `ggml_block_combine_config_t` structure to `src/tensor_descriptor.h`
  - Fields: `target_byteNum`, `max_byteNum`
  - Document field meanings

- [x] 1.2 Update `ggml_to_microarch_direct_map()` declaration
  - Add `combine_config` parameter (nullable)
  - Update function documentation

## 2. Update Implementation

- [x] 2.1 Implement block combine logic in `src/tensor_descriptor.c`
  - Calculate `target_byteNum` (auto or user-specified)
  - Calculate `blocks_per_unit`
  - Calculate `unitNum` with ceil division
  - Set `unitSkip = blocks_per_unit * nb[0]`
  - Handle NULL config (backward compatible)

- [x] 2.2 Add boundary checks
  - Ensure `target_byteNum >= type_size`
  - Ensure `target_byteNum <= max_byteNum`

- [x] 2.3 Test compilation
  - Build successful with no warnings

## 3. Add Full Test Coverage

- [x] 3.1 Create test using `tests/ggml_tensor_test_data.inc`
  - Testing 20 tensors (with limit)
  - All tests pass with block combine

- [x] 3.2 Add type-specific tests
  - Q4_0: target_byteNum=54 ✓
  - F32: target_byteNum=64 ✓
  - Q8_0: target_byteNum=34 ✓

- [x] 3.3 Add access count reduction verification
  - Q4_0: 66.4% reduction ✓
  - Q4_0 (forced): 50% reduction ✓

- [x] 3.4 Add backward compatibility test
  - NULL config works correctly ✓

## 4. Build and Verify

- [x] 4.1 Clean build
  - All targets built successfully

- [x] 4.2 Run GGML tensor traversal test
  - 20/20 passed

- [x] 4.3 Run constraint tests
  - 24/24 passed

- [x] 4.4 Run batch tests
  - 128/128 passed

## 5. Documentation

- [x] 5.1 Update `TODO.md`
  - Marked block combine task complete
  - Documented performance improvements

- [x] 5.2 Add usage example in code comments
  - Added test examples showing auto-selection and forced modes

- [x] 5.3 Update docs/ to sync with src/ implementation
  - Updated `docs/microarch_tensor.md`: Removed power-of-2 references, added direct mapping and block combine
  - Updated `docs/mapping.md`: Synced with block combine implementation
  - Removed all `hasGap` and `next_power_of_2` references from documentation
  - Added block combine performance data and examples

## Implementation Order

1. Update header (Task 1)
2. Implement logic (Task 2)
3. Add tests (Task 3)
4. Build and verify (Task 4)
5. Documentation (Task 5)

## Expected Results

- **Test Coverage**: 102 tensors from test data + type-specific tests
- **Performance**: Q4_0 66% reduction, Q8_0 50% reduction
- **Backward Compatibility**: All existing tests pass
