from . import _use_pyqt
if _use_pyqt:
  from PyQt4.QtCore import *
  Slot = pyqtSlot  
  Signal = pyqtSignal 
else:
  from PySide.QtCore import *
