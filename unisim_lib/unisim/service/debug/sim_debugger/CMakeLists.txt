message ( STATUS "Checking unisim::service::debug::sim_debugger" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes stdio stdlib )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string iostream list )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( sim_debugger_subdirs
	unisim/service/interfaces/memory
	unisim/service/interfaces/memory_access_reporting
	unisim/service/interfaces/debug_control
	unisim/service/interfaces/registers
	unisim/service/interfaces/trap_reporting
	unisim/service/interfaces/disassembly
	unisim/service/interfaces/symbol_table_lookup
	unisim/util/debug
	unisim/util/endian
	unisim/kernel
	)
foreach ( dir ${sim_debugger_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__debug__sim_debugger
		${CMAKE_SOURCE_DIR}/unisim/service/debug/sim_debugger/sim_debugger.hh
		${CMAKE_SOURCE_DIR}/unisim/service/debug/sim_debugger/sim_debugger.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/sim_debugger/sim_debugger.cc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/sim_debugger/sim_debugger_32.cc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/sim_debugger/sim_debugger_64.cc
	)

target_link_libraries ( unisim__service__debug__sim_debugger 
	unisim__util__debug 
	unisim__util__endian
	unisim__kernel
	)

message ( STATUS "Checking unisim::service::debug::sim_debugger - done" )



