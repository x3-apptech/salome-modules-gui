# Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# Qt4 detection for Salome
#
#  !! Please read the generic detection procedure in SalomeMacros.cmake !!
#

SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(Qt4 QT_INCLUDES 2)
MARK_AS_ADVANCED(QT_QMAKE_EXECUTABLE)

# This is only needed to correctly detect Qt help generator tool, to workaround an error 
# coming from ParaView detection procedure
FIND_PROGRAM(QT_HELP_GENERATOR qhelpgenerator
    PATHS "${QT_BINARY_DIR}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
    DOC "qhelpgenerator used to compile Qt help project files"
    )
MARK_AS_ADVANCED(QT_HELP_GENERATOR)

IF(QT4_FOUND) 
  SALOME_ACCUMULATE_HEADERS(QT_INCLUDES)
  SALOME_ACCUMULATE_ENVIRONMENT(PATH ${QT_QMAKE_EXECUTABLE})
  SALOME_ACCUMULATE_ENVIRONMENT(LD_LIBRARY_PATH ${QT_QTCORE_LIBRARY})
  SET(QT_VERSION "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
  MESSAGE(STATUS "Qt version is ${QT_VERSION}")
ENDIF()
