## UNISIM_CHECK_TLM20
## Checks if the OSCI TLM2.0 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_TLM20], [
	# Check if TLM2.0 path has been overloaded
	AC_ARG_WITH(tlm20,
	AS_HELP_STRING([--with-tlm20=<path>], [TLM2.0 library to use (will be completed with /include/tlm)]))
	if test "x$with_tlm20" != "x"; then
		AC_MSG_NOTICE([using TLM2.0 at $with_tlm20])
		CPPFLAGS=${CPPFLAGS}" -I$with_tlm20/include/tlm"
	fi

	# Check for tlm.h
	AC_CHECK_HEADER(tlm.h,, AC_MSG_ERROR([tlm.h not found for TLM2.0. Please install the TLM library (version 2.0). Use --with-tlm20=<path> to overload default includes search path.]))
])
