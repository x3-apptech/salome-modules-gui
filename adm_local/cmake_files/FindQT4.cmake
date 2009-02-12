#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

INCLUDE(FindQt4)
INCLUDE(UseQt4)
SET(DIRS)
FOREACH(D ${QT_INCLUDES})
SET(DIRS ${DIRS} -I${D})
ENDFOREACH(D ${QT_INCLUDES})
SET(QT_INCLUDES ${DIRS})
SET(QT_INCLUDES ${QT_INCLUDES} -DQT_THREAD_SUPPORT)
SET(QT_MT_LIBS ${QT_LIBRARIES} ${QT_QTXML_LIBRARY} ${QT_QTOPENGL_LIBRARY})

FIND_PROGRAM(QT_LRELEASE_EXECUTABLE lrelease)
