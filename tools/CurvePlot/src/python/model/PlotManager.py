from Model import Model
from XYPlotSetModel import XYPlotSetModel
from utils import Logger

class PlotManager(Model):
  def __init__(self, controller):
    from collections import OrderedDict
    Model.__init__(self, controller)
    self._currentPlotSet = None
    self._plotSets = OrderedDict()    # key: plotSet ID, value: instance of XYPlotSetModel. We use an OrderedDict so that
                                      # when removing elemetns, we can easily re-select the last-but-one.
    self._lockRepaint = False  # if True, repaint routines are blocked.
    self._plotSetsRepaint = set() # plot waiting for repaint/update while repaint is locked
  
  def isEmpty(self):
    return len(self._plotSets) == 0
  
  def setCurrentPlotSet(self, plotSetID, silent=False):
    if not self._plotSets.has_key(plotSetID) and plotSetID != -1:
      raise ValueError("Invalid plot set ID (%d)!" % plotSetID)
    self._currentPlotSet = self._plotSets.get(plotSetID, None)
    if not silent:
      self.notifyChange("CurrentPlotSetChange") 
  
  def getCurrentPlotSet(self):
    return self._currentPlotSet
  
  def getPlotSetContainingCurve(self, curveID):
    for ps in self._plotSets.values():
      if ps._curves.has_key(curveID):
        return ps
    return None
  
  def setCurrentCurve(self, curveId):
    ps = self.getPlotSetContainingCurve(curveId)
    if ps is None and curveId != -1:
      raise ValueError("Invalid curve ID (%d)!" % curveId)
    self.clearAllCurrentCurve()
    if curveId == -1:      
      return -1
    ps_id = ps.getID()
    currPs = self.getCurrentPlotSet()
    if currPs is None or currPs.getID() != ps_id: 
      self.setCurrentPlotSet(ps_id)
    ps.setCurrentCurve(curveId)
    return ps_id
  
  def getCurrentCurve(self):
    ps = self.getCurrentPlotSet()
    if ps is None:
      return None
    return ps.getCurrentCurve()
  
  def clearAllCurrentCurve(self, silent=False):
    for psID in self._plotSets:
      self._plotSets[psID].setCurrentCurve(-1)
    if not silent:
      self.notifyChange("CurrentCurveChange")
  
  def createXYPlotSet(self, silent=False):
    cv = XYPlotSetModel(self._controller)
    self._plotSets[cv.getID()] = cv
    self._currentPlotSet = cv
    if not silent:
      self.notifyChange("NewPlotSet")
    return cv
  
  def removeXYPlotSet(self, plotSetID):
    Logger.Debug("====> PlotManager::removeXYPlotSet() %d" % plotSetID)
    if not self._plotSets.has_key(plotSetID):
      print self._plotSets
      raise ValueError("Plot set ID (%d) not found for deletion!" % plotSetID)
    ps = self._plotSets.pop(plotSetID)
    if self._currentPlotSet is ps:
      self._currentPlotSet = None
    self.notifyChange("RemovePlotSet")
    return ps
  
  def clearAll(self):
    self._plotSets = {}
    self._currentPlotSet = None
    self.notifyChange("ClearAll")
    
  def lockRepaint(self):
    self._lockRepaint = True
    self._plotSetsRepaint = set()
    
  def isRepaintLocked(self):
    return self._lockRepaint
  
  def registerRepaint(self, ps_id):
    self._plotSetsRepaint.add(ps_id)
    
  def unlockRepaint(self):
    self._lockRepaint = False
    for obj in self._plotSetsRepaint:
      obj.notifyChange()
    self._plotSetsRepaint = set()
    