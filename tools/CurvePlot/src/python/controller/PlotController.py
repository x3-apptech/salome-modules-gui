from CurveBrowserView import CurveBrowserView
from PlotManager import PlotManager
from CurveTabsView import CurveTabsView
from CurveModel import CurveModel
from TableModel import TableModel
from utils import Logger
import numpy as np

class PlotController(object):
  """ Controller for 2D curve plotting functionalities.
  """
  __UNIQUE_INSTANCE = None  # my poor impl. of a singleton
  
  ## For testing purposes:
  WITH_CURVE_BROWSER = True
  WITH_CURVE_TABS = True
  
  def __init__(self, sgPyQt=None):
    if self.__UNIQUE_INSTANCE is None:
      self.__trueInit(sgPyQt)
    else:
      raise Exception("The PlotController must be a singleton - use GetInstance()")

  def __trueInit(self, sgPyQt=None):
    if sgPyQt is None:
      import SalomePyQt
      sgPyQt = SalomePyQt.SalomePyQt()
    self._sgPyQt = sgPyQt
    self._modelViews = {}
    self._browserContextualMenu = None
    self._blockNotifications = False
    self._blockViewClosing = False
    self._callbacks = []
    
    self._plotManager = PlotManager(self)
    
    if self.WITH_CURVE_BROWSER:
      self._curveBrowserView = CurveBrowserView(self)
      self.associate(self._plotManager, self._curveBrowserView)
    else:
      self._curveBrowserView = None  
    if self.WITH_CURVE_TABS:
      self._curveTabsView = CurveTabsView(self)
      self.associate(self._plotManager, self._curveTabsView)
    else:
      self._curveTabsView = None
    PlotController.__UNIQUE_INSTANCE = self
  
  @classmethod
  def GetInstance(cls, sgPyQt=None):
    if cls.__UNIQUE_INSTANCE is None:
      # First instanciation:
      PlotController(sgPyQt)
    return cls.__UNIQUE_INSTANCE
  
  @classmethod
  def Destroy(cls):
    cls.__UNIQUE_INSTANCE = None
  
  def setFixedSizeWidget(self):
    """ For testing purposes - ensure visible Qt widgets have a fixed size.
    """
    if self.WITH_CURVE_BROWSER:
      self._curveBrowserView.treeWidget.resize(100,200)
    if self.WITH_CURVE_TABS:
      self._sgPyQt._tabWidget.resize(600,600)
  
  def associate(self, model, view):
    """
    Associates a model to a view, and sets the view to listen to this model 
    changes.
    
    :param model: Model -- The model to be associated to the view.
    :param view: View -- The view.
    
    """    
    if model is None or view is None:
        return
  
    view.setModel(model)
    self.setModelListener(model, view)
  
  def setModelListener(self, model, view):
    """
    Sets a view to listen to all changes of the given model
    """
    l = self._modelViews.setdefault(model, [])
    if not view in l and view is not None:
      l.append(view) 
  
  def removeModelListeners(self, model):
    """ 
    Removes the given model from the list of listeners. All views previously connected to this model
    won't receive its update notification anymore.
    """
    self._modelViews.pop(model)
  
  def notify(self, model, what=""):
    """
    Notifies the view when model changes.
    
    :param model: Model -- The updated model.
    """
    if model is None or self._blockNotifications:
      return
    
    if not self._modelViews.has_key(model):
      return
    
    for view in self._modelViews[model]:
      method = "on%s" % what
      if what != "" and what is not None and hasattr(view, method):
        exec "view.%s()" % method
      elif hasattr(view, "update"):
        # Generic update:
        view.update()
    
  def setBrowserContextualMenu(self, menu):
    """ Provide a menu to be contextually shown in the curve browser """
    self._browserContextualMenu = menu
    
  def setCurvePlotRequestingClose(self, bool):
    self._blockViewClosing = bool
    
  def onCurrentCurveChange(self):
    ps = self._plotManager.getCurrentPlotSet()
    if not ps is None:
      crv = ps.getCurrentCurve()
      if crv is not None:
        crv_id = crv.getID() 
        for c in self._callbacks:
          c(crv_id)
    
  #####
  ##### Public static API
  #####
  
  @classmethod
  def AddCurve(cls, x, y, curve_label="", x_label="", y_label="", append=True):
    """ Add a new curve and make the plot set where it is drawn the active one.
        If no plot set exists, or none is active, a new plot set will be created, even if append is True.
        @param x x data
        @param y y data
        @param curve_label label of the curve being ploted (optional, default to empty string). This is what is
        shown in the legend.
        @param x_label label for the X axis
        @param y_label label for the Y axis
        @param append whether to add the curve to the active plot set (default) or into a new one.
        @return the id of the created curve, and the id of the corresponding plot set.
    """
    from XYView import XYView
    control = cls.GetInstance()
    pm = control._plotManager
    t = TableModel(control)
    data = np.transpose(np.vstack([x, y]))
    t.setData(data)
    # ensure a single Matplotlib repaint for all operations to come in AddCurve
    prevLock = pm.isRepaintLocked()
    if not prevLock:
      pm.lockRepaint()
    curveID, plotSetID = control.plotCurveFromTable(t, x_col_index=0, y_col_index=1, 
                                                    curve_label=curve_label, append=append)
    ps = pm._plotSets[plotSetID]
    if x_label != "":
      ps.setXLabel(x_label)
    if y_label != "": 
      ps.setYLabel(y_label)
    if not prevLock:
      pm.unlockRepaint()
    return curveID, plotSetID

  @classmethod  
  def ExtendCurve(cls, crv_id, x, y):
    """ Add new points to an already created curve
    @raise if invalid plot set ID is given
    """
    control = cls.GetInstance()
    ps = control._plotManager.getPlotSetContainingCurve(crv_id)
    if ps is None:
      raise ValueError("Curve ID (%d) not found for extension!" % crv_id)
    crv_mod = ps._curves[crv_id]
    data = np.transpose(np.vstack([x, y]))
    crv_mod.extendData(data)
    
  @classmethod
  def ResetCurve(cls, crv_id):
    """ Reset a given curve: all data are cleared, but the curve is still 
    alive with all its attributes (color, etc ...). Mostly used in conjunction
    with ExtendCurve above
    @raise if invalid plot set ID is given
    """
    control = cls.GetInstance()
    ps = control._plotManager.getPlotSetContainingCurve(crv_id)
    if ps is None:
      raise ValueError("Curve ID (%d) not found for reset!" % crv_id)
    crv_mod = ps._curves[crv_id]
    crv_mod.resetData()
    
  @classmethod
  def AddPlotSet(cls, title=""):
    """ Creates a new plot set (a tab with several curves) and returns its ID. A title can be passed,
    otherwise a default one will be created.
    By default this new plot set becomes the active one.
    """
    control = cls.GetInstance()
    ps = control._plotManager.createXYPlotSet()
    control.setModelListener(ps, control._curveBrowserView)
    # Controller itself must be notified for curve picking:
    control.setModelListener(ps, control)
    if title != "":
      ps.setTitle(title)
    return ps.getID()
            
  @classmethod
  def CopyCurve(cls, curve_id, plot_set_id):
    """ Copy a given curve to a given plot set ID
    @return ID of the newly created curve
    """
    control = cls.GetInstance()
    psID = cls.GetPlotSetID(curve_id)
    if psID == -1:
      raise ValueError("Curve ID (%d) not found for duplication!" % curve_id)
    plot_set_src = control._plotManager._plotSets[psID]
    plot_set_tgt = control._plotManager._plotSets.get(plot_set_id, None)
    if plot_set_tgt is None:
      raise ValueError("Plot set ID (%d) invalid for duplication!" % plot_set_id)
    crv = plot_set_src._curves[curve_id]
    new_crv = crv.clone()
    control.setModelListener(new_crv, control._curveBrowserView)
    plot_set_tgt.addCurve(new_crv)
    return new_crv.getID()
      
  @classmethod
  def DeleteCurve(cls, curve_id=-1):
    """ By default, delete the current curve, if any. Otherwise do nothing.
        @return the id of the deleted curve or -1
    """
    Logger.Debug("Delete curve")
    control = cls.GetInstance()
    # Find the right plot set:
    if curve_id == -1:
      curve_id = cls.GetCurrentCurveID()
      if curve_id == -1:
        # No current curve, do nothing
        return -1 
      
    psID = cls.GetPlotSetID(curve_id)
    if psID == -1:
      raise ValueError("Curve ID (%d) not found for deletion!" % curve_id)
    crv = control._plotManager._plotSets[psID]._curves[curve_id]
    control._plotManager._plotSets[psID].removeCurve(curve_id)
    control.removeModelListeners(crv)
    return curve_id
  
  @classmethod
  def DeletePlotSet(cls, plot_set_id=-1):
    """ By default, delete the current plot set, if any. Otherwise do nothing.
        This will automatically make the last added plot set the current one.
        @return the id of the deleted plot set or -1
    """
    Logger.Debug("PlotController::DeletePlotSet %d" % plot_set_id)
    control = cls.GetInstance()
    # Find the right plot set:
    if plot_set_id == -1:
      plot_set_id = cls.GetCurrentPlotSetID()
      if plot_set_id == -1:
        # No current, do nothing
        return -1

    ps = control._plotManager.removeXYPlotSet(plot_set_id)
    for _, crv in ps._curves.items():
      control.removeModelListeners(crv)
    control.removeModelListeners(ps)
    psets = control._plotManager._plotSets 
    if len(psets):
      control._plotManager.setCurrentPlotSet(psets.keys()[-1])
    return plot_set_id
  
  @classmethod
  def usedMem(cls):
      import gc
      gc.collect()
      import resource
      m = resource.getrusage(resource.RUSAGE_SELF)[2]*resource.getpagesize()/1e6
      print "** Used memory: %.2f Mb" % m
  
  @classmethod
  def DeleteCurrentItem(cls):
    """ Delete currently active item, be it a plot set or a curve.
    @return (True, plot_sed_id) if a plot set was deleted or (False, curve_id) if a curve was deleted, or (True, -1)
    if nothing was deleted.
    """
    c_id = cls.GetCurrentCurveID()
    ps_id = cls.GetCurrentPlotSetID()
    ret = True, -1
    if ps_id == -1:
      Logger.Info("PlotController.DeleteCurrentItem(): nothing selected, nothing to delete!")
      return True,-1
    # Do we delete a curve or a full plot set    
    if c_id == -1:
      cls.DeletePlotSet(ps_id)
      ret = True, ps_id
    else:
      cls.DeleteCurve(c_id)
      ret = False, c_id
    return ret
  
  @classmethod
  def ClearPlotSet(cls, ps_id=-1):
    """ Clear all curves in a given plot set. By default clear the current plot set without deleting it,
    if no default plot set is currently active, do nothing.
    @return id of the cleared plot set
    @raise if invalid plot set ID is given
    """
    pm = cls.GetInstance()._plotManager
    if ps_id == -1:
      ps_id = cls.GetCurrentPlotSetID()
      if ps_id == -1:
        return ps_id
    ps = pm._plotSets.get(ps_id, None)
    if ps is None:
      raise ValueError("Invalid plot set ID (%d)!" % ps_id)
    ps.eraseAll()
    return ps_id
  
