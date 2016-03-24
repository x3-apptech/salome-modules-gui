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
INCLUDE(UseQtExt)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

SET(qtester_SOURCES
  qtester.cxx
  )

SET(qtester_HEADERS 
  ${CMAKE_CURRENT_BINARY_DIR}/QDialogTest.h
  )

SET(qtester_FORMS
   ${CMAKE_CURRENT_BINARY_DIR}/QDialogTest.ui
   )

SET(gtester_SOURCES
  gtester.cxx
  GenericDialog.cxx
  )

SET(gtester_HEADERS 
  ${CMAKE_CURRENT_BINARY_DIR}/GDialogTest.h
  )

SET(gtester_FORMS
   ${CMAKE_CURRENT_BINARY_DIR}/GDialogTest.ui
  )

SET(GenericDialog_HEADERS 
  GenericDialog.h
  )

SET(GenericDialog_FORMS
   GenericDialog.ui
   )

ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_CUSTOM_COMMAND( OUTPUT QDialogTest.cxx  QDialogTest.h  QDialogTest.ui
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dlgfactory.py -s -n QDialogTest -t qdialog
  )

ADD_CUSTOM_COMMAND( OUTPUT GDialogTest.cxx  GDialogTest.h  GDialogTest.ui
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dlgfactory.py -s -n GDialogTest -t gdialog
  )

  QT_WRAP_UIC(qtester_FORMS_HEADERS ${qtester_FORMS} )
  QT_WRAP_MOC(qtester_HEADERS_MOC ${qtester_HEADERS})

  QT_WRAP_UIC(gtester_FORMS_HEADERS ${gtester_FORMS} )
  QT_WRAP_MOC(gtester_HEADERS_MOC ${gtester_HEADERS})

  QT_WRAP_UIC(GenericDialog_FORMS_HEADERS ${GenericDialog_FORMS} )
  QT_WRAP_MOC(GenericDialog_HEADERS_MOC ${GenericDialog_HEADERS})

ADD_EXECUTABLE(qtester ${qtester_SOURCES} 
    ${qtester_HEADERS_MOC} 
    ${qtester_FORMS_HEADERS}
    QDialogTest.cxx
    )
    
TARGET_LINK_LIBRARIES(qtester ${QT_LIBRARIES})

ADD_EXECUTABLE(gtester ${gtester_SOURCES} 
    ${gtester_HEADERS_MOC} 
    ${gtester_FORMS_HEADERS}
    ${GenericDialog_HEADERS_MOC} 
    ${GenericDialog_FORMS_HEADERS}
    GDialogTest.cxx
    )
TARGET_LINK_LIBRARIES(gtester ${QT_LIBRARIES})

INSTALL(TARGETS qtester gtester DESTINATION ${SALOME_INSTALL_BINS})
