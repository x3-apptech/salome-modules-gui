// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef PLOT2D_FITDATADLG_H
#define PLOT2D_FITDATADLG_H

#include "Plot2d.h"
#include <QDialog>

class QGroupBox;
class QRadioButton;
class QLineEdit;
class QPushButton;

class PLOT2D_EXPORT Plot2d_FitDataDlg : public QDialog
{
  Q_OBJECT

public:
// constuctor
  Plot2d_FitDataDlg( QWidget* parent, bool secondAxisY );

// sets range
  void setRange(const double xMin, 
                const double xMax,
                const double yMin,
                const double yMax,
                const double y2Min = 0,
                const double y2Max = 0);
// gets range, returns mode (see getMode())
  int getRange(double& xMin, 
               double& xMax,
               double& yMin,
               double& yMax,
               double& y2Min,
               double& y2Max);
// gets mode : 0 - Fit all; 1 - Fit horizontal, 2 - Fit vertical
  int getMode();

protected slots:
// called when range mode changed
  void onModeChanged(int);

private:
  QGroupBox*              myRangeGrp;
  QRadioButton*           myModeAllRB;
  QRadioButton*           myModeHorRB;
  QRadioButton*           myModeVerRB;
  QLineEdit*              myXMinEdit;
  QLineEdit*              myYMinEdit;
  QLineEdit*              myY2MinEdit;
  QLineEdit*              myXMaxEdit;
  QLineEdit*              myYMaxEdit;
  QLineEdit*              myY2MaxEdit;
  QPushButton*            myOkBtn;
  QPushButton*            myCancelBtn;
  bool                    mySecondAxisY;
};

#endif
