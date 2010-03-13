function (stl_stdexcept)
	include (CheckIncludeFileCXX)

	check_include_file_cxx (stdexcept HAVE_STDEXCEPT)
	if (NOT ${HAVE_STDEXCEPT})
		message (SEND_ERROR "Could not find c++ stl library \"stdexcept\".")
	endif (NOT ${HAVE_STDEXCEPT})
endfunction (stl_stdexcept)

