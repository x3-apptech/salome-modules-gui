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

ADD_SUBDIRECTORY(pluginsdemo)
IF(SALOME_BUILD_TESTS)
   ADD_SUBDIRECTORY(Test)
ENDIF()

INCLUDE(UseQt4Ext)
INCLUDE(${VTK_USE_FILE})

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${QWT_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${OMNIORB_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}/idl
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/CASCatch
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/LightApp
  ${PROJECT_SOURCE_DIR}/src/OBJECT
  ${PROJECT_SOURCE_DIR}/src/ObjBrowser
  ${PROJECT_SOURCE_DIR}/src/Prs
  ${PROJECT_SOURCE_DIR}/src/PyConsole
  ${PROJECT_SOURCE_DIR}/src/PyInterp
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/SVTK
  ${PROJECT_SOURCE_DIR}/src/Session
  ${PROJECT_SOURCE_DIR}/src/TOOLSGUI
  ${PROJECT_SOURCE_DIR}/src/VTKViewer
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(
  ${QT_DEFINITIONS}
  ${OMNIORB_DEFINITIONS}
  ${QWT_DEFINITIONS}
  ${CAS_DEFINITIONS}
  ${BOOST_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
)

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_KERNEL}
  ${QT_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${KERNEL_OpUtil} ${KERNEL_SALOMELocalTrace} ${KERNEL_SalomeDSClient} ${KERNEL_TOOLSDS}
  CASCatch suit std CAM ObjBrowser SalomePrs PyInterp PyConsole Event
  GLViewer Plot2d OCCViewer VTKViewer SalomeObject SPlot2d SOCC SVTK
  LogWindow LightApp SalomePrs ToolsGUI SalomeSession
)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  SalomeApp_Application.h
  SalomeApp_CheckFileDlg.h
  SalomeApp_DataModel.h
  SalomeApp_DoubleSpinBox.h
  SalomeApp_ExitDlg.h
  SalomeApp_IntSpinBox.h
  SalomeApp_ListView.h
  SalomeApp_LoadStudiesDlg.h
  SalomeApp_Module.h
  SalomeApp_NoteBook.h
  SalomeApp_Study.h
  SalomeApp_StudyPropertiesDlg.h
)

# header files / no moc processing
SET(_other_HEADERS
  SalomeApp.h
  SalomeApp_DataObject.h
  SalomeApp_Engine_i.h
  SalomeApp_ExceptionHandler.h
  SalomeApp_Filter.h
  SalomeApp_ImportOperation.h
  SalomeApp_PyInterp.h
  SalomeApp_Tools.h
  SalomeApp_TypeFilter.h
  SalomeApp_VisualState.h
)

# header files / to install
SET(SalomeApp_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/SalomeApp_images.ts
  resources/SalomeApp_msg_en.ts
  resources/SalomeApp_msg_fr.ts
  resources/SalomeApp_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/SalomeApp.ini
  resources/SalomeApp.xml
)

# --- sources ---

# sources / moc wrappings
QT4_WRAP_CPP(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  SalomeApp_Application.cxx
  SalomeApp_CheckFileDlg.cxx
  SalomeApp_DataModel.cxx
  SalomeApp_DataObject.cxx
  SalomeApp_DoubleSpinBox.cxx
  SalomeApp_Engine_i.cxx
  SalomeApp_ExceptionHandler.cxx
  SalomeApp_ExitDlg.cxx
  SalomeApp_Filter.cxx
  SalomeApp_ImportOperation.cxx
  SalomeApp_IntSpinBox.cxx
  SalomeApp_ListView.cxx
  SalomeApp_LoadStudiesDlg.cxx
  SalomeApp_Module.cxx
  SalomeApp_NoteBook.cxx
  SalomeApp_PyInterp.cxx
  SalomeApp_Study.cxx
  SalomeApp_StudyPropertiesDlg.cxx
  SalomeApp_Tools.cxx
  SalomeApp_TypeFilter.cxx
  SalomeApp_VisualState.cxx
)

# sources / to compile
SET(SalomeApp_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- scripts ---

# scripts / bin
SET(_bin_SCRIPTS
  addvars2notebook.py
)

# scripts / python
SET(_py_SCRIPTS
  salome_pluginsmanager.py
)

# --- rules ---

ADD_LIBRARY(SalomeApp ${SalomeApp_SOURCES})
TARGET_LINK_LIBRARIES(SalomeApp ${_link_LIBRARIES})
INSTALL(TARGETS SalomeApp EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${SalomeApp_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT4_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})

SALOME_INSTALL_SCRIPTS("${_py_SCRIPTS}"  ${SALOME_INSTALL_PYTHON})
SALOME_INSTALL_SCRIPTS("${_bin_SCRIPTS}" ${SALOME_INSTALL_SCRIPT_PYTHON})
