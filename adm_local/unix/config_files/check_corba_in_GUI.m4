#  Check availability of full Salome binary distribution
#
#  Author : Marc Tajchman (CEA, 2002)
#------------------------------------------------------------

AC_DEFUN([CHECK_CORBA_IN_GUI],[
  CHECK_GUI([SALOME_Session_Server],
            [CORBA SALOME GUI])
  CORBA_IN_GUI=${SalomeGUI_ok}
  AC_SUBST(CORBA_IN_GUI)
])dnl
 
