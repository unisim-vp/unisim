message ( STATUS "Checking unisim::service::interfaces::blob" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/util/debug/blob )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__blob
	DEPENDS
		unisim__util__debug__blob
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/blob.hh
	)

message ( STATUS "Checking unisim::service::interfaces::blob - done" )
