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

from tubedialog_ui import TubeDialog_UI


class TubeDialog(TubeDialog_UI):
    def setupUi(self):
        TubeDialog_UI.setupUi(self)
        self.handleAcceptWith(self.accept)
        self.handleRejectWith(self.reject)

    def handleAcceptWith(self,callbackFunction):
        """This defines the function to be connected to the signal 'accepted()' (click on Ok)"""
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), callbackFunction)

    def handleRejectWith(self,callbackFunction):
        """This defines the function to be connected to the signal 'rejected()' (click on Cancel)"""
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), callbackFunction)

    def handleApplyWith(self,callbackFunction):
        """This defines the function to be connected to the signal 'apply()' (click on Apply)"""
        button = self.buttonBox.button(QtGui.QDialogButtonBox.Apply)        
        QtCore.QObject.connect(button, QtCore.SIGNAL("clicked()"), callbackFunction);

    def accept(self):
        '''Callback function when dialog is accepted (click Ok)'''
        self._wasOk = True
        # We should test here the validity of values
        QtGui.QDialog.accept(self)

    def reject(self):
        '''Callback function when dialog is rejected (click Cancel)'''
        self._wasOk = False
        QtGui.QDialog.reject(self)

    def wasOk(self):
        return self._wasOk

    def setData(self, radius, length, width):
        self.txtRadius.setText(str(radius))
        self.txtLength.setText(str(length))
        self.txtWidth.setText(str(width))

    def getData(self):
        try:
            radius=eval(str(self.txtRadius.text()))
            length=eval(str(self.txtLength.text()))
            width=eval(str(self.txtWidth.text()))
        except:
            print "pb a la saisie"

        return radius, length, width

    
class TubeDialogOnTopWithApply(TubeDialog):
    def setupUi(self):
        """
        This setupUi adds a button 'Apply' to execute a processing
        tasks (ex: preview), and set a flag that keeps the dialog on
        top of all windows.
        """
        TubeDialog.setupUi(self)
        # Add a button "Apply"
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|
                                          QtGui.QDialogButtonBox.Apply|
                                          QtGui.QDialogButtonBox.Ok)

        # Keep the dialog on top of the windows
        self.setWindowFlags(self.windowFlags() |
                            QtCore.Qt.WindowStaysOnTopHint)


#
# ======================================================================
# Unit test
# ======================================================================
#
def TEST_getData_synchrone():
    """This use case illustrates the MVC pattern on this simple dialog example""" 
    tubedialog = TubeDialog()
    tubedialog.setData(10,50,3)
    tubedialog.exec_()
    if tubedialog.wasOk():
        radius, length, width = tubedialog.getData()
        print radius, length, width


def main( args ):
    a = QtGui.QApplication(sys.argv)
    TEST_getData_synchrone()
    sys.exit(0)

if __name__=="__main__":
    main(sys.argv)

