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
#

AC_INIT([UNISIM Logger Kernel Library], [1.0],
	[Daniel Gracia Perez <daniel.gracia-perez@cea.fr>],
	[service])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_PATH_PROGS(SH, sh)
# AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_LIBTOOL_WIN32_DLL # must be *before* AC_PROG_LIBTOOL
AC_PROG_LIBTOOL
AC_ENABLE_STATIC
AC_ENABLE_SHARED
AC_PROG_LN_S

AC_LANG([C++])

###### CHECK FOR STANDARD C++ HEADERS #########################################
AC_CHECK_HEADERS([sstream string iostream],,\
                  AC_MSG_ERROR([Some standard headers are missing.]))

###### CHECK FOR BOOST ########################################################
## UNISIM_CHECK_BOOST

###### CHECK FOR LIBXML2 ######################################################
UNISIM_CHECK_LIBXML2

UNISIM_LIBTOOL

# files to generate
#   this should include all the makefiles to generate
AC_CONFIG_FILES([Makefile])

# launch the generation of makefiles
#   this must be the last command of configure.ac
AC_OUTPUT
