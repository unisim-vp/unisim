# Class Stub

Class Stub provides support to replace a subprogram with an implementation written in Javascript as a callback.

## `(constructor) Stub(processor: Processor, subprogram: SubProgram) => Stub`

Create a stub for subprogram `subprogram` for processor `processor`.

Example of use:

	let stub = new Stub(processor, findSubProgram('main', { file: 'test.elf' }));

## `(read-only) processor: Processor`

The processor to which this stub is attached.

## `(read-only) subprogram: SubProgram`

Subprogram to which this stub is attached.

## `(async) Stub.on(callback: function)`

Set (or overwrite a previous one) replacement callback for subprogram.
Only one callback can be set at a time.

Example of use:

	stub.on(() => {
		console.log('main will return with exit status -1');
		return -1;
	);

## `Stub.removeListener(callback: function)`

Remove a callback.

Example of use:

	let stub = new Stub(processor, 'test.elf', 'main');
	let f = () => {
		console.log('main will return with exit status -1');
		return -1;
	}
	stub.on(f);
