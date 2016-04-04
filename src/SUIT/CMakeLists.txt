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
  ${QT_INCLUDES}
  ${PROJECT_SOURCE_DIR}/src/Qtx
  ${PROJECT_SOURCE_DIR}/src/ObjBrowser
)

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${PLATFORM_LIBS} ${QT_LIBRARIES} qtx ObjBrowser)

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  SUIT_Accel.h  
  SUIT_ActionOperation.h 
  SUIT_Application.h 
  SUIT_DataBrowser.h 
  SUIT_DataObject.h 
  SUIT_Desktop.h 
  SUIT_FileDlg.h 
  SUIT_LicenseDlg.h
  SUIT_MessageBox.h
  SUIT_Operation.h 
  SUIT_PopupClient.h 
  SUIT_PreferenceMgr.h 
  SUIT_SelectionMgr.h 
  SUIT_Session.h 
  SUIT_ShortcutMgr.h
  SUIT_Study.h
  SUIT_TreeModel.h 
  SUIT_ViewManager.h 
  SUIT_ViewModel.h 
  SUIT_ViewWindow.h 
)

# header files / no moc processing
SET(_other_HEADERS
  SUIT.h
  SUIT_CameraProperties.h
  SUIT_DataObjectIterator.h
  SUIT_DataObjectKey.h
  SUIT_DataOwner.h
  SUIT_ExceptionHandler.h
  SUIT_FileValidator.h
  SUIT_OverrideCursor.h
  SUIT_ResourceMgr.h
  SUIT_SelectionFilter.h
  SUIT_Selector.h
  SUIT_SmartPtr.h
  SUIT_Tools.h
  SUIT_TreeSync.h
)

# header files / to install
SET(suit_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by lrelease
SET(_ts_RESOURCES
  resources/SUIT_msg_en.ts
  resources/SUIT_msg_fr.ts
  resources/SUIT_msg_ja.ts
  resources/SUIT_images.ts
)

# resource files / static
SET(_other_RESOURCES
  resources/icon_visibility_on.png
  resources/icon_visibility_off.png
  resources/view_sync.png
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / static
SET(_other_SOURCES
  SUIT_Accel.cxx
  SUIT_ActionOperation.cxx
  SUIT_Application.cxx
  SUIT_CameraProperties.cxx
  SUIT_DataBrowser.cxx
  SUIT_DataObject.cxx
  SUIT_DataObjectIterator.cxx
  SUIT_DataObjectKey.cxx
  SUIT_DataOwner.cxx
  SUIT_Desktop.cxx
  SUIT_ExceptionHandler.cxx
  SUIT_FileDlg.cxx
  SUIT_FileValidator.cxx
  SUIT_LicenseDlg.cxx
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
  SUIT_ShortcutMgr.cxx
  SUIT_Study.cxx
  SUIT_Tools.cxx
  SUIT_TreeModel.cxx
  SUIT_ViewManager.cxx
  SUIT_ViewModel.cxx
  SUIT_ViewWindow.cxx
)

# sources / to compile
SET(suit_SOURCES ${_other_SOURCES} ${_moc_SOURCES})

# --- rules ---

ADD_LIBRARY(suit ${suit_SOURCES})
TARGET_LINK_LIBRARIES(suit ${_link_LIBRARIES})
INSTALL(TARGETS suit EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${suit_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")

INSTALL(FILES ${_other_RESOURCES} DESTINATION ${SALOME_GUI_INSTALL_RES_DATA})

