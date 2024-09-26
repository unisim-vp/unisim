## UNISIM_CHECK_NODEJS
## Checks if Node.js is installed
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_NODEJS], [
	# Check if Nodejs path has been overloaded
	AC_ARG_WITH(nodejs,
	AS_HELP_STRING([--with-nodejs=<path>], [Node.js JavaScript runtime environment to use (will be completed with /include/node and /lib)]))
	if test "x$with_nodejs" != "x"; then
		AC_MSG_NOTICE([using Node.js JavaScript runtime environment at $with_nodejs])
		
		sep=':'

		case "${build}" in
			*mingw*)
				sep=';'    # separator on a windows build machine is ';', not ':'
				;;
		esac
		export PKG_CONFIG_PATH="${with_nodejs}/lib/pkgconfig${sep}${with_nodejs}/lib64/pkgconfig${sep}${PKG_CONFIG_PATH}"
	fi
	
	PKG_CHECK_MODULES(NODEJS, nodejs, unisim_cv_nodejs=yes, unisim_cv_nodejs=no)
	
	if test "x$unisim_cv_nodejs" = "xyes"; then
		CXXFLAGS="${CXXFLAGS} ${NODEJS_CFLAGS}"
		LIBS="${LIBS} ${NODEJS_LIBS}"
	else
		unisim_check_nodejs_save_LDFLAGS="${LDFLAGS}"
		unisim_check_nodejs_save_CPPFLAGS="${CPPFLAGS}"
		if test "x$with_nodejs" != "x"; then
			LDFLAGS=${LDFLAGS}" -L$with_nodejs/lib"
			CPPFLAGS=${CPPFLAGS}" -I$with_nodejs/include/node"
		else
			CPPFLAGS=${CPPFLAGS}" -I/usr/include/node -I/usr/local/include/node"
		fi
		
		# Check for node.h
		AC_CHECK_HEADER(node.h, unisim_cv_nodejs=yes, unisim_cv_nodejs=no)

		if test "$unisim_cv_nodejs" = "yes"; then
			LIBS="-lnode ${LIBS}"
		else
			LDFLAGS="${unisim_check_nodejs_save_LDFLAGS}"
			CPPFLAGS="${unisim_check_nodejs_save_CPPFLAGS}"
		fi
	fi
	
	if test "$unisim_cv_nodejs" = "yes"; then
		AC_MSG_NOTICE([Node.js JavaScript runtime environment found])
		AC_DEFINE([HAVE_NODEJS], [1], [Define to 1 if you have Node.js JavaScript runtime environment])
	else
		AC_MSG_WARN([Node.js JavaScript runtime environment not found.])
	fi
])
