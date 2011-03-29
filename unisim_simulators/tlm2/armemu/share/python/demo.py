#!/usr/bin/env python

from armemu060 import simulator

class SimulatorHandler(simulator.Simulator):
	def trap_handler(self, context, id):
		print("Python: Received a trap handler call with id = ", id)
		parms = self.get_parameters()
		print("        cpu.trap-on-instruction-counter = ",
				parms['cpu.trap-on-instruction-counter'].value)
		if parms['cpu.trap-on-instruction-counter'].value is 100:
			print("      Setting debugger to step mode")
			self.debugger.set_step_mode()
			print("      Setting trap-on-instruction-counter to 200")
			parms['cpu.trap-on-instruction-counter'].value = 200
			# print("      Calling stop")
			# self.stop()
		elif parms['cpu.trap-on-instruction-counter'].value is 200:
			print("      Setting debugger to continue mode")
			self.debugger.set_continue_mode()
			print("      Setting breakpoint at 'main'", end = "")
			if self.debugger.set_breakpoint("main"):
				print("  OK")
			else:
				print("  ERROR") 
			print("      Setting breakpoint at 'rec_fibonnaci_test'", end = "")
			if self.debugger.set_breakpoint("rec_fibonnaci_test"):
				print("  OK")
			else:
				print("  ERROR")
			print("      Setting trap-on-instruction-counter to 300")
			parms['cpu.trap-on-instruction-counter'].value = 300
		else:
			print("      Resetting cpu.trap-on-instruction-counter")
			parms['cpu.trap-on-instruction-counter'].value = 0

	def breakpoint_handler(self, context, addr):
		print("Python: 0x%08x (%s, %s, %s)" % 
				(addr, 
					addr, 
					self.debugger.is_mode_step(),
					self.debugger.has_breakpoint(addr)))
		if self.debugger.has_breakpoint(addr):
			print("      Removing breakpoint at 0x%08x" % addr)
			self.debugger.delete_breakpoint(addr)

	def __init__(self):
		simulator.Simulator.__init__(self, 
			parms={
				"enable-power-estimation":True, 
				"linux-os.verbose":True})
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
		self.debugger = self.get_debugger("main")
		# self.debugger.set_step_mode()
		self.debugger.set_context(None)
		self.debugger.set_breakpoint_callback(self.breakpoint_handler)

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
		for k, v in sorted(parms.items()):
			print("- %s = %s" % (k, v.str) )

	def dump_statistics(self):
		print("Dumping statistics:")
		stats = self.get_statistics()
		for k, v in stats.items():
			print("- %s = %s" % (k, v.str) )

sim = SimulatorHandler()
sim.setup()
sim.run()
sim.remove_trap_handler()
sim.dump_params()
sim.dump_statistics()

print("Bye!")
