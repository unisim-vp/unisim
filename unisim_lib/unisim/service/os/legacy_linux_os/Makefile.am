INCLUDES=-I$(top_srcdir)/../../../..
lib_LTLIBRARIES = libunisim_service_os_linux_os.la
libunisim_service_os_linux_os_la_SOURCES = \
	linux_os_32.cc linux_os_exception.cc \
	linux_os_32/linux_os_32.cc \
	linux_os_64/linux_os_64.cc

includedir = $(prefix)/include/unisim/service/os/linux_os
sharedir = $(prefix)/share/unisim_lib/unisim/service/os/linux_os
include_HEADERS = \
	linux_os.hh linux_os.tcc linux_os_exception.hh
nobase_include_HEADERS = \
	linux_os_32/linux_os_32.hh \
	linux_os_64/linux_os_64.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
