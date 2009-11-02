AC_DEFUN([UNISIM_CHECK_TIMES], [

	case $host in
		*mingw32*) # win32 host
			AC_CHECK_HEADER([windows.h],, AC_MSG_ERROR([windows.h is missing.]))
			;;

		
		*) # other hosts (darwin, linux and un*x)
			AC_CHECK_HEADER([sys/times.h],, AC_MSG_ERROR([sys/times.h is missing.]))
			;;
	esac

])
