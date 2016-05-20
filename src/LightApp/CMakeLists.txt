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

# --- options ---

# additional include directories
INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  ${HDF5_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/CAM
  ${PROJECT_SOURCE_DIR}/src/CASCatch
  ${PROJECT_SOURCE_DIR}/src/Event
  ${PROJECT_SOURCE_DIR}/src/LogWindow
  ${PROJECT_SOURCE_DIR}/src/ObjBrowser
  ${PROJECT_SOURCE_DIR}/src/Prs
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/STD
  ${PROJECT_SOURCE_DIR}/src/SUIT
  ${PROJECT_SOURCE_DIR}/src/Style
)
IF(SALOME_USE_SALOMEOBJECT)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/OBJECT)
ENDIF()
IF(SALOME_USE_GLVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/GLViewer)
ENDIF()
IF(SALOME_USE_GRAPHICSVIEW)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/GraphicsView)
ENDIF()
IF(SALOME_USE_PYVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/PyEditor)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/PyViewer)
ENDIF()
IF(SALOME_USE_OCCVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/OCCViewer)
  IF(SALOME_USE_SALOMEOBJECT)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/SOCC)
  ENDIF()
ENDIF()
IF(SALOME_USE_PLOT2DVIEWER)
  INCLUDE_DIRECTORIES(${QWT_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Plot2d)
  IF(SALOME_USE_SALOMEOBJECT)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/SPlot2d)
  ENDIF()
ENDIF()
IF(SALOME_USE_VTKVIEWER)
  INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/VTKViewer)
  IF(SALOME_USE_SALOMEOBJECT)
    INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/SVTK)
  ENDIF()
ENDIF()
IF(SALOME_USE_QXGRAPHVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/QxScene)
ENDIF()
IF(SALOME_USE_PVVIEWER)
  INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/PVViewer)
ENDIF()
IF(SALOME_USE_PYCONSOLE)
  INCLUDE_DIRECTORIES(
    ${PYTHON_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/tools/PyConsole/src
    ${PROJECT_SOURCE_DIR}/tools/PyInterp/src
    ${PROJECT_SOURCE_DIR}/src/SUITApp
  )
ENDIF()

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${CAS_DEFINITIONS} ${QT_DEFINITIONS} ${HDF5_DEFINITIONS})
IF(SALOME_USE_PLOT2DVIEWER)
  ADD_DEFINITIONS(${QWT_DEFINITIONS})
ENDIF()
IF(SALOME_USE_PYCONSOLE)
  ADD_DEFINITIONS(${PYTHON_DEFINITIONS})
ENDIF()

# libraries to link to
SET(_link_LIBRARIES
  ${CAS_KERNEL}
  ${QT_LIBRARIES}
  ${HDF5_LIBRARIES}
  CASCatch qtx suit std SalomeStyle SalomePrs CAM LogWindow ObjBrowser Event 
  ${KERNEL_SalomeHDFPersist} ${KERNEL_SALOMELocalTrace}
)
IF(SALOME_USE_SALOMEOBJECT)
  LIST(APPEND _link_LIBRARIES SalomeObject)
ENDIF()
IF(SALOME_USE_GLVIEWER)
  LIST(APPEND _link_LIBRARIES GLViewer)
ENDIF()
IF(SALOME_USE_GRAPHICSVIEW)
  LIST(APPEND _link_LIBRARIES GraphicsView)
ENDIF()
IF(SALOME_USE_PYVIEWER)
  LIST(APPEND _link_LIBRARIES PyEditor PyViewer)
ENDIF()
IF(SALOME_USE_OCCVIEWER)
  LIST(APPEND _link_LIBRARIES OCCViewer)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _link_LIBRARIES SOCC)
  ENDIF()
ENDIF()
IF(SALOME_USE_PLOT2DVIEWER)
  LIST(APPEND _link_LIBRARIES Plot2d)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _link_LIBRARIES SPlot2d)
  ENDIF()
ENDIF()
IF(SALOME_USE_VTKVIEWER)
  LIST(APPEND _link_LIBRARIES VTKViewer)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _link_LIBRARIES SVTK)
  ENDIF()
ENDIF()
IF(SALOME_USE_QXGRAPHVIEWER)
  LIST(APPEND _link_LIBRARIES QxScene)
ENDIF()
IF(SALOME_USE_PVVIEWER)
  LIST(APPEND _link_LIBRARIES PVViewer)
ENDIF()
IF(SALOME_USE_PYCONSOLE)
  LIST(APPEND _link_LIBRARIES PyInterp PyConsole SUITApp)
