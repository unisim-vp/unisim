includedir = $(prefix)/include/unisim/service/loader
libdir= $(prefix)/lib/unisim/service/loader
SUBDIRS=elf_loader pmac_bootx
INCLUDE_SUBDIRS=elf_loader pmac_bootx

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

