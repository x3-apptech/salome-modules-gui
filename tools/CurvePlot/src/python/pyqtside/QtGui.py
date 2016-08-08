from . import _use_pyqt
if _use_pyqt:
  from PyQt4.QtGui import *
  
  # Make QVariant invisible in PyQt4 since they don't exist in
  # PySide ...
  __original_itemData = QComboBox.itemData
  def new_itemData(*args, **kargs):
    from PyQt4.QtCore import QVariant
    variant = __original_itemData(*args, **kargs)
    funcS = lambda : (str(variant.toString()), True)
    dico = {QVariant.Int: variant.toInt, QVariant.String: funcS,
     QVariant.Bool: variant.toBool, QVariant.Double: variant.toDouble}
    conv = dico.get(variant.type(), None)
    if conv is None:
      raise Exception("Unsupported variant type in pyqtside: '%s'!" % variant.typeName())
    return conv()[0]
  
  QComboBox.itemData = new_itemData 
else:
  from PySide.QtGui import *
  
  __original_ofn = QFileDialog.getOpenFileName
  __original_sfn = QFileDialog.getSaveFileName
  
  # In PySide, getOpenFileName and co returns 2 values, and only one in PyQt4 ...
  def newOfn(cls,*args, **kargs):
    tup = __original_ofn(*args, **kargs)
    return tup[0]
    
  def newSfn(cls,*args, **kargs):
    tup = __original_sfn(*args, **kargs)
    return tup[0]
    
  QFileDialog.getOpenFileName = classmethod(newOfn)
  QFileDialog.getSaveFileName = classmethod(newSfn)

