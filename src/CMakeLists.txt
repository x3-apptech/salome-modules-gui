# Copyright (C) 2012-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

##
# Common packages
##
SET(SUBDIRS_COMMON
  CASCatch Qtx HelpBrowser Style DDS QDS ObjBrowser SUIT SUITApp STD CAF
  CAM LogWindow Prs Event OpenGLUtils ViewerData ViewerTools ImageComposer GUI_PY
)

##
# SALOME object
##
IF(SALOME_USE_SALOMEOBJECT)
  SET(SUBDIRS_OBJECT OBJECT)
ENDIF(SALOME_USE_SALOMEOBJECT)

##
# GL viewer
##
IF(SALOME_USE_GLVIEWER)
  SET(SUBDIRS_GLVIEWER GLViewer)
ENDIF(SALOME_USE_GLVIEWER)

##
# VTK viewer
##
IF(SALOME_USE_VTKVIEWER)
  SET(SUBDIRS_VTKVIEWER VTKViewer)

  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND SUBDIRS_VTKVIEWER SVTK)
  ENDIF(SALOME_USE_SALOMEOBJECT)
ENDIF(SALOME_USE_VTKVIEWER)

##
# OCC viewer
##
IF(SALOME_USE_OCCVIEWER)
  SET(SUBDIRS_OCCVIEWER OCCViewer)
  
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND SUBDIRS_OCCVIEWER SOCC)
  ENDIF(SALOME_USE_SALOMEOBJECT)
ENDIF(SALOME_USE_OCCVIEWER)

##
# Plot2d viewer
##
IF(SALOME_USE_PLOT2DVIEWER)
  SET(SUBDIRS_PLOT2DVIEWER Plot2d)

  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND SUBDIRS_PLOT2DVIEWER SPlot2d)
  ENDIF(SALOME_USE_SALOMEOBJECT)
ENDIF(SALOME_USE_PLOT2DVIEWER)

##
# Qx scene viewer
##
IF(SALOME_USE_QXGRAPHVIEWER)
  SET(SUBDIRS_QXGRAPHVIEWER QxScene)
ENDIF(SALOME_USE_QXGRAPHVIEWER)

# Graphics viewer
##
IF(SALOME_USE_GRAPHICSVIEW)
  SET(SUBDIRS_GRAPHICSVIEW GraphicsView)
ENDIF(SALOME_USE_GRAPHICSVIEW)

##
# Python-based packages
##
IF(SALOME_USE_PYCONSOLE) 
  SET(SUBDIRS_PYCONSOLE PyInterp PyConsole)
  SET(SUBDIRS_PYTHON SALOME_PYQT)
  IF(SALOME_USE_VTKVIEWER)
    LIST(APPEND SUBDIRS_PYTHON SALOME_PY)
  ENDIF()
  IF(SALOME_USE_SALOMEOBJECT)
    LIST(APPEND SUBDIRS_PYTHON SALOME_SWIG)
  ENDIF()
ENDIF(SALOME_USE_PYCONSOLE)

##
# Light SALOME packages
##
SET(SUBDIRS_LIGHT LightApp ResExporter)

##
# Full (CORBA) SALOME packages
##
IF(NOT SALOME_LIGHT_ONLY)
  SET(SUBDIRS_CORBA TOOLSGUI SalomeApp Session GuiHelpers TreeData)
ENDIF(NOT SALOME_LIGHT_ONLY)

SET(SUBDIRS
  ${SUBDIRS_COMMON}
  ${SUBDIRS_OBJECT}
  ${SUBDIRS_GLVIEWER}
  ${SUBDIRS_VTKVIEWER}
  ${SUBDIRS_OCCVIEWER}
  ${SUBDIRS_PLOT2DVIEWER}
  ${SUBDIRS_QXGRAPHVIEWER}
  ${SUBDIRS_GRAPHICSVIEW}
  ${SUBDIRS_PYCONSOLE}
  ${SUBDIRS_LIGHT}
  ${SUBDIRS_CORBA}
  ${SUBDIRS_PYTHON}
)

FOREACH(dir ${SUBDIRS})
 ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})
            
