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

"""
Group under one hat PySide and PyQt4. PyQt4 is tried first.
"""

try:
  import os
  if os.getenv("CURVEPLOT_FORCE_PYSIDE") is not None:
    raise Exception
  import PyQt4
  _use_pyqt = True
  print("Using PyQt4 run-time ...")
except:
  try:
    import PySide
    _use_pyqt = False
    print("Using PySide run-time ...")
  except:
    raise Exception("Neither PyQt4 nor PySide could be imported!")

# Matplotlib has to be handled very early, otherwise it will switch to whatever it
# finds first on the machine
try: 
  import matplotlib
  if _use_pyqt:  back = 'PyQt4'
  else:          back = 'PySide'
  matplotlib.rcParams['backend.qt4'] = back
  print("Matplotlib found - Set matplotlib backend to '%s'!" % back) 
except:
  # No matplotlib, silently discard err message.
  pass
