AC_DEFUN([UNISIM_CHECK_SHARED_MEMORY], [
	case $host in
		*mingw32*) # win32 host
			AC_CHECK_HEADER([windows.h],, AC_MSG_ERROR([sys/shm.h is missing.]))
			;;
		*) # other hosts (un*x)
			AC_CHECK_HEADER([sys/shm.h],, AC_MSG_ERROR([sys/shm.h is missing.]))
			;;
	esac
])
