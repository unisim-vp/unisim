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

get_property ( BIN_TO_SHARED_DATA_PATH_SET
	GLOBAL
	PROPERTY UNISIM_KERNEL_BIN_TO_SHARED_DATA_PATH
	SET
	)
get_property ( BIN_TO_SHARED_DATA_PATH
	GLOBAL
	PROPERTY UNISIM_KERNEL_BIN_TO_SHARED_DATA_PATH
	)
if ( NOT ${BIN_TO_SHARED_DATA_PATH_SET} )
	message ( FATAL_ERROR "Relative path to shared directory has not been set.")
endif ( NOT ${BIN_TO_SHARED_DATA_PATH_SET} )
configure_file (
	${CMAKE_SOURCE_DIR}/unisim/kernel/service/config.hh.in
	${CMAKE_BINARY_DIR}/unisim/kernel/service/config.hh
	)

include ( FindLibXml2 )
if ( NOT LIBXML2_FOUND )
	message ( SEND_ERROR "Could not find the libxml2 library." )
endif ( NOT LIBXML2_FOUND )

include ( FindBoost )
find_package ( Boost COMPONENTS graph )

unisim_include ( unisim/kernel/logger )

include_directories ( ${CMAKE_SOURCE_DIR} 
	${LIBXML2_INCLUDE_DIR} 
	${Boost_INCLUDE_DIRS} 
	${CMAKE_BINARY_DIR} )
add_library ( unisim__kernel__service
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/xml_helper.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/xml_helper.hh
		${CMAKE_BINARY_DIR}/unisim/kernel/service/config.hh
	)
set_source_files_properties ( ${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.cc
	PROPERTIES
		COMPILE_DEFINITIONS HAVE_UNISIM__KERNEL__SERVICE__CONFIG_HH
	)

target_link_libraries ( unisim__kernel__service
	${LIBXML2_LIBRARIES} 
	unisim__kernel__logger )
	
message ( STATUS "Checking unisim::kernel::service - done" )

