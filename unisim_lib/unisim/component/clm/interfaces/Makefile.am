includedir = $(prefix)/include/unisim/component/clm
sharedir = $(prefix)/share/unisim_lib/unisim/component/clm

#SUBDIRS=
#INCLUDE_SUBDIRS=

nobase_include_HEADERS = \
	cache/cache_coherency_protocol.hh \
	cache/cache_common.hh \
	cache/cache_container_counter.hh \
	cache/cache_container.hh \
	cache/cache_wb.hh \
	cache/mesi.h \
	cache/MSHR_container.hh \
	cache/MSHR_container_prefetcher.hh \
	cache/msi.h \
\
	debug/log.hh \
	debug/stop_at_cycle.sim \
	debug/svg_memreq.h \
	debug/svg_memreq_interface.hh \
	debug/svg_memreq_service.h \
	debug/svg_memreq.sim \
\
	fsb/bus.hh \
\
	interfaces/instruction_interface.hh \
	interfaces/memreq.h \
	interfaces/memreq_interface.h \
\
	memory/dram/dram_components.hh \
	memory/dram/dram.hh \
	memory/mem_common.h \
	memory/memory_container.hh \
\
	pipeline/commit/reorder_buffer.hh \
	pipeline/common/simple_arbiter.hh \
	pipeline/decode/allocator_renamer.hh \
	pipeline/decode/dispatcher.hh \
	pipeline/execute/address_generation_unit.hh \
	pipeline/execute/functional_unit.hh \
	pipeline/execute/load_store_queue.hh \
	pipeline/fetch/branch_history_table.hh \
	pipeline/fetch/branch_target_buffer.hh \
	pipeline/fetch/fetcher.hh \
	pipeline/fetch/return_address_stack.hh \
	pipeline/issue/register_file.hh \
	pipeline/issue/scheduler.hh \
\
	processor/ooosim/config.hh \
	processor/ooosim/cpu_emulator.hh \
	processor/ooosim/cpu_simulator.hh \
	processor/ooosim/iss_interface.hh \
	processor/ooosim/parameters.hh \
\
	utility/common.h \
	utility/error.h \
	utility/types.hh \
	utility/utility.h


dist_share_DATA = COPYING

uninstall-hook:
	(rmdir $(includedir) || true)
	(rmdir $(sharedir) || true)

ROOT_DIR = ../../../../..