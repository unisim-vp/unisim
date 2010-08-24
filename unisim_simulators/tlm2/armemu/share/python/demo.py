#!/usr/bin/env python

from armemu042 import simulator

class SimulatorHandler(simulator.Simulator):
	def trap_handler(self, context, id):
		print("Python: Received a trap handler call with id = ", id)
		parms = self.get_parameters()
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

	def __init__(self):
		simulator.Simulator.__init__(self, parms={"enable-power-estimation":True})
		parms = self.get_parameters()
		self.insn_threshold = 100
		self.traps_to_logger = True
		print("cpu.trap-on-instruction-counter = ",
			parms['cpu.trap-on-instruction-counter'].value)
		print("trap-handler.send-traps-to-logger =",
				parms['trap-handler.send-traps-to-logger'].value)
		print("Setting cpu.trap-on-instruction-counter to ", 
				self.insn_threshold)
		parms['cpu.trap-on-instruction-counter'].value = \
				self.insn_threshold
		print("Setting trap-handler.send-traps-to-logger to ", 
				self.traps_to_logger)
		parms['trap-handler.send-traps-to-logger'].value = \
			self.traps_to_logger
		print("cpu.trap-on-instruction-counter = ",
			parms['cpu.trap-on-instruction-counter'].value)
		print("trap-handler.send-traps-to-logger =",
				parms['trap-handler.send-traps-to-logger'].value)
		print("Setting external trap handler")
		self.set_trap_handler(None, self.trap_handler)

	def setup(self):
		print("Launching simulator setup.")
		# we can not call self.setup() because it would cause a recursive
		#   call to the SimulatorHandler class. To avoid that we call the
		#   class directly (simulator.Simulator.setup()) giving it the 
		#   reference to itself.
		simulator.Simulator.setup(self)
		print("Launching simulator.")

	def dump_params(self):
		print("Dumping parameters:")
		parms = self.get_parameters()
		for k, v in parms.items():
			print("- ", k, " = ", v.value)

sim = SimulatorHandler()
sim.setup()
sim.run()
sim.remove_trap_handler()
sim.dump_params()

print("Bye!")
