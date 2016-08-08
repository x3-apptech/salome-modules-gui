# -*- coding: utf-8 -*-
#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : A. Bruneton
#

from pyqtside.QtCore import SIGNAL, SLOT, Slot, Qt, QTimer
from pyqtside.QtGui import QMainWindow,QMenu
import numpy as np

import curveplot

try:
  import curveplot
  from curveplot.utils import Logger
except:
  from PlotController import PlotController as curveplot
  from utils import Logger 

class TestDesktop(QMainWindow):
    """ Dummy desktop for testing purposes.
    """    
    def __init__(self, sgPyQt):
        QMainWindow.__init__(self)
        self.cnt = -1
        self.MAX_CNT = 20
        self.timeLap = 100
        self._sgPyQt = sgPyQt
        
    def initialize(self):
        """ Initialize is called later than __init__() so that the Desktop and the SgPyQt
        objects can be properly initialized.
        """
        self._currID = 1234
        
        self._sgDesktop = self
        try:
          self._plotController = curveplot.PlotController.GetInstance(self._sgPyQt)
        except:
          self._plotController = curveplot.GetInstance(self._sgPyQt)
        self.createIDs()
        self.createActions()
        
        context_actions = [self.itemDelAction]
        menu = QMenu(self)
        for a in context_actions:
          menu.addAction(a)
        self._plotController.setBrowserContextualMenu(menu)
        
        self.createToolbars()
        self.createMenus()
        self.createView()
        
        self.connect(self.curveSameFigAction,SIGNAL("activated()"),self.curveSameFig)
        self.connect(self.curveNewFigAction,SIGNAL("activated()"),self.curveNewFig)
        self.connect(self.itemDelAction,SIGNAL("activated()"),self.itemDel)
        self.connect(self.cpsAction,SIGNAL("activated()"),self.clearPlotSet)
        self.connect(self.plotTableAction,SIGNAL("activated()"),self.plotTable)
        self.connect(self.addPSAction,SIGNAL("activated()"),self.addPS)
        self.connect(self.addTabAction,SIGNAL("activated()"),self.addTab)
        self.connect(self.memAction,SIGNAL("activated()"),self.memPrint)
        self.connect(self.perfTestAction,SIGNAL("activated()"),self.perfTest)

    def generateID(self):
        self._currID += 1
        return self._currID
    
    def createIDs(self):
        # Actions
        self.curveSameFigActionID = self.generateID()
        self.curveNewFigActionID = self.generateID()
        self.itemDelActionID = self.generateID()
        self.cpsActionID = self.generateID()
        self.plotTableActionID = self.generateID()
        self.addPSActionID = self.generateID()
        self.addTabActionID = self.generateID()
        self.memActionID = self.generateID()
        self.perfActionID = self.generateID()
        
        # Menus
        self.etudeMenuID = self.generateID()
        self.dummyMenuID = self.generateID()

    def createActions(self):
        ca = self._sgPyQt.createAction
        self.curveSameFigAction = ca(self.curveSameFigActionID, "Curve on same fig", "Curve on same fig", "", "")
        self.curveNewFigAction = ca(self.curveNewFigActionID, "Curve on new fig", "Curve on new fig", "", "")
        self.itemDelAction = ca(self.itemDelActionID, "Delete selected", "Delete selected", "", "")
        self.cpsAction = ca(self.cpsActionID, "Clear plot set", "Clear plot set", "", "")
        self.plotTableAction = ca(self.plotTableActionID, "Plot from table", "Plot from table", "", "")
        self.addPSAction = ca(self.addPSActionID, "Add plot set", "Add plot set", "", "")
        self.addTabAction = ca(self.addTabActionID, "Add tab", "Add tab", "", "")
        self.memAction = ca(self.memActionID, "Display used mem", "Display used mem", "", "")
        self.perfTestAction = ca(self.perfActionID, "Perf test", "Perf test", "", "")

    def createToolbars(self):
        pass
