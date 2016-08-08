"""
CurvePlot Python package.
"""
try:
  # Some unicode chars are improperly rendered with the default 'tkagg' backend (delta for ex)
  # and the Bitstream font which is the first one by default. Try to use DejaVu which is more 
  # comprehensive.
  ## !!Order of the sequence below is highly sensitive!!
  import pyqtside   # will trigger the PySide/PyQt4 switch
  import matplotlib
  matplotlib.use('Qt4Agg')
  import matplotlib.pyplot as plt  # must come after the PySide/PyQt4 switch!
  plt.rcParams['font.sans-serif'].insert(0, u"DejaVu Sans")
except:
  print "Warning: could not switch matplotlib to 'Qt4agg' backend. Some characters might be displayed improperly!"

from PlotController import PlotController
from TableModel import TableModel
from CurveModel import CurveModel
from PlotManager import PlotManager
from XYPlotSetModel import XYPlotSetModel

## The static API of PlotController is the main interface of the package and is hence exposed at package level:
AddCurve = PlotController.AddCurve
AddPlotSet = PlotController.AddPlotSet
ExtendCurve = PlotController.ExtendCurve
ResetCurve = PlotController.ResetCurve
CopyCurve = PlotController.CopyCurve
DeleteCurve = PlotController.DeleteCurve
DeletePlotSet = PlotController.DeletePlotSet
ClearPlotSet = PlotController.ClearPlotSet
SetXLabel = PlotController.SetXLabel
SetYLabel = PlotController.SetYLabel
GetPlotSetID = PlotController.GetPlotSetID
GetPlotSetIDByName = PlotController.GetPlotSetIDByName
GetAllPlotSets = PlotController.GetAllPlotSets
GetCurrentCurveID = PlotController.GetCurrentCurveID
GetCurrentPlotSetID = PlotController.GetCurrentPlotSetID
SetCurrentCurve = PlotController.SetCurrentCurve
SetCurrentPlotSet = PlotController.SetCurrentPlotSet
DeleteCurrentItem = PlotController.DeleteCurrentItem
SetCurveMarker = PlotController.SetCurveMarker
SetCurveLabel = PlotController.SetCurveLabel
SetXLog = PlotController.SetXLog
SetYLog = PlotController.SetYLog
SetXSciNotation = PlotController.SetXSciNotation
SetYSciNotation = PlotController.SetYSciNotation
RegisterCallback = PlotController.RegisterCallback
ClearCallbacks = PlotController.ClearCallbacks
GetSalomeViewID = PlotController.GetSalomeViewID
SetLegendVisible = PlotController.SetLegendVisible
SetPlotSetTitle = PlotController.SetPlotSetTitle

# For advanced usage only:
GetInstance = PlotController.GetInstance
LockRepaint = PlotController.LockRepaint
UnlockRepaint = PlotController.UnlockRepaint

