message ( STATUS "Checking unisim::service::trap_handler" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES string sstream vector )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

# set ( C_INCLUDES inttypes stdint stdlib string time )
# foreach ( C_INCLUDE ${C_INCLUDES} )
# 	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
# 	if ( NOT HAVE_${C_INCLUDE}_H )
# 		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
# 	endif ( NOT HAVE_${C_INCLUDE}_H )
# endforeach ( C_INCLUDE )

set ( trap_handler_subdirs 
	unisim/service/interfaces/trap_reporting
	unisim/kernel
	)

foreach ( dir ${host_time_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_library ( unisim__service__trap_handler
		${CMAKE_SOURCE_DIR}/unisim/service/trap_handler/trap_handler.hh
		${CMAKE_SOURCE_DIR}/unisim/service/trap_handler/trap_handler.cc
		${CMAKE_SOURCE_DIR}/unisim/service/trap_handler/trap_handler_identifier.hh
		${CMAKE_SOURCE_DIR}/unisim/service/trap_handler/trap_handler_identifier.cc
		${CMAKE_SOURCE_DIR}/unisim/service/trap_handler/trap_handler_identifier_interface.hh
	)
	
target_link_libraries ( unisim__service__trap_handler
	unisim__kernel
	)

message ( STATUS "Checking unisim::service::trap_handler - done" )



