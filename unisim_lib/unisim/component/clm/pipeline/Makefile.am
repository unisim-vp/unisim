includedir = $(prefix)/include/unisim/component/clm/pipeline
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm/pipeline

#SUBDIRS=
#INCLUDE_SUBDIRS=

nobase_include_HEADERS = \
	commit/reorder_buffer.hh \
	commit/reorder_buffer_mc.hh \
	common/simple_arbiter.hh \
	common/simple_arbiter_mc.hh \
	common/data_bus_arbiter_mc.hh \
	common/simple_arbiter_onetoall_mc.hh \
	decode/allocator_renamer.hh \
	decode/allocator_renamer_mc.hh \
	decode/dispatcher.hh \
	decode/dispatcher_mc.hh \
	execute/address_generation_unit.hh \
	execute/address_generation_unit_mc.hh \
	execute/functional_unit.hh \
	execute/functional_unit_mc.hh \
	execute/load_store_queue.hh \
	execute/load_store_queue_nospec.hh \
	execute/load_store_queue_mc.hh \
	fetch/branch_history_table.hh \
	fetch/branch_target_buffer.hh \
	fetch/fetcher.hh \
	fetch/fetcher_perfect.hh \
	fetch/fetcher_mc.hh \
	fetch/return_address_stack.hh \
	issue/register_file.hh \
	issue/register_file_mc.hh \
	issue/scheduler.hh \
	issue/scheduler_mc.hh

dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..