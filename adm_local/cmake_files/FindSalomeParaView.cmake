# Copyright (C) 2010-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author: Adrien Bruneton
#

# ParaView detection for salome
#
#  !! Please read the generic detection procedure in SalomeMacros.cmake !!
#

IF(NOT QT_VERSION)
  MESSAGE(FATAL_ERROR "Detection of ParaView requires Qt to be detected first!")
ENDIF()

IF (QT_VERSION VERSION_LESS "5.0")
  SET(PARAVIEW_QT_VERSION "4")
ELSE()
  SET(PARAVIEW_QT_VERSION "5")
ENDIF()

SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(ParaView PARAVIEW_USE_FILE 4)

INCLUDE(${PARAVIEW_USE_FILE})
