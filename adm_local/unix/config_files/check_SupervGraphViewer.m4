dnl  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl  This library is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public
dnl  License as published by the Free Software Foundation; either
dnl  version 2.1 of the License.
dnl
dnl  This library is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl  Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this library; if not, write to the Free Software
dnl  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
dnl
dnl  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
dnl
#
# Check configure option --disable-supervGraphViewer=[yes|no|DIR] taking into account that
# DISABLE_SUPERVGRAPHVIEWER may be already set during build_configure,
# Set DISABLE_SUPERVGRAPHVIEWER to yes|no
#
# Author : Edawrd AGAPOV (OCC, 2005)
#

AC_DEFUN([CHECK_SUPERVGRAPHVIEWER],[

AC_ARG_WITH(supervGraphViewer,
	    [  --disable-supervGraphViewer default=no ],
	    disable_supervGraphViewer="$withval",disable_supervGraphViewer="${DISABLE_SUPERVGRAPHVIEWER}")

case $disable_supervGraphViewer in
  yes)
#         AC_MSG_RESULT(************************************************)
#         AC_MSG_RESULT(*******   DISABLE SupervGraphViewer configuration   ********)
#         AC_MSG_RESULT(************************************************)
        DISABLE_SUPERVGRAPHVIEWER="yes"
        AC_SUBST(DISABLE_SUPERVGRAPHVIEWER)
esac

 
])dnl
