# Class Breakpoint

This class derives from `DebugEvent`.

## `(constructor) Breakpoint(processor: Processor, address: Number) => Breakpoint`

Create a breakpoint at address `address` for processor `processor`.

Example of use:

	let brk1 = new Breakpoint(processor, 0x1234);
	let brk2 = new Breakpoint(processor, BigInt('0xffffffffffffffff'));

## `(read-only) address: Number`

Address that this breakpoint monitors.
