AC_INIT([lib7zip], [1.0.2], [jingnan.si@gmail.com])
AC_CONFIG_SRCDIR(Lib7Zip/lib7zip.h)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_CXX
AC_C_CONST
AC_C_INLINE
AC_HEADER_STDBOOL

AC_PROG_RANLIB

AC_LANG(C++)

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stddef.h string.h unistd.h dlfcn.h dirent.h])
AC_CHECK_FUNCS([memset memmove])
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES([
	Makefile
	includes/Makefile
	includes/CPP/Makefile
	includes/CPP/Common/Makefile
	includes/CPP/Windows/Makefile
	Lib7Zip/Makefile
	Test7Zip/Makefile
	lib/Makefile
])

AC_OUTPUT
