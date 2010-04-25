message ( STATUS "Checking unisim::util::endian" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

if ( APPLE )
	set ( C_INCLUDES sys/types )
	foreach ( C_INCLUDE ${C_INCLUDES} )
		check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
		if ( NOT HAVE_${C_INCLUDE}_H )
			message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
		endif ( NOT HAVE_${C_INCLUDE}_H )
	endforeach ( C_INCLUDE )
elseif ( UNIX )
	set ( C_INCLUDES endian )
	foreach ( C_INCLUDE ${C_INCLUDES} )
		check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
		if ( NOT HAVE_${C_INCLUDE}_H )
			message ( SEND_ERROR "Could not find C standard library header \"${C_INCLUDE}.h\"." )
		endif ( NOT HAVE_${C_INCLUDE}_H )
	endforeach ( C_INCLUDE )
endif ( APPLE )

unisim_include ( unisim/kernel)

add_library ( unisim__util__endian
		${CMAKE_SOURCE_DIR}/unisim/util/endian/endian.hh
		${CMAKE_SOURCE_DIR}/unisim/util/endian/endian.cc
	)

target_link_libraries ( unisim__util__endian
	unisim__kernel
	)
	
include_directories ( ${CMAKE_SOURCE_DIR} )

set_source_files_properties ( ${CMAKE_SOURCE_DIR}/unisim/util/endian/endian.hh
	PROPERTIES
		HEADER_FILE_ONLY TRUE )

message ( STATUS "Checking unisim::util::endian - done" )


