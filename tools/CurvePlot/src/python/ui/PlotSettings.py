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
    xminText = unicode(self.axisXMinEdit.text())
    xmaxText = unicode(self.axisXMaxEdit.text())
    yminText = unicode(self.axisYMinEdit.text())
    ymaxText = unicode(self.axisYMaxEdit.text())
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
