# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

ADD_SUBDIRECTORY(pluginsdemo)

IF(CPPUNIT_IS_OK)
  IF(GUI_ENABLE_CORBA)
    ADD_SUBDIRECTORY(Test)
  ENDIF(GUI_ENABLE_CORBA)
ENDIF(CPPUNIT_IS_OK)

INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/UseQT4EXT.cmake)
INCLUDE(${KERNEL_ROOT_DIR}/salome_adm/cmake_files/SalomeMacros.cmake)

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${QWT_INCLUDE_DIRS}
  ${BOOST_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${OMNIORB_INCLUDE_DIRS}
  ${LIBXML_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_CURRENT_SOURCE_DIR}/../LightApp
  ${CMAKE_CURRENT_SOURCE_DIR}/../CAM
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/../SVTK
  ${CMAKE_CURRENT_SOURCE_DIR}/../STD
  ${CMAKE_CURRENT_SOURCE_DIR}/../VTKViewer
  ${CMAKE_CURRENT_SOURCE_DIR}/../ObjBrowser
  ${CMAKE_CURRENT_SOURCE_DIR}/../PyConsole
  ${CMAKE_CURRENT_SOURCE_DIR}/../TOOLSGUI
  ${CMAKE_CURRENT_SOURCE_DIR}/../PyInterp
  ${CMAKE_CURRENT_SOURCE_DIR}/../Session
  ${CMAKE_BINARY_DIR}/idl
  ${CMAKE_CURRENT_SOURCE_DIR}/../Event
  ${CMAKE_CURRENT_SOURCE_DIR}/../CASCatch
  ${CMAKE_CURRENT_SOURCE_DIR}/../Prs
)

SET(COMMON_FLAGS "${QT_DEFINITIONS} ${LIBXML_DEFINITIONS} ${CORBA_DEFINITIONS} ${VTK_DEFINITIONS} ${QWT_DEFINITIONS} ${CAS_DEFINITIONS} ${BOOST_DEFINITIONS} ${PYTHON_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
SET(COMMON_LIBS 
  ${PYTHON_LIBS}
  ${QT_MT_LIBS}
  ${OpUtil}
  ${SALOMELocalTrace}
  ${SalomeDSClient}
  suit
  std
  CAM
  ObjBrowser
  SalomePrs
  SPlot2d
  GLViewer
  OCCViewer
  VTKViewer
  SalomeObject
  SVTK
  SOCC
  PyInterp
  PyConsole
  LogWindow
  LightApp
  ToolsGUI
  SalomeSession
  Event
  CASCatch
  SalomePrs
  ${CAS_KERNEL}
)

SET(GUI_HEADERS   
  SalomeApp_Application.h
  SalomeApp_DataModel.h
  SalomeApp_Module.h
  SalomeApp_LoadStudiesDlg.h
  SalomeApp_Study.h
  SalomeApp_StudyPropertiesDlg.h
  SalomeApp_ListView.h
  SalomeApp_CheckFileDlg.h
  SalomeApp_ExitDlg.h
  SalomeApp_NoteBookDlg.h
  SalomeApp_DoubleSpinBox.h
  SalomeApp_IntSpinBox.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(SalomeApp_SOURCES
  SalomeApp_Module.cxx
  SalomeApp_Application.cxx
  SalomeApp_DataModel.cxx
  SalomeApp_DataObject.cxx
  SalomeApp_LoadStudiesDlg.cxx
  SalomeApp_Study.cxx
  SalomeApp_ExceptionHandler.cxx
  SalomeApp_PyInterp.cxx
  SalomeApp_Tools.cxx
  SalomeApp_ImportOperation.cxx
  SalomeApp_Filter.cxx
  SalomeApp_TypeFilter.cxx
  SalomeApp_StudyPropertiesDlg.cxx
  SalomeApp_ListView.cxx
  SalomeApp_CheckFileDlg.cxx
  SalomeApp_VisualState.cxx
  SalomeApp_ExitDlg.cxx
  SalomeApp_NoteBookDlg.cxx
  SalomeApp_DoubleSpinBox.cxx
  SalomeApp_IntSpinBox.cxx
)

SET(GUITS_SOURCES
  resources/SalomeApp_images.ts
  resources/SalomeApp_msg_en.ts
  resources/SalomeApp_msg_fr.ts
)

ADD_LIBRARY(SalomeApp SHARED ${SalomeApp_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(SalomeApp PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(SalomeApp ${COMMON_LIBS})
INSTALL(TARGETS SalomeApp DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H 
  SalomeApp.h
  SalomeApp_Application.h
  SalomeApp_DataModel.h
  SalomeApp_DataObject.h
  SalomeApp_LoadStudiesDlg.h
  SalomeApp_Module.h
  SalomeApp_Study.h
  SalomeApp_ExceptionHandler.h
  SalomeApp_PyInterp.h
  SalomeApp_Tools.h
  SalomeApp_ImportOperation.h
  SalomeApp_Filter.h
  SalomeApp_TypeFilter.h
  SalomeApp_StudyPropertiesDlg.h
  SalomeApp_CheckFileDlg.h
  SalomeApp_VisualState.h
  SalomeApp_ExitDlg.h
  SalomeApp_NoteBookDlg.h
  SalomeApp_DoubleSpinBox.h
  SalomeApp_IntSpinBox.h
)

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
SET(INST_DATA ${GUIPNG_DATA} resources/SalomeApp.ini resources/SalomeApp.xml)
INSTALL(FILES ${INST_DATA} DESTINATION ${GUI_salomeres_DATA})

# python modules
SALOME_INSTALL_SCRIPTS("salome_pluginsmanager.py" ${GUI_salomepythondir})
SALOME_INSTALL_SCRIPTS("addvars2notebook.py" ${GUI_salomescript_PYTHON})
