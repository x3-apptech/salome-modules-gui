# Copyright (C) 2012-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

# additional include directories
INCLUDE_DIRECTORIES(
  ${QT_INCLUDES}
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES})

# --- headers ---

# header files / to install
SET(ImageComposer_HEADERS
  ImageComposer.h
  ImageComposer_ColorMaskOperator.h
  ImageComposer_CropOperator.h
  ImageComposer_CutOperator.h
  ImageComposer_FuseOperator.h
  ImageComposer_Image.h
  ImageComposer_MetaTypes.h
  ImageComposer_Operator.h
)

# --- sources ---

# sources / to compile
SET(ImageComposer_SOURCES
  ImageComposer_ColorMaskOperator.cxx
  ImageComposer_CropOperator.cxx
  ImageComposer_CutOperator.cxx
  ImageComposer_FuseOperator.cxx
  ImageComposer_Image.cxx
  ImageComposer_Operator.cxx
)

# --- rules ---

ADD_LIBRARY(ImageComposer ${ImageComposer_SOURCES})
TARGET_LINK_LIBRARIES(ImageComposer ${_link_LIBRARIES})
INSTALL(TARGETS ImageComposer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${ImageComposer_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
