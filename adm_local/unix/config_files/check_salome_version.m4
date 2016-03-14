dnl Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
dnl
dnl Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
dnl CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2.1 of the License, or (at your option) any later version.
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

# Check SALOME version
#

AC_DEFUN([CHECK_SALOME_VERSION],[
AC_REQUIRE([AC_LINKER_OPTIONS])dnl

AC_CHECKING(for salome version)

salome_version_ok=no
SALOME_VERSION=""

salomeVersionFile=${GUI_ROOT_DIR}/include/salome/GUI_version.h
SALOME_VERSION=$(grep GUI_VERSION_STR ${salomeVersionFile} | cut -d'"' -f2 | cut -d"." -f1)

if test "x${SALOME_VERSION}" = "x" ; then
  AC_MSG_WARN("Cannot determine the SALOME version. GUI module is required. Specify the variable GUI_ROOT_DIR")
else
  salome_version_ok=yes
  AC_MSG_RESULT(Using salome version ${SALOME_VERSION})
fi

AC_SUBST(SALOME_VERSION)
AC_MSG_RESULT(for salome version: $salome_version_ok)
 
])dnl
 
