#
#  Copyright (c) 2007,
#  Commissariat a l'Energie Atomique (CEA)
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
#
#   - Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#
#   - Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
#   - Neither the name of CEA nor the names of its contributors may be used to
#     endorse or promote products derived from this software without specific prior
#     written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED.
#  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
#  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
#          Gilles Mouchard (gilles.mouchard@cea.fr)
#

AC_INIT([UNISIM Kernel TLM Library], [1.0])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_SYSTEM
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_PATH_PROGS(SH, sh)
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_LANG([C++])

###### CHECK FOR STANDARD C++ HEADERS #########################################

AC_CHECK_HEADERS([string vector list fstream istream iostream sstream stack deque iosfwd \
                  utility memory cassert map queue stdexcept cstdlib ios],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))

######################## SystemC #############################

# Mimics the behavior of SystemC configure to guess where libsystemc.a is installed (e.g. lib-linux)
CXX_COMP=`basename $CXX`
case "$target" in
    powerpc-apple-macosx*)
        SYSTEMC_TARGET_ARCH="macosx"
        ;;
    sparc-sun-solaris*)
        case "$CXX_COMP" in
            CC)
                SYSTEMC_TARGET_ARCH="sparcOS5"
                ;;
            c++ | g++)
                SYSTEMC_TARGET_ARCH="gccsparcOS5"
                ;;
        esac
        ;;
    x86_64*linux*)
        SYSTEMC_TARGET_ARCH="linux64"
        ;;
    *linux*)
        SYSTEMC_TARGET_ARCH="linux"
        ;;
    *cygwin*)
        SYSTEMC_TARGET_ARCH="cygwin"
        ;;
    *hpux11*)
        case "$CXX_COMP" in
            aCC)
                SYSTEMC_TARGET_ARCH="hpux11"
                ;;
            c++ | g++)
                SYSTEMC_TARGET_ARCH="gcchpux11"
                ;;
        esac
        ;;
    *mingw32*)
       SYSTEMC_TARGET_ARCH="mingw32"
       ;;
esac

# Check if SystemC path has been overloaded
AC_ARG_WITH(systemc,
	AS_HELP_STRING([--with-systemc=<path>], [systemc library to use (will be completed with /include and /lib-linux)]))
if test "x$with_systemc" != "x"; then
	AC_MSG_NOTICE([using SystemC at $with_systemc])
	LIBS+=" "
	CPPFLAGS+=" -I$with_systemc/include -L$with_systemc/lib-$SYSTEMC_TARGET_ARCH"
fi

# Check for systemc.h
AC_CHECK_HEADER(systemc.h,, AC_MSG_ERROR([systemc.h not found. Please install the SystemC library (version >= 2.1). Use --with-systemc=<path> to overload default includes search path.]))

# Check for function 'main' in libsystemc.a
AC_CHECK_LIB(systemc,main,broken_systemc=no,broken_system=yes)

if test "$broken_systemc" == "yes"; then
	AC_MSG_ERROR([installed SystemC is broken. Please install the SystemC library (version > 2.1). Use --with-systemc=<path> to overload default includes search path.])
else
	LIBS+=" -lsystemc"
fi

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
