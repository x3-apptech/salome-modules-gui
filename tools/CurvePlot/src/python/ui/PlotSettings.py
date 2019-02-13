# Copyright (C) 2016-2019  CEA/DEN, EDF R&D
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

from pyqtside import QtGui, QtCore
from pyqtside.uic import loadUiGen
from utils import completeResPath

class PlotSettings(QtGui.QDialog):
  def __init__(self):
    QtGui.QDialog.__init__(self)
    loadUiGen(completeResPath("PlotSettings.ui"), self)
    self.initialize()

  def initialize(self):
    self.legendPositionComboBox.addItem("Bottom")
    self.legendPositionComboBox.addItem("Right")
    self._r = 0
    self._g = 0
    self._b = 1
   
  @QtCore.Slot(int)
  def onShowLegend(self, index):
    if index > 0 :
      self.legendPositionComboBox.setEnabled(True)
    else :
      self.legendPositionComboBox.setEnabled(False)
  
  @QtCore.Slot() 
  def onChangeColor(self):
    col = QtGui.QColorDialog.getColor()

    if col.isValid():
      r, g, b = [c/255.0 for c in col.getRgb()[:3]]
      self.setRGB(r, g, b)
      
  def setSelectedCurveName(self, name):
    if name :
      self.selectedCurvePanel.setTitle("Selected curve : " + name)
      self.selectedCurvePanel.show()
    else :
      self.selectedCurvePanel.hide()
   
  def setRGB(self, r, g, b):
    self._r = r
    self._g = g
    self._b = b
    self.colorCurve.setIcon(QtGui.QIcon(self.drawColorPixmap(int(r*255), int(g*255), int(b*255))))
   
  def getRGB(self):
    return self._r, self._g, self._b
   
  def drawColorPixmap(self, r, g, b):
    pix = QtGui.QPixmap( 16, 16 )
    color = QtGui.QColor(r, g, b)
    pix.fill(color)
    return pix

  def accept(self):
    xminText = str(self.axisXMinEdit.text())
    xmaxText = str(self.axisXMaxEdit.text())
    yminText = str(self.axisYMinEdit.text())
    ymaxText = str(self.axisYMaxEdit.text())
    if (yminText == "" or ymaxText == "") :
      QtGui.QMessageBox.critical(self, "Plot settings", "A field \"YMin\" or \"YMax\" is empty")
    else :
      try:
        xmin = float(xminText)
      except ValueError:
        QtGui.QMessageBox.critical(self, "Plot settings", "It is not possible to convert XMin")
      try:
        xmax = float(xmaxText)
      except ValueError:
        QtGui.QMessageBox.critical(self, "Plot settings", "It is not possible to convert XMax")
      try:
        ymin = float(yminText)
      except ValueError:
        QtGui.QMessageBox.critical(self, "Plot settings", "It is not possible to convert YMin")
      try:
        ymax = float(ymaxText)
      except ValueError:
        QtGui.QMessageBox.critical(self, "Plot settings", "It is not possible to convert YMax")
      if ((xmax-xmin) == 0) :
        QtGui.QMessageBox.critical(self, "Plot settings", "XMax is is equal to XMin.")
        return
      if ((ymax-ymin) == 0) :
        QtGui.QMessageBox.critical(self, "Plot settings", "YMax is is equal to YMin.")
        return
      if ((xmax-xmin) < 0) :
        QtGui.QMessageBox.warning(self, "Plot settings", "XMax is less than XMin.")
      if ((ymax-ymin) < 0) :
        QtGui.QMessageBox.warning(self, "Plot settings", "YMax is less than YMin.")
      super(PlotSettings, self).accept()
