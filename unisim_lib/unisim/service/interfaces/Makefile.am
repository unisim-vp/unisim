includedir = $(prefix)/include/unisim/service/interfaces
sharedir = $(prefix)/share/unisim_lib/unisim/service/interfaces
include_HEADERS = \
	cache_power_estimator.hh \
	cpu_os.hh \
	disassembly.hh \
	linux_os.hh \
	logger.hh \
	memory.hh \
	memory_injection.hh \
	registers.hh \
	symbol_table_build.hh \
	synchronizable.hh \
	video.hh \
	cpu_linux_os.hh \
	debug_control.hh \
	keyboard.hh \
	loader.hh \
	memory_access_reporting.hh \
	os.hh \
	power_mode.hh \
	resetable.hh \
	symbol_table_lookup.hh \
	time.hh \
	statistic_reporting.hh \
	trap_reporting.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../..
include $(top_srcdir)/../../../m4/unisim.am
