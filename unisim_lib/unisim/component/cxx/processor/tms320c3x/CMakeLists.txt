get_property ( UNISIM__COMPONENT__CXX__PROCESSOR__TMS320_DEFINED GLOBAL PROPERTY UNISIM__COMPONENT__CXX__PROCESSOR__TMS320 SET )
if ( NOT ${UNISIM__COMPONENT__CXX__PROCESSOR__TMS320_DEFINED} )
	message ( STATUS "Checking unisim::component::cxx::processor::tms320" )

	set_property ( GLOBAL PROPERTY UNISIM__COMPONENT__CXX__PROCESSOR__TMS320 TRUE )
	include ( CheckIncludeFileCXX )
	include ( CheckIncludeFile )

	set ( CXX_INCLUDES stdexcept iosfwd string map vector cstring iomanip cassert sstream )
	foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
		check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
		if ( NOT HAVE_${CXX_INCLUDE} )
			message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
		endif ( NOT HAVE_${CXX_INCLUDE} )
	endforeach ( CXX_INCLUDE )

	set ( C_INCLUDES inttypes assert string )
	foreach ( C_INCLUDE ${C_INCLUDES} )
		check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
		if ( NOT HAVE_${C_INCLUDE}_H )
			message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
		endif ( NOT HAVE_${C_INCLUDE}_H )
	endforeach ( C_INCLUDE )

	set ( cxx_tms320_subdirs
		unisim/kernel/service
		unisim/kernel/logger
		unisim/service/interfaces/debug_yielding
		unisim/service/interfaces/disassembly
		unisim/service/interfaces/memory_access_reporting
		unisim/service/interfaces/memory
		unisim/service/interfaces/memory_injection
		unisim/service/interfaces/trap_reporting
		unisim/service/interfaces/registers
		unisim/service/interfaces/symbol_table_lookup
		unisim/service/interfaces/ti_c_io
		unisim/service/interfaces/loader
		unisim/util/debug
		unisim/util/arithmetic
		unisim/util/endian
		genisslib
		)
	
	foreach ( dir ${cxx_tms320_subdirs} )
		add_subdirectory ( 
			${CMAKE_SOURCE_DIR}/${dir}
			${CMAKE_BINARY_DIR}/${dir}
			)
	endforeach ( dir )

	# file ( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/unisim/component/cxx/processor/tms320 )
	
	add_custom_command (
		OUTPUT
			${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.hh
			${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.tcc
		COMMAND
			genisslib -o ${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320 -w 32 -I ${CMAKE_CURRENT_SOURCE_DIR}/isa ${CMAKE_CURRENT_SOURCE_DIR}/isa/tms320.isa
		DEPENDS
			${CMAKE_CURRENT_SOURCE_DIR}/isa/tms320.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/actions_dec.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/2op.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/3op.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/control.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/interlock.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/load_store.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/parallel.isa
			${CMAKE_CURRENT_SOURCE_DIR}/isa/power.isa
		COMMENT
			"Building TMS320 instruction set simulator"
		)
		
	set ( TMS320_SRCS
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/cpu.hh
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/cpu.tcc
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/cpu_tms320vc33.cc
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/cpu_tms320vc33_debug.cc
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/exception.hh
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/exception.tcc
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/register.hh
		${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/tms320/register.cc
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.hh
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.tcc
		)

	set_source_files_properties (
			${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.hh
			${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.tcc
		PROPERTIES
			GENERATED TRUE )

	add_custom_target ( unisim__component__cxx__processor__tms320_generated_files DEPENDS
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.hh
		${CMAKE_BINARY_DIR}/unisim/component/cxx/processor/tms320/isa_tms320.tcc
		)
	
	include_directories (
		${CMAKE_SOURCE_DIR}
		${CMAKE_BINARY_DIR}
		)

	add_library ( unisim__component__cxx__processor__tms320
		STATIC
			${TMS320_SRCS}
		)
	
	add_dependencies ( unisim__component__cxx__processor__tms320 
		unisim__component__cxx__processor__tms320_generated_files )
	
	message ( STATUS "Checking unisim::component::cxx::processor::tms320 - done" )
endif ( NOT ${UNISIM__COMPONENT__CXX__PROCESSOR__TMS320_DEFINED} )


