## UNISIM_CHECK_FLOAT16
## Checks if compiler provides support _Float16 type
## Does not take parameters
## unisim_cv_have_float16 is set to either 'yes' or 'no'
## HAVE_FLOAT16 is defined to 1 if found
#####################################################
AC_DEFUN([UNISIM_CHECK_FLOAT16], [
  unisim_check_float16_save_LIBS="${LIBS}"
  LIBS=
  AC_MSG_CHECKING([whether _Float16 is supported])
  AC_LINK_IFELSE([AC_LANG_SOURCE([[
extern "C" int main(int argc, char **argv); int main(int argc, char **argv) { volatile _Float16 a = 1; return (int) a; }]])],
      AC_MSG_RESULT([yes]); unisim_cv_have_float16=yes,
      AC_MSG_RESULT([no]); unisim_cv_have_float16=no)
  AC_MSG_CHECKING([whether std::numeric_limits<_Float16> is supported])
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <limits>
extern "C" int main(int argc, char **argv); int main(int argc, char **argv) { return (int) !std::numeric_limits<_Float16>::is_specialized; }]])],
      AC_MSG_RESULT([yes]); unisim_cv_have_std_numeric_limits_float16=yes,
      AC_MSG_RESULT([no]); unisim_cv_have_std_numeric_limits_float16=no)
  LIBS="${unisim_check_float16_save_LIBS}"
  if test "x$unisim_cv_have_float16" = "xyes"; then
    AC_DEFINE([HAVE_FLOAT16], [1], [Define to 1 if _Float16 is available])
  fi
  if test "x$unisim_cv_have_std_numeric_limits_float16" = "xyes"; then
    AC_DEFINE([HAVE_STD_NUMERIC_LIMITS_FLOAT16], [1], [Define to 1 if std::numeric_limits<_Float16> is available])
  fi
])