ENDIF()

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  LightApp_AboutDlg.h
  LightApp_Application.h
  LightApp_DataModel.h
  LightApp_Dialog.h
  LightApp_Module.h
  LightApp_ModuleAction.h
  LightApp_ModuleDlg.h
  LightApp_NameDlg.h
  LightApp_OBSelector.h
  LightApp_Operation.h
  LightApp_Preferences.h
  LightApp_PreferencesDlg.h
  LightApp_SelectionMgr.h
  LightApp_ShowHideOp.h
  LightApp_Study.h
  LightApp_SwitchOp.h
  LightApp_WgViewModel.h
)
IF(SALOME_USE_GLVIEWER)
  LIST(APPEND _moc_HEADERS LightApp_GLSelector.h)
ENDIF()
IF(SALOME_USE_GRAPHICSVIEW)
  LIST(APPEND _moc_HEADERS LightApp_GVSelector.h)
ENDIF()
IF(SALOME_USE_OCCVIEWER)
  LIST(APPEND _moc_HEADERS LightApp_OCCSelector.h)
ENDIF()
IF(SALOME_USE_PLOT2DVIEWER)
  LIST(APPEND _moc_HEADERS LightApp_Plot2dSelector.h)
ENDIF()
IF(SALOME_USE_VTKVIEWER)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _moc_HEADERS LightApp_VTKSelector.h)
  ENDIF()
ENDIF()
IF(SALOME_USE_PYCONSOLE)
  LIST(APPEND _moc_HEADERS LightApp_PyEditor.h)
ENDIF()

# header files / no moc processing
SET(_other_HEADERS
  LightApp.h
  LightApp_DataObject.h
  LightApp_DataOwner.h
  LightApp_DataSubOwner.h
  LightApp_Displayer.h
  LightApp_Driver.h
  LightApp_EventFilter.h
  LightApp_FileValidator.h
  LightApp_FullScreenHelper.h
  LightApp_HDFDriver.h
  LightApp_Selection.h
  LightApp_UpdateFlags.h
)

# header files / to install
SET(LightApp_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by rcc
SET(_rcc_RESOURCES LightApp.qrc)

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/LightApp_images.ts
  resources/LightApp_msg_en.ts
  resources/LightApp_msg_fr.ts
  resources/LightApp_msg_ja.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/icon_about.png
  resources/icon_applogo.png
  resources/icon_default.png
  resources/icon_module.png
  resources/icon_module_big.png
  resources/icon_select.png
  resources/LightApp.ini
  resources/LightApp.xml
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  LightApp_AboutDlg.cxx
  LightApp_Application.cxx
  LightApp_DataModel.cxx
  LightApp_DataObject.cxx
  LightApp_DataOwner.cxx
  LightApp_DataSubOwner.cxx
  LightApp_Dialog.cxx
  LightApp_Displayer.cxx
  LightApp_Driver.cxx
  LightApp_EventFilter.cxx
  LightApp_FileValidator.cxx
  LightApp_FullScreenHelper.cxx
  LightApp_HDFDriver.cxx
  LightApp_Module.cxx
  LightApp_ModuleAction.cxx
  LightApp_ModuleDlg.cxx
  LightApp_NameDlg.cxx
  LightApp_OBSelector.cxx
  LightApp_Operation.cxx
  LightApp_Preferences.cxx
  LightApp_PreferencesDlg.cxx
  LightApp_Selection.cxx
  LightApp_SelectionMgr.cxx
  LightApp_ShowHideOp.cxx
  LightApp_Study.cxx
  LightApp_SwitchOp.cxx
  LightApp_WgViewModel.cxx
)
IF(SALOME_USE_GLVIEWER)
  LIST(APPEND _other_SOURCES LightApp_GLSelector.cxx)
ENDIF()
IF(SALOME_USE_GRAPHICSVIEW)
  LIST(APPEND _other_SOURCES LightApp_GVSelector.cxx)
ENDIF()
IF(SALOME_USE_OCCVIEWER)
  LIST(APPEND _other_SOURCES LightApp_OCCSelector.cxx)
ENDIF()
IF(SALOME_USE_PLOT2DVIEWER)
  LIST(APPEND _other_SOURCES LightApp_Plot2dSelector.cxx)
ENDIF()
IF(SALOME_USE_VTKVIEWER)
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND _other_SOURCES LightApp_VTKSelector.cxx)
  ENDIF()
ENDIF()
IF(SALOME_USE_PYCONSOLE)
  LIST(APPEND _other_SOURCES LightApp_PyEditor.cxx)
ENDIF()

# sources / to compile
SET(LightApp_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_rcc_SOURCES})

# --- rules ---

ADD_LIBRARY(LightApp ${LightApp_SOURCES})
TARGET_LINK_LIBRARIES(LightApp ${_link_LIBRARIES})
INSTALL(TARGETS LightApp EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${LightApp_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})