#         self.Toolbar = self._sgPyQt.createTool(self.tr("Toolbar"))
#         self._sgPyQt.createTool(self.fileNewAction, self.Toolbar)
#         self._sgPyQt.createTool(self.filePrintAction, self.Toolbar)
#         sep = self._sgPyQt.createSeparator()
#         self._sgPyQt.createTool(sep, self.Toolbar)
#         self._sgPyQt.createTool(self.editUndoAction, self.Toolbar)
#         self._sgPyQt.createTool(self.editRedoAction, self.Toolbar)

    def createMenus(self):
        curveMenu = self._sgPyQt.createMenu( "Curve test", -1, self.etudeMenuID, self._sgPyQt.defaultMenuGroup() )
        self._sgPyQt.createMenu(self.curveSameFigAction, curveMenu)
        self._sgPyQt.createMenu(self.curveNewFigAction, curveMenu)
        self._sgPyQt.createMenu(self.itemDelAction, curveMenu)
        self._sgPyQt.createMenu(self.cpsAction, curveMenu)
        self._sgPyQt.createMenu(self.plotTableAction, curveMenu)
        self._sgPyQt.createMenu(self.addPSAction, curveMenu)
        self._sgPyQt.createMenu(self.memAction, curveMenu)
        self._sgPyQt.createMenu(self.perfTestAction, curveMenu)
        
        dummyMenu = self._sgPyQt.createMenu( "Dummy", -1, self.dummyMenuID, self._sgPyQt.defaultMenuGroup() )
        self._sgPyQt.createMenu(self.addTabAction, dummyMenu)

    def createView(self):
        pass
          
    def showCurveTreeView(self) :
        self._dockCurveBrowserView = self._plotController._curveBrowserView
        self._sgDesktop.addDockWidget(Qt.LeftDockWidgetArea, self._dockCurveBrowserView)

    def __generateRandomData(self, nPoints=100):
      from random import random
      x = np.arange(nPoints) / 5.0
      ampl = 20.0*random() + 1.0
      y = ampl * np.sin(x*random())
#       x = np.arange(5e5)
#       y = x
      return x, y
       
    @Slot()  
    def curveSameFig(self):
      x, y = self.__generateRandomData()
      _, ps_id = curveplot.AddCurve(x, y, x_label="the x axis", y_label="the y axis", append=True)
      curveplot.SetLegendVisible(ps_id, True)
      if self.cnt >= 0:
        QTimer.singleShot(self.timeLap, self, SLOT("itemDel()"))
            
    def curveNewFig(self):
      x, y = self.__generateRandomData()
      curveplot.AddCurve(x, y, x_label="the x axis", y_label="the y axis", append=False)
    
    @Slot()
    def itemDel(self):
      curveplot.DeleteCurrentItem()
      if self.cnt >= 0:
        QTimer.singleShot(self.timeLap, self, SLOT("memPrint()"))

    @Slot()
    def perfTest(self):
      lx, ly = [], []
      nC = 200
      for _ in range(nC):
        x, y = self.__generateRandomData(1000)
        lx.append(x); ly.append(y)
      print "Done generating"
      from time import time
      t0 = time()
      curveplot.LockRepaint()
      for i in range(nC): 
        curveplot.AddCurve(lx[i], ly[i], append=True)
      curveplot.UnlockRepaint()
      print "Elapsed: %.2f" % ( time() - t0)

    def clearPlotSet(self):
      curveplot.ClearPlotSet()
      
    def addPS(self):
      # Also a test for unicode!
      curveplot.AddPlotSet(u'ça m embête')
      
    def addTab(self):
      pass
#      from PyQt4.QtGui import QPushButton
#      self.qp = QPushButton("Hi!")
#      self._sgPyQt.createView("Dummy", self.qp)  
      
    def plotTable(self):
      from curveplot import TableModel
      t = TableModel(None)
      t.setTitle("coucou")
      t.addColumn([1.0,2.0,3.0,4.0])
      t.addColumn([1.0,2.0,3.0,4.0])
      t.addColumn([1.0,4.0,9.0,16.0])
      t.setColumnTitle(0, "X-s")
      t.setColumnTitle(1, "Identity")
      t.setColumnTitle(2, "Square")
      cont = curveplot.PlotController.GetInstance()
      cont.plotCurveFromTable(t, y_col_index=1, append=False)
      cont.plotCurveFromTable(t, y_col_index=2, append=True)
    
    @Slot()
    def memPrint(self):
      i, t = curveplot.GetAllPlotSets()
      print zip(i, t)
      new_id = curveplot.CopyCurve(curve_id=0, plot_set_id=1)
      print "created  curve: %d" % new_id
      import resource
      m = resource.getrusage(resource.RUSAGE_SELF)[2]*resource.getpagesize()/1e6
      print "** Used memory: %.2f Mb" % m
      if self.cnt >= 0 and self.cnt < self.MAX_CNT:
        self.cnt += 1
        QTimer.singleShot(self.timeLap, self, SLOT("curveSameFig()"))
