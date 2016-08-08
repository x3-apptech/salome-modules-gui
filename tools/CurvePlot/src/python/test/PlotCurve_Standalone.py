# -*- coding: latin-1 -*-
#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : A. Bruneton
#
from pyqtside.QtGui import QApplication 
from pyqtside.QtCore import SIGNAL, SLOT, QTimer, QTranslator

from TestDesktop import TestDesktop
import SalomePyQt_MockUp

desktop = None

def activate():
    """This method mimicks SALOME's module activation """
    global desktop

    desktop.showCurveTreeView()
    return True

def activeViewChanged( viewID ):
    from curveplot import PlotController
    PlotController.ActiveViewChanged(viewID)

def main(args) :
    global desktop
      
    app = QApplication(args)
    ts_files = ["/export/home/adrien/Projets/salome/modules/V7_main/CURVEPLOT_INSTALL/share/salome/resources/curveplot/CURVEPLOT_msg_fr.qm",
                "/export/home/adrien/Projets/salome/modules/V7_main/CURVEPLOT_INSTALL/share/salome/resources/curveplot/CURVEPLOT_msg_en.qm"
                ]
    trans = QTranslator()
    for f in ts_files:
      if not trans.load(f):
        print "could not load translation %s!" % f
    app.installTranslator(trans)
    dw = app.desktop()
    x, y = dw.width()*0.25, dw.height()*0.7
    
    desktop = TestDesktop(None)
    sgPyQt = SalomePyQt_MockUp.SalomePyQt(desktop)
    sgPyQt.currentTabChanged.connect(activeViewChanged)
    desktop._sgPyQt = sgPyQt 
    desktop.initialize()
    desktop.resize(x,y)
    desktop.show()
    activate()
    #
    QTimer.singleShot(200, desktop, SLOT("curveSameFig()"))
    #
    app.connect(app,SIGNAL("lastWindowClosed()"),app,SLOT("quit()"))
    app.exec_()

if __name__ == "__main__" :
    import sys
    main(sys.argv)
