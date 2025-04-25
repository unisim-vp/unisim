# Class AddressHook

Class AddressHook provides support for triggering the execution of Javascript callbacks when program reaches a particular address.

## `(constructor) AddressHook(processor: Processor, address: Number) => AddressHook`

Create an hook at address `address` for processor `processor`.

Example of use:

	let hook1 = new AddressHook(processor, 0x1234);
	let hook2 = new AddressHook(processor, BigInt('0xffffffffffffffff'));

## `(read-only) address: Number`

Address for which this hook applies.
