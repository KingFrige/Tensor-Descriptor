## ADDED Requirements

### Requirement: Configuration Option for Skip Alignment
The system SHALL provide a configuration option to control whether unitSkip uses power-of-2 alignment.

#### Scenario: Default behavior unchanged
- **GIVEN** existing code without the new configuration option
- **WHEN** calling `tensor_descriptor_convert()`
- **THEN** it SHALL use power-of-2 alignment (backward compatible)

#### Scenario: Enable power-of-2 mode explicitly
- **GIVEN** `microarch_constraints_t` with `enablePowerOf2Skip = 1`
- **WHEN** converting tensor descriptor
- **THEN** unitSkip SHALL be `next_power_of_2(byteNum)`
- **AND** hasGap SHALL be calculated based on alignment

#### Scenario: Disable power-of-2 mode
- **GIVEN** `microarch_constraints_t` with `enablePowerOf2Skip = 0`
- **WHEN** converting tensor descriptor
- **THEN** unitSkip SHALL equal byteNum directly
- **AND** hasGap SHALL be 0

### Requirement: Memory Optimization for Quantized Types
When power-of-2 alignment is disabled, quantized types SHALL have reduced memory footprint.

#### Scenario: Q4_0 memory usage
- **GIVEN** a Q4_0 tensor with byteNum=18
- **WHEN** `enablePowerOf2Skip = 0`
- **THEN** unitSkip SHALL be 18 (not 32)
- **AND** memory overhead SHALL be 0% (vs 78% with alignment)

#### Scenario: Q8_0 memory usage
- **GIVEN** a Q8_0 tensor with byteNum=34
- **WHEN** `enablePowerOf2Skip = 0`
- **THEN** unitSkip SHALL be 34 (not 64)
- **AND** memory overhead SHALL be 0% (vs 88% with alignment)

### Requirement: Backward Compatibility
The default behavior SHALL remain unchanged to ensure backward compatibility.

#### Scenario: NULL constraints
- **GIVEN** NULL constraints pointer
- **WHEN** converting tensor descriptor
- **THEN** it SHALL use power-of-2 alignment (default behavior)

#### Scenario: Uninitialized new field
- **GIVEN** constraints structure without initializing `enablePowerOf2Skip`
- **WHEN** converting (assuming memory may have garbage)
- **THEN** the system SHALL treat non-zero as power-of-2 mode
- **AND** zero as direct mapping mode

### Requirement: Upper Skip Calculation
Upper level skips (sliceSkip, planeSkip, cubeSkip) SHALL be calculated consistently regardless of unitSkip mode.

#### Scenario: Skip hierarchy with direct mapping
- **GIVEN** `enablePowerOf2Skip = 0`
- **WHEN** calculating skips
- **THEN** sliceSkip = unitNum × unitSkip
- **AND** planeSkip = sliceNum × sliceSkip
- **AND** cubeSkip = planeNum × planeSkip

#### Scenario: Skip hierarchy with power-of-2
- **GIVEN** `enablePowerOf2Skip = 1`
- **WHEN** calculating skips
- **THEN** the same multiplicative hierarchy SHALL apply

### Requirement: Test Coverage
All existing tests SHALL pass, and new tests SHALL cover both modes.

#### Scenario: Existing tests with default mode
- **GIVEN** existing test cases
- **WHEN** running tests
- **THEN** all tests SHALL pass without modification

#### Scenario: New test for direct mapping
- **GIVEN** new test case with `enablePowerOf2Skip = 0`
- **WHEN** converting Q4_0 tensor (byteNum=18)
- **THEN** unitSkip SHALL equal 18
- **AND** hasGap SHALL equal 0

#### Scenario: Memory comparison test
- **GIVEN** same tensor with both modes
- **WHEN** calculating total memory
- **THEN** direct mapping SHALL use less memory than power-of-2
