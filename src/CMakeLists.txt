# Copyright (C) 2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

##
# Common packages
##
SET(SUBDIRS_COMMON
  CASCatch
  Qtx
  Style
  DDS
  QDS
  ObjBrowser
  SUIT
  SUITApp
  STD
  CAF
  CAM
  LogWindow
  Prs
  Event
  OpenGLUtils
  GUI_PY
)

##
# SALOME object
##
IF(ENABLE_SALOMEOBJECT)
  SET(SUBDIRS_OBJECT OBJECT)
ENDIF(ENABLE_SALOMEOBJECT)

##
# GL viewer
##
IF(ENABLE_GLVIEWER)
  SET(SUBDIRS_GLVIEWER GLViewer)
ENDIF(ENABLE_GLVIEWER)

##
# VTK viewer
##
IF(ENABLE_VTKVIEWER)
  SET(SUBDIRS_VTKVIEWER VTKViewer)

  IF(ENABLE_SALOMEOBJECT)
    SET(SUBDIRS_VTKVIEWER ${SUBDIRS_VTKVIEWER} SVTK)
    SET(SUBDIRS_VIEWERTOOLSViewerTools)
  ENDIF(ENABLE_SALOMEOBJECT)
ENDIF(ENABLE_VTKVIEWER)

##
# OCC viewer
##
IF(ENABLE_OCCVIEWER)
  SET(SUBDIRS_OCCVIEWER OCCViewer)
  SET(SUBDIRS_VIEWERTOOLS ViewerTools)
  
  IF(ENABLE_SALOMEOBJECT)
    SET(SUBDIRS_OCCVIEWER ${SUBDIRS_OCCVIEWER} SOCC)
  ENDIF(ENABLE_SALOMEOBJECT)
ENDIF(ENABLE_OCCVIEWER)

##
# Plot2d viewer
##
IF(ENABLE_PLOT2DVIEWER)
  SET(SUBDIRS_PLOT2DVIEWER  Plot2d)

  IF(ENABLE_SALOMEOBJECT)
    SET(SUBDIRS_PLOT2DVIEWER ${SUBDIRS_PLOT2DVIEWER} SPlot2d)
  ENDIF(ENABLE_SALOMEOBJECT)
ENDIF(ENABLE_PLOT2DVIEWER)

##
# SUPERV graph viewer
##
IF(ENABLE_SUPERVGRAPHVIEWER) 
  SET(SUBDIRS_SUPERVGRAPHVIEWER SUPERVGraph)
ENDIF(ENABLE_SUPERVGRAPHVIEWER)

##
# Qx scene viewer
##
###VSR: QxGraph has been replaced by QxScene
###SUBDIRS_QXGRAPHVIEWER = QxGraph
IF(ENABLE_QXGRAPHVIEWER)
  SET(SUBDIRS_QXGRAPHVIEWER QxScene)
ENDIF(ENABLE_QXGRAPHVIEWER)

##
# Python console (base)
##
IF(ENABLE_PYCONSOLE) 
  SET(SUBDIRS_PYCONSOLE PyInterp PyConsole)
ENDIF(ENABLE_PYCONSOLE)

##
# Light SALOME packages
##
SET(SUBDIRS_LIGHT
  LightApp
  ResExporter
)

##
# Full (CORBA) SALOME packages
##
IF(GUI_ENABLE_CORBA)
  SET(SUBDIRS_CORBA
    TOOLSGUI
    Session
    SalomeApp
    GuiHelpers
    TreeData
  )
ENDIF(GUI_ENABLE_CORBA)

##
# Extra Python packages
##
IF(ENABLE_PYCONSOLE)
  SET(SUBDIRS_PY_LIGHT
    SALOME_SWIG
    SALOME_PY
    SALOME_PYQT
  )
ENDIF(ENABLE_PYCONSOLE)

SET(SUBDIRS
  ${SUBDIRS_COMMON}
  ${SUBDIRS_OBJECT}
  ${SUBDIRS_VIEWERTOOLS}
  ${SUBDIRS_GLVIEWER}
  ${SUBDIRS_VTKVIEWER}
  ${SUBDIRS_OCCVIEWER}
  ${SUBDIRS_PLOT2DVIEWER}
  ${SUBDIRS_SUPERVGRAPHVIEWER}
  ${SUBDIRS_QXGRAPHVIEWER}
  ${SUBDIRS_PYCONSOLE}
  ${SUBDIRS_LIGHT}
  ${SUBDIRS_CORBA}
  ${SUBDIRS_PY_LIGHT}
)

FOREACH(dir ${SUBDIRS})
 ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})
            
