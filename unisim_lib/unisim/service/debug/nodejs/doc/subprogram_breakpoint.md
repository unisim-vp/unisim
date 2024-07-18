# Class SubProgramBreakpoint

This class derives from `DebugEvent`.

## `(constructor) SubProgramBreakpoint(processor: Processor, subprogram: SubProgram) => SubProgramBreakpoint`

Create a breakpoint at subprogram `subprogram` for processor `processor`.
See also `findSubProgram`.

Example of use:

	let brk1 = new SubProgramBreakpoint(processor, findSubProgram('main', { file: 'test.elf' }));

## `(read-only) subprogram: SubProgram`

Subprogram that this breakpoint monitors.
