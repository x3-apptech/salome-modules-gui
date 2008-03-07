#  Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#  File   : Makefile.in
#  Author : Vladimir Klyachin (OCN)
#  Module : suit
#  $Header$

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# header files

salomeinclude_HEADERS= Qtx.h \
	QtxAction.h \
	QtxColorScale.h \
	QtxComboBox.h \
	QtxDblSpinBox.h \
	QtxDialog.h \
	QtxDockAction.h \
	QtxDockWindow.h \
	QtxGroupBox.h \
	QtxIntSpinBox.h \
	QtxListAction.h \
	QtxListBox.h \
	QtxListOfOperations.h \
	QtxLogoMgr.h \
	QtxMenuButton.h \
	QtxMRUAction.h \
	QtxOperations.h \
	QtxPathDialog.h \
	QtxPopupMgr.h \
	QtxResourceMgr.h \
	QtxStdOperations.h \
	QtxTable.h \
	QtxToolBar.h \
	QtxToolTip.h \
	QtxParser.h \
	QtxPopupMenu.h \
	QtxActionMgr.h \
	QtxActionMenuMgr.h \
	QtxActionToolMgr.h \
	QtxMainWindow.h \
	QtxWorkspaceAction.h \
	QtxWorkstackAction.h \
	QtxListResourceEdit.h \
	QtxWorkstack.h \
	QtxResourceEdit.h \
	QtxListView.h \
	QtxDirListEditor.h \
	QtxDblValidator.h \
	QtxSplash.h

# Libraries targets

lib_LTLIBRARIES= libqtx.la

dist_libqtx_la_SOURCES= \
	Qtx.cxx \
	QtxAction.cxx \
	QtxColorScale.cxx \
	QtxComboBox.cxx \
	QtxDblSpinBox.cxx \
	QtxDialog.cxx \
	QtxDockAction.cxx \
	QtxDockWindow.cxx \
	QtxGroupBox.cxx \
	QtxIntSpinBox.cxx \
	QtxListAction.cxx \
	QtxListBox.cxx \
	QtxListOfOperations.cxx \
	QtxLogoMgr.cxx \
	QtxMenuButton.cxx \
	QtxMRUAction.cxx \
	QtxPathDialog.cxx \
	QtxPopupMgr.cxx \
	QtxResourceMgr.cxx \
	QtxStdOperations.cxx \
	QtxTable.cxx \
	QtxToolBar.cxx \
	QtxToolTip.cxx \
	QtxOperations.cxx \
	QtxParser.cxx \
	QtxPopupMenu.cxx \
	QtxActionMgr.cxx \
	QtxActionMenuMgr.cxx \
	QtxActionToolMgr.cxx \
	QtxMainWindow.cxx \
	QtxWorkspaceAction.cxx \
	QtxWorkstackAction.cxx \
	QtxListResourceEdit.cxx \
	QtxResourceEdit.cxx \
	QtxWorkstack.cxx \
	QtxListView.cxx \
	QtxDirListEditor.cxx \
	QtxDblValidator.cxx \
	QtxSplash.cxx

MOC_FILES= \
	QtxAction_moc.cxx \
	QtxColorScale_moc.cxx \
	QtxComboBox_moc.cxx \
	QtxDblSpinBox_moc.cxx \
	QtxDialog_moc.cxx \
	QtxDockAction_moc.cxx \
	QtxDockWindow_moc.cxx \
	QtxGroupBox_moc.cxx \
	QtxIntSpinBox_moc.cxx \
	QtxListAction_moc.cxx \
	QtxListBox_moc.cxx \
	QtxLogoMgr_moc.cxx \
	QtxMenuButton_moc.cxx \
	QtxMRUAction_moc.cxx \
	QtxPathDialog_moc.cxx \
	QtxPopupMgr_moc.cxx \
	QtxPopupMenu_moc.cxx \
	QtxTable_moc.cxx \
	QtxToolBar_moc.cxx \
	QtxToolTip_moc.cxx \
	QtxActionMgr_moc.cxx \
	QtxActionMenuMgr_moc.cxx \
	QtxActionToolMgr_moc.cxx \
	QtxMainWindow_moc.cxx \
	QtxWorkspaceAction_moc.cxx \
	QtxWorkstackAction_moc.cxx \
	QtxWorkstack_moc.cxx \
	QtxListView_moc.cxx \
	QtxListResourceEdit_moc.cxx \
	QtxDirListEditor_moc.cxx \
	QtxDblValidator_moc.cxx \
	QtxSplash_moc.cxx
nodist_libqtx_la_SOURCES= $(MOC_FILES)

libqtx_la_CPPFLAGS=$(QT_INCLUDES)

libqtx_la_LDFLAGS=$(QT_MT_LIBS)
