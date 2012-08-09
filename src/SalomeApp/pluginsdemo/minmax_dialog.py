# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'minmax.ui'
#
# Created: Thu Mar  1 15:23:57 2012
#      by: PyQt4 UI code generator 4.8.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_Dialog(object):
    def setupUi(self, Dialog):
        Dialog.setObjectName(_fromUtf8("Dialog"))
        Dialog.resize(178, 156)
        Dialog.setSizeGripEnabled(True)
        self.gridLayout = QtGui.QGridLayout(Dialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.label_2 = QtGui.QLabel(Dialog)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 0, 0, 1, 1)
        self.mesh = QtGui.QLineEdit(Dialog)
        self.mesh.setReadOnly(True)
        self.mesh.setObjectName(_fromUtf8("mesh"))
        self.gridLayout.addWidget(self.mesh, 0, 1, 1, 2)
        self.label_3 = QtGui.QLabel(Dialog)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.gridLayout.addWidget(self.label_3, 1, 0, 1, 1)
        self.control = QtGui.QComboBox(Dialog)
        self.control.setObjectName(_fromUtf8("control"))
        self.gridLayout.addWidget(self.control, 1, 1, 1, 2)
        self.label = QtGui.QLabel(Dialog)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 2, 0, 1, 1)
        self.minvalue = QtGui.QLineEdit(Dialog)
        self.minvalue.setReadOnly(True)
        self.minvalue.setObjectName(_fromUtf8("minvalue"))
        self.gridLayout.addWidget(self.minvalue, 2, 1, 1, 2)
        self.label_4 = QtGui.QLabel(Dialog)
        self.label_4.setObjectName(_fromUtf8("label_4"))
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.maxvalue = QtGui.QLineEdit(Dialog)
        self.maxvalue.setReadOnly(True)
        self.maxvalue.setObjectName(_fromUtf8("maxvalue"))
        self.gridLayout.addWidget(self.maxvalue, 3, 1, 1, 2)
        self.buttonBox = QtGui.QDialogButtonBox(Dialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Close|QtGui.QDialogButtonBox.Help)
        self.buttonBox.setObjectName(_fromUtf8("buttonBox"))
        self.gridLayout.addWidget(self.buttonBox, 4, 0, 1, 3)

        self.retranslateUi(Dialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("rejected()")), Dialog.OnCancel)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL(_fromUtf8("helpRequested()")), Dialog.helpMessage)
        QtCore.QObject.connect(self.control, QtCore.SIGNAL(_fromUtf8("activated(QString)")), Dialog.compute_minmax)
        QtCore.QMetaObject.connectSlotsByName(Dialog)
        Dialog.setTabOrder(self.mesh, self.control)
        Dialog.setTabOrder(self.control, self.minvalue)
        Dialog.setTabOrder(self.minvalue, self.maxvalue)
        Dialog.setTabOrder(self.maxvalue, self.buttonBox)

    def retranslateUi(self, Dialog):
        Dialog.setWindowTitle(QtGui.QApplication.translate("Dialog", "Get min and max value of control", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Dialog", "Mesh", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Dialog", "Control", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Dialog", "Min", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Dialog", "Max", None, QtGui.QApplication.UnicodeUTF8))

