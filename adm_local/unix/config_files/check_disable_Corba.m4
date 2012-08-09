dnl Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License.
dnl
dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl

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
