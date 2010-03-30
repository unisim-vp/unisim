message ( STATUS "Checking unisim::service::power" )

if ( with_cacti_4_2 )
	SET ( CACTI_4_2_INCLUDE_HINT ${with_cacti_4_2}/include )
	SET ( CACTI_4_2_LIB_HINT ${with_cacti_4_2}/lib )
else ( with_cacti_4_2 )
	SET ( CACTI_4_2_INCLUDE_HINT /usr/include )
	SET ( CACTI_4_2_LIB_HINT /usr/lib )
endif ( with_cacti_4_2 )

find_path ( CACTI_4_2_INCLUDE_DIR
		cacti4_2.hh
	HINTS
		${CACTI_4_2_INCLUDE_HINT}
	)
	
find_library ( CACTI_4_2_LIBRARY
	NAMES
		cacti4_2
	HINTS
		${CACTI_4_2_LIB_HINT}
	)

include ( FindPackageHandleStandardArgs )

find_package_handle_standard_args (
	cacti4_2 
	DEFAULT_MSG 
	CACTI_4_2_LIBRARY 
	CACTI_4_2_INCLUDE_DIR )
mark_as_advanced ( CACTI_4_2_LIBRARY CACTI_4_2_INCLUDE_DIR )
if ( NOT CACTI4_2_FOUND )
	if ( with_cacti_4_2 )
		message ( FATAL_ERROR
			"Could not find the cacti 4.2 library at the defined path"
			" (${with_cacti_4_2}). Please check that the cacti library was"
			" correctly installed.")
	else ( with_cacti_4_2 )
		message ( WARNING
			"Compiling the unisim::service::power service without cacti"
			" support. To provide full power/consumption support,"
			" install cacti 4.2 and indicate the"
			" path with '-Dwith_cacti_4_2=<path to the cacti library>'.")
		set ( UNISIM__SERVICE__POWER__SUPPORT FALSE PARENT_SCOPE) 
	endif ( with_cacti_4_2 )
else ( NOT CACTI4_2_FOUND )
	set ( UNISIM__SERVICE__POWER__SUPPORT TRUE PARENT_SCOPE)
endif ( NOT CACTI4_2_FOUND )

include ( CheckIncludeFileCXX )
include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( C_INCLUDES inttypes stdio math stdlib string )
foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could not find C header \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( CXX_INCLUDES iostream map )
foreach ( CXX_INCLUDE ${CXX_INCLUDES} )
	check_include_file_cxx ( ${CXX_INCLUDE} HAVE_${CXX_INCLUDE} )
	if ( NOT HAVE_${CXX_INCLUDE} )
		message ( SEND_ERROR "Could not find C++ header \"${CXX_INCLUDE}\"." )
	endif ( NOT HAVE_${CXX_INCLUDE} )
endforeach ( CXX_INCLUDE )

set ( power_subdirs 
	unisim/kernel/service
	unisim/kernel/logger
	unisim/service/interfaces/cache_power_estimator
	unisim/service/interfaces/power_mode
	unisim/service/interfaces/time
	)
foreach ( dir ${power_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )


if ( CACTI4_2_FOUND )
	set ( HAVE_CACTI4_2 TRUE )
	include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CACTI_4_2_INCLUDE_DIR})
else ( CACTI4_2_FOUND )
	set ( HAVE_CACTI4_2 FALSE )
	include_directories ( ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} )
endif ( CACTI4_2_FOUND )
configure_file ( 
	${CMAKE_SOURCE_DIR}/unisim/service/power/config.hh.in 
	${CMAKE_BINARY_DIR}/unisim/service/power/config.hh )
	
add_library ( unisim__service__power
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/service/power/cache_power_estimator.hh
		${CMAKE_SOURCE_DIR}/unisim/service/power/cache_power_estimator.cc
		${CMAKE_BINARY_DIR}/unisim/service/power/config.hh
	)

set ( unisim__service__power_libraries
	unisim__kernel__service
	unisim__kernel__logger
	)
if ( CACTI4_2_FOUND )
	list ( APPEND unisim__service__power_libraries
		${CACTI_4_2_LIBRARY}
		)
endif ( CACTI4_2_FOUND )
target_link_libraries ( unisim__service__power
	${unisim__service__power_libraries}
	)
if ( CACTI4_2_FOUND )
	set_target_properties ( unisim__service__power
		PROPERTIES
			COMPILE_DEFINITIONS "HAVE_CONFIG_HH"
		)
endif ( CACTI4_2_FOUND )

add_dependencies ( unisim__service__power
	unisim__kernel__service
	unisim__kernel__logger
	unisim__service__interfaces__cache_power_estimator
	unisim__service__interfaces__power_mode
	unisim__service__interfaces__time
	)
	
set ( UNISIM__SERVICE__POWER__SUPPORT true )

message ( STATUS "Checking unisim::service::power - done" )



