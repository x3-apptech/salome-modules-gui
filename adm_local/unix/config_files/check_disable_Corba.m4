#
# Check configure option --disable-corba=[yes|no|DIR] taking into account that
# GUI_DISABLE_CORBA may be already set during build_configure,
# Set GUI_DISABLE_CORBA to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_DISABLE_CORBA],[

AC_ARG_WITH(corba,
	    [  --disable-corba-gen default=no ],
	    disable_corba="$withval",disable_corba="${GUI_DISABLE_CORBA}")

case $disable_corba in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*******   DISABLE CORBA configuration   ********)
#         AC_MSG_RESULT(************************************************)
        GUI_DISABLE_CORBA="yes";;
   *)
        GUI_DISABLE_CORBA="no"
esac

AC_SUBST(GUI_DISABLE_CORBA)
 
])dnl
