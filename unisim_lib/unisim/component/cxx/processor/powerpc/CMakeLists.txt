message ( STATUS "Checking unisim::component::cxx::processor::powerpc" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES map iosfwd sstream stdexcept string vector cstring cstdio list cassert iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES inttypes stdarg stdlib stdio unistd )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( cxx_powerpc_subdirs
	unisim/kernel/service
	unisim/kernel/logger
	unisim/component/cxx/cache
	unisim/component/cxx/tlb
	unisim/service/interfaces/memory
	unisim/service/interfaces/memory_injection
	unisim/service/interfaces/debug_yielding
	unisim/service/interfaces/memory_access_reporting
	unisim/service/interfaces/disassembly
	unisim/service/interfaces/loader
	unisim/service/interfaces/linux_os
	unisim/service/interfaces/symbol_table_lookup
	unisim/service/interfaces/cache_power_estimator
	unisim/service/interfaces/power_mode
	unisim/service/interfaces/synchronizable
	unisim/service/interfaces/trap_reporting
	unisim/service/interfaces/registers
	unisim/util/debug
	unisim/util/endian
	unisim/util/arithmetic
	unisim/util/queue
	unisim/util/simfloat
	genisslib
	)

foreach ( dir ${cxx_powerpc_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

set ( powerpc_isa_files
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/altivec.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/branch.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/condition.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/floating.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/integer.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/loadstore.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/misc.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/perf.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/ppc.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/sim_branch.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/sim_dependencies.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/sim_latencies.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/sim_load.isa
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/sim_ppc.isa
	)

add_custom_command (
	OUTPUT
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.hh
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.tcc
	COMMAND
		genisslib -o ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc -w 32 -I ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ppc.isa
	DEPENDS
		${powerpc_isa_files}
	COMMENT
		"Building PowerPC instruction set simulator"
	)

set ( cxx_powerpc_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/config.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/config.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/cpu.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/cpu.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/cpu.tcc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/exception.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/exception.hh
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/exception.tcc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/floating.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.tcc
	)

set_source_files_properties (
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.hh
	${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.tcc
	PROPERTIES
		GENERATED TRUE )

add_custom_target ( unisim__component__cxx__processor__powerpc_generated_files 
	DEPENDS
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.hh
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/powerpc/powerpc.tcc
	)

add_custom_target ( unisim__component__cxx__processor__powerpc_isa
	DEPENDS
		${powerpc_isa_files}
	)

set_source_files_properties (
	${powerpc_isa_files}
	PROPERTIES
		LANGUAGE C	
		HEADER_FILE_ONLY TRUE
	)

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	)

add_library ( unisim__component__cxx__processor__powerpc
	STATIC
		${cxx_powerpc_srcs}
	)

add_dependencies ( unisim__component__cxx__processor__powerpc
	unisim__component__cxx__processor__powerpc_generated_files
	unisim__component__cxx__processor__powerpc_isa
	unisim__kernel__service
	unisim__kernel__logger
	unisim__component__cxx__cache
	unisim__component__cxx__tlb
	unisim__service__interfaces__memory
	unisim__service__interfaces__memory_injection
	unisim__service__interfaces__debug_yielding
	unisim__service__interfaces__memory_access_reporting
	unisim__service__interfaces__disassembly
	unisim__service__interfaces__loader
	unisim__service__interfaces__linux_os
	unisim__service__interfaces__symbol_table_lookup
	unisim__service__interfaces__cache_power_estimator
	unisim__service__interfaces__power_mode
	unisim__service__interfaces__synchronizable
	unisim__service__interfaces__trap_reporting
	unisim__service__interfaces__registers
	unisim__util__debug
	unisim__util__endian
	unisim__util__arithmetic
	unisim__util__queue
	unisim__util__simfloat
	unisim_tools__genisslib
	)

target_link_libraries ( unisim__component__cxx__processor__powerpc
	unisim__kernel__service
	unisim__kernel__logger
	unisim__util__debug
	unisim__util__endian
	unisim__util__queue
	)

message ( STATUS "Checking unisim::component::cxx::processor::powerpc - done" )


