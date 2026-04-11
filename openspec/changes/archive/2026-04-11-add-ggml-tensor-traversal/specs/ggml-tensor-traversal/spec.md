## ADDED Requirements

### Requirement: GGML Type Enumeration
The system SHALL define GGML data types as an enumeration matching llama.cpp values.

#### Scenario: Type values match llama.cpp
- **WHEN** checking type constants
- **THEN** they SHALL match:
  - GGML_TYPE_F32 = 0
  - GGML_TYPE_F16 = 1
  - GGML_TYPE_Q4_0 = 2
  - GGML_TYPE_Q8_0 = 8

### Requirement: Type Size Query
The system SHALL provide type size information for each supported type.

#### Scenario: Get type_size for F32
- **WHEN** calling `ggml_type_get_size(GGML_TYPE_F32)`
- **THEN** it SHALL return 4 (bytes per element)

#### Scenario: Get type_size for Q4_0
- **WHEN** calling `ggml_type_get_size(GGML_TYPE_Q4_0)`
- **THEN** it SHALL return 18 (bytes per block)

### Requirement: Block Size Query
The system SHALL provide block size information for each supported type.

#### Scenario: Get blck_size for F32
- **WHEN** calling `ggml_type_get_blck_size(GGML_TYPE_F32)`
- **THEN** it SHALL return 1 (elements per block)

#### Scenario: Get blck_size for Q4_0
- **WHEN** calling `ggml_type_get_blck_size(GGML_TYPE_Q4_0)`
- **THEN** it SHALL return 32 (elements per block)

### Requirement: Tensor Descriptor Structure
The system SHALL define a tensor descriptor structure containing dimension, stride, type, and data pointer.

#### Scenario: Create descriptor for Q4_0 tensor
- **WHEN** populating `ggml_tensor_desc_t`
- **THEN** it SHALL contain:
  - ne[4]: element counts for 4 dimensions
  - nb[4]: byte strides for 4 dimensions
  - type: GGML type enum value
  - type_size: bytes per block
  - blck_size: elements per block
  - data: pointer to tensor data

### Requirement: Traversal Count Calculation
The system SHALL calculate the number of blocks to traverse.

#### Scenario: Calculate blocks for 4D tensor
- **GIVEN** a tensor with ne = {4096, 1024, 1, 1}, blck_size = 32
- **WHEN** calling `ggml_tensor_get_traversal_count()`
- **THEN** it SHALL return 128 * 1024 (blocks in dim0 * dim1)

### Requirement: Block-Level Traversal
The system SHALL traverse a GGML tensor block by block, returning address and element info for each block.

#### Scenario: Traverse Q4_0 weight matrix
- **GIVEN** a 4096x151936 Q4_0 matrix from qwen_tensor.json
- **WHEN** calling `ggml_tensor_traversal()`
- **THEN** it SHALL return an array of traversal items
- **AND** each item SHALL contain:
  - block_idx: sequential block index
  - block_addr: calculated address using stride formula
  - elem_start: starting element index within tensor
  - elem_count: 32 (Q4_0 block size)

#### Scenario: Address calculation uses strides
- **GIVEN** tensor with nb = {18, 2304, 350060544, 350060544}
- **WHEN** traversing block at (i0=1, i1=1, i2=0, i3=0)
- **THEN** block_addr SHALL be data + 1*18 + 1*2304

### Requirement: Memory Management
The traversal function SHALL allocate memory for results that must be freed by caller.

#### Scenario: Allocate and free traversal results
- **WHEN** calling `ggml_tensor_traversal()`
- **THEN** it SHALL return malloc'd array
- **AND** caller SHALL free the array when done

### Requirement: Test Data Extraction
The system SHALL provide a Python script to extract test data from qwen_tensor.json.

#### Scenario: Extract first tensor from JSON
- **GIVEN** docs/qwen_tensor.json exists
- **WHEN** running `python3 scripts/extract_qwen_tensor.py`
- **THEN** it SHALL generate tests/ggml_tensor_test_data.inc
- **AND** the file SHALL contain C array with tensor metadata

### Requirement: Independent Implementation
All GGML tensor code SHALL be independent of llama.cpp source.

#### Scenario: No llama.cpp includes
- **WHEN** compiling src/ggml_tensor.c
- **THEN** it SHALL NOT include any llama.cpp headers
- **AND** it SHALL use only self-contained type definitions
