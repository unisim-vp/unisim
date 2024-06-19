UNISIM-VP debugging front-end for the Node.js JavaScript runtime environment
============================================================================

Javascript API
==============

## Global definitions

### processors : Array of Processor

### quit()

Quit and stop simulation.

### continueExecution() => Promise

Continue execution until any breakpoint or watchpoint, or Ctrl-C is pressed in REPL.

## class Processor

Class Processor provides basic support for controlling execution at processor level:

* step into
* step over
* finish
* return

### [constructor] Processor(id : Number) => Processor

Get processor identified by `id`.

### [read-only] id : Number

The processor identifier (i.e. processor number).

### [async] stepInstruction(callback : function)

Asynchronously step an instruction.

Once instruction has been stepped, the `callback` function is called.

Example of use:

	processor.stepInstruction(() => console.log('instruction stepped'));

### [async] stepInstruction() => Promise

Asynchronously step an instruction.

Once instruction has been stepped, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.stepInstruction();
		console.log('instruction stepped');
	}

### [async] nextInstruction(callback : function)

Asynchronously step over an instruction.

Once instruction has been stepped over, the `callback` function is called.

Example of use:

	processor.nextInstruction(() => console.log('instruction stepped over'));

### [async] nextInstruction() => Promise

Asynchronously step over an instruction.

Once instruction has been stepped over, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.nextInstruction();
		console.log('instruction stepped over');
	}

### [async] step(callback : function)

Asynchronously step into a source code statement.

Once source code statement has been stepped, the `callback` function is called.

Example of use:

	processor.nextInstruction(() => console.log('instruction stepped'));

### [async] step() => Promise

Asynchronously step into a source code statement.

Once source code statement has been stepped, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.step();
		console.log('source code statement stepped');
	}

### [async] next(callback : function)

Asynchronously step over a source code statement.

Once source code statement has been stepped over, the `callback` function is called.

Example of use:

	processor.nextInstruction(() => console.log('instruction stepped over'));

### [async] next() => Promise

Asynchronously step into a source code statement.

Once source code statement has been stepped over, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.step();
		console.log('source code statement stepped over');
	}

### [async] finish(callback : function)

Asynchronously finish current function.

Once current function has returned, the `callback` function is called.

Example of use:

	processor.finish(() => console.log('function has returned'));

### [async] finish() => Promise

Asynchronously finish current function.

Once current function has returned, the returned promise is resolved.

Example of use:

	while(true) {
		await processor.finish();
		console.log('function has returned');
	}

### returnFromFunction([ret_value: *])

Return from current function with an optional return value.

Example of use:

	processor.returnFromFunction(2);

## class Breakpoint

### [constructor] Breakpoint(processor: Processor, addr: Number) => Breakpoint

Create a breakpoint at address `addr` for processor `processor`.

Example of use:

	let brk1 = AddressHook(processor, 0x1234);
	let brk2 = AddressHook(processor, BigInt('0xffffffffffffffff'));

### enable()

Enable breakpoint.

Example of use:

	brk1.enable();
	brk2.enable();

### disable()

Disable breakpoint.

Example of use:

	brk1.disable();
	brk2.disable();

## class SourceCodeBreakpoint

### [constructor] SourceCodeBreakpoint(processor: Processor, file : string, loc : (Object|string)) => SourceCodeBreakpoint

Create a breakpoint at source code location `loc` executed from executable file `file` for processor `processor`.

Valid source code location are `{ source_code_filename: 'main.c', lineno: 1234 }` or '`main.c:1234`'.

Example of use:

	let brk1 = SourceCodeHook(processor, 'test.elf', 'main.c:1234');
	let brk2 = SourceCodeHook(processor, 'test.elf', { source_code_filename: 'main.c', lineno: 1234 });
	let brk3 = SourceCodeHook(processor, 'test.elf', SourceCodeLocation('main.c', 1234));

### enable()

Enable breakpoint.

Example of use:

	brk1.enable();
	brk2.enable();

### disable()

Disable breakpoint.

Example of use:

	brk1.disable();
	brk2.disable();

## class SubProgramBreakpoint

### [constructor] SubProgramBreakpoint(processor: Processor, file : string, name : string) => SubProgramBreakpoint

Create a breakpoint at subprogram `name` executed from executable file `file` for processor `processor`.

Example of use:

	let brk1 = SourceCodeHook(processor, 'test.elf', 'main');

### enable()

Enable breakpoint.

Example of use:

	brk1.enable();

### disable()

Disable breakpoint.

Example of use:

	brk1.disable();

## class SourceCodeLocation

### [constructor] SourceCodeLocation(loc: string) => SourceCodeLocation

Create a source code location pointing from a formated string such as '`main.c:1234`'.

### [constructor] SourceCodeLocation(source_code_filename: string, lineno: Number, [colno: Number]) => SourceCodeLocation

Create a source code location pointing to source code file `source_code_filename` at line number `lineno` and column number `colno`.

## class DataObject

### [constructor] DataObject(processor: Processor, expr : string) => DataObject

Create a data object handle tracking an expression `exp` from processor `processor` point of view.

Valid expressions are C-like expressions such as '`*s.p->a[2]`'.

### set(value : *, [options : Object])

Assign a value to a data object.
When `options.flatten` is true, the data object and the value are crawled recursively, following pointers and references, to write each properties.

### get([options : Object]) => *

Retrieve the data value.
The data object is crawled recursively to build the returned value.

`options.flatten` controls how to handle pointers:

* `options.flatten=true`: follow pointers and references, effectively flattening the data object representation into the returned value,
* `options.flatten=false`: do not follow pointers and references, effectively replacing each pointer in the data object with an instance of class Pointer into the returned value.

