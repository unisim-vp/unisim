message ( STATUS "Checking unisim::service::pim::network" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes string unistd stdint sys/types errno stdlib stdio signal pthread fcntl )
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

set ( CXX_INCLUDES queue iostream vector sstream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( pim_server_network_subdirs 
	unisim/kernel/debug
	)
foreach ( dir ${pim_server_network_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__service__pim__network
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/BlockingQueue.cpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/BlockingQueue.hpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/BlockingQueue.tcc
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/GenericThread.cpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/GenericThread.hpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketThread.cpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketThread.hpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketReader.cpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketReader.hpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketWriter.cpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketWriter.hpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketClientThread.cpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketClientThread.hpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketServerThread.cpp
		${CMAKE_SOURCE_DIR}/unisim/service/pim/network/SocketServerThread.hpp
	)

set ( link_libraries
	unisim__kernel
	)
target_link_libraries ( unisim__service__pim__network
	${link_libraries}
	pthread
	)

message ( STATUS "Checking unisim::service::pim::network - done" )


