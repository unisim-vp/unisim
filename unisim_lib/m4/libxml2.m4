## UNISIM_CHECK_LIBXML2
## Checks if the libxml2 library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBXML2], [
    # Check if libxml2 path has been overloaded
    AC_ARG_WITH(libxml2,
	AS_HELP_STRING([--with-libxml2=<path>], [libxml2 library to use (will be completed with /include and /lib)]))

    if test "x$with_libxml2" != "x"; then
		AC_MSG_NOTICE([using libxml2 at $with_libxml2])
		if test $host = $build; then
			# We are not crosscompiling so we can execute xml2-config on 'build' machine
			AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no, $with_libxml2/bin)
			if test "x$xml2_config_installed" != "xyes"; then
					AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
			fi
			AC_MSG_NOTICE([xml2-config found])
			libxml2_cflags="`$with_libxml2/bin/xml2-config --cflags`"
			libxml2_libs="`$with_libxml2/bin/xml2-config --libs`"
			AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2])
			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
			LDFLAGS=${LDFLAGS}" ${libxml2_libs}"
		else
			CPPFLAGS=${CPPFLAGS}" -I$with_libxml2/include/libxml2"
			LDFLAGS=${LDFLAGS}" -L$with_libxml2/lib"
		fi
    else
		if test $host = $build; then
			# We are not crosscompiling so we can execute xml2-config on 'build' machine
			AC_CHECK_PROG(xml2_config_installed, xml2-config, yes, no)
			if test "x$xml2_config_installed" != "xyes"; then
					AC_MSG_ERROR([xml2-config not found. Please install libxml2 development library.])
			fi
			AC_MSG_NOTICE([xml2-config found])
			libxml2_cflags="`xml2-config --cflags`"
			libxml2_libs="`xml2-config --libs`"
			AC_MSG_NOTICE([xml2-config says compiler needs option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2])
			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
			LDFLAGS=${LDFLAGS}" ${libxml2_libs}"
		else
			libxml2_cflags="-I/usr/include/libxml2"
			libxml2_libs="-L/usr/lib"
			AC_MSG_NOTICE([Trying with compiler option ${libxml2_cflags} ${libxml2_libs} to compile and link with libxml2.])
			CPPFLAGS=${CPPFLAGS}" ${libxml2_cflags}"
			LDFLAGS=${LDFLAGS}" ${libxml2_libs}"
		fi
	fi
	
    # Check for some libxml2 headers
    AC_CHECK_HEADER(libxml/encoding.h, broken_incxml2=no, broken_incxml2=yes)
    AC_CHECK_HEADER(libxml/xmlwriter.h, , broken_incxml2=yes)
	
    if test "$broken_incxml2" == "yes"; then
		AC_MSG_ERROR([libxml2 includes not found (libxml/*.hh). Please use --with-libxml2=<path>])
    fi

    # Check for libxml2 functions
    AC_CHECK_LIB(xml2, xmlNewTextWriterFilename,
    AC_CHECK_LIB(xml2, xmlTextWriterSetIndent,
    AC_CHECK_LIB(xml2, xmlTextWriterStartDocument,
    AC_CHECK_LIB(xml2, xmlTextWriterStartElement, 
    AC_CHECK_LIB(xml2, xmlTextWriterEndElement, 
    AC_CHECK_LIB(xml2, xmlFreeTextWriter, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteAttribute, 
    AC_CHECK_LIB(xml2, xmlTextWriterWriteFormatString, broken_libxml2=no,
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes),
    broken_libxml2=yes)
	
    if test "$broken_libxml2" == "yes"; then
		AC_MSG_ERROR([installed xml2 Library is broken.])
    else
		LIBS="-lxml2 ${LIBS}"
    fi
])
