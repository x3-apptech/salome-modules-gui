#
# Check configure option --disable-pyConsole=[yes|no|DIR] taking into account that
# DISABLE_PYCONSOLE may be already set during build_configure,
# Set DISABLE_PYCONSOLE to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_PYCONSOLE],[

AC_ARG_WITH(pyConsole,
	    [  --disable-pyConsole default=no ],
	    disable_pyConsole="$withval",disable_pyConsole="${DISABLE_PYCONSOLE}")

case $disable_pyConsole in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*****   DISABLE PyConsole configuration   ******)
#         AC_MSG_RESULT(************************************************)
        DISABLE_PYCONSOLE="yes"
        AC_SUBST(DISABLE_PYCONSOLE)
esac

 
])dnl
