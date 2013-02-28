# Copyright (C) 2010-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
# Author : Guillaume Boulant (EDF/R&D) 

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Program targets
bin_PROGRAMS = qtester gtester

# QDialog uic files
UIC_FILES_QDIALOG = \
	ui_QDialogTest.h

# QDialog moc files
MOC_FILES_QDIALOG = \
	QDialogTest_moc.cxx

# GDialog uic files
UIC_FILES_GDIALOG= \
	ui_GenericDialog.h \
	ui_GDialogTest.h

# GDialog moc files
MOC_FILES_GDIALOG= \
	GenericDialog_moc.cxx \
	GDialogTest_moc.cxx

QDIALOG_TEMPLATES = __QDIALOG__.ui __QDIALOG__.h __QDIALOG__.cxx
GDIALOG_TEMPLATES = __GDIALOG__.ui __GDIALOG__.h __GDIALOG__.cxx
QDIALOG_FILES = QDialogTest.h QDialogTest.cxx QDialogTest.ui
GDIALOG_FILES = GDialogTest.h GDialogTest.cxx GDialogTest.ui 

# generated sources
BUILT_SOURCES = $(UIC_FILES_QDIALOG) $(UIC_FILES_GDIALOG)

# extra distributed files
EXTRA_DIST += $(QDIALOG_TEMPLATES) $(GDIALOG_TEMPLATES) GenericDialog.ui README.txt dlgfactory.py

mostlyclean-local:
	rm -f $(QDIALOG_FILES) $(GDIALOG_FILES)
	rm -f @builddir@/*_moc.cxx @builddir@/ui_*.h

$(QDIALOG_FILES): $(QDIALOG_TEMPLATES) dlgfactory.py
	$(srcdir)/dlgfactory.py -s -n QDialogTest -t qdialog

$(GDIALOG_FILES): $(GDIALOG_TEMPLATES) dlgfactory.py
	$(srcdir)/dlgfactory.py -s -n GDialogTest -t gdialog

QT_CXXFLAGS=@QT_INCLUDES@ @QT_MT_INCLUDES@
QT_LIBS=@QT_LIBS@

# QDialog tester
qtester_SOURCES = \
        qtester.cxx
nodist_qtester_SOURCES =  \
	QDialogTest.cxx \
	$(MOC_FILES_QDIALOG) \
	$(UIC_FILES_QDIALOG)

qtester_CPPFLAGS = \
	$(QT_CXXFLAGS) \
	-I.

qtester_LDFLAGS = \
        $(QT_LIBS)

gtester_SOURCES = \
	gtester.cxx \
	GenericDialog.h \
	GenericDialog.cxx
nodist_gtester_SOURCES = \
	GDialogTest.cxx \
	$(MOC_FILES_GDIALOG) \
	$(UIC_FILES_GDIALOG)

gtester_CPPFLAGS = \
	$(QT_CXXFLAGS) \
	-I@srcdir@ \
	-I.

gtester_LDFLAGS = \
        $(QT_LIBS)
