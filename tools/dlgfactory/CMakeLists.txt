# Copyright (C) 2012-2013  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dlgfactory.py -s -n QDialogTest -t qdialog
  )

ADD_CUSTOM_COMMAND( OUTPUT GDialogTest.cxx  GDialogTest.h  GDialogTest.ui
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/dlgfactory.py -s -n GDialogTest -t gdialog
  )

QT4_WRAP_UI(qtester_FORMS_HEADERS ${qtester_FORMS} )
QT4_WRAP_CPP(qtester_HEADERS_MOC ${qtester_HEADERS})


QT4_WRAP_UI(gtester_FORMS_HEADERS ${gtester_FORMS} )
QT4_WRAP_CPP(gtester_HEADERS_MOC ${gtester_HEADERS})

QT4_WRAP_UI(GenericDialog_FORMS_HEADERS ${GenericDialog_FORMS} )
QT4_WRAP_CPP(GenericDialog_HEADERS_MOC ${GenericDialog_HEADERS})

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

INSTALL(TARGETS qtester gtester DESTINATION ${GUI_salomebin_BINS})
