# Class Watchpoint

This class derives from `DebugEvent`.

## `(constructor) Watchpoint(processor: Processor, addr : Number, size: Number, [options: { [memoryType: string], [memoryAccessType: string], [overlook: boolean] }]) => Watchpoint`

Create a watchpoint monitoing `size` bytes starting at address `address` for processor `processor`.

Options:

* `memoryType`: one of '`insn`', '`data`' (default: '`data`')
* `memoryAccessType`: one of '`read`', '`write`', '`read-write`' (default: '`write`')
* `overlook`: true or false (default: true)

Example of use:

	let brk1 = new Watchpoint(processor, 0x1234, 4, { memoryType: 'data', memoryAccessType: 'write'} );
	let brk2 = new Watchpoint(processor, BigInt('0xffffffffffffffff'), 1, { memoryType: 'data', memoryAccessType: 'read'} );

## `(read-only) processor: Processor`

The processor to which this watchpoint is attached. 

## `(read-only) memoryAccessType: string`

Memory access type that this watchpoint monitors:

* '`read`': read access
* '`write`': write access
* '`read-write`': read or write access

## `(read-only) memoryType: string`

Memory type that this watchpoint monitors:

* '`insn`': instruction access
* '`data`': data access

## `(read-only) address: Number`

Starting address that this watchpoint monitors.

## `(read-only) size: Number`

Number of bytes that this watchpoint monitors.

## `(read-only) overlook: boolean`

Indicates whether watchpoint triggers before (`overlook=false`) or after (`overlook=true`) memory access occurs.
