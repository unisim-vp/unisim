INCLUDES = -I$(top_srcdir)/../../..
includedir = $(prefix)/include/unisim/kernel/logger
sharedir = $(prefix)/share/unisim_lib/unisim/kernel/logger
include_HEADERS = logger.hh logger_server.hh

lib_LIBRARIES = libunisim_kernel_logger.a
libunisim_kernel_logger_a_SOURCES = logger.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