#   @classmethod
#   def ClearAll(cls):
#     # TODO: optimize
#     pm = cls.GetInstance()._plotManager
#     ids = pm._plotSets.keys()
#     for i in ids:
#       cls.DeletePlotSet(i)
  
  @classmethod
  def SetXLabel(cls, x_label, plot_set_id=-1):
    """  By default set the X axis label for the current plot set, if any. Otherwise do nothing.
         @return True if the label was set
    """
    pm = cls.GetInstance()._plotManager
    if plot_set_id == -1:
      plot_set_id = cls.GetCurrentPlotSetID()
      if plot_set_id == -1:
        # Do nothing
        return False 
    ps = pm._plotSets.get(plot_set_id, None)
    if ps is None:
      raise Exception("Invalid plot set ID (%d)!" % plot_set_id)
    ps.setXLabel(x_label)
    return True
     
  @classmethod 
  def SetYLabel(cls, y_label, plot_set_id=-1):
    """ By default set the Y axis label for the current plot set, if any. Otherwise do nothing.
         @return True if the label was set
    """
    pm = cls.GetInstance()._plotManager
    if plot_set_id == -1:
      plot_set_id = cls.GetCurrentPlotSetID()
      if plot_set_id == -1:
        # Do nothing
        return False 
    ps = pm._plotSets.get(plot_set_id, None)
    if ps is None:
      raise Exception("Invalid plot set ID (%d)!" % plot_set_id)
    ps.setYLabel(y_label)
    return True
     
  @classmethod 
  def SetPlotSetTitle(cls, title, plot_set_id=-1):
    """ By default set the title for the current plot set, if any. Otherwise do nothing.
         @return True if the title was set
    """
    pm = cls.GetInstance()._plotManager
    if plot_set_id == -1:
      plot_set_id = cls.GetCurrentPlotSetID()
      if plot_set_id == -1:
        # Do nothing
        return False 
    ps = pm._plotSets.get(plot_set_id, None)
    if ps is None:
      raise Exception("Invalid plot set ID (%d)!" % plot_set_id)
    ps.setTitle(title)
    return True
  
  @classmethod
  def GetPlotSetID(cls, curve_id):
    """ @return plot set id for a given curve or -1 if invalid curve ID
    """
    control = cls.GetInstance()
    cps = control._plotManager.getPlotSetContainingCurve(curve_id)
    if cps is None:
      return -1
    return cps.getID()
  
  @classmethod
  def GetPlotSetIDByName(cls, name):
    """ @return the first plot set whose name matches the provided name. Otherwise returns -1
    """ 
    pm = cls.GetInstance()._plotManager
    for _, ps in pm._plotSets.items():
      if ps._title == name:
        return ps.getID()
    return -1
  
  @classmethod
  def GetAllPlotSets(cls):
    """ @return two lists: plot set names, and corresponding plot set IDs
    """
    pm = cls.GetInstance()._plotManager
    it = pm._plotSets.items()
    ids, inst, titles = [], [], []
    if len(it):  
      ids, inst = zip(*it)        
    if len(inst):
      titles = [i.getTitle() for i in inst]
    return list(ids), titles
  
  @classmethod
  def GetCurrentCurveID(cls):
    """ @return current curve ID or -1 if no curve is currently active
    """
    control = cls.GetInstance()
    crv = control._plotManager.getCurrentCurve()
    if crv is None:
      return -1
    return crv.getID()
     
  @classmethod   
  def GetCurrentPlotSetID(cls):
    """ @return current plot set ID or -1 if no plot set is currently active
    """
    control = cls.GetInstance()
    cps = control._plotManager.getCurrentPlotSet()
    if cps is None:
      return -1
    return cps.getID()  

  @classmethod
  def SetCurrentPlotSet(cls, ps_id):
    """ Set the current active plot set. Use -1 to unset any current plot set.
    @throw if invalid ps_id
    """
    control = cls.GetInstance()
    control._plotManager.setCurrentPlotSet(ps_id)

  @classmethod
  def SetCurrentCurve(cls, crv_id):
    """ Set the current active curve.
    @return corresponding plot set ID
    @throw if invalid crv_id
    """
    control = cls.GetInstance()
    ps_id = control._plotManager.setCurrentCurve(crv_id)
    return ps_id

  @classmethod
  def ActiveViewChanged(cls, viewID):
    """ This method is to be plugged direclty in the activeViewChanged() slot of a standard
    Python SALOME module so that the curve browser stays in sync with the selected SALOME view
    """
    control = cls.GetInstance()
    # Get XYView from SALOME view ID
    xyview = control._curveTabsView._XYViews.get(viewID, None)
    if not xyview is None:
      plotSetID = xyview.getModel().getID()
      control._plotManager.setCurrentPlotSet(plotSetID)

  @classmethod
  def ToggleCurveBrowser(cls, active):
    if cls.__UNIQUE_INSTANCE is not None:
      raise Exception("ToggleCurveBrowser() must be invoked before doing anything in plot2D!")
    cls.WITH_CURVE_BROWSER = active
    
  @classmethod
  def IsValidPlotSetID(cls, plot_set_id):
    """ 
    @return True if plot_set_id is the identifier of a valid and existing plot set.
    """
    control = cls.GetInstance()
    return control._plotManager._plotSets.has_key(plot_set_id)

  @classmethod
  def GetSalomeViewID(cls, plot_set_id):
    """
    @return the salome view ID associated to a given plot set. -1 if invalid plot_set_id
    """
    control = cls.GetInstance()
    d = control._curveTabsView.mapModId2ViewId()
    return d.get(plot_set_id, -1)

  @classmethod
  def OnSalomeViewTryClose(cls, salome_view_id):
    control = cls.GetInstance()
    if not control._blockViewClosing:
      Logger.Debug("PlotController::OnSalomeViewTryClose %d" % salome_view_id)
