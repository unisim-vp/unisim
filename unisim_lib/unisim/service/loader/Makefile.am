includedir = $(prefix)/include/unisim/service/loader
sharedir = $(prefix)/share/unisim_lib/unisim/service/loader
SUBDIRS=
INCLUDE_SUBDIRS=

if COND_elf_loader
SUBDIRS += elf_loader
INCLUDE_SUBDIRS += elf_loader
endif

if COND_s19_loader
SUBDIRS += s19_loader
INCLUDE_SUBDIRS += s19_loader
endif

if COND_coff_loader
SUBDIRS += coff_loader
INCLUDE_SUBDIRS += coff_loader
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

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

