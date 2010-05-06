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

# function ( unisim_set_unisim_libs TARGET )
# 	set ( LIBS ${ARGV} )
# 	list ( REMOVE_AT LIBS 0 )
# 	set_property ( GLOBAL PROPERTY ${CMAKE_PROJECT_NAME}_${TARGET}_unisim_libs ${LIBS} )
# endfunction ( unisim_set_unisim_libs TARGET LIBS )

# function ( unisim_get_unisim_libs RESULT_LIBS TARGET )
# 	get_property ( LIBS
# 		GLOBAL PROPERTY ${CMAKE_PROJECT_NAME}_${TARGET}_unisim_libs )
# 	set ( ${RESULT_LIBS} ${LIBS} PARENT_SCOPE )
# endfunction ( unisim_get_unisim_libs RESULT_LIBS TARGET )

# function ( unisim_set_output_libs TARGET )
# 	set ( LIBS ${ARGV} )
# 	list ( REMOVE_AT LIBS 0 )
# 	set_property ( GLOBAL PROPERTY ${CMAKE_PROJECT_NAME}_${TARGET}_output_libs ${LIBS} )
# endfunction ( unisim_set_output_libs TARGET LIBS )

# function ( unisim_get_output_libs RESULT_LIBS TARGET )
# 	message ( " ++++ unisim_get_output_libs ${TARGET}" )
# 	get_property ( LIBS
# 		GLOBAL PROPERTY ${CMAKE_PROJECT_NAME}_${TARGET}_output_libs )
# 	message ( "      LIBS = ${LIBS}" )
# 	get_property ( TARGETS
# 		GLOBAL PROPERTY ${CMAKE_PROJECT_NAME}_${TARGET}_unisim_libs )
# 	message ( "      TARGETS = ${TARGETS}" )
# 	foreach ( _TARGET ${TARGETS} )
# 		unisim_get_output_libs ( _LIBS ${_TARGET} )
# 		list ( APPEND LIBS ${_LIBS} )
# 		if ( LIBS )
# 			list ( REMOVE_DUPLICATES LIBS )
# 		endif ( LIBS )
# 	endforeach ( _TARGET ${TARGETS} )
# 	set ( ${RESULT_LIBS} ${LIBS} PARENT_SCOPE )
# endfunction ( unisim_get_output_libs RESULT_LIBS TARGET )

# function ( unisim_set_imported_libs TARGET )
# 	set ( LIBS ${ARGV} )
# 	list ( REMOVE_AT LIBS 0 )
# 	set_property ( GLOBAL PROPERTY ${CMAKE_PROJECT_NAME}_${TARGET}_imported_libs ${LIBS} )
# endfunction ( unisim_set_imported_libs TARGET LIBS )

# function ( unisim_get_imported_libs RESULT_LIBS TARGET )
# 	get_property ( LIBS
# 		GLOBAL PROPERTY ${CMAKE_PROJECT_NAME}_${TARGET}_imported_libs )
# 	set ( ${RESULT_LIBS} ${LIBS} PARENT_SCOPE )
# endfunction ( unisim_get_imported_libs LIBS TARGET )
