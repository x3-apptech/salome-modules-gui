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

# Sip common sources
SIP_SRC = sipSalomePyQtSalomePyQt.cc \
	  sipSalomePyQtSALOME_Selection.cc \
          sipSalomePyQtQtxAction.cc

# Sip version-specific sources
if IS_SIP_V4
SIP_SRC  += sipSalomePyQtcmodule.cc
else !IS_SIP_V4
SIP_SRC  += SalomePyQtcmodule.cc
endif

# Sip/moc sources and Sip definition files
if IS_SIP_V4
if IS_SIP_NEW
MOC_SRC   = 
MOC_H     = 
SIP_FILES = SalomePyQt_v4.sip
else !IS_SIP_NEW
MOC_SRC   = sipSalomePyQtcmodule_moc.cxx
MOC_H     = sipSalomePyQtcmodule.h
SIP_FILES = SalomePyQt_v4.sip
endif
else !IS_SIP_V4
SIP_FILES = SalomePyQt.sip
if IS_SIP_NEW
MOC_SRC   = SalomePyQtcmodule_moc.cxx
MOC_H     = SalomePyQtcmodule.h
else !IS_SIP_NEW
MOC_SRC   = sipSalomePyQtProxySalomePyQt_moc.cxx
MOC_H     = sipSalomePyQtProxySalomePyQt.h
endif
endif

EXTRA_DIST+= SalomePyQt.sip SalomePyQt_v4.sip

# Libraries targets
MOC_FILES= SalomePyQt_moc.cxx

COMMON_CPP_FLAGS=$(QT_INCLUDES) $(SIP_INCLUDES) $(PYTHON_INCLUDES) \
	$(CAS_CPPFLAGS) $(VTK_INCLUDES) $(OGL_INCLUDES) $(BOOST_CPPFLAGS) \
	-DHAVE_CONFIG_H @KERNEL_CXXFLAGS@ -I$(top_builddir)/salomeadn/unix \
	-I$(top_builddir)/idl \
	-I$(srcdir)/../../SUIT -I$(srcdir)/../../CAM -I$(srcdir)/../../STD \
	-I$(srcdir)/../../Qtx -I$(srcdir)/../../SalomeApp -I$(srcdir)/../../Event \
	-I$(srcdir)/../../SalomeSession -I$(srcdir)/../../LogWindow \
	-I$(srcdir)/../../VTKViewer -I$(srcdir)/../../TOOLSGUI \
	-I$(srcdir)/../SALOME_PYQT_GUI -I$(srcdir)/../../PyInterp \
	-I$(srcdir)/../../LightApp -I$(srcdir)/../../ObjBrowser \
	-I$(srcdir)/../../OBJECT \
	-I$(top_builddir)/salome_adm/unix @CORBA_CXXFLAGS@ @CORBA_INCLUDES@

COMMON_LIBS=$(PYTHON_LIBS) $(SIP_LIBS) $(PYQT_LIBS) $(VTK_LIBS) \
	$(OGL_LIBS) ../../SUIT/libsuit.la ../../CAM/libCAM.la ../../STD/libstd.la \
	../../Qtx/libqtx.la ../../SalomeApp/libSalomeApp.la ../../Event/libEvent.la \
	../../Session/libSalomeSession.la ../../LogWindow/libLogWindow.la \
	../../VTKViewer/libVTKViewer.la ../../TOOLSGUI/libToolsGUI.la \
	../SALOME_PYQT_GUI/libSalomePyQtGUI.la

if IS_SIP_V4

lib_LTLIBRARIES = libSalomePyQt.la
dist_libSalomePyQt_la_SOURCES= SalomePyQt.cxx
nodist_libSalomePyQt_la_SOURCES= $(MOC_FILES) $(SIP_SRC) $(MOC_SRC) 
libSalomePyQt_la_CPPFLAGS = $(COMMON_CPP_FLAGS)
libSalomePyQt_la_LIBADD = $(COMMON_LIBS)

else !IS_SIP_V4

lib_LTLIBRARIES = libSalomePyQtcmodule.la
dist_libSalomePyQtcmodule_la_SOURCES= SalomePyQt.cxx
nodist_libSalomePyQtcmodule_la_SOURCES= $(MOC_FILES) $(SIP_SRC) $(MOC_SRC)
libSalomePyQtcmodule_la_CPPFLAGS = $(MY_CPP_FLAGS)
libSalomePyQtcmodule_la_LIBADD = $(COMMON_LIBS)

endif

if IS_SIP_V4
else
nodist_sharedpkgpython_PYTHON= SalomePyQt.py
endif

# Custom build step: generate C++ wrapping according to $(SIP_FILES)
$(SIP_SRC): $(SIP_FILES)
	$(SIP) $(SIP_FLAGS) $<

$(MOC_H): $(SIP_FILES)
	$(SIP) $(SIP_FLAGS) $<

if IS_SIP_V4
install-exec-hook: $(libdir)/SalomePyQt.so
$(libdir)/SalomePyQt.so:
	(cd $(libdir); ln -s libSalomePyQt.so SalomePyQt.so;)
else !IS_SIP_V4
install-exec-hook: $(libdir)/SalomePyQtcmodule.so
$(libdir)/SalomePyQt.so:
	(cd $(libdir); ln -sf libSalomePyQtcmodule.so SalomePyQt.so;)
endif

$(SIP_SRC): $(MOC_SRC)