#       control._sgPyQt.setViewClosable(salome_view_id, False)
      # Get XYView from SALOME view ID
      xyview = control._curveTabsView._XYViews.get(salome_view_id, None)
      if not xyview is None:
        plotSetID = xyview.getModel().getID()
        Logger.Debug("PlotController::OnSalomeViewTryClose internal CurvePlot view ID is %d" % plotSetID)
        control._plotManager.removeXYPlotSet(plotSetID)
      else:
        Logger.Warning("Internal error - could not match SALOME view ID %d with CurvePlot view!" % salome_view_id)

  @classmethod
  def SetCurveMarker(cls, crv_id, marker):
    """ Change curve marker. Available markers are:
    CURVE_MARKERS = [ "o" ,#  circle
                    "*",  # star
                    "+",  # plus
                    "x",  # x
                    "s",  # square
                    "p",  # pentagon
                    "h",  # hexagon1
                    "8",  # octagon
                    "D",  # diamond
                    "^",  # triangle_up
                    "<",  # triangle_left
                    ">",  # triangle_right
                    "1",  # tri_down
                    "2",  # tri_up
                    "3",  # tri_left
                    "4",  # tri_right
                    "v",  # triangle_down
                    "H",  # hexagon2
                    "d",  # thin diamond
                    "",   # NO MARKER
                   ]
    @raise if invalid curve ID or marker
    """
    from XYView import XYView
    from CurveView import CurveView
    if not marker in XYView.CURVE_MARKERS:
      raise ValueError("Invalid marker: '%s'" % marker)
    
    cont = cls.GetInstance()
    for mod, views in cont._modelViews.items():
      if isinstance(mod, CurveModel) and mod.getID() == crv_id:
        for v in views:
          if isinstance(v, CurveView):
            v.setMarker(marker)
            # Update curve display and legend:
            v._parentXYView.repaint()
            v._parentXYView.showHideLegend()
            found = True
        
    if not found:
      raise Exception("Invalid curve ID or curve currently not displayed (curve_id=%d)!" % crv_id)

  @classmethod
  def SetCurveLabel(cls, crv_id, label):
    """ Change curve label
    @raise if invalid curve id
    """
    cont = cls.GetInstance()
    cps = cont._plotManager.getPlotSetContainingCurve(crv_id)
    if cps is None:
      raise ValueError("Invalid curve ID: %d" % crv_id)
    cps._curves[crv_id].setTitle(label)

  @classmethod
  def __XYViewOperation(cls, func, ps_id, args, kwargs):
    """ Private. To factorize methods accessing the XYView to change a display element. """
    from XYPlotSetModel import XYPlotSetModel
    from XYView import XYView
    
    cont = cls.GetInstance()
    for mod, views in cont._modelViews.items():
      if isinstance(mod, XYPlotSetModel) and mod.getID() == ps_id:
        for v in views:
          if isinstance(v, XYView):
            exec "v.%s(*args, **kwargs)" % func
            found = True
    if not found:
      raise Exception("Invalid plot set ID or plot set currently not displayed (ps_id=%d)!" % ps_id)


  @classmethod
  def SetXLog(cls, ps_id, log=True):
    """ Toggle the X axis into logarithmic scale.
    @param ps_id plot set ID
    @param log if set to True, log scale is used, otherwise linear scale is used
    @raise if invalid plot set ID
    """
    args, kwargs = [log], {}
    cls.__XYViewOperation("setXLog", ps_id, args, kwargs)

  @classmethod
  def SetYLog(cls, ps_id, log=True):
    """ Toggle the Y axis into logarithmic scale.
    @param ps_id plot set ID
    @param log if set to True, log scale is used, otherwise linear scale is used
    @raise if invalid plot set ID
    """
    args, kwargs = [log], {}
    cls.__XYViewOperation("setYLog", ps_id, args, kwargs)
     
  @classmethod
  def SetXSciNotation(cls, ps_id, sciNotation=False):
    """ Change the format (scientific notation or not) of the X axis.
    @param ps_id plot set ID
    @param sciNotation if set to True, scientific notation is used, otherwise plain notation is used
    @raise if invalid plot set ID
    """
    args, kwargs = [sciNotation], {}
    cls.__XYViewOperation("setXSciNotation", ps_id, args, kwargs)
   
  @classmethod
  def SetYSciNotation(cls, ps_id, sciNotation=False):
    """ Change the format (scientific notation or not) of the Y axis.
    @param ps_id plot set ID
    @param sciNotation if set to True, scientific notation is used, otherwise plain notation is used
    @raise if invalid plot set ID
    """
    args, kwargs = [sciNotation], {}
    cls.__XYViewOperation("setYSciNotation", ps_id, args, kwargs)

  @classmethod
  def SetLegendVisible(cls, ps_id, visible=True):
    """ Change the visibility of the legend.
    @param ps_id plot set ID
    @param visible if set to True, show legend, otherwise hide it.
    @raise if invalid plot set ID
    """
    args, kwargs = [visible], {}
    cls.__XYViewOperation("setLegendVisible", ps_id, args, kwargs)
    

  ###
  ### More advanced functions
  ###
  @classmethod
  def RegisterCallback(cls, callback):
    cont = cls.GetInstance()
    cont._callbacks.append(callback)
  
  @classmethod
  def ClearCallbacks(cls):
    cont = cls.GetInstance()
    cont._callbacks = []
  
  @classmethod
  def LockRepaint(cls):
    control = cls.GetInstance()
    control._plotManager.lockRepaint()
  
  @classmethod
  def UnlockRepaint(cls):
    control = cls.GetInstance()
    control._plotManager.unlockRepaint()  
  
  def createTable(self, data, table_name="table"):
    t = TableModel(self)
    t.setData(data)
    t.setTitle(table_name)
    return t
     
  def plotCurveFromTable(self, table, x_col_index=0, y_col_index=1, curve_label="", append=True):
    """
    :returns: a tuple containing the unique curve ID and the plot set ID 
    """
    # Regardless of 'append', we must create a view if none there:
    if self._plotManager.getCurrentPlotSet() is None or not append:
      ps = self._plotManager.createXYPlotSet()
      self.setModelListener(ps, self._curveBrowserView)
      # For curve picking, controller must listen:
      self.setModelListener(ps, self)
      cps_title = table.getTitle()
    else:
      cps_title = None

    cps = self._plotManager.getCurrentPlotSet()
    
    cm = CurveModel(self, table, y_col_index)
    cm.setXAxisIndex(x_col_index)
    
    # X axis label
    tix = table.getColumnTitle(x_col_index)
    if tix != "":
      cps.setXLabel(tix)
    
    # Curve label
    if curve_label != "":
      cm.setTitle(curve_label)
    else:
      ti = table.getColumnTitle(y_col_index)
      if ti != "":
        cm.setTitle(ti)

    # Plot set title
    if cps_title != "" and cps_title is not None:
      Logger.Debug("about to set title to: " + cps_title)  
      cps.setTitle(cps_title)
    
    cps.addCurve(cm)
    mp = self._curveTabsView.mapModId2ViewId()
    xyview_id = mp[cps.getID()]
    xyview = self._curveTabsView._XYViews[xyview_id]
    
    if cps_title is None:  # no plot set was created above
      self._plotManager.setCurrentPlotSet(cps.getID())
      
    # Make CurveBrowser and CurveView depend on changes in the curve itself:
    self.setModelListener(cm, self._curveBrowserView)
    self.setModelListener(cm, xyview._curveViews[cm.getID()])
    # Upon change on the curve also update the full plot, notably for the auto-fit and the legend:
    self.setModelListener(cm, xyview)
        
    return cm.getID(),cps.getID()
