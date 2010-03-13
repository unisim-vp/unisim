function (clib_signal_h)
	include (CheckIncludeFile)

	check_include_file (signal.h HAVE_CLIB_SIGNAL_H)
	if (NOT ${HAVE_CLIB_SIGNAL_H})
		message ( SEND_ERROR "Could not find \"signal.h\"" )
	endif (NOT ${HAVE_CLIB_SIGNAL_H})
endfunction (clib_signal_h)


