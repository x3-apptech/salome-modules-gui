import matplotlib.pyplot as plt
import matplotlib.colors as colors
from View import View
from CurveView import CurveView

from utils import Logger, trQ
from PlotWidget import PlotWidget
from PlotSettings import PlotSettings
from pyqtside import QtGui, QtCore
from pyqtside.QtCore import QObject
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg, NavigationToolbar2QT

class EventHandler(QObject):
  """ Handle the right-click properly so that it only triggers the contextual menu """
  def __init__(self,parent=None):
    QObject.__init__(self, parent)
  
  def eventFilter(self, obj, event):
    if event.type() == QtCore.QEvent.MouseButtonPress:
      if event.button() == 2:
        # Discarding right button press to only keep context menu display
        return True # Event handled (and hence not passed to matplotlib)
    return QObject.eventFilter(self, obj, event)

class XYView(View):
  AUTOFIT_MARGIN = 0.03  # 3%
  
  # See http://matplotlib.org/api/markers_api.html:
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
  
  _DEFAULT_LEGEND_STATE = False   # for test purposes mainly - initial status of the legend
  
  def __init__(self, controller):
    View.__init__(self, controller)
    self._eventHandler = EventHandler()
    
    self._curveViews = {}    # key: curve (model) ID, value: CurveView
    self._salomeViewID = None
    self._mplFigure = None
    self._mplAxes = None
    self._mplCanvas = None
    self._plotWidget = None
    self._sgPyQt = self._controller._sgPyQt
    self._toolbar = None
    self._mplNavigationActions = {}
    self._toobarMPL = None
    self._grid = None
    self._currCrv = None   # current curve selected in the view
    
    self._legend = None
    self._legendLoc = "right"  # "right" or "bottom"
    
    self._fitArea = False
    self._zoomPan = False
    self._dragOnDrop = False
    self._move = False
    
    self._patch = None
    self._xdata = None
    self._ydata = None
    self._defaultLineStyle = None
    self._last_point = None
    self._lastMarkerID = -1
    self._blockLogSignal = False
    
    self._axisXSciNotation = False
    self._axisYSciNotation = False
    self._prevTitle = None
    
  def __repaintOK(self):
    """ To be called inside XYView each time a low-level expansive matplotlib methods is to be invoked.
    @return False if painting is currently locked, in which case it will also register the current XYView 
    as needing a refresh when unlocked
    """
    ret = self._controller._plotManager.isRepaintLocked()
    if ret:
      self._controller._plotManager.registerRepaint(self._model)
    return (not ret)
    
  def appendCurve(self, curveID):
    newC = CurveView(self._controller, self)
    newC.setModel(self._model._curves[curveID])
    newC.setMPLAxes(self._mplAxes) 
    newC.draw()
    newC.setMarker(self.getMarker(go_next=True))
    self._curveViews[curveID] = newC 
    
  def removeCurve(self, curveID):
    v = self._curveViews.pop(curveID)
    v.erase()
    if self._currCrv is not None and self._currCrv.getID() == curveID:
      self._currCrv = None
  
  def cleanBeforeClose(self):
    """ Clean some items to avoid accumulating stuff in memory """
    self._mplFigure.clear()
    plt.close(self._mplFigure)
    self._plotWidget.clearAll()
    # For memory debugging only:
    import gc
    gc.collect()
  
  def repaint(self):
    if self.__repaintOK():
      Logger.Debug("XYView::draw")
      self._mplCanvas.draw()

  def onXLabelChange(self):
    if self.__repaintOK():
      self._mplAxes.set_xlabel(self._model._xlabel)
      self.repaint()
    
  def onYLabelChange(self):
    if self.__repaintOK():
      self._mplAxes.set_ylabel(self._model._ylabel)
      self.repaint()
  
  def onTitleChange(self):
    if self.__repaintOK():
      self._mplAxes.set_title(self._model._title)
      self.updateViewTitle()
      self.repaint()
  
  def onCurveTitleChange(self):
    # Updating the legend should suffice
    self.showHideLegend()
  
  def onClearAll(self):
    """ Just does an update with a reset of the marker cycle. """
    if self.__repaintOK():
      self._lastMarkerID = -1
      self.update()
  
  def onPick(self, event):
    """ MPL callback when picking
    """
    if event.mouseevent.button == 1:
      selected_id = -1
      a = event.artist
      for crv_id, cv in self._curveViews.items():
        if cv._mplLines[0] is a:
          selected_id = crv_id
      # Use the plotmanager so that other plot sets get their current reset:
      self._controller._plotManager.setCurrentCurve(selected_id)
  
  def createAndAddLocalAction(self, icon_file, short_name):
    return self._toolbar.addAction(self._sgPyQt.loadIcon("CURVEPLOT", icon_file), short_name)
    
  def createPlotWidget(self):
    self._mplFigure = Figure((8.0,5.0), dpi=100)
    self._mplCanvas = FigureCanvasQTAgg(self._mplFigure)
    self._mplCanvas.installEventFilter(self._eventHandler)
    self._mplCanvas.mpl_connect('pick_event', self.onPick)
    self._mplAxes = self._mplFigure.add_subplot(1, 1, 1)
    self._plotWidget = PlotWidget()
    self._toobarMPL = NavigationToolbar2QT(self._mplCanvas, None) 
    for act in self._toobarMPL.actions():
      actionName = str(act.text()).strip()
      self._mplNavigationActions[actionName] = act
    self._plotWidget.setCentralWidget(self._mplCanvas)
    self._toolbar = self._plotWidget.toolBar
    self.populateToolbar()
     
    self._popupMenu = QtGui.QMenu()
    self._popupMenu.addAction(self._actionLegend)
    
    # Connect evenement for the graphic scene
    self._mplCanvas.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
    self._mplCanvas.customContextMenuRequested.connect(self.onContextMenu) 
    self._mplCanvas.mpl_connect('scroll_event', self.onScroll)
    self._mplCanvas.mpl_connect('button_press_event', self.onMousePress)
  
  def populateToolbar(self):
    # Action to dump view in a file
    a = self.createAndAddLocalAction("dump_view.png", trQ("DUMP_VIEW_TXT"))
    a.triggered.connect(self.dumpView)
    self._toolbar.addSeparator()
    # Actions to manipulate the scene
    a = self.createAndAddLocalAction("fit_all.png", trQ("FIT_ALL_TXT"))
    a.triggered.connect(self.autoFit)
    #    Zoom and pan are mutually exclusive but can be both de-activated: 
    self._zoomAction = self.createAndAddLocalAction("fit_area.png", trQ("FIT_AREA_TXT"))
    self._zoomAction.triggered.connect(self.zoomArea)
    self._zoomAction.setCheckable(True)
    self._panAction = self.createAndAddLocalAction("zoom_pan.png", trQ("ZOOM_PAN_TXT"))
    self._panAction.triggered.connect(self.pan)
    self._panAction.setCheckable(True)
    self._toolbar.addSeparator()
    # Actions to change the representation of curves
    self._curveActionGroup = QtGui.QActionGroup(self._plotWidget)
    self._pointsAction = self.createAndAddLocalAction("draw_points.png", trQ("DRAW_POINTS_TXT"))
    self._pointsAction.setCheckable(True)
    self._linesAction = self.createAndAddLocalAction("draw_lines.png", trQ("DRAW_LINES_TXT"))
    self._linesAction.setCheckable(True)
    self._curveActionGroup.addAction(self._pointsAction)
    self._curveActionGroup.addAction(self._linesAction)
    self._linesAction.setChecked(True)
    self._curveActionGroup.triggered.connect(self.changeModeCurve)
    self._curveActionGroup.setExclusive(True)
    self._toolbar.addSeparator()
    # Actions to draw horizontal curves as linear or logarithmic
    self._horActionGroup = QtGui.QActionGroup(self._plotWidget)
    self._horLinearAction = self.createAndAddLocalAction("hor_linear.png", trQ("HOR_LINEAR_TXT"))
    self._horLinearAction.setCheckable(True)
    self._horLogarithmicAction = self.createAndAddLocalAction("hor_logarithmic.png", trQ("HOR_LOGARITHMIC_TXT"))
    self._horLogarithmicAction.setCheckable(True)
    self._horActionGroup.addAction(self._horLinearAction)
    self._horActionGroup.addAction(self._horLogarithmicAction)
    self._horLinearAction.setChecked(True)
    self._horActionGroup.triggered.connect(self.onViewHorizontalMode)
    self._toolbar.addSeparator()
    # Actions to draw vertical curves as linear or logarithmic
    self._verActionGroup = QtGui.QActionGroup(self._plotWidget)
    self._verLinearAction = self.createAndAddLocalAction("ver_linear.png", trQ("VER_LINEAR_TXT"))
    self._verLinearAction.setCheckable(True)
    self._verLogarithmicAction = self.createAndAddLocalAction("ver_logarithmic.png", trQ("VER_LOGARITHMIC_TXT"))
    self._verLogarithmicAction.setCheckable(True)
    self._verActionGroup.addAction(self._verLinearAction)
    self._verActionGroup.addAction(self._verLogarithmicAction)
    self._verLinearAction.setChecked(True)
    self._verActionGroup.triggered.connect(self.onViewVerticalMode)
    self._verActionGroup.setExclusive(True)
    self._toolbar.addSeparator()
    # Action to show or hide the legend 
    self._actionLegend = self.createAndAddLocalAction("legend.png", trQ("SHOW_LEGEND_TXT"))
    self._actionLegend.setCheckable(True)
    self._actionLegend.triggered.connect(self.showHideLegend)
    if self._DEFAULT_LEGEND_STATE:
      self._actionLegend.setChecked(True)
    self._toolbar.addSeparator()
    # Action to set the preferences
    a = self.createAndAddLocalAction("settings.png", trQ("SETTINGS_TXT"))
    a.triggered.connect(self.onSettings)
    pass
    
  def dumpView(self):
    # Choice of the view backup file
    filters = []
    for form in ["IMAGES_FILES", "PDF_FILES", "POSTSCRIPT_FILES", "ENCAPSULATED_POSTSCRIPT_FILES"]:
      filters.append(trQ(form))
    fileName = self._sgPyQt.getFileName(self._sgPyQt.getDesktop(),
                                        "",
                                        filters,
                                        trQ("DUMP_VIEW_FILE"),
                                        False )
    if not fileName.isEmpty():
      name = str(fileName)
      self._mplAxes.figure.savefig(name)
    pass
    
  def autoFit(self, check=True, repaint=True):
    if self.__repaintOK():
      self._mplAxes.relim()
      xm, xM = self._mplAxes.xaxis.get_data_interval()
      ym, yM = self._mplAxes.yaxis.get_data_interval()
      i = yM-ym
      self._mplAxes.axis([xm, xM, ym-i*self.AUTOFIT_MARGIN, yM+i*self.AUTOFIT_MARGIN])
      if repaint:
        self.repaint()
  
  def zoomArea(self):
    if self._panAction.isChecked() and self._zoomAction.isChecked():
      self._panAction.setChecked(False)
    # Trigger underlying matplotlib action:
    self._mplNavigationActions["Zoom"].trigger()
  
  def pan(self):
    if self._panAction.isChecked() and self._zoomAction.isChecked():
      self._zoomAction.setChecked(False)
    # Trigger underlying matplotlib action:
    self._mplNavigationActions["Pan"].trigger()

  def getMarker(self, go_next=False):
    if go_next:
      self._lastMarkerID = (self._lastMarkerID+1) % len(self.CURVE_MARKERS)
    return self.CURVE_MARKERS[self._lastMarkerID]

  def changeModeCurve(self, repaint=True):
    if not self.__repaintOK():
      return
    action = self._curveActionGroup.checkedAction()
    if action is self._pointsAction :
      for crv_view in self._curveViews.values():
        crv_view.setLineStyle("None")
    elif action is self._linesAction :
      for crv_view in self._curveViews.values():
        crv_view.setLineStyle("-")
    else :
      raise NotImplementedError
    if repaint:
      self.repaint()
  
  def setXLog(self, log, repaint=True):
    if not self.__repaintOK():
      return
    self._blockLogSignal = True
    if log:
      self._mplAxes.set_xscale('log')
      self._horLogarithmicAction.setChecked(True)
    else:
      self._mplAxes.set_xscale('linear')
      self._horLinearAction.setChecked(True)
    if repaint:
      self.autoFit()
      self.repaint()
    self._blockLogSignal = False

  def setYLog(self, log, repaint=True):
    if not self.__repaintOK():
      return
    self._blockLogSignal = True
    if log:
      self._mplAxes.set_yscale('log')
      self._verLogarithmicAction.setChecked(True)
    else:
      self._mplAxes.set_yscale('linear')
      self._verLinearAction.setChecked(True)
    if repaint:
      self.autoFit()
      self.repaint()
    self._blockLogSignal = False
    
  def setXSciNotation(self, sciNotation, repaint=True):
    self._axisXSciNotation = sciNotation
    self.changeFormatAxis()
    if repaint:
      self.repaint()
   
  def setYSciNotation(self, sciNotation, repaint=True):
    self._axisYSciNotation = sciNotation
    self.changeFormatAxis()
    if repaint:
      self.repaint()
    
  def onViewHorizontalMode(self, checked=True, repaint=True):
    if self._blockLogSignal:
      return
    action = self._horActionGroup.checkedAction()  
    if action is self._horLinearAction:
      self.setXLog(False, repaint)
    elif action is self._horLogarithmicAction:
      self.setXLog(True, repaint)
    else:
      raise NotImplementedError
  
  def onViewVerticalMode(self, checked=True, repaint=True):
    if self._blockLogSignal:
      return
    action = self._verActionGroup.checkedAction()  
    if action is self._verLinearAction:
      self.setYLog(False, repaint)
    elif action is self._verLogarithmicAction:
      self.setYLog(True, repaint)
    else:
      raise NotImplementedError
    if repaint:
      self.repaint()
  
  def __adjustFigureMargins(self, withLegend):
    """ Adjust figure margins to make room for the legend """
    if withLegend:
      leg = self._legend
      bbox = leg.get_window_extent()
      # In axes coordinates: 
      bbox2 = bbox.transformed(leg.figure.transFigure.inverted())
      if self._legendLoc == "right":
        self._mplFigure.subplots_adjust(right=1.0-(bbox2.width+0.02))
      elif self._legendLoc == "bottom":
        self._mplFigure.subplots_adjust(bottom=bbox2.height+0.1)
    else:
      # Reset to default (rc) values
      self._mplFigure.subplots_adjust(bottom=0.1, right=0.9)
  
  def setLegendVisible(self, visible, repaint=True):
    if visible and not self._actionLegend.isChecked():
      self._actionLegend.setChecked(True)
      self.showHideLegend(repaint=repaint)
    if not visible and self._actionLegend.isChecked():
      self._actionLegend.setChecked(False)
      self.showHideLegend(repaint=repaint)
  
  def showHideLegend(self, actionChecked=None, repaint=True):
    if not self.__repaintOK():  # Show/hide legend is extremely costly
      return
    
    show = self._actionLegend.isChecked()
    nCurves = len(self._curveViews)
    if nCurves > 10: fontSize = 'x-small'
    else:            fontSize = None
    
    if nCurves == 0:
      # Remove legend 
      leg = self._mplAxes.legend()
      if leg is not None: leg.remove()
    if show and nCurves > 0:
      # Recreate legend from scratch
      if self._legend is not None:
        self._legend = None
        self._mplAxes._legend = None
      if self._legendLoc == "bottom":
        self._legend = self._mplAxes.legend(loc="upper left", bbox_to_anchor=(0.0, -0.05, 1.0, -0.05), 
                                            borderaxespad=0.0, mode="expand", fancybox=True, 
                                            shadow=True, ncol=3, prop={'size':fontSize, 'style': 'italic'})
      elif self._legendLoc == "right":
        self._legend = self._mplAxes.legend(loc="upper left", bbox_to_anchor=(1.02,1.0), borderaxespad=0.0,
                                            ncol=1, fancybox=True, shadow=True, prop={'size':fontSize, 'style': 'italic'})
      else:
        raise Exception("Invalid legend placement! Must be 'bottom' or 'right'")
      # Canvas must be drawn so we can adjust the figure placement:
      self._mplCanvas.draw()
      self.__adjustFigureMargins(withLegend=True)
    else:
      if self._legend is None:
        # Nothing to do
        return
      else:
        self._legend.set_visible(False)
        self._legend = None
        self._mplAxes._legend = None
        self._mplCanvas.draw()
        self.__adjustFigureMargins(withLegend=False)
    
    curr_crv = self._model._currentCurve
    if curr_crv is None: curr_title = None
    else:                curr_title = curr_crv.getTitle()
    if self._legend is not None:
      for label in self._legend.get_texts() :
        text = label.get_text()
        if (text == curr_title):
          label.set_backgroundcolor('0.85')
        else :
          label.set_backgroundcolor('white')
        
    if repaint:
      self.repaint()
   
  def onSettings(self, trigger=False, dlg_test=None):
    dlg = dlg_test or PlotSettings()
    dlg.titleEdit.setText(self._mplAxes.get_title())
    dlg.axisXTitleEdit.setText(self._mplAxes.get_xlabel())
    dlg.axisYTitleEdit.setText(self._mplAxes.get_ylabel())
    dlg.gridCheckBox.setChecked(self._mplAxes.xaxis._gridOnMajor)  # could not find a relevant API to check this
    dlg.axisXSciCheckBox.setChecked(self._axisXSciNotation)
    dlg.axisYSciCheckBox.setChecked(self._axisYSciNotation)
    xmin, xmax = self._mplAxes.get_xlim()
    ymin, ymax = self._mplAxes.get_ylim()
    xminText = "%g" %xmin
    xmaxText = "%g" %xmax
    yminText = "%g" %ymin
    ymaxText = "%g" %ymax
    dlg.axisXMinEdit.setText(xminText)
    dlg.axisXMaxEdit.setText(xmaxText)
    dlg.axisYMinEdit.setText(yminText)
    dlg.axisYMaxEdit.setText(ymaxText)
    # List of markers
    dlg.markerCurve.clear()
    for marker in self.CURVE_MARKERS :
      dlg.markerCurve.addItem(marker)
    curr_crv = self._model.getCurrentCurve()
    if not curr_crv is None:
      dlg.colorCurve.setEnabled(True)
      dlg.markerCurve.setEnabled(True)
      name = curr_crv.getTitle()
      dlg.nameCurve.setText(name)
      view = self._curveViews[curr_crv.getID()] 
      marker = view.getMarker()
      color = view.getColor()
      index = dlg.markerCurve.findText(marker)
      dlg.markerCurve.setCurrentIndex(index)
      rgb = colors.colorConverter.to_rgb(color)
      dlg.setRGB(rgb[0],rgb[1],rgb[2])
    else :
      dlg.colorCurve.setEnabled(False)
      dlg.markerCurve.setEnabled(False)
      dlg.nameCurve.setText("")
      view = None
    if self._legend is None:
      dlg.showLegendCheckBox.setChecked(False)
      dlg.legendPositionComboBox.setEnabled(False)
    else :
      if self._legend.get_visible():
        dlg.showLegendCheckBox.setChecked(True)
        dlg.legendPositionComboBox.setEnabled(True)
        if self._legendLoc == "bottom":
          dlg.legendPositionComboBox.setCurrentIndex(0)
        elif self._legendLoc == "right" :
          dlg.legendPositionComboBox.setCurrentIndex(1)
      else :
        dlg.showLegendCheckBox.setChecked(False)
        dlg.legendPositionComboBox.setEnabled(False)    
             
    if dlg.exec_():
      # Title
      self._model.setTitle(dlg.titleEdit.text())
      # Axis
      self._model.setXLabel(dlg.axisXTitleEdit.text())
      self._model.setYLabel(dlg.axisYTitleEdit.text())
      # Grid
      if dlg.gridCheckBox.isChecked() :
        self._mplAxes.grid(True)
      else :
        self._mplAxes.grid(False)
      # Legend
      if  dlg.showLegendCheckBox.isChecked():
        self._actionLegend.setChecked(True)
        if dlg.legendPositionComboBox.currentIndex() == 0 :
          self._legendLoc = "bottom"
        elif dlg.legendPositionComboBox.currentIndex() == 1 :
          self._legendLoc = "right"
      else :
        self._actionLegend.setChecked(False)
      xminText = dlg.axisXMinEdit.text()
      xmaxText = dlg.axisXMaxEdit.text()
      yminText = dlg.axisYMinEdit.text()
      ymaxText = dlg.axisYMaxEdit.text()
      self._mplAxes.axis([float(xminText), float(xmaxText), float(yminText), float(ymaxText)] )
      self._axisXSciNotation = dlg.axisXSciCheckBox.isChecked()
      self._axisYSciNotation = dlg.axisYSciCheckBox.isChecked()
      self.changeFormatAxis()
      # Color and marker of the curve
      if view:
        view.setColor(dlg.getRGB()) 
        view.setMarker(self.CURVE_MARKERS[dlg.markerCurve.currentIndex()])
      self.showHideLegend(repaint=True)
      self._mplCanvas.draw()
    pass
    
  def updateViewTitle(self):
    s = ""
    if self._model._title != "":
      s = " - %s" % self._model._title
    title = "CurvePlot (%d)%s" % (self._model.getID(), s)
    self._sgPyQt.setViewTitle(self._salomeViewID, title)
    
  def onCurrentPlotSetChange(self):
    """ Avoid a unnecessary call to update() when just switching current plot set! """ 
    pass
  
  def onCurrentCurveChange(self):
    curr_crv2 = self._model.getCurrentCurve()
    if curr_crv2 != self._currCrv:
      if self._currCrv is not None:
        view = self._curveViews[self._currCrv.getID()]
        view.toggleHighlight(False)
      if not curr_crv2 is None:
        view = self._curveViews[curr_crv2.getID()] 
        view.toggleHighlight(True)
      self._currCrv = curr_crv2
      self.showHideLegend(repaint=False) # redo legend
      self.repaint() 
      
  def changeFormatAxis(self) :
    if not self.__repaintOK():
      return
    
    # don't try to switch to sci notation if we are not using the 
    # matplotlib.ticker.ScalarFormatter (i.e. if in Log for ex.)
    if self._horLinearAction.isChecked():
      if self._axisXSciNotation :
        self._mplAxes.ticklabel_format(style='sci',scilimits=(0,0), axis='x')
      else :
        self._mplAxes.ticklabel_format(style='plain',axis='x')
    if self._verLinearAction.isChecked():    
      if self._axisYSciNotation :
        self._mplAxes.ticklabel_format(style='sci',scilimits=(0,0), axis='y')
      else :
        self._mplAxes.ticklabel_format(style='plain',axis='y')
    
  def update(self):
    if self._salomeViewID is None:
      self.createPlotWidget()
      self._salomeViewID = self._sgPyQt.createView("CurvePlot", self._plotWidget)
      Logger.Debug("Creating SALOME view ID=%d" % self._salomeViewID)
      self._sgPyQt.setViewVisible(self._salomeViewID, True)
    
    self.updateViewTitle()
    
    # Check list of curve views:
    set_mod = set(self._model._curves.keys())
    set_view = set(self._curveViews.keys())
    
    # Deleted/Added curves:
    dels = set_view - set_mod
    added = set_mod - set_view
    
    for d in dels:
      self.removeCurve(d)

    if not len(self._curveViews):
      # Reset color cycle 
      self._mplAxes.set_color_cycle(None)

    for a in added:
      self.appendCurve(a)

    # Axes labels and title
    self._mplAxes.set_xlabel(self._model._xlabel)
    self._mplAxes.set_ylabel(self._model._ylabel)
    self._mplAxes.set_title(self._model._title)

    self.onViewHorizontalMode(repaint=False)
    self.onViewVerticalMode(repaint=False)
    self.changeModeCurve(repaint=False)
    self.showHideLegend(repaint=False)   # The canvas is repainted anyway (needed to get legend bounding box)
    self.changeFormatAxis()

    # Redo auto-fit
    self.autoFit(repaint=False)
    self.repaint()
  
  def onDataChange(self):
    # the rest is done in the CurveView:
    self.autoFit(repaint=True)
    
  def onMousePress(self, event):
    if event.button == 3 :
      if self._panAction.isChecked():
        self._panAction.setChecked(False)
      if self._zoomAction.isChecked():
        self._zoomAction.setChecked(False)
    
  def onContextMenu(self, position):
    pos = self._mplCanvas.mapToGlobal(QtCore.QPoint(position.x(),position.y()))
    self._popupMenu.exec_(pos)
    
  def onScroll(self, event):
    # Event location (x and y)
    xdata = event.xdata
    ydata = event.ydata
    
    cur_xlim = self._mplAxes.get_xlim()
    cur_ylim = self._mplAxes.get_ylim()
    
    base_scale = 2.
    if event.button == 'down':
      # deal with zoom in
      scale_factor = 1 / base_scale
    elif event.button == 'up':
      # deal with zoom out
      scale_factor = base_scale
    else:
      # deal with something that should never happen
      scale_factor = 1
    
    new_width = (cur_xlim[1] - cur_xlim[0]) * scale_factor
    new_height = (cur_ylim[1] - cur_ylim[0]) * scale_factor

    relx = (cur_xlim[1] - xdata)/(cur_xlim[1] - cur_xlim[0])
    rely = (cur_ylim[1] - ydata)/(cur_ylim[1] - cur_ylim[0])

    self._mplAxes.set_xlim([xdata - new_width * (1-relx), xdata + new_width * (relx)])
    self._mplAxes.set_ylim([ydata - new_height * (1-rely), ydata + new_height * (rely)])
    
    self.repaint()
    pass
    
  def onPressEvent(self, event):
    if event.button == 3 :
      #self._mplCanvas.emit(QtCore.SIGNAL("button_release_event()"))
      canvasSize = event.canvas.geometry()
      point = event.canvas.mapToGlobal(QtCore.QPoint(event.x,canvasSize.height()-event.y))
      self._popupMenu.exec_(point)
    else :
      print "Press event on the other button"
    #if event.button == 3 :
    #  canvasSize = event.canvas.geometry()
    #  point = event.canvas.mapToGlobal(QtCore.QPoint(event.x,canvasSize.height()-event.y))
    #  self._popupMenu.move(point)
    #  self._popupMenu.show()
   
  def onMotionEvent(self, event):
    print "OnMotionEvent ",event.button
    #if event.button == 3 :
    #  event.button = None
    #  return True
   
  def onReleaseEvent(self, event):
    print "OnReleaseEvent ",event.button
    #if event.button == 3 :
    #  event.button = None
    #  return False
