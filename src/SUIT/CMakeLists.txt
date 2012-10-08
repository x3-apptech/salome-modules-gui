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
  ${QT_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../Qtx
  ${CMAKE_CURRENT_SOURCE_DIR}/../ObjBrowser
)

SET(COMMON_LIBS
  qtx
  ObjBrowser
  ${QT_LIBS}
  ${PLATFORM_LIBS}
)
SET(COMMON_FLAGS "${PLATFORM_DEFINITIONS}")
IF(ENABLE_TESTRECORDER)
  SET(COMMON_FLAGS "${COMMON_FLAGS} -DENABLE_TESTRECORDER ${TESTRECORDER_DEFINITIONS}")
  SET(COMMON_LIBS ${COMMON_LIBS} ${TESTRECORDER_LIBS})
ENDIF(ENABLE_TESTRECORDER)

SET(GUI_HEADERS 
  SUIT_Accel.h  
  SUIT_ActionOperation.h 
  SUIT_Application.h 
  SUIT_DataBrowser.h 
  SUIT_DataObject.h 
  SUIT_Desktop.h 
  SUIT_LicenseDlg.h
  SUIT_FileDlg.h 
  SUIT_Operation.h 
  SUIT_PopupClient.h 
  SUIT_PreferenceMgr.h 
  SUIT_SelectionMgr.h 
  SUIT_Selector.h 
  SUIT_Session.h 
  SUIT_Study.h
  SUIT_TreeModel.h 
  SUIT_ViewManager.h 
  SUIT_ViewModel.h 
  SUIT_ViewWindow.h 
  SUIT_ShortcutMgr.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(suit_SOURCES
  SUIT_Accel.cxx
  SUIT_ActionOperation.cxx
  SUIT_Application.cxx
  SUIT_DataBrowser.cxx
  SUIT_DataObject.cxx
  SUIT_DataObjectIterator.cxx
  SUIT_DataObjectKey.cxx
  SUIT_DataOwner.cxx
  SUIT_Desktop.cxx
  SUIT_ExceptionHandler.cxx
  SUIT_FileDlg.cxx
  SUIT_LicenseDlg.cxx
  SUIT_FileValidator.cxx
  SUIT_MessageBox.cxx
  SUIT_Operation.cxx
  SUIT_OverrideCursor.cxx
  SUIT_PopupClient.cxx
  SUIT_PreferenceMgr.cxx
  SUIT_ResourceMgr.cxx
  SUIT_SelectionFilter.cxx
  SUIT_SelectionMgr.cxx
  SUIT_Selector.cxx
  SUIT_Session.cxx
  SUIT_Study.cxx
  SUIT_Tools.cxx
  SUIT_TreeModel.cxx
  SUIT_ViewManager.cxx
  SUIT_ViewModel.cxx
  SUIT_ViewWindow.cxx
  SUIT_ShortcutMgr.cxx
)
SET(GUITS_SOURCES
  resources/SUIT_msg_en.ts
  resources/SUIT_msg_fr.ts
  resources/SUIT_images.ts
)

ADD_LIBRARY(suit SHARED ${suit_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(suit PROPERTIES COMPILE_FLAGS "${COMMON_FLAGS}")
TARGET_LINK_LIBRARIES(suit ${COMMON_LIBS})
INSTALL(TARGETS suit DESTINATION ${GUI_salomelib_LIBS})

FILE(GLOB COMMON_HEADERS_H "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")

FILE(GLOB GUIPNG_DATA "${CMAKE_CURRENT_SOURCE_DIR}/resources/*.png")
INSTALL(FILES ${GUIPNG_DATA} DESTINATION ${GUI_salomeres_DATA})

