AC_DEFUN([UNISIM_CHECK_TIMES], [

	case $host in
		*) # other hosts (darwin, linux and un*x)
			AC_CHECK_HEADER([sys/time.h],, AC_MSG_ERROR([sys/times.h is missing.]))
			;;
	esac

])
