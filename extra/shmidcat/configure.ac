# Creating configure: aclocal -I m4 && autoconf --force && autoheader && libtoolize && automake -ac

AC_INIT([libshmidcat], [3.3.21])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_PATH_PROGS(SH, sh)
AC_PROG_CC
AC_PROG_INSTALL
AC_LIBTOOL_WIN32_DLL # must be *before* AC_PROG_LIBTOOL
AC_PROG_LIBTOOL
AC_ENABLE_SHARED
AC_PROG_LN_S

AC_LANG([C])

AC_CHECK_HEADERS([stdio.h stdlib.h string.h sys/time.h sys/types.h unistd.h signal.h],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))


case $host in
*mingw32*)  # win32 host
	AC_CHECK_HEADERS([windows.h],,\
	                 AC_MSG_ERROR([windows.h is missing.]))
	;;
*)
	AC_CHECK_HEADERS([sys/ipc.h sys/shm.h],,\
                     AC_MSG_ERROR([Some IPC System V headers are missing.]))
	;;
esac

case $host in
	*mingw32*)  # win32 host
		if [ "$enable_win32_dll" = "yes" ]; then
			LDFLAGS="-no-undefined ${LDFLAGS}"
		fi
		;;
esac

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
