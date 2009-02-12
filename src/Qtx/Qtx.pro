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
TEMPLATE = lib
TARGET = qtx
DESTDIR = ../../lib
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

QT += xml
CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32
DEFINES += QTX_EXPORTS

HEADERS  = Qtx.h
HEADERS += QtxAction.h
HEADERS += QtxActionMenuMgr.h
HEADERS += QtxActionMgr.h
HEADERS += QtxActionSet.h
HEADERS += QtxActionToolMgr.h
HEADERS += QtxColorScale.h
HEADERS += QtxComboBox.h
HEADERS += QtxDialog.h
HEADERS += QtxDockAction.h
HEADERS += QtxDockWidget.h
HEADERS += QtxDoubleSpinBox.h
HEADERS += QtxEvalExpr.h
HEADERS += QtxGridBox.h
HEADERS += QtxGroupBox.h
HEADERS += QtxIntSpinBox.h
HEADERS += QtxListAction.h
HEADERS += QtxLogoMgr.h
HEADERS += QtxMainWindow.h
HEADERS += QtxMap.h
HEADERS += QtxMRUAction.h
HEADERS += QtxPathDialog.h
HEADERS += QtxPopupMgr.h
HEADERS += QtxResourceMgr.h
HEADERS += QtxSplash.h
HEADERS += QtxToolBar.h
HEADERS += QtxToolTip.h
HEADERS += QtxValidator.h
HEADERS += QtxWorkspace.h
HEADERS += QtxWorkspaceAction.h
HEADERS += QtxWorkstack.h
HEADERS += QtxWorkstackAction.h
#HEADERS += QtxDirListEditor.h
#HEADERS += QtxListBox.h
#HEADERS += QtxListResourceEdit.h
#HEADERS += QtxListView.h
#HEADERS += QtxPopupMenu.h
#HEADERS += QtxResourceEdit.h
#HEADERS += QtxTable.h

SOURCES  = Qtx.cxx
SOURCES += QtxAction.cxx
SOURCES += QtxActionMenuMgr.cxx
SOURCES += QtxActionMgr.cxx
SOURCES += QtxActionSet.cxx
SOURCES += QtxActionToolMgr.cxx
SOURCES += QtxColorScale.cxx
SOURCES += QtxComboBox.cxx
SOURCES += QtxDialog.cxx
SOURCES += QtxDockAction.cxx
SOURCES += QtxDockWidget.cxx
SOURCES += QtxDoubleSpinBox.cxx
SOURCES += QtxEvalExpr.cxx
SOURCES += QtxGridBox.cxx
SOURCES += QtxGroupBox.cxx
SOURCES += QtxIntSpinBox.cxx
SOURCES += QtxListAction.cxx
SOURCES += QtxLogoMgr.cxx
SOURCES += QtxMainWindow.cxx
SOURCES += QtxMRUAction.cxx
SOURCES += QtxPathDialog.cxx
SOURCES += QtxPopupMgr.cxx
SOURCES += QtxResourceMgr.cxx
SOURCES += QtxSplash.cxx
SOURCES += QtxToolBar.cxx
SOURCES += QtxToolTip.cxx
SOURCES += QtxValidator.cxx
SOURCES += QtxWorkspace.cxx
SOURCES += QtxWorkspaceAction.cxx
SOURCES += QtxWorkstack.cxx
SOURCES += QtxWorkstackAction.cxx
#SOURCES += QtxDirListEditor.cxx
#SOURCES += QtxListBox.cxx
#SOURCES += QtxListResourceEdit.cxx
#SOURCES += QtxListView.cxx
#SOURCES += QtxPopupMenu.cxx
#SOURCES += QtxResourceEdit.cxx
#SOURCES += QtxTable.cxx

includes.files = $$HEADERS
includes.path = ../../include

INSTALLS += includes
