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
#  Author : Alexander SOLOVYOV (OCN)
#  Module : QDS
#  $Header: /home/server/cvs/GUI/GUI_SRC/src/QDS/Makefile.in

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = libQDS.la

# header files 
salomeinclude_HEADERS= QDS.h \
	QDS_CheckBox.h \
	QDS_ComboBox.h \
	QDS_Datum.h \
	QDS_LineEdit.h \
	QDS_SpinBox.h \
	QDS_SpinBoxDbl.h \
	QDS_TextEdit.h \
	QDS_Validator.h

dist_libQDS_la_SOURCES= \
	QDS.cxx \
	QDS_CheckBox.cxx \
	QDS_ComboBox.cxx \
	QDS_Datum.cxx \
	QDS_LineEdit.cxx \
	QDS_SpinBox.cxx \
	QDS_SpinBoxDbl.cxx \
	QDS_TextEdit.cxx \
	QDS_Validator.cxx
 
MOC_FILES= \
	QDS_CheckBox_moc.cxx \
	QDS_ComboBox_moc.cxx \
	QDS_Datum_moc.cxx \
	QDS_LineEdit_moc.cxx \
	QDS_SpinBox_moc.cxx \
	QDS_SpinBoxDbl_moc.cxx \
	QDS_TextEdit_moc.cxx
nodist_libQDS_la_SOURCES= $(MOC_FILES)

# may be neccessary for the external projects or future improvements
EXTRA_DIST+= QDS_RadioBox.cxx QDS_RadioBox.h resources/QDS_msg_en.po

libQDS_la_CPPFLAGS=$(QT_INCLUDES) $(CAS_CPPFLAGS) \
                   -I$(srcdir)/../Qtx -I$(srcdir)/../DDS

libQDS_la_LDFLAGS=$(QT_MT_LIBS) $(CAS_KERNEL)
libQDS_la_LIBADD=../DDS/libDDS.la ../Qtx/libqtx.la

