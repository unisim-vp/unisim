INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/processor/hcs12x
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/processor/hcs12x
lib_LIBRARIES = libunisim_component_cxx_processor_hcs12x.a
libunisim_component_cxx_processor_hcs12x_a_SOURCES = cpu.cc mmc.cc hcs12x.cc xb.cc
include_HEADERS = cpu.hh mmc.hh hcs12x.isa xb.isa \
	load_store.isa transfer_exchange.isa move.isa addition_substraction.isa decrement_increment.isa \
	compare_test.isa
	
nodist_include_HEADERS = hcs12x.hh xb.hh xb.sdh
BUILT_SOURCES = hcs12x.hh hcs12x.cc xb.cc xb.hh xb.sdh
CLEANFILES = hcs12x.cc hcs12x.hh xb.cc xb.hh xb.sdh

xb.cc: xb.hh
xb.sdh: xb.hh
xb.hh: $(srcdir)/xb.isa
	$(GENISSLIB_PATH) -I $(srcdir) -S XB -o xb -w 32 $(srcdir)/xb.isa

hcs12x.cc: hcs12x.hh
hcs12x.hh: $(srcdir)/hcs12x.isa $(srcdir)/load_store.isa $(srcdir)/xb.sdh
	$(GENISSLIB_PATH) -I $(srcdir) -o hcs12x -w 32 $(srcdir)/hcs12x.isa 


dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
