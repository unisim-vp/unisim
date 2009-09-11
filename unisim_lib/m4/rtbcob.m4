## UNISIM_CHECK_RTBCOB
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_RTBCOB], [
	# Check if RTBuilder Cob path has been overloaded
	AC_ARG_WITH(rtbcob,
	AS_HELP_STRING([--with-rtbcob=<path>], [RTBuilder Cob library to use (will be completed with /include)]))
	if test "x$with_rtbcob" != "x"; then
		AC_MSG_NOTICE([using RTBuilder Cob at $with_rtbcob])
		CPPFLAGS=${CPPFLAGS}" -I$with_rtbcob/include"
	fi

])
