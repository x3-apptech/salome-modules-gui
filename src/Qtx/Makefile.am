#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : suit
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files

#VSR: already migrated to Qt4 files
salomeinclude_HEADERS =		\
	Qtx.h			\
	QtxAction.h		\
	QtxActionGroup.h	\
	QtxActionMenuMgr.h	\
	QtxActionMgr.h		\
	QtxActionSet.h		\
	QtxActionToolMgr.h	\
	QtxColorButton.h	\
	QtxColorScale.h		\
	QtxComboBox.h		\
	QtxDialog.h		\
	QtxDockAction.h		\
	QtxDockWidget.h		\
	QtxDoubleSpinBox.h	\
	QtxEvalExpr.h		\
	QtxFontEdit.h		\
	QtxGridBox.h		\
	QtxGroupBox.h		\
	QtxIntSpinBox.h		\
	QtxListAction.h		\
	QtxTreeView.h		\
	QtxLogoMgr.h		\
	QtxMainWindow.h 	\
	QtxMap.h		\
	QtxMenu.h		\
	QtxMultiAction.h	\
	QtxMRUAction.h		\
	QtxPagePrefMgr.h	\
	QtxPathDialog.h		\
	QtxPathEdit.h		\
	QtxPathListEdit.h	\
	QtxPopupMgr.h		\
	QtxPreferenceMgr.h	\
	QtxResourceMgr.h	\
	QtxRubberBand.h         \
	QtxSearchTool.h		\
	QtxSplash.h		\
	QtxToolBar.h		\
	QtxToolTip.h		\
	QtxTranslator.h		\
	QtxValidator.h		\
	QtxWorkspace.h		\
	QtxWorkspaceAction.h	\
	QtxWorkstack.h		\
	QtxWorkstackAction.h

#VSR: not yet migrated to Qt4 files
#				\
	QtxListBox.h		\
	QtxTable.h

# Libraries targets

lib_LTLIBRARIES = libqtx.la

#VSR: already migrated to Qt4 files
dist_libqtx_la_SOURCES =	\
	Qtx.cxx			\
	QtxAction.cxx		\
	QtxActionGroup.cxx	\
	QtxActionMenuMgr.cxx	\
	QtxActionMgr.cxx	\
	QtxActionSet.cxx	\
	QtxActionToolMgr.cxx	\
	QtxColorButton.cxx	\
	QtxColorScale.cxx	\
	QtxComboBox.cxx		\
	QtxDialog.cxx		\
	QtxDockAction.cxx	\
	QtxDockWidget.cxx	\
	QtxDoubleSpinBox.cxx	\
	QtxEvalExpr.cxx		\
	QtxFontEdit.cxx		\
	QtxGridBox.cxx		\
	QtxGroupBox.cxx		\
	QtxIntSpinBox.cxx	\
	QtxListAction.cxx	\
	QtxTreeView.cxx		\
	QtxLogoMgr.cxx		\
	QtxMainWindow.cxx	\
	QtxMenu.cxx		\
	QtxMultiAction.cxx	\
	QtxMRUAction.cxx	\
	QtxPagePrefMgr.cxx	\
	QtxPathDialog.cxx	\
	QtxPathEdit.cxx		\
	QtxPathListEdit.cxx	\
	QtxPopupMgr.cxx		\
	QtxPreferenceMgr.cxx	\
	QtxResourceMgr.cxx	\
	QtxRubberBand.cxx       \
	QtxSearchTool.cxx	\
	QtxSplash.cxx		\
	QtxToolBar.cxx		\
	QtxToolTip.cxx		\
	QtxTranslator.cxx	\
	QtxValidator.cxx	\
	QtxWorkspace.cxx	\
	QtxWorkspaceAction.cxx	\
	QtxWorkstack.cxx	\
	QtxWorkstackAction.cxx

#VSR: not yet migrated to Qt4 files
#				\
	QtxListBox.cxx		\
	QtxTable.cxx

#VSR: already migrated to Qt4 files
MOC_FILES =				\
	QtxAction_moc.cxx		\
	QtxActionGroup_moc.cxx		\
	QtxActionMenuMgr_moc.cxx	\
	QtxActionMgr_moc.cxx		\
	QtxActionSet_moc.cxx		\
	QtxActionToolMgr_moc.cxx	\
	QtxColorButton_moc.cxx		\
	QtxColorScale_moc.cxx		\
	QtxComboBox_moc.cxx		\
	QtxDialog_moc.cxx		\
	QtxDockAction_moc.cxx		\
	QtxDockWidget_moc.cxx		\
	QtxDoubleSpinBox_moc.cxx	\
	QtxFontEdit_moc.cxx		\
	QtxGridBox_moc.cxx		\
	QtxGroupBox_moc.cxx		\
	QtxIntSpinBox_moc.cxx		\
	QtxListAction_moc.cxx		\
	QtxTreeView_moc.cxx		\
	QtxLogoMgr_moc.cxx		\
	QtxMainWindow_moc.cxx		\
	QtxMenu_moc.cxx 		\
	QtxMultiAction_moc.cxx		\
	QtxMRUAction_moc.cxx		\
	QtxPagePrefMgr_moc.cxx		\
	QtxPathDialog_moc.cxx		\
	QtxPathEdit_moc.cxx		\
	QtxPathListEdit_moc.cxx		\
	QtxPopupMgr_moc.cxx		\
	QtxRubberBand_moc.cxx           \
	QtxSearchTool_moc.cxx		\
	QtxSplash_moc.cxx		\
	QtxToolBar_moc.cxx		\
	QtxToolTip_moc.cxx		\
	QtxValidator_moc.cxx		\
	QtxWorkspace_moc.cxx		\
	QtxWorkspaceAction_moc.cxx	\
	QtxWorkstack_moc.cxx		\
	QtxWorkstackAction_moc.cxx

#VSR: not yet migrated to Qt4 files
#					\
	QtxListBox_moc.cxx		\
	QtxTable_moc.cxx

nodist_libqtx_la_SOURCES = $(MOC_FILES)

libqtx_la_CPPFLAGS = $(QT_INCLUDES)
libqtx_la_LDFLAGS  = $(QT_MT_LIBS)
