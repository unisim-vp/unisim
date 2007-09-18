includedir = $(prefix)/include/unisim/util
SUBDIRS=
INCLUDE_SUBDIRS=

if COND_xml
SUBDIRS += xml
INCLUDE_SUBDIRS += xml
endif

if COND_arithmetic
SUBDIRS += arithmetic
INCLUDE_SUBDIRS += arithmetic
endif

if COND_endian
SUBDIRS += endian
INCLUDE_SUBDIRS += endian
endif

if COND_garbage_collector
SUBDIRS += garbage_collector
INCLUDE_SUBDIRS += garbage_collector
endif

if COND_hash_table
SUBDIRS += hash_table
INCLUDE_SUBDIRS += hash_table
endif

if COND_debug
SUBDIRS += debug
INCLUDE_SUBDIRS += debug
endif

if COND_simfloat
SUBDIRS += simfloat
INCLUDE_SUBDIRS += simfloat
endif

if COND_device
SUBDIRS += device
INCLUDE_SUBDIRS += device
endif

uninstall-hook:
	(rmdir $(includedir) || true)
