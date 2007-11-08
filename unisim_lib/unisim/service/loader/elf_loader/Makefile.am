INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/elf_loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/elf_loader
include_HEADERS = elf_loader.hh elf_loader.tcc elf32.h elf64.h elf_common.h

lib_LIBRARIES = libunisim_service_loader_elf_loader.a
libunisim_service_loader_elf_loader_a_SOURCES = elf32_loader.cc elf64_loader.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
	