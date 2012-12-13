# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libsuit.la

salomeinclude_HEADERS =			\
	SUIT.h				\
	SUIT_Accel.h			\
	SUIT_ActionOperation.h		\
	SUIT_Application.h		\
	SUIT_DataBrowser.h		\
	SUIT_DataObject.h		\
	SUIT_DataObjectIterator.h	\
	SUIT_DataObjectKey.h		\
	SUIT_DataOwner.h		\
	SUIT_Desktop.h			\
	SUIT_ExceptionHandler.h		\
	SUIT_FileDlg.h			\
	SUIT_FileValidator.h		\
	SUIT_LicenseDlg.h		\
	SUIT_MessageBox.h		\
	SUIT_Operation.h		\
	SUIT_OverrideCursor.h		\
	SUIT_PopupClient.h		\
	SUIT_PreferenceMgr.h		\
	SUIT_ResourceMgr.h		\
	SUIT_SelectionFilter.h		\
	SUIT_SelectionMgr.h		\
	SUIT_Selector.h			\
	SUIT_Session.h 			\
	SUIT_SmartPtr.h			\
	SUIT_Study.h			\
	SUIT_Tools.h			\
	SUIT_TreeSync.h			\
	SUIT_TreeModel.h		\
	SUIT_ViewManager.h		\
	SUIT_ViewModel.h		\
	SUIT_ViewWindow.h		\
	SUIT_ShortcutMgr.h		\
	SUIT_CameraProperties.h

dist_libsuit_la_SOURCES =		\
	SUIT_Accel.cxx			\
	SUIT_ActionOperation.cxx	\
	SUIT_Application.cxx		\
	SUIT_DataBrowser.cxx		\
	SUIT_DataObject.cxx		\
	SUIT_DataObjectIterator.cxx	\
	SUIT_DataObjectKey.cxx		\
	SUIT_DataOwner.cxx		\
	SUIT_Desktop.cxx		\
	SUIT_ExceptionHandler.cxx	\
	SUIT_FileDlg.cxx		\
	SUIT_LicenseDlg.cxx		\
	SUIT_FileValidator.cxx		\
	SUIT_MessageBox.cxx		\
	SUIT_Operation.cxx		\
	SUIT_OverrideCursor.cxx		\
	SUIT_PopupClient.cxx		\
	SUIT_PreferenceMgr.cxx		\
	SUIT_ResourceMgr.cxx		\
	SUIT_SelectionFilter.cxx	\
	SUIT_SelectionMgr.cxx		\
	SUIT_Selector.cxx		\
	SUIT_Session.cxx		\
	SUIT_Study.cxx			\
	SUIT_Tools.cxx			\
	SUIT_TreeModel.cxx		\
	SUIT_ViewManager.cxx		\
	SUIT_ViewModel.cxx		\
	SUIT_ViewWindow.cxx		\
	SUIT_ShortcutMgr.cxx		\
	SUIT_CameraProperties.cxx

MOC_FILES =				\
	SUIT_Accel_moc.cxx		\
	SUIT_ActionOperation_moc.cxx	\
	SUIT_Application_moc.cxx	\
	SUIT_DataBrowser_moc.cxx	\
	SUIT_DataObject_moc.cxx		\
	SUIT_Desktop_moc.cxx		\
	SUIT_LicenseDlg_moc.cxx		\
	SUIT_FileDlg_moc.cxx		\
	SUIT_Operation_moc.cxx		\
	SUIT_PopupClient_moc.cxx	\
	SUIT_PreferenceMgr_moc.cxx	\
	SUIT_SelectionMgr_moc.cxx	\
	SUIT_Selector_moc.cxx		\
	SUIT_Session_moc.cxx		\
	SUIT_Study_moc.cxx		\
	SUIT_TreeModel_moc.cxx		\
	SUIT_ViewManager_moc.cxx	\
	SUIT_ViewModel_moc.cxx		\
	SUIT_ViewWindow_moc.cxx		\
	SUIT_ShortcutMgr_moc.cxx

nodist_libsuit_la_SOURCES = $(MOC_FILES)

nodist_salomeres_DATA =			\
	SUIT_msg_en.qm			\
	SUIT_msg_fr.qm                  \
	SUIT_images.qm

dist_salomeres_DATA =                     \
	resources/icon_visibility_on.png  \
	resources/icon_visibility_off.png \
	resources/view_sync.png

libsuit_la_CPPFLAGS = $(QT_INCLUDES) -I$(srcdir)/../Qtx -I$(srcdir)/../ObjBrowser

if ENABLE_TESTRECORDER
  libsuit_la_CPPFLAGS+= -DENABLE_TESTRECORDER $(TESTRECORDER_INCLUDES)
endif

libsuit_la_LDFLAGS  = $(QT_MT_LIBS)

if ENABLE_TESTRECORDER
  libsuit_la_LDFLAGS+= $(TESTRECORDER_LIBS)
endif

libsuit_la_LIBADD   = ../Qtx/libqtx.la ../ObjBrowser/libObjBrowser.la
