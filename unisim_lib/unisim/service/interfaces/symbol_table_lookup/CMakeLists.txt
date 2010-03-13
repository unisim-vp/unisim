message ( STATUS "Checking unisim::service::interfaces::symbol_table_lookup" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

unisim_include ( unisim/util/debug )

include_directories ( ${CMAKE_SOURCE_DIR} )

add_custom_target ( unisim__service__interfaces__symbol_table_lookup
	DEPENDS
		unisim__util__debug
	SOURCES
		${CMAKE_SOURCE_DIR}/unisim/service/interfaces/symbol_table_lookup.hh
	)


message ( STATUS "Checking unisim::service::interfaces::symbol_table_lookup - done" )

