includedir = $(prefix)/include/unisim/service/loader
libdir= $(prefix)/lib/unisim/service/loader
SUBDIRS=
INCLUDE_SUBDIRS=

if COND_elf_loader
SUBDIRS += elf_loader
INCLUDE_SUBDIRS += elf_loader
endif

if COND_pmac_bootx
SUBDIRS += pmac_bootx
INCLUDE_SUBDIRS += pmac_bootx
endif

if COND_pmac_linux_kernel_loader
SUBDIRS += pmac_linux_kernel_loader
INCLUDE_SUBDIRS += pmac_linux_kernel_loader
endif

if COND_linux_loader
SUBDIRS += linux_loader
INCLUDE_SUBDIRS += linux_loader
endif

if COND_arm_generic_kernel_loader
SUBDIRS += arm_generic_kernel_loader
INCLUDE_SUBDIRS += arm_generic_kernel_loader
endif

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)

