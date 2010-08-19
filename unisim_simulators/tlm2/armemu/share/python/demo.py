#!/usr/bin/env python

from armemu042 import simulator

print("Instantiating a new simulator.")
sim = simulator.Simulator()
parms = sim.get_parameters()
insn_threshold = '100'
traps_to_logger = 'true'
print("Setting cpu.trap-on-instruction-counter to %s", 100)
parms['cpu.trap-on-instruction-counter'].value = insn_threshold
print("Setting trap-handler.send-traps-to-logger to %s", traps_to_logger)
parms['trap-handler.send-traps-to-logger'].value = traps_to_logger
print("Launching simulator setup.")
sim.setup()
print("Launching simulator.")
print("================================================================================")
sim.run()
print("================================================================================")

print("Bye!")
