includedir = $(prefix)/include/unisim/service/interfaces
include_HEADERS = \
	cache_power_estimator.hh \
	cpu_os.hh \
	disassembly.hh \
	linux_os.hh \
	logger.hh \
	memory.hh \
	pci_device.hh \
	registers.hh \
	symbol_table_build.hh \
	synchronizable.hh \
	video.hh \
	cpu_linux_os.hh \
	debug_control.hh \
	keyboard.hh \
	loader.hh \
	memory_access_reporting.hh \
	os.hh \
	power_mode.hh \
	resetable.hh \
	symbol_table_lookup.hh \
	time.hh

# INCLUDES=-I$(top_srcdir)/../../..
# libdir = $(prefix)/lib/unisim/service/interfaces
# lib_LIBRARIES = libinterfaces.a
# libinterfaces_a_SOURCES = logger.cc

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
