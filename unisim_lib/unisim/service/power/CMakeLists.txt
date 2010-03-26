message ( STATUS "Checking unisim::service::power" )

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

include_directories ( ${CMAKE_SOURCE_DIR} )

add_library ( unisim__service__power
	STATIC 
		${CMAKE_SOURCE_DIR}/unisim/service/power/cache_power_estimator.hh
		${CMAKE_SOURCE_DIR}/unisim/service/power/cache_power_estimator.cc
	)

target_link_libraries ( unisim__service__power
	unisim__kernel__service
	unisim__kernel__logger
	)

add_dependencies ( unisim__service__power
	unisim__kernel__service
	unisim__kernel__logger
	unisim__service__interfaces__cache_power_estimator
	unisim__service__interfaces__power_mode
	unisim__service__interfaces__time
	)

message ( WARNING "TODO: unisim::service::power compilation needs to be completed to include cacti." )

message ( STATUS "Checking unisim::service::power - done" )




