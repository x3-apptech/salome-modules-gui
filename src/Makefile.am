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

#  File   : Makefile.am
#  Author : Patrick GOLDBRONN (CEA)

include $(top_srcdir)/adm_local/unix/make_common_starter.am

##
# Common packages
##

SUBDIRS_COMMON = CASCatch Qtx Style DDS QDS ObjBrowser SUIT SUITApp STD CAF CAM LogWindow Prs Event OpenGLUtils GUI_PY

##
# SALOME object
##
if ENABLE_SALOMEOBJECT
  SUBDIRS_OBJECT = OBJECT
endif

##
# GL viewer
##
if ENABLE_GLVIEWER
  SUBDIRS_GLVIEWER = GLViewer
endif

##
# VTK viewer
##
if ENABLE_VTKVIEWER
  SUBDIRS_VTKVIEWER = VTKViewer
if ENABLE_SALOMEOBJECT
    SUBDIRS_VTKVIEWER += SVTK
    SUBDIRS_VIEWERTOOLS = ViewerTools
endif
endif

##
# OCC viewer
##
if ENABLE_OCCVIEWER
  SUBDIRS_OCCVIEWER = OCCViewer
  SUBDIRS_VIEWERTOOLS = ViewerTools
if ENABLE_SALOMEOBJECT
    SUBDIRS_OCCVIEWER += SOCC
endif
endif

##
# Plot2d viewer
##
if ENABLE_PLOT2DVIEWER
  SUBDIRS_PLOT2DVIEWER = Plot2d
if ENABLE_SALOMEOBJECT
    SUBDIRS_PLOT2DVIEWER += SPlot2d
endif
endif

##
# SUPERV graph viewer
##
if ENABLE_SUPERVGRAPHVIEWER
  SUBDIRS_SUPERVGRAPHVIEWER = SUPERVGraph
endif

##
# Qx scene viewer
##
###VSR: QxGraph has been replaced by QxScene
###SUBDIRS_QXGRAPHVIEWER = QxGraph
if ENABLE_QXGRAPHVIEWER
  SUBDIRS_QXGRAPHVIEWER = QxScene
endif

##
# Python console (base)
##
if ENABLE_PYCONSOLE
  SUBDIRS_PYCONSOLE = PyInterp PyConsole
endif

##
# Light SALOME packages
##
SUBDIRS_LIGHT = LightApp ResExporter

##
# Full (CORBA) SALOME packages
##
if GUI_ENABLE_CORBA
  SUBDIRS_CORBA = TOOLSGUI Session SalomeApp GuiHelpers TreeData
endif


##
# Extra Python packages
##
if ENABLE_PYCONSOLE
  SUBDIRS_PY_LIGHT = SALOME_SWIG SALOME_PY SALOME_PYQT
endif

SUBDIRS = $(SUBDIRS_COMMON) $(SUBDIRS_OBJECT) $(SUBDIRS_VIEWERTOOLS) $(SUBDIRS_GLVIEWER) $(SUBDIRS_VTKVIEWER) \
          $(SUBDIRS_OCCVIEWER) $(SUBDIRS_PLOT2DVIEWER) $(SUBDIRS_SUPERVGRAPHVIEWER) $(SUBDIRS_QXGRAPHVIEWER) \
          $(SUBDIRS_PYCONSOLE) $(SUBDIRS_LIGHT) $(SUBDIRS_CORBA) $(SUBDIRS_PY_LIGHT)

DIST_SUBDIRS = CASCatch Qtx Style DDS QDS ObjBrowser SUIT SUITApp STD CAF CAM LogWindow Prs Event OpenGLUtils \
               OBJECT ViewerTools GLViewer VTKViewer SVTK OCCViewer SOCC Plot2d SPlot2d SUPERVGraph QxGraph QxScene \
               PyInterp PyConsole LightApp ResExporter TOOLSGUI Session SalomeApp GuiHelpers TreeData \
               SALOME_SWIG SALOME_PY SALOME_PYQT GUI_PY
