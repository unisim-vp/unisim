# Class SubProgramHook

Class SubProgramHook provides support for triggering the execution of Javascript callbacks when program enters a subprogram.

## `(constructor) SubProgramHook(processor: Processor, subprogram: SubProgram) => SubProgramHook`

Create a hook at subprogram `subprogram` for processor `processor`.

Example of use:

	let hook = new SubProgramHook(processor, findSubProgram('main', { file: 'test.elf' }));

## `(read-only) subprogram: SubProgram`

Subprogram for which this hooks applies.
