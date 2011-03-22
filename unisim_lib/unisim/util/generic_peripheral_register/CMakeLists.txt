message ( STATUS "Checking unisim::util::generic_peripheral_register" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes stdlib )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string cstring sstream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

unisim_include ( 
	unisim/kernel
	)

add_library ( unisim__util__generic_peripheral_register
		${CMAKE_SOURCE_DIR}/unisim/util/generic_peripheral_register/generic_peripheral_register.hh
		${CMAKE_SOURCE_DIR}/unisim/util/generic_peripheral_register/generic_peripheral_register.cc
	)

target_link_libraries ( unisim__util__generic_peripheral_register
	unisim__kernel
	)

include_directories ( ${CMAKE_SOURCE_DIR} )

message ( STATUS "Checking unisim::util::generic_peripheral_register - done" )
