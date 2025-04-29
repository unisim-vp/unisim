# Global definitions

## `(read-only) processors: Array of Processor`

The list of processors.

## `(read-only) simulator: UnisimObject`

The UNISIM-VP simulator object hierarchy width their variables (parameters and statistics).

## `(read-only) inInteractiveMode: boolean`

Whether simulator is running in interactive mode (see Run-time parameter `interactive` of Node.js service).

## `(read-only) inBuiltinREPL: boolean`

Whether running in simulator builtin REPL.

## `quit()`

Quit and stop simulation.

## `continueExecution([options: object]) => Promise`

Get a promise that resolves once any debug events occur, or Ctrl-C is pressed in REPL.

The fulfillment value of the promise is the processor for which debug a event occurs, or `undefined` when Ctrl-C is pressed.

This function is *only* useful when Node.js service of simulator is configured for interactive (blocking) mode.

Options:

* `unblock`: if true and simulator is in interactive mode, this function also let simulator
  runs until any debug events occur, or Ctrl-C is pressed in REPL.

Examples of use:

	continueExecution(/* unblock */ true).then((processor) => {
		let pc = processor.registers.pc.get();
		console.log('0x' + pc.toString(16) + ': ' + processor.disasm(pc));
	});

or

	let processor = await continueExecution(/* unblock */ true);
	let pc = processor.registers.pc.get();
	console.log('0x' + pc.toString(16) + ': ' + processor.disasm(pc));

## `loadDebugInfo(file: string)`

Load debug info from File `file`.

## `getExecutableBinaryFiles() => Array of ExecutableBinaryFile`

Get the list of executable binary file for which debug infos have been loaded.

## `getSymbols() => Array of Symbol`

Get all symbols.

## `findSymbol((name: string | address: Number), [type: string]) => Symbol`

Find a symbol by address `address` or by name `name` (and optionally with type `type`).

`type` may have one of the following values:

* '`unspecified-type`'
* '`data-object`'
* '`function`'
* '`section`'
* '`source-file`'
* '`uninitialized-common-block`'
* '`tls-object`'
* '`operating-system-specific-type`'
* '`processor-specific-type`' 

## `getStatements([file: string]) => Array of Statement`

Get all statements from all enabled executable files (or optionally only from File `file`).

## `findStatement((address: Number | loc: (Object|string)), [options: Object]) => Statement`

Find one statement by address `address` or by source code location `loc`.

Valid source code location are `{ sourceCodeFile: 'main.c', lineno: 1234 }` or '`main.c:1234`'.

`options` properties:

* `file`: string
  * search only from executable binary file `file`
* `scope`: string
  * When searching by address, it controls which statement is returned:
    * '`nearest-lower-or-equal-stmt`': address of returned statement is lower or equal to `address`
    * '`nearest-lower-or-equal-stmt-within-function`': address of returned statement is lower or equal to `address` and within function that contains address `address`
    * '`exact-stmt`': address of returned statement is strictly equal to `address` (this is the default)
    * '`next-stmt`': address of returned statement is strictly greater than `address`
    * '`next-stmt-within-function`': address of returned statement is strictly greater than `address` and within function that contains address `address`

## `findStatements((address: Number | loc: (Object|string)) => Array of Statement`

Find one or more statements by address `address` or by source code location `loc`.

Valid source code location are `{ sourceCodeFile: 'main.c', lineno: 1234 }` or '`main.c:1234`'.

`options` properties:

* `file`: string
  * search only from executable binary file `file`

## `findSubProgram((name: string | address: Number), [options: Object]) => SubProgram`

Find a sub-program by address `address` or by name `name`.

`options` properties:

* `file`: string
  * when searching by name, limit the search to executable binary file `file`
* `compilationUnit`:
  * when searching by name, limit the search to compilation unit `compilationUnit`
