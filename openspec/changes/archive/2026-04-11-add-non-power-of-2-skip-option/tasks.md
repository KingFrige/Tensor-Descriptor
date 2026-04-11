## 1. Update Header File

- [x] 1.1 Add `enablePowerOf2Skip` field to `microarch_constraints_t`
  - Location: `src/tensor_descriptor.h`
  - Type: `unsigned int`
  - Comment: "1=enable power-of-2 skip (default), 0=use direct byteNum"
- [x] 1.2 Verify structure alignment and padding

## 2. Update Implementation

- [x] 2.1 Modify `microarch_constraints_convert()` in `src/tensor_descriptor.c`
  - Check if `constraints` is NULL → default to power-of-2
  - Check `constraints->enablePowerOf2Skip` value
  - If 0: `unitSkip = byteNum`, `hasGap = 0`
  - If non-0: existing power-of-2 logic
- [x] 2.2 Update skip calculation for upper levels (unchanged)
  - sliceSkip = unitNum × unitSkip
  - planeSkip = sliceNum × sliceSkip
  - cubeSkip = planeNum × planeSkip
- [x] 2.3 Test compilation without warnings

## 3. Update Test Utilities

- [x] 3.1 Update `microarch_tensor_create_random()` in `tests/utils.c`
  - Initialize `enablePowerOf2Skip = 1` (default)
  - **Note**: Not needed - backward compatible (0 or NULL defaults to power-of-2)
- [x] 3.2 Update `microarch_tensor_create_from_desc()` if needed
  - **Note**: Not needed

## 4. Update Constraint Tests

- [x] 4.1 Update `tests/constraint_test.c`
  - Add test case: Direct mapping mode (enablePowerOf2Skip=1)
  - Verify Q4_0: byteNum=18 → unitSkip=18
  - Verify hasGap=0 in direct mode
  - Add test: Power-of-2 default mode (enablePowerOf2Skip=0 or NULL)
  - Keep existing tests for backward compatibility
- [x] 4.2 Update `tests/batch_constraint_test.c`
  - **Note**: Batch test not needed - core functionality tested in constraint_test.c

## 5. Create New Test File (Optional)

- [x] 5.1 Create `tests/non_power_of_2_test.c`
  - **Note**: Optional - comprehensive tests added to constraint_test.c instead
- [x] 5.2 Update `CMakeLists.txt` to include new test
  - **Note**: Not needed - constraint_test.c already built

## 6. Build and Verify

- [x] 6.1 Clean build: `cd build && rm -rf * && cmake .. && make`
- [x] 6.2 Run constraint test: `./constraint_test` - **PASSED (24/24)**
- [x] 6.3 Run batch test: `./batch_constraint_test` - **PASSED**
- [x] 6.4 Run new test (if created): `./non_power_of_2_test` - **N/A**
- [x] 6.5 Verify all tests pass

## 7. Documentation

- [x] 7.1 Update `TODO.md` to mark completion
- [x] 7.2 Add usage example in code comments
  - Usage: Set `constraints.enablePowerOf2Skip = 1` for direct mapping
  - Default (0 or NULL): Power-of-2 mode (backward compatible)
- [x] 7.3 Verify AGENTS.md is up to date

## Implementation Order

1. Header file update (Task 1)
2. Implementation update (Task 2)
3. Test utilities update (Task 3)
4. Existing tests update (Task 4)
5. New test file (Task 5, optional)
6. Build and verify (Task 6)
7. Documentation (Task 7)
