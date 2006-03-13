#
# Check configure option --disable-salomeObject=[yes|no|DIR] taking into account that
# DISABLE_SALOMEOBJECT may be already set during build_configure,
# Set DISABLE_SALOMEOBJECT to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_SALOMEOBJECT],[

AC_ARG_WITH(salomeObject,
	    [  --disable-salomeObject default=no ],
	    disable_salomeObject="$withval",disable_salomeObject="${DISABLE_SALOMEOBJECT}")

case $disable_salomeObject in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(***   DISABLE Salome object configuration   ****)
#         AC_MSG_RESULT(************************************************)
        DISABLE_SALOMEOBJECT="yes"
        AC_SUBST(DISABLE_SALOMEOBJECT)
esac

 
])dnl
