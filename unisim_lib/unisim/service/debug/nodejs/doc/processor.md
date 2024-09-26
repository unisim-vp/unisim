# Class Processor

This class is not construtible.

Class Processor provides support for:

* stepping into
* stepping over
* finishing the current function
* return from current function
* disassembling instructions
* getting names of available data objects
* reading/writing registers
* reading/writing memory
* getting information about stack frames
* getting time

## `(constructor) Processor(id: Number) => Processor`

Get processor identified by `id`.

## `(read-only) id: Number`

The processor identifier (i.e. processor number).

## `(read-only) registers: Object`

The processor registers.
Each property, of type Register, is named as the register it exposes.

## `(async) stepInstruction(callback: function)`

Asynchronously step an instruction.

Once instruction has been stepped, the `callback` function is called.

Example of use:

	processor.stepInstruction(() => console.log('instruction stepped'));

## `(async) stepInstruction() => Promise`

Asynchronously step an instruction.

Once instruction has been stepped, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.stepInstruction();
		console.log('instruction stepped');
	}

## `(async) nextInstruction(callback: function)`

Asynchronously step over an instruction.

Once instruction has been stepped over, the `callback` function is called.

Example of use:

	processor.nextInstruction(() => console.log('instruction stepped over'));

## `(async) nextInstruction() => Promise`

Asynchronously step over an instruction.

Once instruction has been stepped over, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.nextInstruction();
		console.log('instruction stepped over');
	}

## `(async) step(callback: function)`

Asynchronously step into a source code statement.
Function `stepInto'` is an alias of this function.

Once source code statement has been stepped, the `callback` function is called.

Example of use:

	processor.nextInstruction(() => console.log('instruction stepped'));

## `(async) step() => Promise`

Asynchronously step into a source code statement.
Function `stepInto'` is an alias of this function.

Once source code statement has been stepped, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.step();
		console.log('source code statement stepped');
	}

## `(async) next(callback: function)`

Asynchronously step over a source code statement.
Function `stepOver'` is an alias of this function.

Once source code statement has been stepped over, the `callback` function is called.

Example of use:

	processor.nextInstruction(() => console.log('instruction stepped over'));

## `(async) next() => Promise`

Asynchronously step into a source code statement.
Function `stepOver'` is an alias of this function.

Once source code statement has been stepped over, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.step();
		console.log('source code statement stepped over');
	}

## `(async) finish(callback: function)`

Asynchronously finish current function.

Once current function has returned, the `callback` function is called.

Example of use:

	processor.finish(() => console.log('function has returned'));

## `(async) finish() => Promise`

Asynchronously finish current function.
Function `stepOut'` is an alias of this function.

Once current function has returned, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.finish();
		console.log('function has returned');
	}

## `returnFromFunction([ret_value: *])`

Return from current function with an optional return value.

Example of use:

	processor.returnFromFunction(2);

## `disasm(address: Number) => string`

Disassemble one instruction at address `address`.

## `getTime() => string`

Get the time.

## `getDataObjectNames([scope: string]) => Array of string`

Get the list of available data object names.

Parameter `scope` controls the search scope:

* '`global-only`': to only consider global variables.
* '`local-only`': to only consider local variables of current subprogram.
* '`both-global-and-local`': to consider both global and local variables.

## `readMemory(address: Number, size: Number) => Buffer`

Read `size` bytes at address `address` from memory.
The result is an instance of Node.js Buffer.

Example of use:

	let buf = processors[0].readMemory(0x1234, 4);
	console.log(buf);

## `writeMemory(address: Number, buffer: Buffer)`

Write bytes from `buffer` into memory at address `address`.

Example of use:

	let buf = Buffer.from([0xde, 0xad, 0xbe, 0xef]); 
	processors[0].writeMemory(0x1234, buf);

## `getStackFrameInfos([maxStackFrames: Number]) => Array of StackFrameInfo`

Get information about stack frames.

## `selectStackFrame(frameId: Number) => boolean`

Select a stack frame.

Subsequent commands will be related to the selected frame until execution resumes.
