//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : SPlot2d_SetupPlot2dDlg.h
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#ifndef SPlot2d_SetupPlot2dDlg_H
#define SPlot2d_SetupPlot2dDlg_H

#include "Plot2d_Curve.h"

#include <SALOMEDSClient_SObject.hxx>

#include <qdialog.h>
#include <qptrlist.h>

//=================================================================================
// class    : SPlot2d_SetupPlot2dDlg
// purpose  : Dialog box for setup Plot2d view
//=================================================================================

class QScrollView;
class QPushButton;
class QLabel;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QToolButton;
class SPlot2d_ItemContainer;

class SPlot2d_SetupPlot2dDlg : public QDialog
{ 
  Q_OBJECT

public:
  SPlot2d_SetupPlot2dDlg( _PTR(SObject) object, QWidget* parent = 0 );
  ~SPlot2d_SetupPlot2dDlg();

  void getCurves( QPtrList<Plot2d_Curve>& container );
  void getCurvesSource( int& horIndex, QValueList<int>& verIndexes );
  bool getCurveAttributes( const int vIndex, bool& isAuto, int& marker, int& line, int& lineWidth, QColor& color);

private slots:
  void onHBtnToggled( bool );
  void onVBtnToggled( bool );
  void enableControls();

private:
  QScrollView*                myView;
  QPushButton*                myOkBtn;
  QPushButton*                myCancelBtn;
  QPtrList<SPlot2d_ItemContainer> myItems;

  _PTR(SObject)               myObject;
};

class SPlot2d_ItemContainer : public QObject
{
  Q_OBJECT

public:
  SPlot2d_ItemContainer( QObject* parent = 0, const char* name = 0 );
  
  void   createWidgets( QWidget* parentWidget );
  void   enableWidgets( bool enable );

  void   setHorizontalOn( bool on );
  bool   isHorizontalOn() const;
  void   setVerticalOn( bool on );
  bool   isVerticalOn() const;
  bool   isAutoAssign() const;
  void   setAutoAssign( bool on );
  void   setLine( const int line, const int width );
  int    getLine() const;
  int    getLineWidth() const;
  void   setMarker( const int marker );
  int    getMarker() const;
  void   setColor( const QColor& color );
  QColor getColor() const;

protected:
  void   updateState();

signals:
  void   autoClicked();
  void   horToggled( bool );
  void   verToggled( bool );

public slots:
  void   onAutoChanged();
  void   onColorChanged();
  void   onHVToggled( bool );

public:
  bool                  myEnabled;
  QToolButton*          myHBtn;
  QToolButton*          myVBtn;
  QLabel*               myTitleLab;
  QLabel*               myUnitLab;
  QCheckBox*            myAutoCheck;
  QComboBox*            myLineCombo;
  QSpinBox*             myLineSpin;
  QComboBox*            myMarkerCombo;
  QToolButton*          myColorBtn;
};

#endif // SPlot2d_SetupPlot2dDlg_H

