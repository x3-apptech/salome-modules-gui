# Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either 
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful 
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
#  File   : Makefile.in
#  Author : Nicolas REJNERI
#  Module : SALOME

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Sip flags
SIP_FLAGS = -t WS_X11 -t $(QT_VERS) $(PYQT_SIPFLAGS) -s ".cc" -c . -I $(PYQT_SIPS)

######################

# Library target
lib_LTLIBRARIES= libSalomePyQtGUI.la


# Sip sources
if IS_SIP_V4
SIP_SRC   = sipSalomePyQtGUIcmodule.cc
SIP_FILES = SALOME_PYQT_GUI.sip
else !IS_SIP_V4
SIP_SRC   = SalomePyQtGUIcmodule.cc
SIP_FILES = SALOME_PYQT_GUI.sip
endif

EXTRA_DIST+= $(SIP_FILES)

salomeinclude_HEADERS = \
	SALOME_PYQT_GUI.h \
	SALOME_PYQT_PyInterp.h \
	SALOME_PYQT_Module.h

dist_libSalomePyQtGUI_la_SOURCES= \
	SALOME_PYQT_PyInterp.cxx \
	SALOME_PYQT_Module.cxx

dist_libSalomePyQtGUI_la_SOURCES: $(SIP_SRC)

# moc pre-processing
MOC_FILES= SALOME_PYQT_Module_moc.cxx
nodist_libSalomePyQtGUI_la_SOURCES= $(MOC_FILES) $(SIP_SRC)

libSalomePyQtGUI_la_CPPFLAGS= $(QT_INCLUDES) $(SIP_INCLUDES) $(PYTHON_INCLUDES) \
	$(CAS_CPPFLAGS) $(VTK_INCLUDES) $(OGL_INCLUDES) $(BOOST_CPPFLAGS) \
	-DHAVE_CONFIG_H @KERNEL_CXXFLAGS@ -D__CALL_OLD_METHODS__ \
	-I$(srcdir)/../../PyInterp -I$(srcdir)/../../SalomeApp \
	-I$(srcdir)/../../SUIT -I$(srcdir)/../../Qtx -I$(srcdir)/../../LightApp \
	-I$(srcdir)/../../SalomeApp -I$(srcdir)/../../CAM -I$(srcdir)/../../STD \
	-I$(top_builddir)/salome_adm/unix @CORBA_CXXFLAGS@ @CORBA_INCLUDES@
libSalomePyQtGUI_la_LIBADD= $(PYTHON_LIBS) $(SIP_LIBS) $(PYQT_LIBS) $(VTK_LIBS) \
	$(OGL_LIBS) ../../PyInterp/libPyInterp.la ../../SalomeApp/libSalomeApp.la

##################################

# Custom build step: generate C++ wrapping according to $(SIP_FILES)

$(SIP_SRC): $(SIP_FILES)
	$(SIP) $(SIP_FLAGS) $<

$(dist_libSalomePyQtGUI_la_SOURCES): $(SIP_SRC)

###################################
