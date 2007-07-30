includedir = $(prefix)/include/unisim/service/interfaces
include_HEADERS = \
    cache_power_estimator.hh \
    keyboard.hh \
    memory.hh \
    symbol_table_build.hh \
    cpu_linux_os.hh \
    linux_os.hh \
    os.hh \
    symbol_table_lookup.hh \
    cpu_os.hh \
    loader.hh \
    power_mode.hh \
    time.hh \
    debug_control.hh \
    logger.hh \
    registers.hh \
    video.hh \
    disassembly.hh \
    memory_access_reporting.hh \
    resetable.hh

# INCLUDES=-I$(top_srcdir)/../../..
# libdir = $(prefix)/lib/unisim/service/interfaces
# lib_LIBRARIES = libinterfaces.a
# libinterfaces_a_SOURCES = logger.cc

uninstall-hook:
	(rmdir $(includedir) || true)
