#
# Check configure option --disable-glViewer=[yes|no|DIR] taking into account that
# DISABLE_GLVIEWER may be already set during build_configure,
# Set DISABLE_GLVIEWER to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_GLVIEWER],[

AC_ARG_WITH(glViewer,
	    [  --disable-glViewer default=no ],
	    disable_glViewer="$withval",disable_glViewer="${DISABLE_GLVIEWER}")

case $disable_glViewer in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*******   DISABLE GLViewer configuration   *****)
#         AC_MSG_RESULT(************************************************)
        DISABLE_GLVIEWER="yes"
        AC_SUBST(DISABLE_GLVIEWER)
esac

 
])dnl
