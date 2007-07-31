INCLUDES=-I$(top_srcdir)/../../..
libdir = $(prefix)/lib/unisim/service/logger
lib_LIBRARIES = liblogger.a
liblogger_a_SOURCES = logger.cc logger_server.cc

includedir = $(prefix)/include/unisim/service/logger
include_HEADERS = logger.hh logger_server.hh logger_server_interface.hh

install-exec-hook: $(libdir)/liblogger.a
	rm -f $(prefix)/lib/libunisim_service_logger.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_logger.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_logger.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
