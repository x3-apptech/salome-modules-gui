from View import View
from XYView import XYView
from utils import Logger

class CurveTabsView(View):
  def __init__(self, controller):
    View.__init__(self, controller)
    self._XYViews = {}  # key: SALOME view ID, value: XYView
      
  def closeXYView(self, salomeViewID):
    Logger.Debug("CurveTabsView::closeXYView: %d" % salomeViewID)
    self._controller.setCurvePlotRequestingClose(True)
    self._controller._sgPyQt.closeView(salomeViewID)
    self._controller.setCurvePlotRequestingClose(False)
    # Destroy the view
    self._XYViews.pop(salomeViewID)
    Logger.Debug("CurveTabsView::closeXYView count %d" % len(self._XYViews))
  
  def createXYView(self, model):
    v = XYView(self._controller)
    self._controller.associate(model, v)
    return v
  
  def onCurrentPlotSetChange(self):
    """ Avoid a unnecessary call to update() when just switching current plot set! """
    cps = self._model.getCurrentPlotSet()
    if not cps is None:
      mp = self.mapModId2ViewId()
      salomeViewID = mp[cps.getID()]
      self._controller._sgPyQt.activateView(salomeViewID)
  
  def mapModId2ViewId(self):
    """ Gives a map from model ID (the model behind the XYView) to view ID
    """
    lst = [(v._model.getID(), view_id) for view_id, v in self._XYViews.items()]
    return dict(lst)
  
  def update(self):
    """
    Updates the list of tabs shown in the GUI
    """
    if self._model is None:
        return
    
    # Check list of tabs:
    set_mod = set(self._model._plotSets.keys())
    set_view = set([ v._model.getID() for v in self._XYViews.values() ])
    mp = self.mapModId2ViewId()
    
    # Deleted/Added curves:
    dels = set_view - set_mod
    added = set_mod - set_view
    
    for d in dels:
      salomeViewID = mp[d]
      v = self._XYViews[salomeViewID]
      v.cleanBeforeClose()
      self.closeXYView(salomeViewID)
    
    newViews = []
    for a in added:
      newViews.append(self.createXYView(self._model._plotSets[a]))
    
    # Now update all tabs individually (this will trigger creation of new ones if not already there):
    for v in self._XYViews.values() + newViews:
      # The update on newViews will trigger the SALOME view creation:
      v.update() 
    
    # And complete internal structure for new views
    # This is not done in 
    for v in newViews:
      self._XYViews[v._salomeViewID] = v
      
    # Finally activate the proper tab:
    self.onCurrentPlotSetChange()
    