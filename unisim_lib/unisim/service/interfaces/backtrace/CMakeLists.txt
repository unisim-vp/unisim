message ( STATUS "Checking unisim::service::interfaces::backtrace" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/kernel/service )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__backtrace
	DEPENDS
		unisim__util__debug__backtrace
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/backtrace.hh
	)

message ( STATUS "Checking unisim::service::interfaces::backtrace - done" )
