# -*- coding: utf-8 -*-
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
# Author : Guillaume Boulant (EDF)

def minmax(context):
  # get context study, studyId, salomeGui
  study = context.study
  studyId = context.studyId
  sg = context.sg

  from PyQt4.QtGui import QDialog
  from PyQt4.QtGui import QMessageBox
  from PyQt4.QtCore import Qt
  from PyQt4.QtCore import SIGNAL

  from minmax_dialog import Ui_Dialog

  import salome
  import smesh

  controls_dict = {
    "Aspect Ratio 3D" :     smesh.FT_AspectRatio3D,
    "Volume" :              smesh.FT_Volume3D,
    "Element Diameter 3D" : smesh.FT_MaxElementLength3D,
    "Length 2D" :           smesh.FT_Length2D,
    "MultiConnection 2D" :  smesh.FT_MultiConnection2D,
    "Area" :                smesh.FT_Area,
    "Taper" :               smesh.FT_Taper,
    "Aspect Ratio" :        smesh.FT_AspectRatio,
    "Minimum Angle" :       smesh.FT_MinimumAngle,
    "Warping" :             smesh.FT_Warping,
    "Skew" :                smesh.FT_Skew,
    "Element Diameter 2D" : smesh.FT_MaxElementLength2D,
    "Length" :              smesh.FT_Length,
    "MultiConnection" :     smesh.FT_MultiConnection,
    }

  controls_3d = [
    "Aspect Ratio 3D",
    "Volume",
    "Element Diameter 3D",
    ]
  controls_2d = [
    "Length 2D",
    "MultiConnection 2D",
    "Area",
    "Taper",
    "Aspect Ratio",
    "Minimum Angle",
    "Warping",
    "Skew",
    "Element Diameter 2D"
    ]
  controls_1d = [
    "Length",
    "MultiConnection",
    ]

  class MinmaxDialog(QDialog):
    def __init__(self):
      QDialog.__init__(self, None, Qt.Tool)
      # Set up the user interface from Designer.
      self.ui = Ui_Dialog()
      self.ui.setupUi(self)
      self.show()

      self.clearLineEdit()

      # Connect up the selectionChanged() event of the object browser.
      self.connect(sg.getObjectBrowser(), SIGNAL("selectionChanged()"), self.select)

      self.mm = None
      self.ui.control.setFocus()
      self.select()

      pass

    def OnCancel(self):
      self.disconnect(sg.getObjectBrowser(), SIGNAL("selectionChanged()"), self.select)
      self.reject()
      pass

    def clearLineEdit(self):
      self.ui.mesh.setText("Select a Mesh")
      self.ui.mesh.setStyleSheet("QLineEdit { color: grey }")
      self.ui.minvalue.setText("")
      self.ui.maxvalue.setText("")

    def select(self):
      self.disconnect(sg.getObjectBrowser(), SIGNAL("selectionChanged()"), self.select)
      self.ui.control.clear()
      self.ui.minvalue.setText("")
      self.ui.maxvalue.setText("")
      objId = salome.sg.getSelected(0)
      if objId:
        mm = study.FindObjectID(objId).GetObject()
        mesh = None
        try:
          mesh = mm.GetMEDMesh()
        except:
          #print "No mesh selected"
          self.clearLineEdit()
          mesh = None
          pass
        if mesh:
          self.ui.mesh.setStyleSheet("")
          self.ui.mesh.setText(mesh.getName())
          #print "Mesh selected: ", mesh.getName()
          self.mm = mm
          e = self.mm.NbEdges()
          f = self.mm.NbFaces()
          v = self.mm.NbVolumes()
          #print "NbEdges: ",e
          #print "NbFaces: ",f
          #print "NbVolumes: ",v
          controls = []
          if e:
            controls += controls_1d
            pass
          if f:
            controls += controls_2d
            pass
          if v:
            controls += controls_3d
            pass
          self.ui.control.addItems(controls)
          self.compute_minmax()
      self.connect(sg.getObjectBrowser(), SIGNAL("selectionChanged()"), self.select)
      pass

    def helpMessage(self):
      QMessageBox.about(None, "About Min/Max value of control",
      """
      Displays the min/max value of a control
      ---------------------------------

This plugin displays the min and max value of a control
on a mesh.
Inputs:
  - The mesh to analyse
  - The control to compute
      """)
      pass

    def compute_minmax(self):
      if self.mm:
        control = self.ui.control.currentText()
        #print "Compute control: ",control
        fun = smesh.GetFunctor(controls_dict[str(control)])
        fun.SetMesh(self.mm.GetMesh())
        hist = fun.GetHistogram(1)
        maxVal = hist[0].max
        minVal = hist[0].min
        #print "Max value for %s: %f"%(control, maxVal)
        #print "Min value for %s: %f"%(control, minVal)
        self.ui.maxvalue.setText("%f"%(maxVal))
        self.ui.minvalue.setText("%f"%(minVal))
      else:
        print "Pas de maillage"
        pass
      pass
    pass

  window = MinmaxDialog()
  window.exec_()
  pass

