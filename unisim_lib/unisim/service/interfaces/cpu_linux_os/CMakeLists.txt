message ( STATUS "Checking unisim::service::interfaces::cpu_linux_os" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/service/interfaces/cpu_os )
 
include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__cpu_linux_os
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/cpu_linux_os.hh
	)

add_dependencies ( unisim__service__interfaces__cpu_linux_os
	unisim__service__interfaces__cpu_os
	)

message ( STATUS "Checking unisim::service::interfaces::cpu_linux_os - done" )
