INCLUDES=-I$(top_srcdir)/../../..
lib_LIBRARIES = libunisim_service_logger.a
libunisim_service_logger_a_SOURCES = logger.cc logger_server.cc

includedir = $(prefix)/include/unisim/service/logger
include_HEADERS = logger.hh logger_server.hh logger_server_interface.hh

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../..
include $(top_srcdir)/../../m4/unisim.am
