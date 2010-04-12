message ( STATUS "Checking unisim::service::time::host_time" )

include ( CheckIncludeFile )
include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

if ( WIN32 )
	set ( C_INCLUDES windows )
else ( WIN32 )
	set ( C_INCLUDES sys/times )
endif ( WIN32 )

foreach ( C_INCLUDE ${C_INCLUDES} )
	check_include_file ( ${C_INCLUDE}.h HAVE_${C_INCLUDE}_H )
	if ( NOT HAVE_${C_INCLUDE}_H )
		message ( SEND_ERROR "Could C \"${C_INCLUDE}.h\"." )
	endif ( NOT HAVE_${C_INCLUDE}_H )
endforeach ( C_INCLUDE )

set ( host_time_subdirs
	unisim/service/interfaces/time
	unisim/kernel/service
	)

foreach ( dir ${host_time_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_library ( unisim__service__time__host_time
		${CMAKE_SOURCE_DIR}/unisim/service/time/host_time/time.hh
		${CMAKE_SOURCE_DIR}/unisim/service/time/host_time/time.cc
	)

message ( STATUS "Checking unisim::service::time::host_time - done" )


