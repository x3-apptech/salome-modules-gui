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
#  Author : Alexander SOLOVYOV (OCN)
#  Module : QDS
#  $Header: /home/server/cvs/GUI/GUI_SRC/src/QDS/Makefile.in
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libQDS.la

# header files 

#MKR: already migrated to Qt4 files
salomeinclude_HEADERS =		\
	QDS.h            	\
	QDS_CheckBox.h 		\
	QDS_ComboBox.h 		\
	QDS_Datum.h 		\
	QDS_LineEdit.h 		\
	QDS_SpinBox.h 		\
	QDS_SpinBoxDbl.h 	\
	QDS_TextEdit.h 		\
	QDS_Validator.h 	\
	QDS_RadioBox.h

#MKR: not yet migrated to Qt4 files
#				\
	QDS_Table.h

#MKR: already migrated to Qt4 files
dist_libQDS_la_SOURCES = 	\
	QDS.cxx 		\
	QDS_CheckBox.cxx 	\
	QDS_ComboBox.cxx 	\
	QDS_Datum.cxx 		\
	QDS_LineEdit.cxx 	\
	QDS_SpinBox.cxx 	\
	QDS_SpinBoxDbl.cxx 	\
	QDS_TextEdit.cxx 	\
	QDS_Validator.cxx 	\
	QDS_RadioBox.cxx

#MKR: not yet migrated to Qt4 files
#				\
	QDS_Table.cxx

#MKR: already migrated to Qt4 files
MOC_FILES =			\
	QDS_CheckBox_moc.cxx	\
	QDS_ComboBox_moc.cxx	\
	QDS_Datum_moc.cxx	\
	QDS_LineEdit_moc.cxx	\
	QDS_SpinBox_moc.cxx	\
	QDS_SpinBoxDbl_moc.cxx	\
	QDS_TextEdit_moc.cxx	\
	QDS_RadioBox_moc.cxx

#MKR: not yet migrated to Qt4 files
#				\
	QDS_Table_moc.cxx

nodist_libQDS_la_SOURCES = $(MOC_FILES)

nodist_salomeres_DATA = QDS_msg_en.qm

libQDS_la_CPPFLAGS = $(QT_INCLUDES) $(CAS_CPPFLAGS) \
                   -I$(srcdir)/../Qtx -I$(srcdir)/../DDS

libQDS_la_LDFLAGS  = $(QT_MT_LIBS) $(CAS_KERNEL)
libQDS_la_LIBADD   = ../DDS/libDDS.la ../Qtx/libqtx.la

