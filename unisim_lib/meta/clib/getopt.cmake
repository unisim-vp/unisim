function (clib_getopt_h)
	include (CheckIncludeFile)

	check_include_file (getopt.h HAVE_GETOPT_H)
	if (NOT ${HAVE_GETOPT_H})
		message ( SEND_ERROR "Could not find \"getopt.h\"" )
	endif (NOT ${HAVE_GETOPT_H})
endfunction (clib_getopt_h)
