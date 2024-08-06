AC_DEFUN([UNISIM_CHECK_DARWIN_CRT_EXTERNS], [

	case $host in
		*-darwin*)  # darwin host
			AC_CHECK_HEADER([crt_externs.h],, AC_MSG_ERROR([crt_externs.h is missing.]))
			;;
	esac

])
