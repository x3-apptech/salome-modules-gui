#  Check availability of Salome binary distribution
#
#  Author : Marc Tajchman (CEA, 2002)
#------------------------------------------------------------

AC_DEFUN([CHECK_SALOME_GUI],[

AC_CHECKING(for SalomeGUI)

SalomeGUI_ok=no

AC_ARG_WITH(gui,
	    --with-salome_gui=DIR root directory path of SALOME GUI installation,
	    SALOME_GUI_DIR="$withval",SALOME_GUI_DIR="")

if test "x$SALOME_GUI_DIR" = "x" ; then

# no --with-gui-dir option used

  if test "x$GUI_ROOT_DIR" != "x" ; then

    # SALOME_ROOT_DIR environment variable defined
    SALOME_GUI_DIR=$GUI_ROOT_DIR

  else

    # search Salome binaries in PATH variable
    AC_PATH_PROG(TEMP, libLightApp.so)
    if test "x$TEMP" != "x" ; then
      SALOME_BIN_DIR=`dirname $TEMP`
      SALOME_GUI_DIR=`dirname $SALOME_BIN_DIR`
    fi

  fi
#
fi

if test -f ${SALOME_GUI_DIR}/lib/salome/libLightApp.so  ; then
  SalomeGUI_ok=yes
  AC_MSG_RESULT(Using SALOME GUI distribution in ${SALOME_GUI_DIR})

  if test "x$GUI_ROOT_DIR" == "x" ; then
    GUI_ROOT_DIR=${SALOME_GUI_DIR}
  fi
  AC_SUBST(GUI_ROOT_DIR)
else
  AC_MSG_WARN("Cannot find compiled SALOME GUI distribution")
fi
  
AC_MSG_RESULT(for SALOME GUI: $SalomeGUI_ok)
 
])dnl
 
