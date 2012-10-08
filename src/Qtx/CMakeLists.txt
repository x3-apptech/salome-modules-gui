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
)

SET(GUI_HEADERS   
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
  QtxFontEdit.h
  QtxGridBox.h
  QtxGroupBox.h
  QtxIntSpinBox.h
  QtxListAction.h
  QtxTreeView.h
  QtxLogoMgr.h
  QtxMainWindow.h
  QtxMenu.h
  QtxMultiAction.h
  QtxMRUAction.h
  QtxPagePrefMgr.h
  QtxPathDialog.h
  QtxPathEdit.h
  QtxPathListEdit.h
  QtxPopupMgr.h
  QtxRubberBand.h
  QtxSearchTool.h
  QtxShortcutEdit.h
  QtxSplash.h
  QtxToolBar.h
  QtxToolTip.h
  QtxValidator.h
  QtxWorkspace.h
  QtxWorkspaceAction.h
  QtxWorkstack.h
  QtxWorkstackAction.h
  QtxWebBrowser.h
)
QT4_WRAP_CPP(GUI_HEADERS_MOC ${GUI_HEADERS})

SET(qtx_SOURCES
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
  QtxEvalExpr.cxx
  QtxFontEdit.cxx
  QtxGridBox.cxx
  QtxGroupBox.cxx
  QtxIntSpinBox.cxx
  QtxListAction.cxx
  QtxTreeView.cxx
  QtxLogoMgr.cxx
  QtxMainWindow.cxx
  QtxMenu.cxx
  QtxMultiAction.cxx
  QtxMRUAction.cxx
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
  QtxSplash.cxx
  QtxToolBar.cxx
  QtxToolTip.cxx
  QtxTranslator.cxx
  QtxValidator.cxx
  QtxWorkspace.cxx
  QtxWorkspaceAction.cxx
  QtxWorkstack.cxx
  QtxWorkstackAction.cxx
  QtxWebBrowser.cxx
)
SET(GUITS_SOURCES
  resources/Qtx_msg_fr.ts
)

ADD_LIBRARY(qtx SHARED ${qtx_SOURCES} ${GUI_HEADERS_MOC})
SET_TARGET_PROPERTIES(qtx PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${PLATFORM_DEFINITIONS}")
TARGET_LINK_LIBRARIES(qtx ${QT_MT_LIBS} ${OGL_LIBS})
INSTALL(TARGETS qtx DESTINATION ${GUI_salomelib_LIBS})

SET(COMMON_HEADERS_H
  Qtx.h
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
  QtxEvalExpr.h
  QtxFontEdit.h
  QtxGridBox.h
  QtxGroupBox.h
  QtxIntSpinBox.h
  QtxListAction.h
  QtxTreeView.h
  QtxLogoMgr.h
  QtxMainWindow.h
  QtxMap.h
  QtxMenu.h
  QtxMultiAction.h
  QtxMRUAction.h
  QtxPagePrefMgr.h
  QtxPathDialog.h
  QtxPathEdit.h
  QtxPathListEdit.h
  QtxPopupMgr.h
  QtxPreferenceMgr.h
  QtxResourceMgr.h
  QtxRubberBand.h
  QtxSearchTool.h
  QtxShortcutEdit.h
  QtxSplash.h
  QtxToolBar.h
  QtxToolTip.h
  QtxTranslator.h
  QtxValidator.h
  QtxWorkspace.h
  QtxWorkspaceAction.h
  QtxWorkstack.h
  QtxWorkstackAction.h
  QtxWebBrowser.h
)
INSTALL(FILES ${COMMON_HEADERS_H} DESTINATION ${GUI_salomeinclude_HEADERS})
QT4_INSTALL_TS_RESOURCES("${GUITS_SOURCES}" "${GUI_salomeres_DATA}")
