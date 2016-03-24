// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SVTK_RECORDERDLG_H
#define SVTK_RECORDERDLG_H

#include <QDialog>

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLCDNumber;
class QLineEdit;
class QPushButton;
class QTimer;

class QtxDoubleSpinBox;
class QtxIntSpinBox;

class SVTK_Recorder;

class SVTK_RecorderDlg : public QDialog
{
  Q_OBJECT

public:
  SVTK_RecorderDlg( QWidget*, SVTK_Recorder* );
  ~SVTK_RecorderDlg();

  int              exec();

  QString          fileName() const { return myFileName; }

private:
  virtual void     keyPressEvent( QKeyEvent* );

protected slots:
  void             onStart();
  void             onClose();
  void             onHelp();

  bool             onBrowseFile();

private:
  SVTK_Recorder*   myRecorder;
  QString          myFileName;

  QLineEdit*       myFileNameLineEdit;

  QComboBox*        myRecordingModeComboBox;
  QtxDoubleSpinBox* myFPSSpinBox;
  QtxIntSpinBox*    myQualitySpinBox;
  QCheckBox*        myProgressiveCheckBox;
};

#endif
