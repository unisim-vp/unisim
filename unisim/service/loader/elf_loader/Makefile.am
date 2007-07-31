INCLUDES = -I$(top_srcdir)/../../../..
libdir = $(prefix)/lib/unisim/service/loader/elf_loader
includedir = $(prefix)/include/unisim/service/loader/elf_loader
include_HEADERS = elf_loader.hh elf_loader.tcc

lib_LIBRARIES = libelf_loader.a
libelf_loader_a_SOURCES = elf32_loader.cc elf64_loader.cc
install-exec-hook: $(libdir)/libelf_loader.a
	rm -f $(prefix)/lib/libunisim_service_loader_elf_loader.a
	(cd $(prefix)/lib && $(LN_S) $< libunisim_service_loader_elf_loader.a)
uninstall-hook:
	rm -f $(prefix)/lib/libunisim_service_loader_elf_loader.a
	(rmdir $(libdir) || true)
	(rmdir $(includedir) || true)
