message ( STATUS "Checking unisim::kernel" )
include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCITLM2.cmake )
include ( ${CMAKE_SOURCE_DIR}/cmake/FindOSCISystemC.cmake )

set ( C_INCLUDES string stdlib )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES stack iosfwd iostream string list queue map vector sstream fstream sstream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

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

if ( APPLE )
	add_library ( libxml2 SHARED IMPORTED )
	set_target_properties ( libxml2
		PROPERTIES
		IMPORTED_LOCATION
			/usr/lib/libxml2.dylib )
else ( APPLE )
	include ( FindLibXml2 )
	if ( NOT LIBXML2_FOUND )
		message ( SEND_ERROR "Could not find the libxml2 library." )
	endif ( NOT LIBXML2_FOUND )

	# include ( FindZLIB )
	# if ( NOT ZLIB_FOUND )
	#	message ( SEND_ERROR "Could not find the zlib library." )
	# endif ( NOT ZLIB_FOUND )
endif ( APPLE )

include ( FindBoost )
find_package ( Boost COMPONENTS graph )

include_directories ( ${CMAKE_SOURCE_DIR} 
	${LIBXML2_INCLUDE_DIR} 
	${Boost_INCLUDE_DIRS} 
	${CMAKE_BINARY_DIR}
	${OSCI_SYSTEMC_INCLUDE_DIR} )
add_library ( unisim__kernel
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/xml_helper.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/service/xml_helper.hh
		${CMAKE_BINARY_DIR}/unisim/kernel/service/config.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/logger/logger.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/logger/logger.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/logger/logger_server.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/logger/logger_server.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/debug/debug.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/debug/debug.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/tlm/tlm.hh
		${CMAKE_SOURCE_DIR}/unisim/kernel/tlm/tlm.cc
		${CMAKE_SOURCE_DIR}/unisim/kernel/tlm2/tlm.hh
	)
set_source_files_properties ( ${CMAKE_SOURCE_DIR}/unisim/kernel/service/service.cc
	PROPERTIES
		COMPILE_DEFINITIONS HAVE_UNISIM__KERNEL__SERVICE__CONFIG_HH
	)

set ( unisim__kernel_libs
	${OSCI_SYSTEMC_LIBRARIES} )
if ( APPLE )
	list ( APPEND unisim__kernel_libs 
		libxml2 )
	#target_link_libraries ( unisim__kernel LINK_INTERFACE_LIBRARIES
	#	libxml2 )
#	string ( REPLACE "/libxml2.dylib"
#	#	"/"
#	#	LIBXML2_LIBRARY_PATH
#	#	${LIBXML2_LIBRARIES} )
#	
#	add_library ( libxml2 STATIC IMPORTED )
#	add_library ( libz STATIC IMPORTED )
#	add_library ( libiconv STATIC IMPORTED )
#	
#	set_target_properties ( libxml2
#	#	PROPERTIES
#	#	#	IMPORTED_LOCATION ${LIBXML2_LIBRARY_PATH}/libxml2.a
#	#	)
#	set_target_properties ( libz
#	#	PROPERTIES
#	#	#	IMPORTED_LOCATION ${LIBXML2_LIBRARY_PATH}/libz.a
#	#	)
#	set_target_properties ( libiconv
#	#	PROPERTIES
#	#	#	IMPORTED_LOCATION ${LIBXML2_LIBRARY_PATH}/libiconv.a
#	#	)
#	
#	list ( APPEND unisim__kernel_link_libraries libxml2 libz libiconv ) 
	
#	target_link_libraries ( unisim__kernel
#		${OSCI_SYSTEMC_LIBRARIES}
#		${LIBXML2_LIBRARY_PATH}/libxml2.a
#		${LIBXML2_LIBRARY_PATH}/libz.a
#		${LIBXML2_LIBRARY_PATH}/libiconv.a
#		)
else ( APPLE )
	list ( APPEND unisim__kernel_libs
		${LIBXML2_LIBRARIES} )
endif ( APPLE )
target_link_libraries ( unisim__kernel
		 ${unisim__kernel_libs} )
		 
message ( STATUS "Checking unisim::kernel- done" )

