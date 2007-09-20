INCLUDES=-I$(top_srcdir)/../../../..
lib_LIBRARIES = libunisim_service_os_linux_os.a
libunisim_service_os_linux_os_a_SOURCES = linux_os_32.cc linux_os_exception.cc

includedir = $(prefix)/include/unisim/service/os/linux_os
include_HEADERS = linux_os.hh linux_os.tcc linux_os_exception.hh

uninstall-hook:
	(rmdir $(includedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
