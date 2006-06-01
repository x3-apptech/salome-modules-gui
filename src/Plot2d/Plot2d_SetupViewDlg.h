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
#ifndef PLOT2D_SETUPVIEWDLG_H
#define PLOT2D_SETUPVIEWDLG_H

#include "Plot2d.h"
#include <qdialog.h>

class QSpinBox;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QToolButton;
class QPushButton;

class PLOT2D_EXPORT Plot2d_SetupViewDlg : public QDialog
{ 
  Q_OBJECT

public:
  Plot2d_SetupViewDlg( QWidget* parent = 0, bool showDefCheck = false, bool secondAxisY = false );
  ~Plot2d_SetupViewDlg();

  void    setMainTitle( bool enable, const QString& title = QString::null );
  bool    isMainTitleEnabled();
  QString getMainTitle();
  void    setXTitle( bool enable, const QString& title = QString::null );
  bool    isXTitleEnabled();
  QString getXTitle();
  void    setYTitle( bool enable, const QString& title = QString::null );
  void    setY2Title( bool enable, const QString& title = QString::null );
  bool    isYTitleEnabled();
  bool    isY2TitleEnabled();
  QString getYTitle();
  QString getY2Title();
  void    setCurveType( const int type );
  int     getCurveType();
  void    setLegend( bool enable, int pos );
  bool    isLegendEnabled();
  int     getLegendPos();
  void    setMarkerSize( const int size );
  int     getMarkerSize();
  void    setBackgroundColor( const QColor& color );
  QColor  getBackgroundColor();
  void    setMajorGrid( bool enableX, const int xdiv, bool enableY, const int divY,
                        bool enableY2, const int divY2 );
  void    getMajorGrid( bool& enableX, int& xdiv, bool& enableY, int& divY,
                        bool& enableY2, int& divY2);
  void    setMinorGrid( bool enableX, const int xdiv, bool enableY, const int divY,
                        bool enableY2, const int divY2);
  void    getMinorGrid( bool& enableX, int& xdiv, bool& enableY, int& divY,
                        bool& enableY2, int& divY2);
  void    setScaleMode( const int xMode, const int yMode );
  int     getXScaleMode();
  int     getYScaleMode();
  bool    isSetAsDefault();

protected slots:
  void   onMainTitleChecked();
  void   onXTitleChecked();
  void   onYTitleChecked();
  void   onY2TitleChecked();
  void   onBackgroundClicked();
  void   onLegendChecked();
  void   onXGridMajorChecked();
  void   onYGridMajorChecked();
  void   onY2GridMajorChecked();
  void   onXGridMinorChecked();
  void   onYGridMinorChecked();
  void   onY2GridMinorChecked();

private:
  QCheckBox*     myTitleCheck;
  QLineEdit*     myTitleEdit;
  QCheckBox*     myTitleXCheck;
  QLineEdit*     myTitleXEdit;
  QCheckBox*     myTitleYCheck;
  QCheckBox*     myTitleY2Check;
  QLineEdit*     myTitleYEdit;
  QLineEdit*     myTitleY2Edit;
  QToolButton*   myBackgroundBtn;
  QCheckBox*     myXGridCheck;
  QSpinBox*      myXGridSpin;
  QCheckBox*     myYGridCheck;
  QCheckBox*     myY2GridCheck;
  QSpinBox*      myYGridSpin;
  QSpinBox*      myY2GridSpin;
  QCheckBox*     myXMinGridCheck;
  QSpinBox*      myXMinGridSpin;
  QCheckBox*     myYMinGridCheck;
  QCheckBox*     myY2MinGridCheck;
  QSpinBox*      myYMinGridSpin;
  QSpinBox*      myY2MinGridSpin;
  QComboBox*     myCurveCombo;
  QCheckBox*     myLegendCheck;
  QComboBox*     myLegendCombo;
  QSpinBox*      myMarkerSpin;
  QComboBox*     myXModeCombo;
  QComboBox*     myYModeCombo;
  QComboBox*     myY2ModeCombo;
  QCheckBox*     myDefCheck;

  QPushButton*   myOkBtn;
  QPushButton*   myCancelBtn;
  bool           mySecondAxisY;
};

#endif
