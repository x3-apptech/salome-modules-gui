# -*- coding: iso-8859-1 -*-
# Copyright (C) 2010-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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
# Author : Guillaume Boulant (EDF)

import sys
from PyQt4 import QtGui
from PyQt4 import QtCore


class TubeDialog_UI(QtGui.QDialog):
    """
    This class defines the design of a Qt dialog box dedicated to the
    salome plugin examples. It presents a UI form that contains
    parameters for the spatial dimensions of geometrical object.  
    """
    def __init__(self, parent=None):
        QtGui.QDialog.__init__(self, parent)
        self.setupUi()

    def setupUi(self):
        self.setObjectName("Dialog")
        self.resize(400, 300)
        self.hboxlayout = QtGui.QHBoxLayout(self)
        self.hboxlayout.setMargin(9)
        self.hboxlayout.setSpacing(6)
        self.hboxlayout.setObjectName("hboxlayout")
        self.vboxlayout = QtGui.QVBoxLayout()
        self.vboxlayout.setMargin(0)
        self.vboxlayout.setSpacing(6)
        self.vboxlayout.setObjectName("vboxlayout")
        self.hboxlayout1 = QtGui.QHBoxLayout()
        self.hboxlayout1.setMargin(0)
        self.hboxlayout1.setSpacing(6)
        self.hboxlayout1.setObjectName("hboxlayout1")
        self.vboxlayout1 = QtGui.QVBoxLayout()
        self.vboxlayout1.setMargin(0)
        self.vboxlayout1.setSpacing(6)
        self.vboxlayout1.setObjectName("vboxlayout1")
        self.lblRadius = QtGui.QLabel(self)
        self.lblRadius.setObjectName("lblRadius")
        self.vboxlayout1.addWidget(self.lblRadius)
        self.lblLength = QtGui.QLabel(self)
        self.lblLength.setObjectName("lblLength")
        self.vboxlayout1.addWidget(self.lblLength)
        self.lblWidth = QtGui.QLabel(self)
        self.lblWidth.setObjectName("lblWidth")
        self.vboxlayout1.addWidget(self.lblWidth)
        self.hboxlayout1.addLayout(self.vboxlayout1)
        self.vboxlayout2 = QtGui.QVBoxLayout()
        self.vboxlayout2.setMargin(0)
        self.vboxlayout2.setSpacing(6)
        self.vboxlayout2.setObjectName("vboxlayout2")
        self.txtRadius = QtGui.QLineEdit(self)
        self.txtRadius.setObjectName("txtRadius")
        self.vboxlayout2.addWidget(self.txtRadius)
        self.txtLength = QtGui.QLineEdit(self)
        self.txtLength.setObjectName("txtLength")
        self.vboxlayout2.addWidget(self.txtLength)
        self.txtWidth = QtGui.QLineEdit(self)
        self.txtWidth.setObjectName("txtWidth")
        self.vboxlayout2.addWidget(self.txtWidth)
        self.hboxlayout1.addLayout(self.vboxlayout2)
        self.vboxlayout.addLayout(self.hboxlayout1)
        spacerItem = QtGui.QSpacerItem(20, 40, QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Expanding)
        self.vboxlayout.addItem(spacerItem)
        self.buttonBox = QtGui.QDialogButtonBox(self)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.NoButton|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.vboxlayout.addWidget(self.buttonBox)
        self.hboxlayout.addLayout(self.vboxlayout)

        self.setWindowTitle("Tube construction")
        self.lblRadius.setText("Rayon")
        self.lblLength.setText("Longueur")
        self.lblWidth.setText("Epaisseur")

#
# ======================================================================
# Unit test
# ======================================================================
#
def main( args ):
    a = QtGui.QApplication(sys.argv)
    tubedialog = TubeDialog_UI()
    sys.exit(tubedialog.exec_())

if __name__=="__main__":
    main(sys.argv)

