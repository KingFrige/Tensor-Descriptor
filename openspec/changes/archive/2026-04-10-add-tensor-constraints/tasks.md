## 1. Update Header Files

- [x] 1.1 Add `microarch_constraints_t` structure to `src/microarch_tensor.h`
- [x] 1.2 Add `microarch_conversion_result_t` structure to `src/microarch_tensor.h`
- [x] 1.3 Add error code constants (E_SUCCESS, E_BYTE_NUM_EXCEEDED, etc.) to `src/microarch_tensor.h`
- [x] 1.4 Add `arch_tensor_convert_with_constraints()` function declaration to `src/arch_tensor.h`
- [x] 1.5 Add `arch_tensor_convert_sub_with_constraints()` function declaration to `src/arch_tensor.h`

## 2. Implement Constraint Checking Logic

- [x] 2.1 Implement `check_byte_num_limit()` - verify dimension[0] <= 64
- [x] 2.2 Implement `adjust_dimension_to_constraint()` - reduce dimension to meet constraint while preserving total bytes
- [x] 2.3 Implement `detect_has_gap()` - calculate gap based on byteNum vs stride alignment
- [x] 2.4 Implement `normalize_zero_dimensions()` - convert 0 to 1 in dimensions
- [x] 2.5 Implement `validate_constraints_feasible()` - detect over-constrained scenarios

## 3. Implement Conversion Functions

- [x] 3.1 Implement `arch_tensor_convert_with_constraints()` with constraint checking
- [x] 3.2 Implement `arch_tensor_convert_sub_with_constraints()` for sub-tensor
- [x] 3.3 Calculate stride values: stride[0] = 2^ceil(log2(byteNum)), stride[i] = dim[i] * stride[i-1]
- [x] 3.4 Fill `microarch_conversion_result_t` with desc, hasGap, errorCode

## 4. Update Build System

- [x] 4.1 Add `tests/constraint_test.c` to CMakeLists.txt
- [x] 4.2 Verify compilation with `-Wall -Wextra`

## 5. Write Tests

- [x] 5.1 Test valid conversion with no constraints (hasGap=0)
- [x] 5.2 Test valid conversion with constraints that require adjustment
- [x] 5.3 Test byteNum > 64 returns E_BYTE_NUM_EXCEEDED
- [x] 5.4 Test over-constrained scenario returns E_OVER_CONSTRAINED
- [x] 5.5 Test hasGap detection (byteNum not power of 2)
- [x] 5.6 Test zero dimension normalization to 1
- [x] 5.7 Test sub-tensor conversion with constraints

## 6. Verification

- [x] 6.1 Build project: `cd build && cmake .. && make`
- [x] 6.2 Run `./microarchTensorDescriptor` - verify no regression
- [x] 6.3 Run `./archTensorDescriptor` - verify no regression
- [x] 6.4 Run `./constraint_test` - verify all test cases pass
- [x] 6.5 Update TODO.md to mark completion