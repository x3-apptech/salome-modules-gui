#  Check availability of full Salome binary distribution
#
#  Author : Marc Tajchman (CEA, 2002)
#------------------------------------------------------------

AC_DEFUN([CHECK_CORBA_IN_GUI],[

AC_CHECKING(for full SalomeGUI)

SalomeGUI_ok=no
CORBA_IN_GUI="no"

AC_ARG_WITH(gui,
	    --with-salome_gui=DIR root directory path of full SALOME GUI installation,
	    SALOME_GUI_DIR="$withval",SALOME_GUI_DIR="")

if test "x$SALOME_GUI_DIR" = "x" ; then

# no --with-gui-dir option used

  if test "x$GUI_ROOT_DIR" != "x" ; then

    # SALOME_ROOT_DIR environment variable defined
    SALOME_GUI_DIR=$GUI_ROOT_DIR

  else

    # search Salome binaries in PATH variable
    AC_PATH_PROG(TEMP, libSalomeApp.so)
    if test "x$TEMP" != "x" ; then
      SALOME_BIN_DIR=`dirname $TEMP`
      SALOME_GUI_DIR=`dirname $SALOME_BIN_DIR`
    fi

  fi
#
fi

if test -f ${SALOME_GUI_DIR}/lib/salome/libSalomeApp.so  ; then
  SalomeGUI_ok=yes
  CORBA_IN_GUI=yes
  AC_MSG_RESULT(Using SALOME GUI distribution in ${SALOME_GUI_DIR})

  if test "x$GUI_ROOT_DIR" == "x" ; then
    GUI_ROOT_DIR=${SALOME_GUI_DIR}
  fi
else
  AC_MSG_WARN("Cannot find compiled full SALOME GUI distribution")
fi
AC_SUBST(CORBA_IN_GUI)
  
AC_MSG_RESULT(for full SALOME GUI: $SalomeGUI_ok)
 
])dnl
 