### isObject() => Bool

Test whether data object is either a structure, a union, a class, or an interface.

### isStructure() => Bool

Test whether data object is a structure.

### isUnion() => Bool

Test whether data object is a union.

### isClass() => Bool

Test whether data object is a class instance.

### isInterface() => Bool

Test whether data object is an interface.

### isBasic() => Bool

Test whether data object is either of type `char`, an integer, a boolean, or a floating-point number.

### isChar() => Bool

Test whether data object is of type `char`.

### isInteger() => Bool

Test whether data object is an integer.

### isBoolean() => Bool

Test whether data object is a boolean.

### isFloat() => Bool

Test whether data object is a floating-point number.

### isPointer() => Bool

Test whether data object is a pointer.

### isFunction() => Bool

Test whether data object is a function.

### isConst() => Bool

Test whether data object type has `const` modifier.

### isEnum() => Bool

Test whether data object type is an `enum`.

### isVoid() => Bool

Test whether data object type is `void`.

### isVolatile() => Bool

Test whether data object type has `volatile` modifier.

### isSigned() => Bool

Test whether data object type is signed.

## class Pointer

This class is not construtible.
It is created when calling `get` method (with `options.flatten` evaluated as `false`) of a DataObject instance, see `DataObject` class description for details.

### set(value : *)

Set value of pointer (i.e. address of pointed value).

### get() => *

Get value of pointer (i.e. address of pointed value).

### deref() => DataObject

Dereference a pointer, then return the pointed data object.

## class AddressHook

Class AddressHook provides support for triggering the execution of Javascript callbacks when program reaches a specific address.

### [constructor] AddressHook(processor: Processor, addr : Number) => AddressHook

Create an hook at address `addr` for processor `processor`.

Example of use:

	let hook1 = AddressHook(processor, 0x1234);
	let hook2 = AddressHook(processor, BigInt('0xffffffffffffffff'));

### [async] AddressHook.on(callback : function)

Install a callback which is called when address hook is reached.
The number of callbacks is unlimited.

Example of use:

	hook.on(() => console.log('reached hook'));

### [async] AddressHook.on() => Promise

Return a promise which is resolved when address hook is reached.
The number promises is unlimited.

Example of use:

	await hook.on();
	console.log('reached hook');

### AddressHook.removeListener(callback : function)

Remove a callback.

Example of use:

	let hook = AddressHook(processor, 0x1234);
	let f = () => {
		console.log('reached hook');
		hook.removeListener(f);
	}
	hook.on(f);

## class SourceCodeHook

Class SourceCodeHook provides support for triggering the execution of Javascript callbacks when reaching a source code statement.

### [constructor] SourceCodeHook(processor: Processor, file : string, loc : (Object|string)) => SourceCodeHook

Create a hook at source code location `loc` executed from executable file `file` for processor `processor`.

Valid source code location are `{ source_code_filename: 'main.c', lineno: 1234 }` or '`main.c:1234`'.

Example of use:

	let hook = SourceCodeHook(processor, 'test.elf', 'main.c:1234');

### [async] SourceCodeHook.on(callback : function)

Install a callback which is called when source code hook is reached.
The number of callbacks is unlimited.

Example of use:

	hook.on(() => console.log('reached hook'));

### [async] SourceCodeHook.on() => Promise

Return a promise which is resolved when source code hook is reached.
The number promises is unlimited.

Example of use:

	await hook.on();
	console.log('reached hook');

### SourceCodeHook.removeListener(callback : function)

Remove a callback.

Example of use:

	let hook = SourceCodeHook(processor, 'test.elf', 'main.c:1234');
	let f = () => {
		console.log('reached hook');
		hook.removeListener(f);
	}
	hook.on(f);

## class SubProgramHook

Class SubProgramHook provides support for triggering the execution of Javascript callbacks when entering into a subprogram.

### [constructor] SubProgramHook(processor: Processor, file : string, name : string) => SubProgramHook

Create a hook at subprogram `name` executed from executable file `file` for processor `processor`.

Example of use:

	let hook = SubProgramHook(processor, 'test.elf', 'main');

### [async] SubProgramHook.on(callback : function)

Install a callback which is called when subprogram hook is reached.
The number of callbacks is unlimited.

Example of use:

	hook.on(() => console.log('reached main'));

### [async] SubProgramHook.on() => Promise

Return a promise which is resolved when subprogram hook is reached.
The number promises is unlimited.

Example of use:

	await hook.on();
	console.log('reached hook');

### SubProgramHook.removeListener(callback : function)

Remove a callback.

Example of use:

	let hook = SubProgramHook(processor, 'test.elf', 'main');
	let f = () => {
		console.log('reached main');
		hook.removeListener(f);
	}
	hook.on(f);

## class Stub

Class Stub provides support to replace a subprogram with an implementation written in Javascript as a callback.

### [constructor] Stub(processor: Processor, file : string, name : string) => Stub

Create a stub for subprogram `name` executed from executable file `file` for processor `processor`.

Example of use:

	let stub = Stub(processor, 'test.elf', 'main');

### [async] Stub.on(callback : function)

Set (or overwrite a previous one) replacement callback for subprogram.
Only one callback can be set at a time.

Example of use:

	stub.on(() => {
		console.log('main will return with exit status -1');
		return -1;
	);

### Stub.removeListener(callback : function)

Remove a callback.

Example of use:

	let stub = Stub(processor, 'test.elf', 'main');
	let f = () => {
		console.log('main will return with exit status -1');
		return -1;
	}
	stub.on(f);
