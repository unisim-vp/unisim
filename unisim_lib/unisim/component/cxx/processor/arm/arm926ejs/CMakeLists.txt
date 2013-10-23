message ( STATUS "Checking unisim::component::cxx::processor::arm::arm926ejs" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES stdexcept cstring cassert string sstream iostream vector queue map )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES inttypes stdint stdlib string time )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( cxx_arm_subdirs
	unisim/kernel
	unisim/util/endian
	unisim/util/arithmetic
	unisim/util/debug
	unisim/util/random
	unisim/service/interfaces/memory
	unisim/service/interfaces/loader
	unisim/service/interfaces/linux_os
	unisim/service/interfaces/debug_control
	unisim/service/interfaces/disassembly
	unisim/service/interfaces/memory_access_reporting
	unisim/service/interfaces/symbol_table_lookup
	unisim/service/interfaces/memory_injection
	unisim/service/interfaces/registers
	unisim/service/interfaces/trap_reporting
	unisim/service/interfaces/cache_power_estimator
	unisim/service/interfaces/power_mode
	genisslib
	)

foreach ( dir ${cxx_arm_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

set ( arm32_isa_files
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/actions_dec.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/load_store_base.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/arm32.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/load_store_multiple.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/branch.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/load_store_v4.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/constructors_dec.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/load_store_v5e.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/coprocessor.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/misc_arithmetic.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/coprocessor_v5e.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/multiply.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/data_processing.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/multiply_v5e.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/data_processing_v5e.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/profiling.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/dependency.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/semaphore.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/exception.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/specialization.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/execution_latency.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/status_register_access.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/load_store.isa
	)

add_custom_command (
	OUTPUT
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.tcc
	COMMAND
		genisslib -o ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32 -w 32 -I ${CMAKE_CURRENT_SOURCE_DIR}/../isa/arm32 ${CMAKE_CURRENT_SOURCE_DIR}/../isa/arm32/arm32.isa
	DEPENDS
		${arm32_isa_files}
	COMMENT
		"Building ARM (ARM32) instruction set simulator"
	)

set ( thumb_isa_files
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/branch.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/exception.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa
	)

add_custom_command (
	OUTPUT
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.tcc
	COMMAND
		genisslib -o ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb -w 32 -I ${CMAKE_CURRENT_SOURCE_DIR}/../isa/thumb ${CMAKE_CURRENT_SOURCE_DIR}/../isa/thumb/thumb.isa
	DEPENDS
		${thumb_isa_files}
	COMMENT
		"Building ARM (THUMB) instruction set simulator"
	)

set ( cxx_arm_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/masks.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/coprocessor_interface.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/cpu.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/cpu.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/cpu.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/cpu.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/cache.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/cache.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/lockdown_tlb.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/lockdown_tlb.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/tlb.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/tlb.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/cp15.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/cp15.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/exception.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/memory_op.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/memory_op.hh
	
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/carry_overflow.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/carry_overflow.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_data_processing.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_data_processing.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_load_store.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_load_store.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_load_store_multiple.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_load_store_multiple.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_misc_load_store.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_misc_load_store.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_copro_load_store.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/decode_copro_load_store.cc
	
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/disasm.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/disasm.cc

	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.cc
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.tcc
	
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.cc
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.tcc
	)

set_source_files_properties (
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.tcc
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.tcc
	PROPERTIES
		GENERATED TRUE )

add_custom_target ( unisim__component__cxx__processor__arm__arm926ejs_generated_files DEPENDS
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_arm32.tcc
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/arm/arm926ejs/isa_thumb.tcc
	)

add_custom_target ( unisim__component__cxx__processor__arm__arm926ejs_arm32_isa
	DEPENDS
		${arm32_isa_files}
		${thumb_isa_files}
	)

set_source_files_properties (
	${arm32_isa_files}
	${thumb_isa_files}
	PROPERTIES
		LANGUAGE C	
		HEADER_FILE_ONLY TRUE
	)

set_source_files_properties (
	${cxx_arm_srcs}
	PROPERTIES
		COMPILE_FLAGS "-D__STDC_CONSTANT_MACROS"
	)

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	)

add_library ( unisim__component__cxx__processor__arm__arm926ejs
		${cxx_arm_srcs}
	)

add_dependencies ( unisim__component__cxx__processor__arm__arm926ejs
	unisim__component__cxx__processor__arm__arm926ejs_generated_files
	unisim__component__cxx__processor__arm__arm926ejs_arm32_isa
	unisim__component__cxx__processor__arm__arm926ejs_thumb_isa
	unisim__kernel
	unisim__util__endian
	unisim__util__arithmetic
	unisim__util__debug
	unisim__util__random
	unisim__service__interfaces__memory
	unisim__service__interfaces__loader
	unisim__service__interfaces__linux_os
	unisim__service__interfaces__debug_control
	unisim__service__interfaces__disassembly
	unisim__service__interfaces__memory_access_reporting
	unisim__service__interfaces__symbol_table_lookup
	unisim__service__interfaces__memory_injection
	unisim__service__interfaces__registers
	unisim__service__interfaces__trap_reporting
	unisim__service__interfaces__cache_power_estimator
	unisim__service__interfaces__power_mode
	genisslib
	)
	
list ( APPEND arm926ejs_link_libraries
	unisim__kernel
	unisim__util__endian
	unisim__util__debug
	unisim__util__random
	) 
target_link_libraries ( unisim__component__cxx__processor__arm__arm926ejs
	${arm926ejs_link_libraries}
	)
	
message ( STATUS "Checking unisim::component::cxx::processor::arm::arm926ejs - done" )


