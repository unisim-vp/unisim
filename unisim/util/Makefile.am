includedir = $(prefix)/include/unisim/util
libdir= $(prefix)/lib/unisim/util
SUBDIRS=xml arithmetic endian garbage_collector hash_table debug simfloat
INCLUDE_SUBDIRS=xml arithmetic endian garbage_collector hash_table debug simfloat

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(libdir) || true)
