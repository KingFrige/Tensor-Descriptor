# Proposal: Add Microarchitecture Tensor Documentation

## Background

The project currently has a functional tensor descriptor system that converts between architecture-level and microarchitecture-level tensor representations. However, the **microarch_tensor** structure and its relationship to hardware DMA units lacks comprehensive documentation.

## Problem Statement

Multiple hardware specification documents exist but are scattered:
- `tensor_engine_v1.5.html` (Section 4.8 DMA Unit)
- `tensor_streaming_accelerator_architecture.html` (Section 3.6.1 Data Structure)
- `assets/model_spec.md` (Functional logic)

There is no unified technical document explaining:
1. The 4-level nested loop addressing model (Unit→Slice→Plane→Cube)
2. How `microarch_tensor` maps to hardware DMA registers
3. Skip calculation requirements (power-of-2 alignment)
4. Status flags and their hardware meaning

## Objectives

Create `docs/microarch_tensor.md` that:
1. **Synthesizes** information from all hardware specification documents
2. **Maps** code implementation to hardware concepts
3. **Documents** the constraint system and physical limits
4. **Explains** address calculation formulas with examples

## Success Criteria

- [ ] Document covers all 4 hardware dimension levels
- [ ] Document explains next_power_of_2 requirement from hardware
- [ ] Document maps code fields to hardware DMA registers
- [ ] Document includes at least 2 usage examples

## Dependencies

- Hardware specification documents (provided in assets/)
- Source code in src/tensor_descriptor.h and .c
- Understanding of DMA hardware concepts

## Scope

**In Scope:**
- Microarchitecture tensor structure documentation
- DMA hardware mapping explanation
- Address calculation formula documentation
- Constraint and limit system explanation

**Out of Scope:**
- Architecture tensor (arch_tensor) deep dive
- Constraint solver algorithm implementation
- Tensor conversion algorithm pseudocode

## Estimated Effort

2-3 hours of focused documentation writing and synthesis
