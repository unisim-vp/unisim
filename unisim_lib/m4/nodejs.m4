## UNISIM_CHECK_NODEJS
## Checks if Node.js is installed
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_NODEJS], [
	# Check if Nodejs path has been overloaded
	AC_ARG_WITH(nodejs,
	AS_HELP_STRING([--with-nodejs=<path>], [Node.js JavaScript runtime environment to use (will be completed with /include/node and /lib)]))
	unisim_check_nodejs_save_LDFLAGS="${LDFLAGS}"
	unisim_check_nodejs_save_CPPFLAGS="${CPPFLAGS}"
	if test "x$with_nodejs" != "x"; then
		AC_MSG_NOTICE([using Node.js JavaScript runtime environment at $with_nodejs])
		LDFLAGS=${LDFLAGS}" -L$with_nodejs/lib"
		CPPFLAGS=${CPPFLAGS}" -I$with_nodejs/include/node"
	else
		CPPFLAGS=${CPPFLAGS}" -I/usr/include/node -I/usr/local/include/node"
	fi
	
	# Check for node.h
	AC_CHECK_HEADER(node.h, found_nodejs=yes, found_nodejs=no)

	if test "$found_nodejs" = "yes"; then
		AC_MSG_NOTICE([Node.js JavaScript runtime environment found])
		LIBS="-lnode ${LIBS}"
		AC_DEFINE([HAVE_NODEJS], [1], [Define to 1 if you have Node.js JavaScript runtime environment])
	else
		AC_MSG_WARN([Node.js JavaScript runtime environment not found.])
		LDFLAGS="${unisim_check_nodejs_save_LDFLAGS}"
		CPPFLAGS="${unisim_check_nodejs_save_CPPFLAGS}"
	fi
])
