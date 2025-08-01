# Class RegisterValueChangedEvent

## `(constructor) RegisterValueChangedEvent(processor: Processor, reg_name : string) => RegisterValueChangedEvent`

Create a register value changed event for register named `reg_name` for processor `processor`.

Example of use:

	let ev = new RegisterValueChangedEvent(processor, "r12");
	ev.on(() => console.log("r12=" + processor.registers.r12.get()));

## `(read-only) registerName: string`

The name of register for which the event occurs.
