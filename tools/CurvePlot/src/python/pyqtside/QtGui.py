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

from . import _use_pyqt
if _use_pyqt:
  from PyQt5.QtGui import *

  # Make QVariant invisible in PyQt5 since they don't exist in
  # PySide ...
#  __original_itemData = QComboBox.itemData
#  def new_itemData(*args, **kargs):
#    from PyQt5.QtCore import QVariant
#    variant = __original_itemData(*args, **kargs)
#    funcS = lambda : (str(variant.toString()), True)
#    dico = {QVariant.Int: variant.toInt, QVariant.String: funcS,
#     QVariant.Bool: variant.toBool, QVariant.Double: variant.toDouble}
#    conv = dico.get(variant.type(), None)
#    if conv is None:
#      raise Exception("Unsupported variant type in pyqtside: '%s'!" % variant.typeName())
#    return conv()[0]
#
#  QComboBox.itemData = new_itemData 
else:
  from PySide.QtGui import *

  __original_ofn = QFileDialog.getOpenFileName
  __original_sfn = QFileDialog.getSaveFileName

  # In PySide, getOpenFileName and co returns 2 values, and only one in PyQt ...
  def newOfn(cls,*args, **kargs):
    tup = __original_ofn(*args, **kargs)
    return tup[0]

  def newSfn(cls,*args, **kargs):
    tup = __original_sfn(*args, **kargs)
    return tup[0]

  QFileDialog.getOpenFileName = classmethod(newOfn)
  QFileDialog.getSaveFileName = classmethod(newSfn)

