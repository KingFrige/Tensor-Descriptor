## ADDED Requirements

### Requirement: Microarchitecture tensor descriptor structure
The system SHALL define a C structure `microarch_tensor_descriptor_t` equivalent to the C++ `microarchTensorDescriptor`.

#### Scenario: Structure field equivalence
- **WHEN** comparing C and C++ descriptor structures
- **THEN** the C structure SHALL have identical fields:
  - `baseAddr` (int)
  - `byteNum`, `unitNum`, `sliceNum`, `planeNum`, `cubeNum` (int)
  - `unitSkip`, `sliceSkip`, `planeSkip`, `cubeSkip` (int)

### Requirement: Microarchitecture tensor lifecycle
The system SHALL provide explicit lifecycle management for microarchitecture tensors.

#### Scenario: Create tensor with random data
- **WHEN** calling `microarch_tensor_create_random()`
- **THEN** it SHALL allocate and initialize a tensor with random valid values
- **AND** the skip values SHALL be calculated as:
  - `unitSkip = 1 << ceil(log2(byteNum))`
  - `sliceSkip = unitNum * unitSkip`
  - `planeSkip = sliceNum * sliceSkip`
  - `cubeSkip = planeNum * planeSkip`

#### Scenario: Create tensor from descriptor
- **WHEN** calling `microarch_tensor_create_from_desc(&desc)`
- **THEN** it SHALL allocate and initialize a tensor with the provided descriptor values

#### Scenario: Destroy tensor
- **WHEN** calling `microarch_tensor_destroy(tensor)`
- **THEN** it SHALL free all memory associated with the tensor

### Requirement: Generate sub-tensor
The system SHALL generate sub-tensor descriptors with random ranges within parent bounds.

#### Scenario: Generate valid sub-tensor
- **WHEN** calling `microarch_tensor_gen_sub_tensor(parent, &sub_desc)`
- **THEN** the sub-tensor fields SHALL satisfy:
  - `sub.byteNum ≤ parent.byteNum`
  - `sub.unitNum ≤ parent.unitNum`
  - `sub.sliceNum ≤ parent.sliceNum`
  - `sub.planeNum ≤ parent.planeNum`
  - `sub.cubeNum ≤ parent.cubeNum`
- **AND** the skip values SHALL be copied from parent

### Requirement: Address traversal
The system SHALL traverse tensor memory and calculate all address points.

#### Scenario: Calculate traversal count
- **WHEN** calling `microarch_tensor_get_traversal_count(&desc)`
- **THEN** it SHALL return `unitNum * sliceNum * planeNum * cubeNum`

#### Scenario: Traverse and collect addresses
- **WHEN** calling `microarch_tensor_traversal(tensor, &addresses)`
- **THEN** it SHALL allocate an array of size `get_traversal_count()`
- **AND** populate it with all memory addresses in traversal order:
  - Outer loop: cube (0 to cubeNum-1)
  - Middle loop: plane (0 to planeNum-1)
  - Inner loop: slice (0 to sliceNum-1)
  - Innermost: unit (0 to unitNum-1), incrementing by unitSkip
- **AND** addresses SHALL wrap around 22-bit space: `addr & ((1<<22)-1)`

### Requirement: Architecture tensor descriptor structure
The system SHALL define C structures equivalent to C++ `archTensor` classes.

#### Scenario: Architecture descriptor fields
- **WHEN** inspecting `arch_tensor_descriptor_t`
- **THEN** it SHALL contain:
  - `baseAddr` (unsigned int)
  - `dimension[5]` array (unsigned int)
  - `stride[5]` array (unsigned int)

#### Scenario: Sub-architecture descriptor fields
- **WHEN** inspecting `sub_arch_tensor_descriptor_t`
- **THEN** it SHALL contain:
  - `coords[5]` array (unsigned int) - starting coordinates
  - `range[5]` array (unsigned int) - dimension ranges
  - `traversal_stride[5]` array (unsigned int) - stride multipliers

### Requirement: Architecture tensor lifecycle
The system SHALL manage architecture tensor lifecycle explicitly.

#### Scenario: Create architecture tensor
- **WHEN** calling `arch_tensor_create_random()`
- **THEN** it SHALL allocate and initialize a 5D tensor descriptor
- **AND** stride[0] SHALL be `1 << ceil(log2(dimension[0]))`
- **AND** stride[i] SHALL be `dimension[i] * stride[i-1]` for i > 0

#### Scenario: Generate sub-tensor from architecture
- **WHEN** calling `arch_tensor_gen_sub_tensor(arch, &sub_desc)`
- **THEN** coords[i] SHALL be random values ≤ dimension[i]
- **AND** range[i] SHALL be random values ≤ dimension[i] - coords[i]

### Requirement: Convert architecture to microarchitecture
The system SHALL convert architecture tensors to microarchitecture representation.

#### Scenario: Convert full tensor
- **WHEN** calling `arch_tensor_convert_to_microarch(arch, &micro_desc)`
- **THEN** the mapping SHALL be:
  - `baseAddr = arch->baseAddr`
  - `byteNum = arch->dimension[0]`, `unitNum = arch->dimension[1]`, etc.
  - `unitSkip = arch->stride[0]`, `sliceSkip = arch->stride[1]`, etc.

#### Scenario: Convert sub-tensor
- **WHEN** calling `arch_tensor_convert_sub_to_microarch(arch, &sub_desc, &micro_desc)`
- **THEN** baseAddr SHALL be calculated as: `Σ(coords[i] * stride[i])`
- **AND** range values map to microarch dimensions
- **AND** traversal_stride values multiply the original stride

### Requirement: API compatibility
The C API SHALL provide equivalent functionality to the C++ API.

#### Scenario: Functional equivalence test
- **WHEN** running identical operations on C++ and C versions with same seed
- **THEN** the outputs SHALL be identical (except for random variations)
- **AND** address calculations SHALL produce the same results
