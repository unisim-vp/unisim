macro ( unisim_include MODULE_PATH )
	string ( REGEX REPLACE "/" "__" MODULE ${MODULE_PATH} )
	get_property ( ${MODULE}_DEFINED GLOBAL PROPERTY ${MODULE} SET )
	if ( NOT ${MODULE}_DEFINED )
		set_property ( GLOBAL PROPERTY ${MODULE} TRUE )
		add_subdirectory (
			${CMAKE_SOURCE_DIR}/${MODULE_PATH}
			${CMAKE_BINARY_DIR}/${MODULE_PATH}
			)
	else ( NOT ${MODULE}_DEFINED )
		message ( STATUS "Skipping ${MODULE}" )
	endif ( NOT ${MODULE}_DEFINED )
endmacro ( unisim_include MODULE_PATH )
