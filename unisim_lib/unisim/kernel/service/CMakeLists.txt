message ( STATUS "Checking unisim::kernel::service" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( CXX_INCLUDES iostream string list queue map vector sstream fstream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( C_INCLUDES string stdlib )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

include ( FindLibXml2 )
if ( NOT LIBXML2_FOUND )
	message ( SEND_ERROR "Could not find the libxml2 library." )
endif ( NOT LIBXML2_FOUND )

include ( FindBoost )
find_package ( Boost COMPONENTS graph )

unisim_include ( unisim/kernel/logger )

include_directories ( ${CMAKE_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} )
add_library ( unisim__kernel__service
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/xml_helper.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/xml_helper.hh
	)

target_link_libraries ( unisim__kernel__service
	${LIBXML2_LIBRARIES} 
	unisim__kernel__logger )
	
message ( STATUS "Checking unisim::kernel::service - done" )

