message ( STATUS "Checking unisim::service::pim" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes string unistd sys/types errno stdlib pthread fcntl )
if ( WIN32 )
	list ( APPEND C_INCLUDE winsock2 )
else ( WIN32 )
	list ( APPEND C_INCLUDE sys/socket netinet/in arpa/inet netinet/tcp netdb sys/times )
endif ( WIN32 )

foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string vector iostream list sstream typeinfo stdexcept fstream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

include ( FindLibXml2 )
if ( NOT LIBXML2_FOUND )
	message ( SEND_ERROR "Could not find the libxml2 library." )
endif ( NOT LIBXML2_FOUND )

set ( pim_server_subdirs 
	unisim/service/interfaces/memory
	unisim/service/interfaces/memory_access_reporting
	unisim/service/interfaces/debug_control
	unisim/service/interfaces/registers
	unisim/service/interfaces/trap_reporting
	unisim/util/endian
	unisim/util/debug
	unisim/kernel
	unisim/util/xml
	unisim/service/pim/network
	)
foreach ( dir ${pim_server_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} 
	${LIBXML2_INCLUDE_DIR} 
	)
	
add_library ( unisim__service__pim
		${CMAKE_SOURCE_DIR}/unisim/service/pim/convert.hh
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim_server_32.cc
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim_server_64.cc
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim_server.cc
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim_server.hh
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim_server.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim_thread.cc
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim_thread.hh
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim.cc
		${CMAKE_SOURCE_DIR}/unisim/service/pim/pim.hh
	)

set ( link_libraries
	unisim__util__endian
	unisim__util__xml
	unisim__util__debug
	unisim__kernel
	unisim__util__xml
	unisim__service__pim__network
	)
target_link_libraries ( unisim__service__pim
	${link_libraries}
	${LIBXML2_LIBRARIES}
	pthread
	)

message ( STATUS "Checking unisim::service::pim - done" )


