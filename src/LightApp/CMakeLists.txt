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
INCLUDE(${CMAKE_SOURCE_DIR}/adm_local/cmake_files/UseQT4EXT.cmake)

INCLUDE_DIRECTORIES(
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ${HDF5_INCLUDE_DIRS}
  ${PYTHON_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${QWT_INCLUDE_DIRS}
  ${KERNEL_ROOT_DIR}/include/salome
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUIT
  ${CMAKE_CURRENT_SOURCE_DIR}/../STD
  ${CMAKE_CURRENT_SOURCE_DIR}/../CAM
  ${CMAKE_CURRENT_SOURCE_DIR}/../ObjBrowser
  ${CMAKE_CURRENT_SOURCE_DIR}/../LogWindow
  ${CMAKE_CURRENT_SOURCE_DIR}/../Prs
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../Event
  ${CMAKE_CURRENT_SOURCE_DIR}/../Style
  ${CMAKE_CURRENT_SOURCE_DIR}/../CASCatch
)

SET(COMMON_FLAGS "${PYTHON_DEFINITIONS} ${HDF5_DEFINITIONS} ${CAS_DEFINITIONS} ${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
IF(NOT WINDOWS)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -Wno-deprecated")
ENDIF(NOT WINDOWS)

SET(COMMON_LIBS 
  ${QT_LIBS}
  suit
  std
  CAM
  LogWindow
  ${CAS_KERNEL}
  SalomePrs
  ObjBrowser
  SalomeStyle
  Event
  ${HDF5_LIBS}
  ${KERNEL_LDFLAGS}
  ${SalomeHDFPersist}
  ${SALOMELocalTrace}
)

SET(GUI_HEADERS   
  LightApp_AboutDlg.h
  LightApp_Application.h
  LightApp_DataModel.h
  LightApp_Dialog.h
  LightApp_OBSelector.h
  LightApp_Operation.h
  LightApp_Module.h
  LightApp_ModuleAction.h
  LightApp_ModuleDlg.h
  LightApp_NameDlg.h
  LightApp_SelectionMgr.h
  LightApp_ShowHideOp.h
  LightApp_Study.h
  LightApp_SwitchOp.h
  LightApp_Preferences.h
  LightApp_PreferencesDlg.h
  LightApp_WgViewModel.h
)

SET(RCCS LightApp.qrc)

SET(LightApp_SOURCES
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
  LightApp_HDFDriver.cxx
  LightApp_Module.cxx
  LightApp_ModuleAction.cxx
  LightApp_ModuleDlg.cxx
  LightApp_NameDlg.cxx
  LightApp_OBSelector.cxx
  LightApp_Operation.cxx
  LightApp_Selection.cxx
  LightApp_SelectionMgr.cxx
  LightApp_ShowHideOp.cxx
  LightApp_Study.cxx
  LightApp_SwitchOp.cxx
  LightApp_Preferences.cxx
  LightApp_PreferencesDlg.cxx
  LightApp_WgViewModel.cxx
  LightApp_FullScreenHelper.cxx
)

SET(GUITS_SOURCES
  resources/LightApp_images.ts
  resources/LightApp_msg_en.ts
  resources/LightApp_msg_fr.ts
)

SET(COMMON_HEADERS_H 
  LightApp.h
  LightApp_AboutDlg.h
  LightApp_Application.h
  LightApp_DataModel.h
  LightApp_DataObject.h
  LightApp_DataOwner.h
  LightApp_DataSubOwner.h
  LightApp_Dialog.h
  LightApp_Displayer.h
  LightApp_Driver.h
  LightApp_EventFilter.h
  LightApp_HDFDriver.h
  LightApp_Module.h
  LightApp_ModuleAction.h
  LightApp_ModuleDlg.h
  LightApp_NameDlg.h
  LightApp_OBSelector.h
  LightApp_Operation.h
  LightApp_Selection.h
  LightApp_SelectionMgr.h
  LightApp_ShowHideOp.h
  LightApp_Study.h
  LightApp_SwitchOp.h
  LightApp_Preferences.h
  LightApp_PreferencesDlg.h
  LightApp_UpdateFlags.h
  LightApp_WgViewModel.h
  LightApp_FullScreenHelper.h
)

IF(ENABLE_PYCONSOLE)
  INCLUDE_DIRECTORIES(
  ${PYTHON_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../PyConsole
  ${CMAKE_CURRENT_SOURCE_DIR}/../PyInterp
  ${CMAKE_CURRENT_SOURCE_DIR}/../SUITApp
  )
  SET(COMMON_FLAGS "${COMMON_FLAGS} ${PYTHON_DEFINITIONS}")
  SET(COMMON_HEADERS_H ${COMMON_HEADERS_H} LightApp_PyInterp.h)
  SET(LightApp_SOURCES ${LightApp_SOURCES} LightApp_PyInterp.cxx)
  SET(COMMON_LIBS ${COMMON_LIBS} PyInterp PyConsole SUITApp)
ELSE(ENABLE_PYCONSOLE)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_PYCONSOLE")
ENDIF(ENABLE_PYCONSOLE)

IF(ENABLE_VTKVIEWER)
  INCLUDE_DIRECTORIES(
  ${VTK_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/../VTKViewer
  )
  SET(COMMON_LIBS ${COMMON_LIBS} ${VTK_LIBS} VTKViewer)
  IF(ENABLE_SALOMEOBJECT)
    SET(COMMON_HEADERS_H ${COMMON_HEADERS_H} LightApp_VTKSelector.h)
    SET(LightApp_SOURCES ${LightApp_SOURCES} LightApp_VTKSelector.cxx)
    SET(GUI_HEADERS ${GUI_HEADERS} LightApp_VTKSelector.h)
    SET(COMMON_LIBS ${COMMON_LIBS} SVTK)
  ENDIF(ENABLE_SALOMEOBJECT)
ELSE(ENABLE_VTKVIEWER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_VTKVIEWER")
ENDIF(ENABLE_VTKVIEWER)

IF(ENABLE_OCCVIEWER)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../OCCViewer)
  SET(COMMON_HEADERS_H ${COMMON_HEADERS_H} LightApp_OCCSelector.h)
  SET(LightApp_SOURCES ${LightApp_SOURCES} LightApp_OCCSelector.cxx)
  SET(COMMON_LIBS ${COMMON_LIBS} OCCViewer)
  SET(GUI_HEADERS ${GUI_HEADERS} LightApp_OCCSelector.h)
ELSE(ENABLE_OCCVIEWER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_OCCVIEWER")
ENDIF(ENABLE_OCCVIEWER)

IF(ENABLE_GLVIEWER)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../GLViewer)
  SET(COMMON_HEADERS_H ${COMMON_HEADERS_H} LightApp_GLSelector.h)
  SET(LightApp_SOURCES ${LightApp_SOURCES} LightApp_GLSelector.cxx)
  SET(GUI_HEADERS ${GUI_HEADERS} LightApp_GLSelector.h)
  SET(COMMON_LIBS ${COMMON_LIBS} GLViewer)
ELSE(ENABLE_GLVIEWER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_GLVIEWER")
ENDIF(ENABLE_GLVIEWER)

IF(ENABLE_PLOT2DVIEWER)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../Plot2d)
  SET(COMMON_FLAGS "${COMMON_FLAGS} ${QWT_DEFINITIONS}")
  SET(COMMON_HEADERS_H ${COMMON_HEADERS_H} LightApp_Plot2dSelector.h)
  SET(LightApp_SOURCES ${LightApp_SOURCES} LightApp_Plot2dSelector.cxx)
  SET(GUI_HEADERS ${GUI_HEADERS} LightApp_Plot2dSelector.h)
  SET(COMMON_LIBS ${COMMON_LIBS} Plot2d)
ELSE(ENABLE_PLOT2DVIEWER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_PLOT2DVIEWER")
ENDIF(ENABLE_PLOT2DVIEWER)

IF(ENABLE_SUPERVGRAPHVIEWER)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../SUPERVGraph)
  SET(COMMON_LIBS ${COMMON_LIBS} SUPERVGraph)
ELSE(ENABLE_SUPERVGRAPHVIEWER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_SUPERVGRAPHVIEWER")
ENDIF(ENABLE_SUPERVGRAPHVIEWER)

IF(ENABLE_QXGRAPHVIEWER)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../QxScene)
  SET(COMMON_LIBS ${COMMON_LIBS} QxScene)
ELSE(ENABLE_QXGRAPHVIEWER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_QXGRAPHVIEWER")
ENDIF(ENABLE_QXGRAPHVIEWER)

IF(ENABLE_SALOMEOBJECT)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../OBJECT)
  SET(COMMON_LIBS ${COMMON_LIBS} SalomeObject)
  IF(ENABLE_PLOT2DVIEWER)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../SPlot2d)
    SET(COMMON_LIBS ${COMMON_LIBS} SPlot2d)
  ENDIF(ENABLE_PLOT2DVIEWER)
 
  IF(ENABLE_OCCVIEWER)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../SOCC)
    SET(COMMON_LIBS ${COMMON_LIBS} SOCC)
  ENDIF(ENABLE_OCCVIEWER)

  IF(ENABLE_VTKVIEWER)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../SVTK)
  ENDIF(ENABLE_VTKVIEWER)
ELSE(ENABLE_SALOMEOBJECT)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DDISABLE_SALOMEOBJECT")
ENDIF(ENABLE_SALOMEOBJECT)
    
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})
QT4_ADD_RESOURCES(RCC_SRCS ${RCCS})
ADD_LIBRARY(LightApp SHARED ${LightApp_SOURCES} ${GUI_HEADERS_MOC} ${RCC_SRCS})
SET_TARGET_PROPERTIES(LightApp PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(LightApp ${COMMON_LIBS})
INSTALL(TARGETS LightApp DESTINATION ${GUI_salomelib_LIBS})

INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
SET(GUI_DATA ${GUIPNG_DATA} ${CMAKE_CURRENT_SOURCE_DIR}/resources/LightApp.ini ${CMAKE_CURRENT_SOURCE_DIR}/resources/LightApp.xml)
INSTALL(FILES ${GUI_DATA} DESTINATION ${GUI_salomeres_DATA})
