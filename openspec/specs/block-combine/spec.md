## ADDED Requirements

### Requirement: Block Combine Configuration
The system SHALL provide a configuration structure for block combine behavior.

#### Scenario: Default auto-selection
- **GIVEN** `combine_config.target_byteNum = 0`
- **WHEN** calling `ggml_to_microarch_direct_map()`
- **THEN** it SHALL automatically calculate optimal `target_byteNum`
- **AND** the value SHALL be `floor(max_byteNum / type_size) * type_size`

#### Scenario: User-specified target
- **GIVEN** `combine_config.target_byteNum = 36`
- **WHEN** calling the function with type_size=18
- **THEN** it SHALL use 36 as the target (2 blocks per unit)

### Requirement: Block Combine Calculation
The system SHALL correctly calculate combined unit parameters.

#### Scenario: Q4_0 block combine
- **GIVEN** Q4_0 tensor with type_size=18, 128 blocks
- **WHEN** combining with max_byteNum=64
- **THEN** target_byteNum SHALL be 54
- **AND** blocks_per_unit SHALL be 3
- **AND** unitNum SHALL be 43 (ceil(128/3))
- **AND** unitSkip SHALL be 54 (3 * 18)

#### Scenario: F32 block combine
- **GIVEN** F32 tensor with type_size=4, 4096 elements
- **WHEN** combining with max_byteNum=64
- **THEN** target_byteNum SHALL be 64
- **AND** blocks_per_unit SHALL be 16
- **AND** unitNum SHALL be 256 (ceil(4096/16))

### Requirement: Full Tensor Coverage
The system SHALL support all GGML types defined in `ggml_tensor.h`.

#### Scenario: F32 support
- **GIVEN** type=0 (F32), type_size=4
- **WHEN** applying block combine
- **THEN** it SHALL produce valid microarch descriptor

#### Scenario: F16 support
- **GIVEN** type=1 (F16), type_size=2
- **WHEN** applying block combine
- **THEN** it SHALL produce valid microarch descriptor

#### Scenario: Q4_0 support
- **GIVEN** type=2 (Q4_0), type_size=18
- **WHEN** applying block combine
- **THEN** it SHALL produce valid microarch descriptor

#### Scenario: Q8_0 support
- **GIVEN** type=8 (Q8_0), type_size=34
- **WHEN** applying block combine
- **THEN** it SHALL produce valid microarch descriptor

### Requirement: Full Test Data Validation
The system SHALL correctly process all 102 tensors from test data.

#### Scenario: Process all test tensors
- **GIVEN** `tests/ggml_tensor_test_data.inc` with 102 tensors
- **WHEN** iterating through all tensors
- **THEN** each SHALL produce valid conversion result
- **AND** no conversion SHALL fail

### Requirement: Backward Compatibility
The system SHALL maintain backward compatibility when config is NULL.

#### Scenario: NULL config
- **GIVEN** `combine_config = NULL`
- **WHEN** calling `ggml_to_microarch_direct_map()`
- **THEN** it SHALL behave as before (no block combine)
- **AND** byteNum SHALL equal type_size

## Scenarios

### Scenario: End-to-end Q4_0 combine
- **GIVEN** ne={4096, 151936, 1, 1}, type_size=18, block_size=32
- **AND** max_byteNum=64
- **WHEN** converting with block combine
- **THEN** result byteNum=54, unitNum=43, unitSkip=54
- **AND** access count reduced from 1940M to 653M

### Scenario: Edge case - small tensor
- **GIVEN** tensor with only 2 blocks
- **AND** blocks_per_unit=3
- **WHEN** converting
- **THEN** unitNum=1 (ceil(2/3))
- **AND** conversion succeeds

### Scenario: Type that cannot combine
- **GIVEN** Q8_0 with type_size=34
- **AND** max_byteNum=64
- **WHEN** calculating target_byteNum
- **THEN** result is 34 (floor(64/34)*34)
- **AND** blocks_per_unit=1 (no combine possible)
