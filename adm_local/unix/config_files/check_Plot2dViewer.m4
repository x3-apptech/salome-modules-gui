#
# Check configure option --disable-plot2dViewer=[yes|no|DIR] taking into account that
# DISABLE_PLOT2DVIEWER may be already set during build_configure,
# Set DISABLE_PLOT2DVIEWER to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_PLOT2DVIEWER],[

AC_ARG_WITH(plot2dViewer,
	    [  --disable-plot2dViewer default=no ],
	    disable_plot2dViewer="$withval",disable_plot2dViewer="${DISABLE_PLOT2DVIEWER}")

case $disable_plot2dViewer in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(***   DISABLE Plot2dViewer configuration   *****)
#         AC_MSG_RESULT(************************************************)
          DISABLE_PLOT2DVIEWER="yes"
          AC_SUBST(DISABLE_PLOT2DVIEWER)
esac

 
])dnl
