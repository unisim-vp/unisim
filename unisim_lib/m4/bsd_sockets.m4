## UNISIM_CHECK_BSD_SOCKETS
## Checks if BSD sockets style are available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BSD_SOCKETS], [
	case $host in
	*linux*)   # linux host
		AC_CHECK_HEADERS([sys/socket.h netinet/in.h arpa/inet.h netinet/tcp.h netdb.h],, AC_MSG_ERROR([Some standard headers are missing.]))
		;;
	*mingw32*)  # win32 host
		# Note: we can't check socket functions from libwsock32 because of the PASCAL calling convention. cdecl is mandatory for autoconf.
		LIBS="-lwsock32 ${LIBS}"
		;;
	esac
])
