# Class SourceCodeBreakpoint

This class derives from `DebugEvent`.

## `(constructor) SourceCodeBreakpoint(processor: Processor, loc: (Object|string), [file: string]) => SourceCodeBreakpoint`

Create a breakpoint at source code location `loc` executed from executable file `file` for processor `processor`.

Valid source code location are `{ sourceCodeFile: 'main.c', lineno: 1234 }` or '`main.c:1234`'.

Example of use:

	let brk1 = new SourceCodeBreakpoint(processor, 'main.c:1234', 'test.elf');
	let brk2 = new SourceCodeBreakpoint(processor, { sourceCodeFile: 'main.c', lineno: 1234 }, 'test.elf');
	let brk3 = new SourceCodeBreakpoint(processor, SourceCodeLocation('main.c', 1234), 'test.elf');

## `(read-only) id: Number`

The breakpoint identifier.

## `(read-only) file: string`

Name of executable file that this breakpoint monitors.

## `(read-only) loc: SourceCodeLocation`

Source code location that this breakpoint monitors.
