message ( STATUS "Checking unisim::component::cxx::processor::powerpc::mpc7447a" )

include ( ${CMAKE_SOURCE_DIR}/cmake/UnisimInclude.cmake )

set ( cxx_powerpc_mpc7447a_subdirs
	unisim/component/cxx/processor/powerpc
	)

foreach ( dir ${cxx_powerpc_mpc74474a_subdirs} )
	unisim_include ( ${dir} )
endforeach ( dir )

set ( cxx_powerpc_mpc7447a_srcs
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/mpc7447a.cc
	${CMAKE_SOURCE_DIR}/unisim/component/cxx/processor/powerpc/mpc7447a_debug.cc
	)

include_directories (
	${CMAKE_SOURCE_DIR}
	${CMAKE_BINARY_DIR}
	)

add_library ( unisim__component__cxx__processor__powerpc__mpc7447a
	STATIC
		${cxx_powerpc_mpc7447a_srcs}
	)

add_dependencies ( unisim__component__cxx__processor__powerpc__mpc7447a
	unisim__component__cxx__processor__powerpc
	)

target_link_libraries ( unisim__component__cxx__processor__powerpc__mpc7447a
	unisim__component__cxx__processor__powerpc
	)

message ( STATUS "Checking unisim::component::cxx::processor::powerpc::mpc7447a - done" )



