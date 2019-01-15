from . import _use_pyqt
if _use_pyqt:
  from PyQt5.QtWidgets import *
else:
  from PySide.QtWidgets import *
