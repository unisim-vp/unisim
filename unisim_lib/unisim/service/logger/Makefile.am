INCLUDES=-I$(top_srcdir)/../../..
lib_LTLIBRARIES = libunisim_service_logger.la
libunisim_service_logger_la_SOURCES = logger.cc logger_server.cc

includedir = $(prefix)/include/unisim/service/logger
sharedir = $(prefix)/share/unisim_lib/unisim/service/logger
include_HEADERS = logger.hh logger_server.hh logger_server_interface.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
