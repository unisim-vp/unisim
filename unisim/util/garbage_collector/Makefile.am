INCLUDES = -I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/util/garbage_collector
includedir = $(prefix)/include/unisim/util/garbage_collector
include_HEADERS = garbage_collector.hh

lib_LIBRARIES = libgarbage_collector.a
libgarbage_collector_a_SOURCES = garbage_collector.cc

install-exec-hook: $(libdir)/libgarbage_collector.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_util_garbage_collector.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_util_garbage_collector.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
