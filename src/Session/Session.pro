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
#TEMPLATE = lib
#TARGET = SalomeSession
#DESTDIR = ../../lib
#MOC_DIR = ../../moc
#OBJECTS_DIR = ../../obj/$$TARGET

# ================>

TEMPLATE = 
TARGET = SALOME_Session_Server
DESTDIR = ../../bin
MOC_DIR = ../../moc
OBJECTS_DIR = ../../obj/$$TARGET

# <================


QT_MT_INCLUDES = $$(QTDIR)/include $$(QTDIR)/include/QtCore $$(QTDIR)/include/QtGui $$(QTDIR)/include/QtOpenGL $$(QTDIR)/include/QtXml

PYTHON_INCLUDES = $$(PYTHONHOME)/include/python2.4

HDF5_INCLUDES = $$(HDF5HOME)/include

BOOST_CPPFLAGS = $$(BOOSTDIR)/include

KERNEL_CXXFLAGS = $$(KERNEL_ROOT_DIR)/include/salome

CASROOT = $$(CASROOT)
CAS_CPPFLAGS = $${CASROOT}/inc

OMNIORBDIR = $$(OMNIORBDIR)
CORBA_INCLUDES = $${OMNIORBDIR}/include $${OMNIORBDIR}/include/omniORB4 $${OMNIORBDIR}/include/COS

INCLUDEPATH += $${QT_MT_INCLUDES} $${PYTHON_INCLUDES} $${HDF5_INCLUDES} $${BOOST_CPPFLAGS} $${KERNEL_CXXFLAGS} $${CAS_CPPFLAGS} $${CORBA_INCLUDES} ../../salome_adm/unix $$(GUI_ROOT_DIR)/idl ../Qtx ../SUIT ../Event

QT_MT_LIBS = -L$$(QTDIR)/lib -lQtCore -lQtXml -lQtGui -lQtOpenGL

KERNEL_LDFLAGS = -L$$(KERNEL_ROOT_DIR)/lib/salome

CAS_KERNEL = -L$${CASROOT}/Linux/lib -lTKernel

LIBS += $${QT_MT_LIBS}  $${KERNEL_LDFLAGS} -lSalomeNS -lSalomeLifeCycleCORBA -lOpUtil -lSALOMELocalTrace -lSalomeCatalog -lSalomeDSClient $${CAS_KERNEL} -L../../lib -lwith_loggerTraceCollector -lsuit -lEvent -L$$(GUI_ROOT_DIR)/idl -lSalomeIDLGUI

# ================>

OMNIORB_LIBS = -L$${OMNIORBDIR}/lib -lomniORB4 -lomniDynamic4 -lCOS4 -lCOSDynamic4 -lomnithread

HDF5_LIBS = -L$$(HDF5HOME)/lib -lhdf5

INCLUDEPATH = $${INCLUDEPATH}
LIBS = $${LIBS} $${OMNIORB_LIBS} $${HDF5_LIBS} -lSalomeContainer -lSalomeResourcesManager -lTOOLSDS -lSalomeHDFPersist -lSalomeDSImpl -lSalomeGenericObj -lRegistry -lSalomeNotification -lSALOMEBasics -L$$(GUI_ROOT_DIR)/lib -lqtx -lSalomeSession

# <================


CONFIG -= debug release debug_and_release
CONFIG += qt thread debug dll shared

win32:DEFINES += WIN32 
DEFINES += SESSION_EXPORTS OCC_VERSION_MAJOR=6 OCC_VERSION_MINOR=1 OCC_VERSION_MAINTENANCE=1 LIN LINTEL CSFDB No_exception HAVE_CONFIG_H HAVE_LIMITS_H HAVE_WOK_CONFIG_H OCC_CONVERT_SIGNALS OMNIORB_VERSION=4 __x86__ __linux__ COMP_CORBA_DOUBLE COMP_CORBA_LONG

HEADERS  = Session_Session_i.hxx
HEADERS += Session_ServerLauncher.hxx
HEADERS += Session_ServerThread.hxx
HEADERS += Session_ServerCheck.hxx
HEADERS += SalomeApp_Engine_i.hxx

SOURCES  = Session_Session_i.cxx
SOURCES += Session_ServerThread.cxx
SOURCES += Session_ServerLauncher.cxx
SOURCES += Session_ServerCheck.cxx
SOURCES += SalomeApp_Engine_i.cxx

# ================>

SOURCES += SALOME_Session_Server.cxx

# <================

#includes.files = $$HEADERS
#includes.path = ../../include

#INSTALLS += includes


