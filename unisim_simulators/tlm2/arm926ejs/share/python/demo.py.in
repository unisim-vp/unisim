#!/usr/bin/env python

from arm926ejs040 import simulator

class SimulatorHandler(simulator.Simulator):
	def trap_handler(self, context, id):
		print("Python: Received a trap handler call with id = ", id)
		parms = self.get_parameters()
		stats = self.get_statistics()
		instruction_counter = stats['cpu.instruction-counter'].value
		print("cpu.instruction-counter = ", instruction_counter)
		if instruction_counter >= self.max_insns:
			print("Stopping the simulation")
			self.stop()
		else:
			parms['cpu.trap-on-instruction-counter'].value = (instruction_counter - (instruction_counter % self.insn_threshold)) + self.insn_threshold

	def __init__(self):
		simulator.Simulator.__init__(self, parms={"enable-power-estimation":True})
		parms = self.get_parameters()
		self.max_insns = 1000000
		self.insn_threshold = 10000
		self.traps_to_logger = True
		parms['cpu.trap-on-instruction-counter'].value = self.insn_threshold
		parms['trap-handler.send-traps-to-logger'].value = self.traps_to_logger
		self.set_trap_handler(None, self.trap_handler)

	def setup(self):
		# we can not call self.setup() because it would cause a recursive
		#   call to the SimulatorHandler class. To avoid that we call the
		#   class directly (simulator.Simulator.setup()) giving it the 
		#   reference to itself.
		simulator.Simulator.setup(self)

	def dump_params(self):
		print("Dumping parameters:")
		parms = self.get_parameters()
		for k, v in parms.items():
			print("- %s = %s" % (k, v.str) )

	def dump_statistics(self):
		print("Dumping statistics:")
		stats = self.get_statistics()
		for k, v in stats.items():
			print("- %s = %s" % (k, v.str) )

print("Creating simulator")
sim = SimulatorHandler()
print("Launching simulator setup")
sim.setup()
print("Launching simulation")
sim.run()
print("Simulation finished")
sim.remove_trap_handler()
# sim.dump_params()
# sim.dump_statistics()

print("Bye!")
