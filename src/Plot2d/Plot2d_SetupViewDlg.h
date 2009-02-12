//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : Plot2d_SetupViewDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PLOT2D_SETUPVIEWDLG_H
#define PLOT2D_SETUPVIEWDLG_H

#include "Plot2d.h"

#include <QDialog>

class QSpinBox;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QPushButton;
class QtxColorButton;

class PLOT2D_EXPORT Plot2d_SetupViewDlg : public QDialog
{ 
  Q_OBJECT

public:
  Plot2d_SetupViewDlg( QWidget* = 0, bool = false, bool = false );
  ~Plot2d_SetupViewDlg();

  void            setMainTitle( bool, const QString& = QString() );
  bool            isMainTitleEnabled();
  QString         getMainTitle();

  void            setXTitle( bool, const QString& = QString() );
  bool            isXTitleEnabled();
  QString         getXTitle();

  void            setYTitle( bool, const QString& = QString() );
  void            setY2Title( bool, const QString& = QString() );
  bool            isYTitleEnabled();
  bool            isY2TitleEnabled();
  QString         getYTitle();
  QString         getY2Title();

  void            setCurveType( const int );
  int             getCurveType();

  void            setLegend( bool, int );
  bool            isLegendEnabled();
  int             getLegendPos();
  
  void            setMarkerSize( const int );
  int             getMarkerSize();

  void            setBackgroundColor( const QColor& );
  QColor          getBackgroundColor();

  void            setMajorGrid( bool, const int, bool, const int, bool, const int );
  void            getMajorGrid( bool&, int&, bool&, int&, bool&, int& );
  void            setMinorGrid( bool, const int, bool, const int, bool, const int );
  void            getMinorGrid( bool&, int&, bool&, int&, bool&, int& );

  void            setScaleMode( const int, const int );
  int             getXScaleMode();
  int             getYScaleMode();

  bool            isSetAsDefault();

protected slots:
  void            onMainTitleChecked();
  void            onXTitleChecked();
  void            onYTitleChecked();
  void            onY2TitleChecked();
  void            onLegendChecked();
  void            onXGridMajorChecked();
  void            onYGridMajorChecked();
  void            onY2GridMajorChecked();
  void            onXGridMinorChecked();
  void            onYGridMinorChecked();
  void            onY2GridMinorChecked();
  void            onHelp();

private:
  QCheckBox*      myTitleCheck;
  QLineEdit*      myTitleEdit;
  QCheckBox*      myTitleXCheck;
  QLineEdit*      myTitleXEdit;
  QCheckBox*      myTitleYCheck;
  QCheckBox*      myTitleY2Check;
  QLineEdit*      myTitleYEdit;
  QLineEdit*      myTitleY2Edit;
  QtxColorButton* myBackgroundBtn;
  QCheckBox*      myXGridCheck;
  QSpinBox*       myXGridSpin;
  QCheckBox*      myYGridCheck;
  QCheckBox*      myY2GridCheck;
  QSpinBox*       myYGridSpin;
  QSpinBox*       myY2GridSpin;
  QCheckBox*      myXMinGridCheck;
  QSpinBox*       myXMinGridSpin;
  QCheckBox*      myYMinGridCheck;
  QCheckBox*      myY2MinGridCheck;
  QSpinBox*       myYMinGridSpin;
  QSpinBox*       myY2MinGridSpin;
  QComboBox*      myCurveCombo;
  QCheckBox*      myLegendCheck;
  QComboBox*      myLegendCombo;
  QSpinBox*       myMarkerSpin;
  QComboBox*      myXModeCombo;
  QComboBox*      myYModeCombo;
  QComboBox*      myY2ModeCombo;
  QCheckBox*      myDefCheck;
  QPushButton*    myOkBtn;
  QPushButton*    myCancelBtn;
  QPushButton*    myHelpBtn;
  bool            mySecondAxisY;
};

#endif // PLOT2D_SETUPVIEWDLG_H
