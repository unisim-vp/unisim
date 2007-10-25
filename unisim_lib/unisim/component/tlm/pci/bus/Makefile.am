INCLUDES=-I$(top_srcdir)/../../../../..
includedir = $(prefix)/include/unisim/component/tlm/pci/bus
sharedir = $(prefix)/share/unisim_lib/unisim/component/tlm/pci/bus
include_HEADERS = bus.hh

share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
