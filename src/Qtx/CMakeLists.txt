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
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# additional preprocessor / compiler flags
ADD_DEFINITIONS(${QT_DEFINITIONS})

# libraries to link to
SET(_link_LIBRARIES ${QT_LIBRARIES} ${OPENGL_LIBRARIES})

# --- headers ---

# header files / to be processed by moc
SET(_moc_HEADERS   
  QtxAction.h
  QtxActionGroup.h
  QtxActionMenuMgr.h
  QtxActionMgr.h
  QtxActionSet.h
  QtxActionToolMgr.h
  QtxBackgroundTool.h
  QtxBiColorTool.h
  QtxColorButton.h
  QtxColorScale.h
  QtxComboBox.h
  QtxDialog.h
  QtxDockAction.h
  QtxDockWidget.h
  QtxDoubleSpinBox.h
  QtxDoubleSpinSlider.h
  QtxFontEdit.h
  QtxGridBox.h
  QtxGroupBox.h
  QtxIntSpinBox.h
  QtxIntSpinSlider.h
  QtxListAction.h
  QtxLogoMgr.h
  QtxMRUAction.h
  QtxMainWindow.h
  QtxMenu.h
  QtxMultiAction.h
  QtxPagePrefMgr.h
  QtxPathDialog.h
  QtxPathEdit.h
  QtxPathListEdit.h
  QtxPopupMgr.h
  QtxRubberBand.h
  QtxSearchTool.h
  QtxShortcutEdit.h
  QtxSlider.h
  QtxSplash.h
  QtxToolBar.h
  QtxToolTip.h
  QtxTreeView.h
  QtxValidator.h
  QtxWorkspace.h
  QtxWorkspaceAction.h
  QtxWorkstack.h
  QtxWorkstackAction.h
)

IF(SALOME_GUI_ENABLE_QTXWEBBROWSER)
  SET(_moc_HEADERS ${_moc_HEADERS}  QtxWebBrowser.h)
ENDIF()

# obsolete files: not migrated to Qt4
# _moc_HEADERS << QtxListBox.h QtxTable.h
# TODO: migrate or remove?

# header files / no moc processing
SET(_other_HEADERS
  Qtx.h
  QtxEvalExpr.h
  QtxMap.h
  QtxPreferenceMgr.h
  QtxResourceMgr.h
  QtxTranslator.h
)

# header files / to install
SET(qtx_HEADERS ${_moc_HEADERS} ${_other_HEADERS})

# --- resources ---

# resource files / to be processed by rcc
SET(_rcc_RESOURCES Qtx.qrc)

# resource files / to be processed by lrelease
SET(_ts_RESOURCES resources/Qtx_msg_fr.ts
                  resources/Qtx_msg_ja.ts
)

# --- sources ---

# sources / moc wrappings
QT_WRAP_MOC(_moc_SOURCES ${_moc_HEADERS})

# sources / rcc wrappings
QT_ADD_RESOURCES(_rcc_SOURCES ${_rcc_RESOURCES})

# sources / static
SET(_other_SOURCES
  Qtx.cxx
  QtxAction.cxx
  QtxActionGroup.cxx
  QtxActionMenuMgr.cxx
  QtxActionMgr.cxx
  QtxActionSet.cxx
  QtxActionToolMgr.cxx
  QtxBackgroundTool.cxx
  QtxBiColorTool.cxx
  QtxColorButton.cxx
  QtxColorScale.cxx
  QtxComboBox.cxx
  QtxDialog.cxx
  QtxDockAction.cxx
  QtxDockWidget.cxx
  QtxDoubleSpinBox.cxx
  QtxDoubleSpinSlider.cxx
  QtxEvalExpr.cxx
  QtxFontEdit.cxx
  QtxGridBox.cxx
  QtxGroupBox.cxx
  QtxIntSpinBox.cxx
  QtxIntSpinSlider.cxx
  QtxListAction.cxx
  QtxLogoMgr.cxx
  QtxMRUAction.cxx
  QtxMainWindow.cxx
  QtxMenu.cxx
  QtxMultiAction.cxx
  QtxPagePrefMgr.cxx
  QtxPathDialog.cxx
  QtxPathEdit.cxx
  QtxPathListEdit.cxx
  QtxPopupMgr.cxx
  QtxPreferenceMgr.cxx
  QtxResourceMgr.cxx
  QtxRubberBand.cxx
  QtxSearchTool.cxx
  QtxShortcutEdit.cxx
  QtxSlider.cxx
  QtxSplash.cxx
  QtxToolBar.cxx
  QtxToolTip.cxx
  QtxTranslator.cxx
  QtxTreeView.cxx
  QtxValidator.cxx
  QtxWorkspace.cxx
  QtxWorkspaceAction.cxx
  QtxWorkstack.cxx
  QtxWorkstackAction.cxx
)

IF(SALOME_GUI_ENABLE_QTXWEBBROWSER)
  SET(_other_SOURCES ${_other_SOURCES}  QtxWebBrowser.cxx)
ENDIF()

# obsolete files: not migrated to Qt4
# _other_SOURCES << QtxListBox.cxx QtxTable.cxx
# TODO: migrate or remove?

# sources / to compile
SET(qtx_SOURCES ${_other_SOURCES} ${_moc_SOURCES} ${_rcc_SOURCES})

# --- rules ---

ADD_LIBRARY(qtx ${qtx_SOURCES})
TARGET_LINK_LIBRARIES(qtx ${_link_LIBRARIES})
INSTALL(TARGETS qtx EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_LIBS})

INSTALL(FILES ${qtx_HEADERS} DESTINATION ${SALOME_INSTALL_HEADERS})
QT_INSTALL_TS_RESOURCES("${_ts_RESOURCES}" "${SALOME_GUI_INSTALL_RES_DATA}")
