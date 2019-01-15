# -*- coding: utf-8 -*-
# Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
# Author : A. Bruneton
#
import unittest, sys, os, filecmp, shutil, tempfile
from pyqtside.QtWidgets import QApplication
from pyqtside.QtGui import QPixmap, QPainter
from pyqtside.QtCore import QTimer

from curveplot.PlotController import PlotController
from curveplot.XYView import XYView
from PyQt5.Qt import QMainWindow

def runOnly(func):
  func.__runOnly__ = True
  return func

def processDecorator(mod_name):
  """ Little trick to be able to mark a test with the decorator
    @runOnly
      If one or more tests bear this decorator, only them will be run
  """
  import inspect
  someRunOnly = False
  for name, obj in inspect.getmembers(sys.modules[mod_name]):
    if name == "PlotTest" and inspect.isclass(obj):
      for p in dir(obj):
        if p.startswith("test") and hasattr(obj.__dict__[p], "__runOnly__"):
          someRunOnly = True
          break
  if someRunOnly:
    for name, obj in inspect.getmembers(sys.modules[mod_name]):
      if name == "PlotTest" and inspect.isclass(obj):
        for p in dir(obj):
          # Note the "not":
          if p.startswith("test") and not hasattr(obj.__dict__[p], "__runOnly__"):
            delattr(obj, p)

class PlotTestBase(unittest.TestCase):
  """ Unit test suite for the curve plotter. This class deals with the set up and the screenshot generation/
  comparison. The tests themselves are stored in the derived class PlotTest below.
   
  The class variable below can be turned on to regenerate base line files (reference images).
  All baselines start with the name of the corresponding test, plus a possible suffix.
  The baselines directory is set relative to the path of this script.
  """
  REBUILD_BASELINES = False

  __BASE_LINE_DIR = "baselines"
  __FORMAT = "png"

  def __init__(self, methodName):
    unittest.TestCase.__init__(self, methodName)

    if self.REBUILD_BASELINES:
      self.tmpBaselineDir = os.path.join(tempfile.gettempdir(), "curveplot_baselines")
      if not os.path.isdir(self.tmpBaselineDir):
        os.mkdir(self.tmpBaselineDir)
      print("### Rebuilding base lines. Reference files will be saved to '%s'" % self.tmpBaselineDir)

    PlotController.WITH_CURVE_BROWSER = True
    XYView._DEFAULT_LEGEND_STATE = True   # always show legend by default
    self._this_dir = os.path.dirname(os.path.realpath(__file__))

  def setUp(self):
    import sys
    from curveplot.SalomePyQt_MockUp import SalomePyQt
    from curveplot.TableModel import TableModel
    from curveplot.CurveModel import CurveModel
    from curveplot.XYPlotSetModel import XYPlotSetModel
    from curveplot.TestDesktop import TestDesktop

    self.qpixmap = None
    self.keepDir = False
    if not self.REBUILD_BASELINES:
      self.tmpDir = tempfile.mkdtemp(prefix="curveplot_tests")
    else:
      self.tmpDir = None

    # Minimal UI setup:
    self.app = QApplication(sys.argv)
    desktop = TestDesktop(None)
    self.sgPyQt = SalomePyQt(desktop)
    desktop._sgPyQt = self.sgPyQt
    desktop.initialize()
    self.plotController = PlotController.GetInstance(self.sgPyQt)
    desktop.resize(800, 600)
    desktop.show()
    self._execQtWasCalled = False # Used to automatically finish Qt execution loop on tests not doing a screenshot

    # Reset some class var to make sure IDs appearing in screenshots do not depend on test seq order:
    CurveModel.START_ID = -1
    TableModel.START_ID = -1
    XYPlotSetModel.START_ID = -1

  def tearDown(self):
    if not self.REBUILD_BASELINES:
      # Clean up temp dir where the file comparison has been made:
      if not self.keepDir:
        shutil.rmtree(self.tmpDir, False)
    if not self._execQtWasCalled:
      self._execQt(withShot=False)
    PlotController.Destroy()

  def getTestName(self):
    return self.id().split(".")[-1]

  def saveCurrentPix(self, direct, suffix):
    fileName = os.path.join(direct, self.getTestName() + suffix + "." + self.__FORMAT)
    self.qpixmap.save(fileName, self.__FORMAT)
    return fileName

  def _execQt(self, withShot=False):
    if withShot:
      QTimer.singleShot(50, self._shotEvent)  # take picture
    QTimer.singleShot(200, self.app.quit)    # quit
    self.app.exec_()  # will hang till quit is fired

    # Important make sure previous app is properly killed before launching next test!
    # Qt doesn't like multiple running instance
    import gc
    gc.collect()

  def areScreenshotEqual(self, widget):
    """ Finish the launching of the Qt application so that the widgets and the curve have a chance to display
     and trigger snapshot comparison """
    self.saveW = widget
    self._execQtWasCalled = True
    self._execQt(withShot=True)
    return self.retValue

  def _shotEvent(self):
    self.retValue = self._snapAndCompare(self.saveW)

  def _snapAndCompare(self, widget, suffix=""):
    """ Test equality between a reference file saved in the baseline directory, and whose name is built as
          "<test_name><suffix>.png"
        and the file generated on the fly by taking a snapshot of the widget provided in argument.
        The comparison is made in a temp dir which is kept if the file differ.
    """
    import glob
    # Smiiiile :-)
    self.qpixmap = QPixmap(widget.size())
    widget.repaint()
    widget.render(self.qpixmap)
    #self.qpixmap = widget.grab()

    # Nothing to compare if rebuilding base lines, just saving file:
    if self.REBUILD_BASELINES:
      self.saveCurrentPix(self.tmpBaselineDir, suffix)
      return True

    gen_path = self.saveCurrentPix(self.tmpDir, suffix)
    base_ref = os.path.join(self._this_dir, self.__BASE_LINE_DIR, self.getTestName() + suffix)
    ret = False
    for ref_path in glob.glob("%s_*.%s" % (base_ref, self.__FORMAT)):
      try:
        ret = filecmp.cmp(ref_path, gen_path, shallow=False)
        if ret:
          break
      except OSError:
        ret = False
    if not ret:
      # Keep file if assert is false
      self.keepDir = True
      print("[%s] -- Failed screenshot equality, or unable to open baseline file - directory is kept alive: %s" % (self.getTestName(), self.tmpDir))
    return ret

  def showTabWidget(self):
    tabW = self.plotController._sgPyQt._tabWidget
    # No simpler way found so far:
    tabW.show()
    return tabW

  def getBrowserWidget(self):
    return self.plotController._curveBrowserView._treeWidget
