INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/util/queue
sharedir = $(prefix)/share/unisim_lib/unisim/util/queue
include_HEADERS = queue.hh queue.tcc

lib_LIBRARIES = libunisim_util_queue.a
libunisim_util_queue_a_SOURCES = queue.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
