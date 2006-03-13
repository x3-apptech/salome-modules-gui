#
# Check configure option --disable-supervGraphViewer=[yes|no|DIR] taking into account that
# DISABLE_SUPERVGRAPHVIEWER may be already set during build_configure,
# Set DISABLE_SUPERVGRAPHVIEWER to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_SUPERVGRAPHVIEWER],[

AC_ARG_WITH(supervGraphViewer,
	    [  --disable-supervGraphViewer default=no ],
	    disable_supervGraphViewer="$withval",disable_supervGraphViewer="${DISABLE_SUPERVGRAPHVIEWER}")

case $disable_supervGraphViewer in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*******   DISABLE SupervGraphViewer configuration   ********)
#         AC_MSG_RESULT(************************************************)
        DISABLE_SUPERVGRAPHVIEWER="yes"
        AC_SUBST(DISABLE_SUPERVGRAPHVIEWER)
esac

 
])dnl
