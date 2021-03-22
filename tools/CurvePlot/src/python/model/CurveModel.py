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

from .Model import Model
from .utils import toUnicodeWithWarning

class CurveModel(Model):  
  def __init__(self, controller, table=None, index=-1):
    Model.__init__(self, controller)
    self._title = "Curve %d" % self.getID()
    self._table = table
    self._yaxisIndex = index   # column index in the table
    self._xaxisIndex = 0  # By default the first column of the table is used for the X-s
    
  def clone(self):
    ret = CurveModel(self._controller)
    ret._title = self._title
    ret._table = self._table  # TO CHECK: deep copy here? (I think not needed in Python ...)
    ret._yaxisIndex = self._yaxisIndex
    ret._xaxisIndex = self._xaxisIndex
    return ret
    
  def setTable(self, t, silent=False):
    self._table = t
    if not silent:
      self.notifyChange("DataChange") 
    
  def getTable(self):
    return self._table
    
  def extendData(self, t, silent=False):
    self._table.extend(t)
    if not silent:
      self.notifyChange("DataChange")
    
  def resetData(self):
    self._table.clear()
    self.notifyChange("DataChange")
    
  def setTitle(self, ti, silent=False):
    ti = toUnicodeWithWarning(ti, "CurveModel::setTitle()")
    self._title = ti
    if not silent:
      self.notifyChange("CurveTitleChange") 
    
  def getTitle(self):
    return self._title
    
  def getYAxisIndex(self):
    return self._yaxisIndex
  
  def setYAxisIndex(self, idx, silent=False):
    self._yaxisIndex = idx
    if not silent:
      self.notifyChange("YAxisIndexChange")
  
  def getXAxisIndex(self):
    return self._xaxisIndex
  
  def setXAxisIndex(self, idx, silent=False):
    sh = self._table.getShape()
    if idx >= sh[1]:
      raise ValueError("Index out of bound (is %d, but max is %d)" % (idx, sh[1]))
    self._xaxisIndex = idx
    if not silent:
      self.notifyChange("XAxisIndexChange")
    
  def getID(self):
    return self._id
  
    
