#
# Check configure option --disable-occViewer=[yes|no|DIR] taking into account that
# DISABLE_OCCVIEWER may be already set during build_configure,
# Set DISABLE_OCCVIEWER to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_OCCVIEWER],[

AC_ARG_WITH(occViewer,
	    [  --disable-occViewer default=no ],
	    disable_occViewer="$withval",disable_occViewer="${DISABLE_OCCVIEWER}")

case $disable_occViewer in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*****   DISABLE OCCViewer configuration   ******)
#         AC_MSG_RESULT(************************************************)
        DISABLE_OCCVIEWER="yes"
        AC_SUBST(DISABLE_OCCVIEWER)
esac

 
])dnl
