## UNISIM_CHECK_LUA
## Checks if the lua library is installed
## Takes one parameter: the main function name
#####################################################
AC_DEFUN([UNISIM_CHECK_LUA], [
    # Check for lua.h
    AC_CHECK_HEADER(lua5.1/lua.h,, AC_MSG_ERROR([lua.h not found.]))

    # Check for functions 
    UNISIM_CHECK_LIB(lua5.1, luaL_openlibs, $1, broken_lua=no, broken_lua=yes)

    if test "$broken_lua" == "yes"; then
		AC_MSG_NOTICE([LUA5.1 interpreter not found.])
    else
		LIBS="-llua5.1 ${LIBS}"
		AC_DEFINE([HAVE_LUA], [], [Whether LUA library is available])
    fi
])
