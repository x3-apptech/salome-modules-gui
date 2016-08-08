"""
Group under one hat PySide and PyQt4. PyQt4 is tried first.
"""

try:
  import os
  if os.getenv("CURVEPLOT_FORCE_PYSIDE") is not None:
    raise Exception
  import PyQt4
  _use_pyqt = True
  print "Using PyQt4 run-time ..."
except:
  try:
    import PySide
    _use_pyqt = False
    print "Using PySide run-time ..."
  except:
    raise Exception("Neither PyQt4 nor PySide could be imported!")

# Matplotlib has to be handled very early, otherwise it will switch to whatever it
# finds first on the machine
try: 
  import matplotlib
  if _use_pyqt:  back = 'PyQt4'
  else:          back = 'PySide'
  matplotlib.rcParams['backend.qt4'] = back
  print "Matplotlib found - Set matplotlib backend to '%s'!" % back 
except:
  # No matplotlib, silently discard err message.
  pass
