message ( STATUS "Checking unisim::service::interfaces::cpu_os" )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__cpu_os
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/cpu_os.hh
	)

message ( STATUS "Checking unisim::service::interfaces::cpu_os - done" )



