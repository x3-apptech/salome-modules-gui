# Copyright (C) 2016-2019  CEA/DEN, EDF R&D
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

from pyqtside import QtGui, QtCore
from pyqtside.uic import loadUiGen
from utils import completeResPath

class CurveTreeDockWidget(QtGui.QDockWidget):
  def __init__(self):
    QtGui.QDockWidget.__init__(self)
    loadUiGen(completeResPath("CurveTreeDockWidget.ui"), self)
    self.treeWidget.setHeaderLabel ("Plots")
    self.treeWidget.sortByColumn(0, QtCore.Qt.AscendingOrder)
    self.treeWidget.setSortingEnabled(True);
    self.treeWidget.setColumnHidden(1, True);
            
  def getTreeWidget(self):
      """
      :returns: QTreeWidget -- the (curve) browser
      """
      return self.treeWidget
