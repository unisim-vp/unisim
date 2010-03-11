# -*- coding: utf-8 -*-
from optparse import OptionParser
from puscomp.config import ConfigClass as ConfigClass
from puscomp.repository import RepositoryClass as RepositoryClass
from puscomp.simulator import SimulatorClass as SimulatorClass
import sys

# puscomp v0.1
print("puscomp (Python UniSim Compiler) v0.1")
print("Author: Daniel Gracia Pérez (daniel.gracia-perez@cea.fr)")

usage = """usage: %prog [options]
NOTE: options -c, -s, and -o must be defined"""
parser = OptionParser(usage = usage)
parser.add_option("-c", "--config", dest = "config_filename",
	default = None,
	help = "puscomp configuration file to use", metavar = "FILE")
parser.add_option("-s", "--simulator", dest = "simul_filename",
	default = None,
	help = "simulator description file to use", metavar = "FILE")
parser.add_option("-o", "--output", dest = "output_path",
	default = None,
	help = "location of the resulting output", metavar = "PATH")
parser.add_option("-v", "--verbose", dest = "verbose",
	action = "store_true",
	default = False,
	help = "verbose operation mode")

(options, args) = parser.parse_args()
if not options.config_filename:
	parser.print_help()
	parser.error("missing configuration file")
if not options.simul_filename:
	parser.print_help()
	parser.error("missing simulation description file")
if not options.output_path:
	parser.print_help()
	parser.error("missing output path")

# read configuration

print("Reading configuration file...", end = "")
config = ConfigClass(options.config_filename)
if config.getStatus():
	print("DONE")
else:
	print("Stopping puscomp.")
	sys.exit()
if options.verbose:
	print("============================")
	print("Configuration:")
	config.dump(" + ")
	print("============================")
print("Populating repository...", end = "")
rep = RepositoryClass(config)
if rep.getStatus():
	print("DONE")
else:
	print("Stopping puscomp.")
	sys.exit()
if options.verbose:
	print("============================")
	print("Repository:")
	rep.dump(" + ")
	print("============================")
print("Building simulator...", end = "")
simul = SimulatorClass(options.simul_filename, rep)
if simul.getStatus():
	print("DONE")
else:
	print("Stopping puscomp.")
	sys.exit()
if options.verbose:
	print("============================")
	print("Simulator:")
	simul.dump(" + ")
	print("============================")
print("Generating output...", end = "")
simul.generate(config, options.output_path)
if simul.getStatus():
	print("DONE")
else:
	print("Stopping puscomp.")
	sys.exit()

print("Bye.")

