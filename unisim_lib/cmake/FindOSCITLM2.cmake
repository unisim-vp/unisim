IF ( with_osci_tlm2 )
	SET ( OSCI_TLM2_INCLUDE_HINT ${with_osci_tlm2}/include/tlm )
ELSE ( with_osci_systemc )
	SET ( OSCI_TLM2_INCLUDE_HINT /usr/include/tlm )
ENDIF ( with_osci_tlm2 )

FIND_PATH ( OSCI_TLM2_INCLUDE_DIR tlm.h
	HINTS
		${OSCI_TLM2_INCLUDE_HINT}
	)

INCLUDE ( FindPackageHandleStandardArgs )

FIND_PACKAGE_HANDLE_STANDARD_ARGS ( OSCITLM2 DEFAULT_MSG OSCI_TLM2_INCLUDE_DIR )
IF ( NOT OSCITLM2_FOUND )
	MESSAGE ( FATAL_ERROR
		"Could not find TLM2.0 library, please use -Dwith_osci_tlm2=<path> to indicate its location." )
ENDIF ( NOT OSCITLM2_FOUND )
MARK_AS_ADVANCED ( OSCI_TLM2_INCLUDE_DIR )

