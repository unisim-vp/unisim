INCLUDES=-I$(top_srcdir)/../../../../.. -I$(top_builddir)/../../../../..
includedir = $(prefix)/include/unisim/component/cxx/processor/hcs12x
sharedir = $(prefix)/share/unisim_lib/unisim/component/cxx/processor/hcs12x

lib_LTLIBRARIES = libunisim_component_cxx_processor_hcs12x.la

libunisim_component_cxx_processor_hcs12x_la_SOURCES = cpu.cc mmc.cc ccr.cc exception.cc \
	$(top_srcdir)/hcs12x.cc $(top_srcdir)/xb.cc
	
include_HEADERS = cpu.hh mmc.hh ccr.hh exception.hh types.hh concatenated_register.hh config.hh \
	hcs12x.isa xb.isa load_store.isa transfer_exchange.isa move.isa addition_substraction.isa \
	decrement_increment.isa compare_test.isa boolean-logic.isa bit.isa multiplication_division.isa \
	branch.isa stacking.isa fuzzy-logic.isa others.isa
	
nodist_include_HEADERS = $(top_srcdir)/hcs12x.hh $(top_srcdir)/xb.hh $(top_srcdir)/xb_sub.isa
BUILT_SOURCES = $(top_srcdir)/hcs12x.hh $(top_srcdir)/hcs12x.cc $(top_srcdir)/xb.cc $(top_srcdir)/xb.hh $(top_srcdir)/xb_sub.isa
CLEANFILES = $(top_srcdir)/hcs12x.cc $(top_srcdir)/hcs12x.hh $(top_srcdir)/xb.cc $(top_srcdir)/xb.hh $(top_srcdir)/xb_sub.isa

$(top_srcdir)/xb.cc: $(top_srcdir)/xb.hh
$(top_srcdir)/hcs12x.cc: $(top_srcdir)/hcs12x.hh
$(top_srcdir)/hcs12x.hh: $(top_srcdir)/xb.hh

$(top_srcdir)/xb_sub.isa: $(top_srcdir)/xb.hh
$(top_srcdir)/xb.hh: $(top_srcdir)/xb.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o xb -w 32 xb.isa

$(top_srcdir)/hcs12x.hh: $(top_srcdir)/hcs12x.isa
	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o hcs12x -w 32 hcs12x.isa

# $(top_srcdir)/xb.hh $(top_srcdir)/hcs12x.hh: $(top_srcdir)/%.hh: $(top_srcdir)/%.isa
# 	cd $(top_srcdir); $(GENISSLIB_PATH) -I . -o $(basename $@) -w 32 $<

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..
include $(top_srcdir)/../../../../../m4/unisim.am
