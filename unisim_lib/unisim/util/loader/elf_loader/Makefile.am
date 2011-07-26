INCLUDES = -I$(top_srcdir)/../../../..
includedir = $(prefix)/include/unisim/service/loader/elf_loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader/elf_loader
include_HEADERS = elf_loader.hh elf32_loader.hh elf64_loader.hh elf_loader.tcc elf32.h elf64.h elf_common.h

lib_LTLIBRARIES = libunisim_service_loader_elf_loader.la
libunisim_service_loader_elf_loader_la_SOURCES = elf32_loader.cc elf64_loader.cc

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../..
include $(top_srcdir)/../../../../m4/unisim.am
	