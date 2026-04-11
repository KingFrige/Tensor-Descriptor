## ADDED Requirements

### Requirement: Simplified Dimension Distribution
The system SHALL perform dimension distribution without power-of-2 alignment.

#### Scenario: Q4_0 tensor conversion
- **GIVEN** a tensor with byteNum=18
- **WHEN** converting through dimension distribution
- **THEN** unitSkip SHALL be 18 (not 32)

#### Scenario: Q8_0 tensor conversion
- **GIVEN** a tensor with byteNum=34
- **WHEN** converting through dimension distribution
- **THEN** unitSkip SHALL be 34 (not 64)

#### Scenario: F32 tensor conversion
- **GIVEN** a tensor with byteNum=4
- **WHEN** converting through dimension distribution
- **THEN** unitSkip SHALL be 4

### Requirement: Remove Power-of-2 Flag
The system SHALL NOT require enablePowerOf2Skip flag for conversion.

#### Scenario: Conversion without flag
- **GIVEN** a constraints structure without enablePowerOf2Skip
- **WHEN** calling dimension distribution
- **THEN** it SHALL use direct mapping by default

#### Scenario: Backward compatibility
- **GIVEN** existing code with enablePowerOf2Skip=0
- **WHEN** upgrading to new version
- **THEN** behavior SHALL be equivalent to direct mapping

### Requirement: Simplified Skip Calculation
Skip values SHALL always be calculated as direct multiples.

#### Scenario: Skip calculation
- **GIVEN** dimensions [byteNum, unitNum, sliceNum, planeNum, cubeNum]
- **WHEN** calculating skips
- **THEN** unitSkip SHALL equal byteNum
- **AND** sliceSkip SHALL equal unitNum × unitSkip
- **AND** planeSkip SHALL equal sliceNum × sliceSkip
- **AND** cubeSkip SHALL equal planeNum × planeSkip

### Requirement: DMA Hardware Compatibility
The simplified distribution SHALL work with DMA hardware supporting non-power-of-2 skip.

#### Scenario: Non-power-of-2 skip
- **GIVEN** DMA controller supports arbitrary skip values
- **WHEN** using unitSkip=18 for Q4_0
- **THEN** DMA transfer SHALL complete successfully

#### Scenario: Various byte sizes
- **GIVEN** byteNum values from 1 to 64
- **WHEN** using direct mapping
- **THEN** all skip values SHALL be supported by DMA

## REMOVED Requirements

### Requirement: Power-of-2 Alignment (REMOVED)
~~The system SHALL support power-of-2 alignment for unitSkip.~~
- **Reason**: Hardware now supports non-power-of-2, alignment causes memory waste

### Requirement: hasGap Field (REMOVED)
~~The system SHALL provide hasGap field in result structures.~~
- **Reason**: With direct mapping, there is never a gap, field is unnecessary
- **Action**: Field completely removed from `microarch_conversion_result_t` and `tensor_conversion_result_t`

## MODIFIED Requirements

### Requirement: Memory Optimization
The system SHALL minimize memory overhead.

#### Scenario: Q4_0 memory efficiency
- **GIVEN** Q4_0 tensor with byteNum=18
- **WHEN** allocating memory
- **THEN** overhead SHALL be 0% (not 78%)

#### Scenario: Q8_0 memory efficiency
- **GIVEN** Q8_0 tensor with byteNum=34
- **WHEN** allocating memory
- **THEN** overhead SHALL be 0% (not 88%)

## Scenarios

### Scenario: End-to-end conversion
- **GIVEN** input dimensions [18, 128, 1, 1, 1]
- **AND** constraints with maxByteNum=64
- **WHEN** calling dimension distribution
- **THEN** output byteNum SHALL be 18
- **AND** output unitSkip SHALL be 18
- **AND** output sliceSkip SHALL be 2304 (128 × 18)

### Scenario: Dimension expansion
- **GIVEN** input dimensions [80, 100, 1, 1, 1]
- **AND** constraints with maxByteNum=64
- **WHEN** calling dimension distribution
- **THEN** output byteNum SHALL be 64
- **AND** output unitNum SHALL be 200 (expanded)
- **AND** unitSkip SHALL be 64

### Scenario: Constraint validation
- **GIVEN** input dimensions exceeding maxTotalBytes
- **WHEN** calling dimension distribution
- **THEN** it SHALL return E_OVER_CONSTRAINED

### Scenario: Zero dimension handling
- **GIVEN** input dimension [0, 100, 1, 1, 1]
- **WHEN** calling dimension distribution
- **THEN** byteNum SHALL be treated as 1 internally
- **AND** conversion SHALL succeed
