# Copyright (C) 2016-2021  CEA/DEN, EDF R&D
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
from pyqtside.uic import loadUiGen
from .utils import completeResPath

class PlotWidget(QtWidgets.QMainWindow):
  def __init__(self):
    QtWidgets.QMainWindow.__init__(self)
    loadUiGen(completeResPath("PlotWidget.ui"), self)

  def clearAll(self):
    """ In test context, the PlotWidget is never fully deleted (because the PyQt binding
    of QTabWidget doesn't remove completly the references it holds).
    So clean up manually. 
    """
    self.toolBar = None
    self.setCentralWidget(None)
