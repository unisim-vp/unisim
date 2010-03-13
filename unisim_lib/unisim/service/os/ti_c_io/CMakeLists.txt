get_property ( UNISIM__SERVICE__OS__TI_C_IO_DEFINED GLOBAL PROPERTY UNISIM__SERVICE__OS__TI_C_IO SET )
if ( NOT UNISIM__SERVICE__OS__TI_C_IO_DEFINED )
	message ( STATUS "Checking unisim::service::os::ti_c_io" )
	set_property ( GLOBAL PROPERTY UNISIM__SERVICE__OS__TI_C_IO TRUE )

	include ( CheckIncludeFile )

	set ( C_INCLUDES unistd sys/types sys/stat fcntl errno )
	foreach ( C_INCLUDE ${C_INCLUDES} )
		check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
		if ( NOT HAVE_${C_INCLUDE}_H )
			message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
		endif ( NOT HAVE_${C_INCLUDE}_H )
	endforeach ( C_INCLUDE )

	set ( CXX_INCLUDES map fstream sstream )
	foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
		check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
		if ( NOT HAVE_${CXX_INCLUDE} )
			message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
		endif ( NOT HAVE_${CXX_INCLUDE} )
	endforeach ( CXX_INCLUDE )

	subdirs ( 
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/ti_c_io
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/memory
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/memory_injection
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/symbol_table_lookup
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/registers
		${CMAKE_SOURCE_DIR}/unisim/kernel/service
		${CMAKE_SOURCE_DIR}/unisim/kernel/logger
		)

	set_source_files_properties (
		${CMAKE_SOURCE_DIR}/unisim/service/os/ti_c_io/ti_c_io.hh
		${CMAKE_SOURCE_DIR}/unisim/service/os/ti_c_io/ti_c_io.tcc
		PROPERTIES
			HEADER_FILE_ONLY TRUE
		)
		
	include_directories ( ${CMAKE_SOURCE_DIR} )
	add_library ( unisim__service__os__ti_c_io
		STATIC 
			${CMAKE_SOURCE_DIR}/unisim/service/os/ti_c_io/ti_c_io.hh
			${CMAKE_SOURCE_DIR}/unisim/service/os/ti_c_io/ti_c_io.tcc
			${CMAKE_SOURCE_DIR}/unisim/service/os/ti_c_io/ti_c_io_32.cc
			${CMAKE_SOURCE_DIR}/unisim/service/os/ti_c_io/ti_c_io_64.cc
		)
	
	message ( STATUS "Checking unisim::service::os::ti_c_io - done" )
endif ( NOT UNISIM__SERVICE__OS__TI_C_IO_DEFINED )


