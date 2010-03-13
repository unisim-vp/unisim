message ( STATUS "Checking unisim::service::interfaces::linux_os" )

include ( CheckIncludeFileCXX )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/service/interfaces/os )
 
set ( CXX_INCLUDES string )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__linux_os
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/linux_os.hh
	)

add_dependencies ( unisim__service__interfaces__linux_os
	unisim__service__interfaces__os
	)

message ( STATUS "Checking unisim::service::interfaces::linux_os - done" )
