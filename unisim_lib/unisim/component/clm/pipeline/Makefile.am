includedir = $(prefix)/include/unisim/component/clm/pipeline
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/pipeline

#SUBDIRS=
#INCLUDE_SUBDIRS=

nobase_include_HEADERS = \
	commit/reorder_buffer.hh \
	common/simple_arbiter.hh \
	decode/allocator_renamer.hh \
	decode/dispatcher.hh \
	execute/address_generation_unit.hh \
	execute/functional_unit.hh \
	execute/load_store_queue.hh \
	fetch/branch_history_table.hh \
	fetch/branch_target_buffer.hh \
	fetch/fetcher.hh \
	fetch/return_address_stack.hh \
	issue/register_file.hh \
	issue/scheduler.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..