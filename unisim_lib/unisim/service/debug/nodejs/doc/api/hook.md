# Class Hook

Class Hook provides support for triggering the execution of Javascript callbacks when program reaches a particular location.

It is the superclass of AddressHook, SourceCodeHook, and SubProgramHook:

	                    Hook
	                     │
	      ┌──────────────┼─────────────────┐
	      │              │                 │
	 AddressHook  SourceCodeHook   SubProgramHook  
	

## `(read-only) processor: Processor`

The processor to which this hook is attached.

## `(async) Hook.on(callback: function)`

Install a callback which is called when hook is reached.
The number of callbacks is unlimited.

Example of use:

	hook.on(() => console.log('reached hook'));

## `(async) Hook.on() => Promise`

Return a promise which is resolved when hook is reached.
The number of promises is unlimited.

Example of use:

	await hook.on();
	console.log('reached hook');

## `Hook.removeListener(callback: function)`

Remove a callback.

Example of use:

	let hook = new AddressHook(processor, 0x1234);
	let f = () => {
		console.log('reached hook');
		hook.removeListener(f);
	}
	hook.on(f);
