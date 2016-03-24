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

// File   : Plot2d_SetupCurveDlg.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PLOT2D_SETUPCURVEDLG_H
#define PLOT2D_SETUPCURVEDLG_H

#include "Plot2d.h"

#include <QDialog>

class QPushButton;
class QComboBox;
class QSpinBox;
class QLabel;
class QtxColorButton;

class PLOT2D_EXPORT Plot2d_SetupCurveDlg : public QDialog
{ 
  Q_OBJECT

public:
  Plot2d_SetupCurveDlg( QWidget* = 0 );
  ~Plot2d_SetupCurveDlg();

public:
  void               setLine( Plot2d::LineType, const int );
  Plot2d::LineType   getLine() const;
  int                getLineWidth() const;

  void               setMarker( Plot2d::MarkerType );
  Plot2d::MarkerType getMarker() const ;

  void               setColor( const QColor& );
  QColor             getColor() const;


private slots:
  void               updatePreview();

private:
  QPushButton*       myOkBtn;
  QPushButton*       myCancelBtn;
  QComboBox*         myLineCombo;
  QSpinBox*          myLineSpin;
  QComboBox*         myMarkerCombo;
  QtxColorButton*    myColorBtn;
  QLabel*            myPreview;
};

#endif // PLOT2D_SETUPCURVEDLG_H

