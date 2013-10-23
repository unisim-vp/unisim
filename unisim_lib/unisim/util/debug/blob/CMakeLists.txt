message ( STATUS "Checking unisim::util::debug::blob" )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES stdlib string inttypes )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES string vector iostream )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ STL library \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( blob_subdirs 
	unisim/util/endian
	)

foreach ( dir ${blob_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )
add_library ( unisim__util__debug__blob
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/blob.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/blob.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/blob32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/blob64.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/section.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/section.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/section32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/section64.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/segment.hh
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/segment.tcc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/segment32.cc
		${CMAKE_SOURCE_DIR}/unisim/util/debug/blob/segment64.cc
	)

target_link_libraries ( unisim__util__debug__blob
	unisim__util__endian
	)

add_dependencies ( unisim__util__debug__blob
	unisim__util__endian
	)
	
message ( STATUS "Checking unisim::service::debug::blob - done" )
