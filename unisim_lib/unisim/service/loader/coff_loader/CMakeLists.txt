get_property ( UNISIM__SERVICE__LOADER__COFF_LOADER_DEFINED GLOBAL PROPERTY UNISIM__SERVICE__LOADER__COFF_LOADER SET )
if ( NOT UNISIM__SERVICE__LOADER__COFF_LOADER_DEFINED )
	message ( STATUS "Checking unisim::service::loader::coff_loader" )
	set_property ( GLOBAL PROPERTY UNISIM__SERVICE__LOADER__COFF_LOADER TRUE )

	include ( CheckIncludeFileCXX )
	include ( CheckIncludeFile )

	set ( C_INCLUDES inttypes )
	foreach ( C_INCLUDE ${C_INCLUDES} )
		check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
		if ( NOT HAVE_${C_INCLUDE}_H )
			message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
		endif ( NOT HAVE_${C_INCLUDE}_H )
	endforeach ( C_INCLUDE )

	set ( CXX_INCLUDES iosfwd vector map )
	foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
		check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
		if ( NOT HAVE_${CXX_INCLUDE} )
			message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
		endif ( NOT HAVE_${CXX_INCLUDE} )
	endforeach ( CXX_INCLUDE )

	set ( coff_loader_subdirs
		unisim/service/interfaces/memory
		unisim/service/interfaces/loader
		unisim/service/interfaces/symbol_table_lookup
		unisim/service/interfaces/blob
		unisim/util/endian
		unisim/util/debug
		unisim/kernel/service
		unisim/kernel/logger
		)
	foreach ( dir ${coff_loader_subdirs} )
		add_subdirectory ( 
			${CMAKE_SOURCE_DIR}/${dir}
			${CMAKE_BINARY_DIR}/${dir}
			)
	endforeach ( dir )

	include_directories ( ${CMAKE_SOURCE_DIR} )
	add_library ( unisim__service__loader__coff_loader
		STATIC 
			${CMAKE_SOURCE_DIR}/unisim/service/loader/coff_loader/coff_loader.hh
			${CMAKE_SOURCE_DIR}/unisim/service/loader/coff_loader/coff_loader.tcc
			${CMAKE_SOURCE_DIR}/unisim/service/loader/coff_loader/coff_loader.cc
			${CMAKE_SOURCE_DIR}/unisim/service/loader/coff_loader/ti/ti.hh
			${CMAKE_SOURCE_DIR}/unisim/service/loader/coff_loader/ti/ti.tcc
		)
	
	message ( STATUS "Checking unisim::service::loader::coff_loader - done" )
endif ( NOT UNISIM__SERVICE__LOADER__COFF_LOADER_DEFINED )

