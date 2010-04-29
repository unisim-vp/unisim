message ( STATUS "Checking unisim::service::debug::gdb_server" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes string unistd sys/types errno stdlib )
if ( WIN32 )
	list ( APPEND C_INCLUDE winsock2 )
else ( WIN32 )
	list ( APPEND C_INCLUDE sys/socket netinet/in arpa/inet netinet/tcp netdb sys/times fcntl )
endif ( WIN32 )

foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string vector iostream list )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( gdb_server_subdirs 
	unisim/service/interfaces/memory
	unisim/service/interfaces/memory_access_reporting
	unisim/service/interfaces/debug_control
	unisim/service/interfaces/registers
	unisim/service/interfaces/trap_reporting
	unisim/util/endian
	unisim/util/debug
	unisim/kernel
	unisim/util/xml
	)
foreach ( dir ${gdb_server_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__debug__gdb_server
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_server.hh
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_server.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_server.cc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_server_32.cc
		${CMAKE_SOURCE_DIR}/unisim/service/debug/gdb_server/gdb_server_64.cc
	)

set ( link_libraries
	unisim__util__endian
	unisim__util__xml
	unisim__util__debug
	unisim__kernel
	unisim__util__xml 
	)
target_link_libraries ( unisim__service__debug__gdb_server
	${link_libraries}
	)

message ( STATUS "Checking unisim::service::debug::gdb_server - done" )


