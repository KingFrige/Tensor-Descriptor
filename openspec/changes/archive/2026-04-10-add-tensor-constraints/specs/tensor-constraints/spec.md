## ADDED Requirements

### Requirement: Constraint structure definition
The system SHALL define a `microarch_constraints_t` structure to hold user-configurable constraints.

#### Scenario: Zero constraint means no limit
- **WHEN** a constraint field is set to 0 (e.g., maxUnitNum=0)
- **THEN** that dimension has no upper limit

#### Scenario: Non-zero constraint means upper bound
- **WHEN** a constraint field is set to a positive value (e.g., maxUnitNum=16)
- **THEN** the corresponding dimension MUST NOT exceed that value

### Requirement: Byte number hardware limit
The system SHALL enforce a hard-coded maximum byteNum of 64.

#### Scenario: Valid byteNum
- **WHEN** arch dimension[0] (byteNum) is between 1 and 64
- **THEN** the conversion proceeds normally

#### Scenario: Invalid byteNum exceeds hardware limit
- **WHEN** arch dimension[0] (byteNum) is greater than 64
- **THEN** conversion fails with error E_BYTE_NUM_EXCEEDED

### Requirement: Constraint-based dimension adjustment
When converting arch_tensor to microarch_tensor and constraints are violated, the system SHALL attempt to adjust dimensions while preserving total data bytes.

#### Scenario: Dimension adjustment priority
- **WHEN** constraints require reducing total bytes
- **THEN** dimensions are reduced in priority order: cubeNum > planeNum > sliceNum > unitNum (lowest priority first)

#### Scenario: Successful adjustment
- **WHEN** dimension can be reduced to satisfy constraints while preserving data bytes
- **THEN** conversion succeeds with adjusted dimensions

#### Scenario: Zero dimension normalized to one
- **WHEN** any dimension results in 0 after adjustment
- **THEN** it SHALL be normalized to 1

### Requirement: Over-constraint detection
The system SHALL detect when constraints cannot be satisfied simultaneously.

#### Scenario: Over-constrained dimensions
- **WHEN** user provides constraints that cannot be satisfied together (e.g., unitNum≤16, planeNum≤14, but byteNum≤64 cannot compensate)
- **THEN** conversion fails with error E_OVER_CONSTRAINED

### Requirement: Gap detection in memory layout
The system SHALL detect when byteNum alignment causes memory gaps between arch and microarch representations.

#### Scenario: No gap (tight packing)
- **WHEN** byteNum is a power of 2
- **THEN** arch_total_bytes equals microarch_total_bytes and hasGap is set to 0

#### Scenario: Gap exists (alignment waste)
- **WHEN** byteNum is not a power of 2 (e.g., byteNum=32, actual stride=64)
- **THEN** arch_total_bytes is less than microarch_total_bytes and hasGap is set to 1

#### Scenario: Gap calculation
- **WHEN** computing hasGap flag
- **THEN** microarch_min_bytes = byteNum × unitNum × sliceNum × planeNum × cubeNum is compared against actual allocated bytes = stride × unitNum × sliceNum × planeNum × cubeNum

### Requirement: Conversion result structure
The system SHALL return a conversion result containing the microarch descriptor, hasGap flag, and error code.

#### Scenario: Successful conversion
- **WHEN** conversion completes without errors
- **THEN** result.errorCode is 0, result.hasGap reflects gap status, result.desc contains valid microarch descriptor

#### Scenario: Failed conversion
- **WHEN** conversion fails due to constraints or invalid input
- **THEN** result.errorCode is negative, result.desc is undefined

### Requirement: Sub-tensor constraint handling
The system SHALL apply the same constraint system to sub-tensor conversion.

#### Scenario: Sub-tensor with constraints
- **WHEN** converting sub-arch-tensor to sub-microarch-tensor with constraints
- **THEN** same constraint checking and adjustment logic applies

### Requirement: Data volume invariance
The system SHALL ensure arch_total_bytes equals microarch_min_bytes after successful conversion (when hasGap=0).

#### Scenario: Volume preserved
- **WHEN** conversion succeeds with hasGap=0
- **THEN** product(arch.dimension[0:4]) equals product(microarch.desc.*Num)

#### Scenario: Volume calculation with gap
- **WHEN** hasGap=1
- **THEN** arch_total_bytes < actual_microarch_bytes is expected and valid