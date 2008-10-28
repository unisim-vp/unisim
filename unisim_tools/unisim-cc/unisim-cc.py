#!/usr/bin/python

import sys

from xml.dom import minidom
from UserDict import UserDict
from optparse import OptionParser
from unisim_compiler import ConfigReader
import os.path
import shutil
import filecmp
import pdb

verbose = False

class Source:
	def __init__(self, id):
		self.id = id
		self.file = ""
		self.location = ""
		self.type = "source"

	def setFile(self, file):
		self.file = file

	def getFile(self):
		return self.file
	
	def setLocation(self, location):
		self.location = location

	def getLocation(self):
		return self.location

	def setTypeInclude(self):
		self.type = "include"
	
	def setTypeSource(self):
		self.type = "source"

	def getType(self):
		return self.type

	def isTypeSource(self):
		return self.type == "source"

	def isTypeInclude(self):
		return self.type == "include"

#############################3

def getText(nodelist):
	rc = ""
	for node in nodelist:
		if node.nodeType == node.TEXT_NODE:
			rc = rc + node.data
	return rc

def findFile(file):
	possibilities = g_config.extendPath(file)
	found = ""
	for i in possibilities:
		if not found:
			if os.path.isfile(i):
				found = os.path.abspath(i)
		else:
			if os.path.isfile(i) and found != os.path.abspath(i):
				print "Warning: %s found in two different locations (\n%s, \n%s)" % (file, found, os.path.abspath(i),)
	if not found:
		print "Error: %s not found" % (file,)
		sys.exit(-1)
	return found

def getStringFromTag(node, tag):
	list = []
	taglist = node.getElementsByTagName(tag)
	if len(taglist) == 0:
		return ""
	if len(taglist) != 1:
		print "Warning: tag \"%s\" found multiple times" % (tag,)
	return getText(taglist[0].childNodes);

def getExecName(node):
	found = ""
	found = getStringFromTag(node, "exec_name")
	if found == "":
		print "Could not find simulator executable name"
		sys.exit(-2)
	return found

def getAuthor(node):
	found = ""
	found = getStringFromTag(node, "author")
	if found == "":
		print "Could not find simulator/module author name"
		sys.exit(-2)
	return found

def getAuthorContact(node):
	found = ""
	found = getStringFromTag(node, "author_contact")
	if found == "":
		print "Could not find simulator/module author name"
		sys.exit(-2)
	return found

def getVersion(node):
	found = ""
	found = getStringFromTag(node, "version")
	if found == "":
		print "Could not find simulator/module version"
		sys.exit(-2)
	return found

def parseUnitSource(node):
	nodelist = node.getElementsByTagName("sources")
	filelist = []
	srclist  = []
	inclist  = []
	for node in nodelist:
		filelist += node.getElementsByTagName("file")
		srclist  += node.getElementsByTagName("src")
		inclist  += node.getElementsByTagName("inc")
	for file in filelist:
		source = Source(len(g_sources))
		source.setFile(getText(file.childNodes))
		source.setLocation(findFile(source.getFile()))
		g_sources.append(source)
	for src in srclist:
		source = Source(len(g_sources))
		source.setFile(getText(src.childNodes))
		source.setLocation(findFile(source.getFile()))
		g_sources.append(source)
	for inc in inclist:
		source = Source(len(g_sources))
		source.setFile(getText(inc.childNodes))
		source.setLocation(findFile(source.getFile()))
		g_sources.append(source)

def parseUnitExternals(node):
	nodelist = node.getElementsByTagName("externals")
	filelist = []
	for node in nodelist:
		filelist += node.getElementsByTagName("file")
	for file in filelist:
		external = findFile(getText(file.childNodes))
		if external not in g_externals:
			g_externals.append(external)

def parseUnitRequirements(node):
	nodelist = node.getElementsByTagName("requirements")
	reqlist = []
	for node in nodelist:
		reqlist += node.getElementsByTagName("requires")
	for req in reqlist:
		if getText(req.childNodes) not in g_requirements:
			g_requirements.append(getText(req.childNodes))

def parseUnit(node):
	parseUnitSource(node)
	parseUnitExternals(node)
	parseUnitRequirements(node)

def processExternal(file):
	f = open(file, 'r')
	if not f:
		print "Could not open ", file
		sys.exit(2)
	if g_verbose:
		print "Processing external %s" % (file,)
	dom = minidom.parse(f)

	reflist = dom.getElementsByTagName('module')
	unit = reflist[0]
	parseUnit(unit)
	dom.unlink()
	del dom

def processExternals():
	list = g_externals
	for external in list:
		if external not in g_externals_processed:
			processExternal(external)
			g_externals_processed.append(external)
	while len(g_externals) != len(g_externals_processed):
		processExternals()

def copySources():
	if g_verbose:
		print "Creating sources"
	for source in g_sources:
		dir = os.path.dirname(source.getFile())
		dir = "./source/" + dir
		if not os.path.isdir(dir):
			os.makedirs(dir)
		dst = "./source/" + source.getFile()
		copy = False
		if os.path.exists(dst):
			if not filecmp.cmp(source.getLocation(), dst):
				copy = True
		else:
			copy = True
		if copy:
			shutil.copyfile(source.getLocation(), dst)

