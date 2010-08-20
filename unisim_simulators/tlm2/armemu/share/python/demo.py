#!/usr/bin/env python

from armemu042 import simulator

def trap_handler(context, id):
	print("Python: Received a trap handler call with id = ", id)
	parms = context.get_parameters()
	print("        cpu.trap-on-instruction-counter = ",
			parms['cpu.trap-on-instruction-counter'].value)
	if parms['cpu.trap-on-instruction-counter'].value is 100:
		print("      Setting it to 200")
		parms['cpu.trap-on-instruction-counter'].value = '200'
	elif parms['cpu.trap-on-instruction-counter'].value is 200:
		print("      Setting it to 300")
		parms['cpu.trap-on-instruction-counter'].value = '300'
	else:
		print("      Resetting cpu.trap-on-instruction-counter")
		parms['cpu.trap-on-instruction-counter'].value = '0'

print("Instantiating a new simulator.")
sim = simulator.Simulator()
parms = sim.get_parameters()

print("====> START: Testing boolean parameters")
verbose = parms['cpu.verbose-all']
print("cpu.verbose-all = ",
		parms['cpu.verbose-all'].value)
parms['cpu.verbose-all'].value = True
print("cpu.verbose-all = ",
		parms['cpu.verbose-all'].value)
parms['cpu.verbose-all'].value = False
print("cpu.verbose-all = ",
		parms['cpu.verbose-all'].value)
parms['cpu.verbose-all'].value = verbose
print("<==== END:   Testing boolean parameters")

print("====> START: Testing floating parameters")
ipc = parms['cpu.ipc'].value
print("cpu.ipc = ", ipc)
parms['cpu.ipc'] = ipc + 1.3235
print("cpu.ipc = ", parms['cpu.ipc'])
parms['cpu.ipc'] = ipc
print("cpu.ipc = ", parms['cpu.ipc'])
print("<==== END:   Testing floating parameters")

insn_threshold = 100
traps_to_logger = True
print("Setting cpu.trap-on-instruction-counter to ", insn_threshold)
parms['cpu.trap-on-instruction-counter'].value = insn_threshold
print("Setting trap-handler.send-traps-to-logger to ", traps_to_logger)
parms['trap-handler.send-traps-to-logger'].value = traps_to_logger
print("cpu.trap-on-instruction-counter = ",
		parms['cpu.trap-on-instruction-counter'].value)
print("Setting external trap handler")
sim.set_trap_handler(trap_handler)
print("Launching simulator setup.")
sim.setup()
print("Launching simulator.")
print("================================================================================")
sim.run()
print("================================================================================")
print("Simulation has been stopped")
vars = sim.get_variables()
print("Number of simulated instructions = ",
		vars['cpu.instruction-counter'].value)
sim.run()

print("Bye!")
