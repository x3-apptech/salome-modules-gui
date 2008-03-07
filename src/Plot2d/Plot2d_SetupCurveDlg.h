// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
//  SALOME Plot2d : implementation of desktop and GUI kernel
//
//  File   : Plot2d_SetupCurveDlg.h
//  Author : Vadim SANDLER
//  Module : SALOME

#ifndef Plot2d_SetupCurveDlg_H
#define Plot2d_SetupCurveDlg_H

#include "Plot2d.h"

#include <qdialog.h>

/*!
  \class Plot2d_SetupCurveDlg
  Dialog box for setup Plot2d curve
*/

class QPushButton;
class QComboBox;
class QSpinBox;
class QToolButton;

class PLOT2D_EXPORT Plot2d_SetupCurveDlg : public QDialog
{ 
  Q_OBJECT

public:
  Plot2d_SetupCurveDlg( QWidget* parent = 0 );
  ~Plot2d_SetupCurveDlg();

public:
  void   setLine( const int line, const int width );
  int    getLine() const;
  int    getLineWidth() const;
  void   setMarker( const int marker );
  int    getMarker() const ;
  void   setColor( const QColor& color );
  QColor getColor() const;
  
protected slots:
  void   onColorChanged();

private:
  QPushButton* myOkBtn;
  QPushButton* myCancelBtn;
  QComboBox*   myLineCombo;
  QSpinBox*    myLineSpin;
  QComboBox*   myMarkerCombo;
  QToolButton* myColorBtn;
};

#endif // Plot2d_SetupCurveDlg_H