def createConfigure(exec_name, version, author, author_contact):
	if g_verbose:
		print "Creating \"configure.ac\""
	f = open(g_configure_ac_filename, "w")
	s = "AC_INIT([%s],[%s],[%s(%s)],[%s])\n\n" % (exec_name, version, author, author_contact, exec_name,)
	f.write(s)
	s  = "AC_CONFIG_AUX_DIR(config)\n"
	s += "AC_CONFIG_HEADERS([config.h])\n"
	s += "AC_CANONICAL_BUILD\n"
	s += "AC_CANONICAL_HOST\n"
	s += "AC_CANONICAL_TARGET\n\n"
	# tar-pax option allows long directory+file names (more than 99 characters)
	# subdir-objects compiles each c/c++ file in the directory where it is found
	s += "AM_INIT_AUTOMAKE([tar-pax subdir-objects])\n\n"
	s += "AC_PATH_PROGS(SH, sh)\n"
	s += "AC_PROG_CXX\n"
	s += "AC_PROG_INSTALL\n"
	s += "AC_PROG_RANLIB\n"
	s += "AC_PROG_LN_S\n\n"
	s += "AC_LANG([C++])\n\n"
	f.write(s)

	# add source path for includes
	s = "CPPFLAGS=${CPPFLAGS}\" -Isource\"\n\n"
	f.write(s)

	# create the requirements
	s = ""
	for item in g_requirements:
		s += "%s\n" % (item,)
	s += "\n"
	f.write(s)

	s  = "AC_CONFIG_FILES([Makefile])\n\n"
	s += "AC_OUTPUT\n"
	f.write(s)
	f.close()

def createMakefile(exec_name):
	if g_verbose:
		print "Creating \"Makefile.am\""
	f = open(g_makefile_am_filename, "w")
	s  = "include $(top_srcdir)/m4/unisim.am\n"
	s += "bin_PROGRAMS = %s\n" % (exec_name,)
	s += "%s_SOURCES = " % (exec_name,)
	for source in g_sources:
		file = "source/" + source.getFile()
		s += "\\\n\t%s" % (file,)
	s += "\n"
	f.write(s)

def createM4():
	program_location = os.path.dirname(os.path.realpath(sys.argv[0]))
	if g_verbose:
		print "Creating M4"
	src = "%s/m4" % (program_location,)
	dest = "%s/m4" % (os.path.realpath("."),)
	if g_verbose:
		print " - source = %s" % (src,)
		print " - dest = %s" % (dest,)
	if os.path.isdir(dest):
		shutil.rmtree(dest)
	shutil.copytree(src, dest)

def buildConfigure():
	# we must create a config directory first
	if not os.path.isdir("./config/"):
		os.mkdir("./config/")
	print "launching aclocal"
	os.system('aclocal -I m4')
	print "launching autoconf"
	os.system('autoconf --force')
	print "launching autoheader"
	os.system('autoheader')
	print "launching automake"
	os.system('automake -ac')

def showNextStep():
	print "Everything is now ready."
	print "You just have to perform the typical configure and make to install the simulator."
	print "If you want to install the the simulator in your system use \"make install\"."

# main starts here

def main(argv):
	global g_verbose
	global g_config
	global g_externals
	global g_externals_processed
	global g_sources
	global g_requirements
	global g_configure_ac_filename
	global g_makefile_am_filename

	g_verbose = False
	g_externals = []
	g_externals_processed = []
	g_sources = []
	g_requirements = []

	usage = "usage: %prog [options] <simulator.md>"
	op = OptionParser(usage=usage, version="%prog 1.0")
	op.add_option("-c", "--config", dest="config_filename",
			help="configuration file")
	op.add_option("-a", "--configure-ac-filename", dest="configure_ac_filename",
		help="output configure.ac filename (default \"configure.ac\")")
	op.set_defaults(configure_ac_filename="configure.ac")
	op.add_option("-m", "--makefile-am_filename", dest="makefile_am_filename",
		help="output makefile.am filename (default \"Makefile.am\")")
	op.set_defaults(makefile_am_filename="Makefile.am")
	op.add_option("-v", "--verbose", action="store_true",
			help="verbose")
	(opts, args) = op.parse_args()
	if len(args) != 1:
		op.print_help()
		op.error("incorrect number of arguments" + str(args))
	if opts.config_filename:
		g_config = ConfigReader(opts.config_filename)
	else:
		g_config = ConfigReader()

	g_configure_ac_filename = opts.configure_ac_filename
	g_makefile_am_filename = opts.makefile_am_filename

	if opts.verbose:
		print "verbose operation"
		g_verbose = True
	if g_verbose:
		print "preparing builder for " + args[0]
		g_config.toString()

	f = open(args[0], 'r')
	if not f:
		print "Could not open ", args[0]
		sys.exit(2)
	dom = minidom.parse(f)

	reflist = dom.getElementsByTagName('simulator')
	simulator = reflist[0]

	exec_name = getExecName(simulator)
	author = getAuthor(simulator)
	author_contact = getAuthorContact(simulator)
	version = getVersion(simulator)

	if g_verbose:
		print "Generating simulator: %s" % (exec_name,)
		print "             version: %s" % (version,)
		print "              author: %s" % (author,)
		print "      author_contact: %s" % (author_contact,)

	parseUnit(simulator)
	processExternals()
	if g_verbose:
		print "sources found:"
		for source in g_sources:
			print "- %s (%s)" % (source.getFile(), source.getLocation(),)
		print "externals found:"
		for external in g_externals:
			print "- %s" % (external,)
		print "requirements found:"
		for req in g_requirements:
			print "- %s" % (req,)
	copySources()
	createConfigure(exec_name, version, author, author_contact)
	createMakefile(exec_name)
	createM4()
	buildConfigure()
	showNextStep()

if __name__ == "__main__":
	# pdb.run("main(sys.argv)")
	main(sys.argv)
