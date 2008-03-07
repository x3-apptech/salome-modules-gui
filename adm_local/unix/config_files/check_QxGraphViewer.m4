#
# Check configure option --disable-qxGraphViewer=[yes|no|DIR] taking into account that
# DISABLE_QXGRAPHVIEWER may be already set during build_configure,
# Set DISABLE_QXGRAPHVIEWER to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_QXGRAPHVIEWER],[

AC_ARG_WITH(qxGraphViewer,
	    [  --disable-qxGraphViewer default=no ],
	    disable_qxGraphViewer="$withval",disable_qxGraphViewer="${DISABLE_QXGRAPHVIEWER}")

case $disable_qxGraphViewer in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*******   DISABLE QxGraphViewer configuration   ********)
#         AC_MSG_RESULT(************************************************)
        DISABLE_QXGRAPHVIEWER="yes"
        AC_SUBST(DISABLE_QXGRAPHVIEWER)
esac

 
])dnl
