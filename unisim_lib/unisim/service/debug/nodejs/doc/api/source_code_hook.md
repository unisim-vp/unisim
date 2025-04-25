# Class SourceCodeHook

Class SourceCodeHook provides support for triggering the execution of Javascript callbacks when program reaches a particular source code statement.

## `(constructor) SourceCodeHook(processor: Processor, file: string, loc: (Object|string)) => SourceCodeHook`

Create a hook at source code location `loc` executed from executable file `file` for processor `processor`.

Valid source code location are `{ sourceCodeFile: 'main.c', lineno: 1234 }` or '`main.c:1234`'.

Example of use:

	let hook = new SourceCodeHook(processor, 'test.elf', 'main.c:1234');

## `(read-only) file: string`

Executable file for which this hooks applies.

## `(read-only) loc: SourceCodeLocation`

Source code location for which this hooks applies.
