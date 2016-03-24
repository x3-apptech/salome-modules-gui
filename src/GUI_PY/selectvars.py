# Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

import os

from qtsalome import *

import salome
from salome.kernel.studyedit import getStudyEditor
from salome.kernel.parametric import study_exchange_vars

# ---------------------------------- #
# Dialog box for variables selection #
# ---------------------------------- #

from SelectVarsDialog_ui import Ui_SelectVarsDialog

class MySelectVarsDialog(Ui_SelectVarsDialog, QDialog):

    def __init__(self, parent = None, modal = 0):
        QDialog.__init__(self, parent)
        Ui_SelectVarsDialog.__init__(self)
        self.setupUi(self)
        self.cancelButton.clicked.connect(self.close)
        self.OKButton.clicked.connect(self.accept)
        self.selectButton.clicked.connect(self.initPotentialVariablesFromSelection)
        self.addInputVarButton.clicked.connect(self.addSelectedInputVar)
        self.removeInputVarButton.clicked.connect(self.removeSelectedInputVar)
        self.newInputVarButton.clicked.connect(self.newInputVar)
        self.addOutputVarButton.clicked.connect(self.addSelectedOutputVar)
        self.removeOutputVarButton.clicked.connect(self.removeSelectedOutputVar)
        self.newOutputVarButton.clicked.connect( self.newOutputVar)
        self.loadVarsButton.clicked.connect(self.loadVars)
        self.saveVarsButtonself.clicked.connect(self.saveVars)
        self.refEntry = None

    def setExchangeVariables(self, exchangeVariables):
        if exchangeVariables.refEntry is not None:
            self._initPotentialVariables(exchangeVariables.refEntry)
        self.selectedInputVarListWidget.addItems([x.name for x in exchangeVariables.inputVarList])
        self.selectedOutputVarListWidget.addItems([x.name for x in exchangeVariables.outputVarList])

    def initPotentialVariablesFromSelection(self):
        entries = salome.sg.getAllSelected()
        if len(entries) != 1 :
            QMessageBox.warning(self, self.tr("Error"),
                                self.tr("One item must be selected in the object browser"))
            return
        selectedEntry = entries[0]
        self._initPotentialVariables(selectedEntry)

    def _initPotentialVariables(self, entry):
        sobj = getStudyEditor().study.FindObjectID(entry)
        if sobj is None:
            QMessageBox.warning(self, self.tr("Error"),
                                self.tr('No item at entry %s' % entry))
            return
        exchangeVariables = study_exchange_vars.getExchangeVariablesFromSObject(sobj)
        if exchangeVariables is None:
            QMessageBox.warning(self, self.tr("Error"),
                                self.tr('Item at entry %s is not a valid '
                                              '"Variable List" object' % entry))
            return
        self.refEntry = entry
        self.varListObjLineEdit.setText(sobj.GetName())
        self.allInputVarListWidget.clear()
        self.allOutputVarListWidget.clear()
        self.allInputVarListWidget.addItems([x.name for x in exchangeVariables.inputVarList])
        self.allOutputVarListWidget.addItems([x.name for x in exchangeVariables.outputVarList])

    def addSelectedInputVar(self):
        for item in self.allInputVarListWidget.selectedItems():
            self.selectedInputVarListWidget.addItem(QListWidgetItem(item))

    def removeSelectedInputVar(self):
        for item in self.selectedInputVarListWidget.selectedItems():
            self.selectedInputVarListWidget.takeItem(self.selectedInputVarListWidget.row(item))

    def newInputVar(self):
        newItem = QListWidgetItem("TO EDIT!")
        newItem.setFlags(Qt.ItemIsSelectable|Qt.ItemIsEditable|Qt.ItemIsUserCheckable|Qt.ItemIsEnabled)
        self.selectedInputVarListWidget.addItem(newItem);

    def addSelectedOutputVar(self):
        for item in self.allOutputVarListWidget.selectedItems():
            self.selectedOutputVarListWidget.addItem(QListWidgetItem(item))

    def removeSelectedOutputVar(self):
        for item in self.selectedOutputVarListWidget.selectedItems():
            self.selectedOutputVarListWidget.takeItem(self.selectedOutputVarListWidget.row(item))

    def newOutputVar(self):
        newItem = QListWidgetItem("TO EDIT!")
        newItem.setFlags(Qt.ItemIsSelectable|Qt.ItemIsEditable|Qt.ItemIsUserCheckable|Qt.ItemIsEnabled)
        self.selectedOutputVarListWidget.addItem(newItem);

    def getSelectedExchangeVariables(self):
        inputVarList = []
        outputVarList = []
        for row in range(self.selectedInputVarListWidget.count()):
            name = str(self.selectedInputVarListWidget.item(row).text())
            inputVarList.append(study_exchange_vars.Variable(name))
        for row in range(self.selectedOutputVarListWidget.count()):
            name = str(self.selectedOutputVarListWidget.item(row).text())
            outputVarList.append(study_exchange_vars.Variable(name))
        return study_exchange_vars.ExchangeVariables(inputVarList, outputVarList, self.refEntry)

    def loadVars(self):
        filename = QFileDialog.getOpenFileName(self, self.tr("Import variables from file"),
                                                     os.getenv("HOME"),
                                                     self.tr("XML Files (*.xml)"))
        if not filename:
            return
        try:
            filename = str(filename)
            exchange_variables = study_exchange_vars.loadExchangeVariablesFromXmlFile(filename)
            self.setExchangeVariables(exchange_variables)
        except Exception, e:
            QMessageBox.critical(self, self.tr("Error"),
                                 self.tr("Cannot load file %s:\n%s" % (filename, e)))

    def saveVars(self):
        default = os.path.join(os.getenv("HOME"), "vars.xml")
        filename = QFileDialog.getSaveFileName(self, self.tr("Export variables to file"),
                                                     default, self.tr("XML Files (*.xml)"))
        if not filename:
            return
        try:
            filename = str(filename)
            exchange_variables = self.getSelectedExchangeVariables()
            exchange_variables.saveToXmlFile(filename)
        except Exception, e:
            QMessageBox.critical(self, self.tr("Error"),
                                 self.tr("Cannot save file %s:\n%s" % (filename, e)))
