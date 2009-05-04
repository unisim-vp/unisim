## UNISIM_CHECK_WINSOCK2
## Checks if the winsock2 library is available
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_WINSOCK2], [
	# Note: we can't check socket functions from libwsock32 because of the PASCAL calling convention. cdecl is mandatory for autoconf.
	LIBS="-lwsock32 ${LIBS}"
])
