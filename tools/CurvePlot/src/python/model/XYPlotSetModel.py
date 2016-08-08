from Model import Model
from utils import toUnicodeWithWarning

class XYPlotSetModel(Model):
  
  def __init__(self, controller):
    Model.__init__(self, controller)
    self._title = "Plot set %d" % self.getID()
    self._curves = {}  # Key: model ID, value: CurveModel
    self._currentCurve = None
    self._xlabel = ""
    self._ylabel = ""
    
  def eraseAll(self):
    self._curves = {}
    self._currentCurve = None
    self.notifyChange("ClearAll")
    
  def setTitle(self, title, silent=False):
    title = toUnicodeWithWarning(title, "XYPlotSetModel::setTitle()")
    self._title = title
    if not silent:
      self.notifyChange("TitleChange")
      
  def getTitle(self):
    return self._title
      
  def setCurrentCurve(self, curveID, silent=False):
    if not self._curves.has_key(curveID) and curveID != -1:
      raise ValueError("Invalid curve ID (%d)!" % curveID)
    self._currentCurve = self._curves.get(curveID, None)
    if not silent:
      self.notifyChange("CurrentCurveChange") 

  def getCurrentCurve(self):
    return self._currentCurve
      
  def addCurve(self, curve, silent=False):
    self._curves[curve.getID()] = curve
    if not silent:
      self.notifyChange("AddCurve")
  
  def removeCurve(self, curveID, silent=False):
    if not self._curves.has_key(curveID):
      raise ValueError("Curve ID (%d) not found for deletion!" % curveID)
    c = self._curves.pop(curveID)
    if self._currentCurve is c:
      self._currentCurve = None
    if not silent:
      self.notifyChange("RemoveCurve")
    
  def setXLabel(self, x_label, silent=False):
    x_label = toUnicodeWithWarning(x_label, "XYPlotSetModel::setXLabel()")
    self._xlabel = x_label
    if not silent:
      self.notifyChange("XLabelChange")
  
  def setYLabel(self, y_label, silent=False):
    y_label = toUnicodeWithWarning(y_label, "XYPlotSetModel::setYLabel()")
    self._ylabel = y_label
    if not silent:
      self.notifyChange("YLabelChange")
  