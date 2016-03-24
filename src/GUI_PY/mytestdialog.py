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
from qtsalome import *
from mytestdialog_ui import Ui_MyTestDialog
from genericdialog import GenericDialog

class MyTestDialog(GenericDialog):
    """
    This class is to illustrate the usage of the GenericDialog to implement
    the dialog windows of the application with a common design template provided
    by the generic class GenericDialog.
    """
    def __init__(self, parent=None, name="MyTestDialog"):
        GenericDialog.__init__(self, parent, name)
        # Set up the user interface from Designer.
        self.ui = Ui_MyTestDialog()
        # BE CAREFULL HERE, the ui form is NOT put in the global dialog (already
        # containing some generic widgets) but in the center panel created in the
        # GenericDialog as a void container for the form. The MyTestDialog form
        # is supposed here to create only the widgets to be placed in the center
        # panel
        self.ui.setupUi(self.getPanel())

    #
    # We implement here the interface of the MVC pattern
    #
    def setData(self, name):
        """
        This function implements the mapping from the data model to the widgets
        """
        self.ui.txtName.setText(name)

    def checkData(self):
        """
        This function implements the control to be done on the values contained
        in the widgets when trying to validate the dialog window (click OK first
        trigs this function).
        """
        if ( self.ui.txtName.text().trimmed() == "" ):
            self.checkDataMessage = "The name can't be void"
            return False
        return True

    def getData(self):
        """
        This function implements the mapping from the widgets to the data model
        """
        name = str(self.ui.txtName.text().trimmed().toUtf8())
        # _MEM_: note here that (i) the utf8 format is used and (ii) we must not
        # forget to convert to a standard python string (instead of a QString).
        return name


class MyTestDialogWithSignals(MyTestDialog):
    """
    This class is to illustrate the usage of the GenericDialog in the
    case where the dialog windows is not modal. In such a case, the
    controller must be warn of close events using Qt signals.
    """

    inputValidated = pyqtSignal()

    def __init__(self, parent=None, name="MyTestDialogWithSignals"):
        MyTestDialog.__init__(self, parent, name)

    def accept(self):
        """
        This function is the slot connected to the the OK button
        (click event of the OK button). 
        """
        # The dialog is raised in a non modal mode (for example, to
        # get interactivity with the parents windows. Then we have to
        # emit a signal to warn the parent observer that the dialog
        # has been validated so that it can process the event
        MyTestDialog.accept(self)
        if self.wasOk():
            self.inputValidated.emit()



#
# ==============================================================================
# Basic use case
# ==============================================================================
#

def TEST_MyTestDialog_modal():
    import sys
    from qtsalome import QApplication
    app = QApplication(sys.argv)
    app.lastWindowClosed.connect(app.quit)

    dlg=MyTestDialog()
    dlg.setData("A default name")
    dlg.displayAndWait()
    if dlg.wasOk():
        name = dlg.getData()
        print "The name has been modified to",name


class DialogListener:
    def onProcessEvent(self):
        print "onProcessEvent(): OK has been pressed"
        import sys
        sys.exit(0)
        

def TEST_MyTestDialog_non_modal():
    import sys
    app = QApplication(sys.argv)
    app.lastWindowClosed.connect(app.quit)

    dlg=MyTestDialogWithSignals()
    # This dialog window will emit a inputValidated() signal when the
    # OK button is pressed and the data are validated. Then, we
    # connect this signal to a local slot so that the event can be
    # processed.
    dlgListener = DialogListener()
    dlg.inputValidated.connect(dlgListener.onProcessEvent)
    # This connect instruction means that the signal inputValidated()
    # emited by the dlg Qt object will raise a call to the slot
    # dlgListener.onProcessEvent

    dlg.setData("A default name")
    dlg.show()

    app.exec_()

if __name__ == "__main__":
    #TEST_MyTestDialog_modal()
    TEST_MyTestDialog_non_modal()
