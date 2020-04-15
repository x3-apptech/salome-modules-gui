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

from .View import View
from .utils import Logger

class CurveView(View):
  _PICKER_PRECISION = 20  #pts
    
  def __init__(self, controller, parentXYView):
    View.__init__(self, controller)
    self._mplAxes = None
    self._mplLines = None
    self._isHighlighted = False
    self._initialLineWidth = None
    self._parentXYView = parentXYView
    
    self._marker = None
    self._color = None
    self._lineStyle = None
    
  def setMPLAxes(self, axes):
    self._mplAxes = axes
        
  def erase(self):
    self._mplAxes.lines.remove(self._mplLines[0])
    self._mplLines = None
    
  def draw(self):
    m = self._model
    x_idx, y_idx = m.getXAxisIndex(), m.getYAxisIndex()
    d = self._model.getTable().getData()
    self._mplLines = self._mplAxes.plot(d[:, x_idx], d[:, y_idx], label=m._title, 
                                        picker=self._PICKER_PRECISION)
    self._initialLineWidth = self._mplLines[0].get_linewidth()
  
  def onCurveTitleChange(self):
    if self._mplLines is None:
      return
    self._mplLines[0].set_label(self._model._title)
  
  def update(self):
    Logger.Debug("CurveView::udpate")
    if self._mplLines is None:
      return
    lineStyle, marker, color = self.getLineStyle(), self.getMarker(), self.getColor()
    self.erase()
    self.draw()
    # Reset correctly color, marker and highlight state
    self.setLineStyle(lineStyle)
    self.setMarker(marker)
    self.setColor(color)
    self.toggleHighlight(self._isHighlighted, force=True)
    
  def setLineStyle(self, lin_style):
    lin = self._mplLines[0] 
    lin.set_linestyle(lin_style)
    
  def getLineStyle(self):
    if self._mplLines is None:
      return None
    return self._mplLines[0].get_linestyle()
    
  def setMarker(self, marker):
    lin = self._mplLines[0]
    lin.set_marker(marker)

  def getMarker(self):
    if self._mplLines is None:
      return None
    return self._mplLines[0].get_marker()

  def toggleHighlight(self, highlight, force=False):
    lin = self._mplLines[0]
    if highlight and (force or not self._isHighlighted):
      lin.set_linewidth(2*self._initialLineWidth)
      self._isHighlighted = True
    elif not highlight and (force or self._isHighlighted):
      lin.set_linewidth(self._initialLineWidth)
      self._isHighlighted = False
    else:
      # Nothing to do, already the correct state
      return
    
  def isHighlighted(self):
    return self._isHighlighted
    
  def setColor(self, rgb_color):
    lin = self._mplLines[0]
    lin.set_color(rgb_color)
    
  def getColor(self):
    if self._mplLines is None:
      return None
    return self._mplLines[0].get_color()
