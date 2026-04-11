# Design: Microarchitecture Tensor Documentation

## Document Structure

The final document `docs/microarch_tensor.md` will follow this structure:

```
1. Overview
   - What is microarch_tensor
   - Relationship to arch_tensor
   - Hardware context (DMA unit)

2. 4-Level Nested Loop Addressing
   - Unit → Slice → Plane → Cube hierarchy
   - Hardware registers mapping
   - State transitions (isSliceDone, isPlaneDone, etc.)

3. Data Structure Fields
   - Dimension fields (byteNum, unitNum, sliceNum, planeNum, cubeNum)
   - Skip fields (unitSkip, sliceSkip, planeSkip, cubeSkip)
   - Hardware range limits

4. Address Calculation
   - Physical address formula
   - Next power-of-2 alignment requirement
   - Skip calculation algorithm
   - Why power-of-2 is required by hardware

5. Constraint System
   - User constraints vs physical limits
   - Constraint resolution algorithm
   - Dimension overflow handling

6. Usage Examples
   - Basic tensor configuration
   - Dimension overflow scenario
   - Constraint handling

7. References
   - Links to hardware specs
   - Code references
```

## Key Technical Decisions

### 1. Unified Addressing Model

**Decision**: Document both code implementation and hardware specification in unified terms.

**Rationale**: 
- Hardware engineers use "dimension" and "skip"
- Software uses "unitNum" and "unitSkip"
- Must map both to avoid confusion

### 2. Explicit Power-of-2 Explanation

**Decision**: Dedicate section to explaining why `next_power_of_2()` is required.

**Rationale**:
- Code uses bit manipulation instead of math.h
- Hardware DMA requires aligned memory access
- Users often ask "why not just use byteNum directly?"

### 3. Hardware State Mapping

**Decision**: Map code error codes to hardware status flags.

**Rationale**:
- `isSliceDone`, `isPlaneDone` in hardware
- Correspond to overflow conditions in code
- Helps debug constraint violations

## Content Sources

### From Hardware Specs (tensor_engine_v1.5.html)
- DMA unit parameters (Section 4.8)
- Address skip mode configuration
- Status flags table
- Dimension range limits

### From Architecture Spec (tensor_streaming_accelerator_architecture.html)
- Data structure description (Section 3.6.1)
- 4-level hierarchy diagram
- Plane/Slice traversal logic

### From Model Spec (assets/model_spec.md)
- Functional logic overview
- Addressing model formulas
- Special boundary conditions (Last Slice, Offset)

### From Code (src/tensor_descriptor.h and .c)
- Structure field definitions
- Constraint resolution algorithm
- Skip calculation implementation
- Error code meanings

## Drafting Strategy

1. **Start with skeleton**: Create all section headers
2. **Fill code sections**: Document fields and formulas first
3. **Add hardware context**: Map code to hardware specs
4. **Create examples**: Based on test cases in tests/
5. **Review and refine**: Ensure consistency with all sources

## Success Metrics

- Document passes technical review by hardware team
- Software engineers can implement new tensor configs without reading code
- All hardware specification details are accurately represented
- Examples compile and run correctly
