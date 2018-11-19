#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([vimtest], [1.0], [a@test.com])
AC_CONFIG_SRCDIR([src/vimtest.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_CONFIG_MACRO_DIRS([m4])

AM_PROG_AR


# Checks for programs.
LT_INIT
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LIBTOOL
#AC_LIBTOOL_DLOPEN

CXXFLAGS="$CXXFLAGS -std=c++11"

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
