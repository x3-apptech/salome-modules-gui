//  SALOME Plot2d : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : Plot2d_SetupCurveDlg.h
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#ifndef Plot2d_SetupCurveDlg_H
#define Plot2d_SetupCurveDlg_H

#include <qdialog.h>

/*!
  \class Plot2d_SetupCurveDlg
  Dialog box for setup Plot2d curve
*/

class QPushButton;
class QComboBox;
class QSpinBox;
class QToolButton;

class Plot2d_SetupCurveDlg : public QDialog
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

