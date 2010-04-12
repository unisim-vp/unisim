message ( STATUS "Checking unisim::util::debug")

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES ostream list string iosfwd sstream iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES inttypes string sys/types unistd errno )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )
	
if ( WIN32 )
	check_include_file ( winsock2.h HAVE_WINSOCK2_H )
	if ( NOT HAVE_WINSOCK2_H )
		message ( SEND_ERROR "Could not find win32 C library header \"winsock2.h\"." )
	endif ( NOT HAVE_WINSOCK2_H )
else ( WIN32 )
	set ( C_INCLUDES sys/socket netinet/in sys/un arpa/inet netinet/tcp netdb sys/times fcntl )
	foreach ( C_INCLUDE ${C_INCLUDES} )
		check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
		if ( NOT HAVE_${C_INCLUDE}_H )
			message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
		endif ( NOT HAVE_${C_INCLUDE}_H )
	endforeach ( C_INCLUDE )
endif ( WIN32 )

set ( debug_subdirs
		unisim/service/interfaces/memory_access_reporting
	)

foreach ( dir ${debug_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

set ( debug_files
		${CMAKE_SOURCE_DIR}/unisim/util/debug/breakpoint.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/breakpoint_registry.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/netstub.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/profile.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/register.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/simple_register.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol_table.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/watchpoint.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/watchpoint_registry.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/breakpoint_registry.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/netstub.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/profile.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol_table.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/watchpoint_registry.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/breakpoint_registry_32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/breakpoint_registry_64.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/netstub.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/profile_32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/profile_64.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol_32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol_64.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol_table_32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/symbol_table_64.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/watchpoint_registry_32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/watchpoint_registry_64.cc
		)

include_directories (
	${CMAKE_SOURCE_DIR}
	)

add_library ( unisim__util__debug
		${debug_files}
	)

message ( STATUS "Checking unisim::util::debug - done")

