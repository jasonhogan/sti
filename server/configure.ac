# Process this file with autoconf to produce a configure script.

# Initialization
AC_INIT([STI server], [0.0.1], [mandel@stanford.edu], [server])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_SRCDIR([src])

# Local definitions

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_CPP
AI_CHECK_ORB
if test "x${ORB}" = "xno"; then
    AC_MSG_ERROR([an ORB is required])
fi

# Checks for libraries.
PYTHON_LIB=""
AC_CHECK_LIB([python2.5],[Py_Initialize])
if test "$ac_cv_lib_python2_5_Py_Initialize" == "yes"; then
    PYTHON_LIB=2.5
fi
if test -z "$PYTHON_LIB"; then
    AC_CHECK_LIB([python2.4],[Py_Initialize])
    if test "$ac_cv_lib_python2_4_Py_Initialize" == "yes"; then
        PYTHON_LIB=2.4
    fi
fi
if test -z "$PYTHON_LIB"; then
  AC_MSG_ERROR(Python library not found)
fi

# Checks for header files.
AC_HEADER_STDC
if test "$PYTHON_LIB" == "2.5"; then
    AC_CHECK_HEADERS([python2.5/Python.h],,
        AC_MSG_ERROR(Python header file not found))
    AC_CHECK_HEADERS([python2.5/structmember.h],,
        AC_MSG_ERROR(Python header file not found),
        [#include <python2.5/Python.h>])
elif test "$PYTHON_LIB" == "2.4"; then
    AC_CHECK_HEADERS([python2.4/Python.h],,
        AC_MSG_ERROR(Python header file not found))
    AC_CHECK_HEADERS([python2.4/node.h],,
        AC_MSG_ERROR(Python header file not found),
        [#include <python2.4/Python.h>])
    AC_CHECK_HEADERS([python2.4/structmember.h],,
        AC_MSG_ERROR(Python header file not found),
        [#include <python2.4/Python.h>])
else
    AC_MSG_ERROR(Unknown version of python library found)
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.

# Output the results
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([GNUmakefile
    src/GNUmakefile
    src/corba/GNUmakefile
    src/python/GNUmakefile])
AC_OUTPUT
