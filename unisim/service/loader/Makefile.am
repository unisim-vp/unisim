includedir = $(prefix)/include/unisim/service/loader
libdir= $(prefix)/lib/unisim/service/loader
SUBDIRS=elf_loader pmac_bootx pmac_linux_kernel_loader linux_loader arm_generic_kernel_loader
INCLUDE_SUBDIRS=elf_loader pmac_bootx pmac_linux_kernel_loader linux_loader arm_generic_kernel_loader

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

