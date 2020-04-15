# Copyright (C) 2016-2020  CEA/DEN, EDF R&D
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

from pyqtside import QtWidgets
from pyqtside.QtWidgets import QMenu
from pyqtside.QtCore import Qt

from .View import View
from .CurveTreeDockWidget import CurveTreeDockWidget
from .utils import Logger

class CurveBrowserView( View, CurveTreeDockWidget) :

    def __init__( self, controller) :
        """ Constructor """
        View.__init__( self, controller)
        CurveTreeDockWidget.__init__(self)
        self._noUpdate = False
        
        treeWidget = self.getTreeWidget()
        treeWidget.itemSelectionChanged.connect(self.onItemSelectionChanged)
        treeWidget.itemDoubleClicked.connect(self.onItemDoubleCliked)
        treeWidget.setContextMenuPolicy(Qt.CustomContextMenu)
        treeWidget.customContextMenuRequested.connect(self.openMenu)
            
    def update(self):
        """ Update this view due to model change """
        if self._model == None or self._noUpdate:
            return
          
        # widget clear and repopulation will trigger onItemSelectionChanged(),
        # which in turns triggers update() (via setCurrentCurve()). So avoid recursion:
        self._noUpdate = True
        
        treeWidget = self.getTreeWidget()
        treeWidget.clear()
        
        plotSets = self._model._plotSets
            
        # The second (hidden) column in the tree bares the ID of the object and its nature (plotset or curve)
        for p in list(plotSets.values()):
          item = QtWidgets.QTreeWidgetItem([str(p.getTitle()), str(p.getID()) + '_set'])
          treeWidget.addTopLevelItem(item)
          for c in list(p._curves.values()):
            chld = QtWidgets.QTreeWidgetItem([str(c.getTitle()), str(c.getID()) + '_crv'])
            item.addChild(chld)
          
        treeWidget.expandAll()
        
        # Finally select the proper item in the tree:
        cps = self._model.getCurrentPlotSet()
        if not cps is None:
          ccv = cps.getCurrentCurve()
          if ccv is None:
            key = str(cps.getID()) + '_set'
          else:
            key = str(ccv.getID()) + '_crv'
          listItems = treeWidget.findItems(key, Qt.MatchExactly | Qt.MatchRecursive,1)
          if len(listItems) > 0:
            treeWidget.setCurrentItem(listItems[0])
            
        self._noUpdate = False
    
    def onItemSelectionChanged(self):
        """
        Change the current selected XYplot/curve        
        """
        if self._noUpdate:
          return
        
        # setCurrentCurve() and setCurrentPlotSet() will trigger update(),
        # which in turns triggers onItemSelectionChanged(). So avoid recursion:
        self._noUpdate = True
        
        pm = self._controller._plotManager
        treeWidget = self.getTreeWidget()
        it = treeWidget.currentItem()
        if not it is None:
          objStr = str(it.text(1))   # no unicode here!
          objID = int(objStr[0:-4])
          objTyp = objStr[-3:]
          if objTyp == 'crv':
            # Find correct plot set:
            cps = pm.getPlotSetContainingCurve(objID)
            if not cps is None:
              cps.setCurrentCurve(objID)
              pm.setCurrentPlotSet(cps.getID())
          elif objTyp == 'set':
            pm.clearAllCurrentCurve()
            pm.setCurrentPlotSet(objID)
          else:
            raise Exception("Internal error - should not happen")
        else:
          ps = pm.getCurrentPlotSet()
          if not ps is None:
            ps.setCurrentCurve(-1)
          pm.setCurrentPlotSet(-1)
          
        self._noUpdate = False
    
    def onItemDoubleCliked(self):
        Logger.Debug("item doubled clicked")
    
    def openMenu(self, position):
        menu = self._controller._browserContextualMenu
        treeWidget = self.getTreeWidget()
        menu.exec_(treeWidget.mapToGlobal(position))
