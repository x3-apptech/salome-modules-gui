from . import _use_pyqt
if _use_pyqt:
  from PyQt4.uic import loadUi as loadUiGen
else: 
  from pyside_dynamic import loadUi as loadUiGen


