#
# Check configure option --disable-vtkViewer=[yes|no|DIR] taking into account that
# DISABLE_VTKVIEWER may be already set during build_configure,
# Set DISABLE_VTKVIEWER to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_VTKVIEWER],[

AC_ARG_WITH(vtkViewer,
	    [  --disable-vtkViewer default=no ],
	    disable_vtkViewer="$withval",disable_vtkViewer="${DISABLE_VTKVIEWER}")

case $disable_vtkViewer in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*****   DISABLE VTK Viewer configuration   *****)
#         AC_MSG_RESULT(************************************************)
        DISABLE_VTKVIEWER="yes"
        AC_SUBST(DISABLE_VTKVIEWER)
esac

 
])dnl
