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

SET(QWTHOME $ENV{QWTHOME})
FIND_PATH(QWT_INCLUDE_DIR qwt_plot.h ${QWTHOME}/include)
SET(QWT_INCLUDES -I${QWT_INCLUDE_DIR})
IF(WINDOWS)
SET(QWT_INCLUDES ${QWT_INCLUDES} -DQWT_DLL)
ENDIF(WINDOWS)
IF(WINDOWS)
FIND_LIBRARY(QWT_LIBS qwt5 ${QWTHOME}/lib)
ELSE(WINDOWS)
FIND_LIBRARY(QWT_LIBS qwt ${QWTHOME}/lib)
ENDIF(WINDOWS)
