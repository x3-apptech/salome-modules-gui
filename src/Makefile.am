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
#  Author : Patrick GOLDBRONN (CEA)
#  Module : SALOME
# $Header$
#
SUBDIRS = CASCatch Qtx Style DDS QDS ObjBrowser SUIT SUITApp STD CAF CAM LogWindow Prs Event

if ENABLE_SALOMEOBJECT
  SUBDIRS += OBJECT
endif

if ENABLE_GLVIEWER
  SUBDIRS += GLViewer
endif

if ENABLE_VTKVIEWER
  SUBDIRS += VTKViewer
if ENABLE_SALOMEOBJECT
    SUBDIRS += SVTK
endif
endif
if ENABLE_OCCVIEWER
  SUBDIRS += OCCViewer
if ENABLE_SALOMEOBJECT
    SUBDIRS += SOCC
endif
endif
if ENABLE_PYCONSOLE
  SUBDIRS += PyInterp PyConsole
endif
if ENABLE_PLOT2DVIEWER
  SUBDIRS += Plot2d
if ENABLE_SALOMEOBJECT
    SUBDIRS += SPlot2d
endif
endif
if ENABLE_SUPERVGRAPHVIEWER
  SUBDIRS += SUPERVGraph
endif
if ENABLE_QXGRAPHVIEWER
  SUBDIRS += QxGraph
endif
SUBDIRS += QxScene LightApp ResExporter

if GUI_ENABLE_CORBA
  SUBDIRS += TOOLSGUI Session SalomeApp SALOME_SWIG SALOME_PY SALOME_PYQT
endif

DIST_SUBDIRS = CASCatch Qtx Style DDS QDS ObjBrowser SUIT SUITApp STD CAF CAM LogWindow Prs Event \
               OBJECT GLViewer VTKViewer SVTK OCCViewer SOCC PyInterp PyConsole Plot2d SPlot2d SUPERVGraph \
               QxGraph QxScene LightApp ResExporter TOOLSGUI Session SalomeApp SALOME_SWIG SALOME_PY SALOME_PYQT
