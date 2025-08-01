# Class DebugEvent

This class provides support for debug event (see also `continueExecution()`).
These events are useful when Node.js service of simulator is configured for interactive (blocking) mode, a mode especially designed for a debugger REPL.

It is the superclass of Breakpoint, SourceCodeBreakpoint, SubProgramBreakpoint, and Watchpoint.

	                     DebugEvent
	                         │
	     ┌───────────────────┼────────────────────┬───────────────────┐
	     │                   │                    │                   │
	Breakpoint    SourceCodeBreakpoint   SubProgramBreakpoint    Watchpoint

## `(read-only) processor: Processor`

The processor to which this event is attached.

## `(read/write) enable: boolean`

Whether debug event is enabled.

Example of use:

	let brk1 = new Breakpoint(processor, 0x1234);
	brk1.enable = true;
	continueExecution();
	brk1.enable = false;
	continueExecution();
