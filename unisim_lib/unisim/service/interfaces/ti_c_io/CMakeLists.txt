get_property ( UNISIM__SERVICE__INTERFACES__TI_C_IO_DEFINED 
	GLOBAL 
	PROPERTY UNISIM__SERVICE__INTERFACES__TI_C_IO SET )
if ( NOT ${UNISIM__SERVICE__INTERFACES__TI_C_IO_DEFINED} )
	message ( STATUS "Checking unisim::service::interfaces::ti_c_io" )
	set_property ( GLOBAL PROPERTY UNISIM__SERVICE__INTERFACES__TI_C_IO TRUE )

	set_source_files_properties ( ${CMAKE_SOURCE_DIR}/unisim/service/interfaces/ti_c_io.hh
		PROPERTIES
			HEADER_FILE_ONLY TRUE )

	message ( STATUS "Checking unisim::service::interfaces::ti_c_io - done" )
endif ( NOT ${UNISIM__SERVICE__INTERFACES__TI_C_IO_DEFINED} )

