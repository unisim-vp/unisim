set ( with_osci_tlm2 "" CACHE FILEPATH
	"Path to your OSCI TLM 2 installation." )
if ( with_osci_tlm2 )
	set ( OSCI_TLM2_INCLUDE_HINT ${with_osci_tlm2}/include/tlm )
else ( with_osci_systemc )
	set ( OSCI_TLM2_INCLUDE_HINT /usr/include/tlm )
endif ( with_osci_tlm2 )

find_path ( OSCI_TLM2_INCLUDE_DIR tlm.h
	HINTS
		${OSCI_TLM2_INCLUDE_HINT}
	)

include ( FindPackageHandleStandardArgs )

find_package_handle_standard_args ( OSCITLM2 DEFAULT_MSG OSCI_TLM2_INCLUDE_DIR )
if ( NOT OSCITLM2_FOUND )
	message ( FATAL_ERROR
		"Could not find TLM2.0 library, please use -Dwith_osci_tlm2=<path> to indicate its location." )
endif ( NOT OSCITLM2_FOUND )
mark_as_advanced ( OSCI_TLM2_INCLUDE_DIR )

