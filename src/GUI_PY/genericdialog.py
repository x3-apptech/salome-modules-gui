# -*- coding: iso-8859-1 -*-
# Copyright (C) 2010-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

__author__="gboulant"
__date__ ="$31 mars 2010 17:09:53$"

from qtsalome import QDialog, QMessageBox

from genericdialog_ui import Ui_GenericDialog

class GenericDialog(QDialog):
    """
    This is an abstract generic dialog box for implementing default and
    generic behaviour of dialog windows.
    Note that this class can be instantiated but can't be used because the
    OK behaviour is a default one indicating that the checkData function should
    be implemented in a derived class. The general interface that should be
    implemented in derived class is the MVC pattern:

    - setData(<a_data_model_object>):
      to initiate the values of the dialog windows from a given data model.

    - boolean checkData():
      to verify that the data are valid and notify user if not.

    - <a_data_model_object> getData():
      to get the valid data model from values read in the dialog window.
    """

    __wasOk = False
    checkDataMessage = ""

    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent)
        # Set up the user interface from Designer.
        self.__ui = Ui_GenericDialog()
        self.__ui.setupUi(self)

    def getPanel(self):
        '''
        This returns the central panel where to draw the custom dialog
        widgets.
        '''
        return self.__ui.myCenterPanel

    def getButtonBox(self):
        return self.__ui.buttonBox

    def accept(self):
        """
        Slot function connected to the button OK
        """
        if not self.checkData():
            QMessageBox.warning( self, "Alerte", self.checkDataMessage)
            return
        self.__wasOk = True
        self.hide()

    def displayAndWait(self):
        """
        This function can be used to display the dialog in the case
        where the dialog is modal and does not need interactivity with
        parent windows and other part of the application. When called,
        the dialog is raised visible and keep waiting until the button
        OK or the button CANCEL is pressed. Then the flow can go on
        (see an example of implementation in the tests functions at
        the end of this file).
        In the general case, in particular if you need interaction
        with the graphical framework (clic on widgets embedded in
        other dialogs), you should used instead the show() command
        (for a non modal dialog) or the open() command (for a window
        modal dialog, i.e. a dialog that can not interact with its
        direct parent but can interact with the other parts).
        """
        self.__wasOk = False
        self.exec_()

    def wasOk(self):
        """
        Return True if the button OK was pressed to close the dialog windows.
        """
        return self.__wasOk

    def checkData(self):
        """
        This function should be implemented in a derived class. It should return
        True in the case where the data are estimated to be valid data.
        """
        self.checkDataMessage = "The checkData() function is not implemented yet"
        return True

#
# ==============================================================================
# Basic use cases and unit test functions
# ==============================================================================
#
def TEST_GenericDialog():
    import sys
    from qtsalome import QApplication
    app = QApplication(sys.argv)
    app.lastWindowClosed.connect(app.quit)

    dlg=GenericDialog()
    dlg.displayAndWait()
    if dlg.wasOk():
        print "OK has been pressed"
    else:
        print "Cancel has been pressed"
        
if __name__ == "__main__":
    TEST_GenericDialog()
