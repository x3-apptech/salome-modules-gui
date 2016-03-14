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
// File   : Plot2d_AnalyticalCurveDlg.h
// Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)


#ifndef PLOT2D_ANALYTICAL_CURVE_DLG_H
#define PLOT2D_ANALYTICAL_CURVE_DLG_H

#include "Plot2d.h"
#include "Plot2d_AnalyticalCurve.h"

#include <QDialog>
#include <QMap>

class QListWidget;
class QListWidgetItem;
class QGroupBox;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QPushButton;

class QtxIntSpinBox;
class QtxColorButton;
class QwtPlot;

class Plot2d_AnalyticalCurve;
class Plot2d_CurveContainer;
class Plot2d_ViewFrame;

class PLOT2D_EXPORT Plot2d_AnalyticalCurveDlg : public QDialog
{ 
  Q_OBJECT

  class Updater;
  class UpdateLocker;

  enum { ItemExisting, ItemAdded, ItemRemoved };

public:
  Plot2d_AnalyticalCurveDlg( Plot2d_CurveContainer*, QWidget* );
  ~Plot2d_AnalyticalCurveDlg();

  void accept();
  void reject();

private:  
  
  void                  init();
  void                  initPropsFromCurve(Plot2d_AnalyticalCurve*);
  QListWidgetItem*      selected() const;
  Plot2d_AnalyticalCurve* selectedCurve() const;
  QListWidgetItem*      getItem(Plot2d_AnalyticalCurve*) const;
  bool                  processCurves();
  QwtPlot*              getPlot();
  int                   propStatus( Plot2d_AnalyticalCurve* = 0, const int def = ItemExisting );
  QString               propTitle( Plot2d_AnalyticalCurve* = 0, const QString& def = "" );
  bool                  propVisible( Plot2d_AnalyticalCurve* = 0, bool def = true );
  QString               propFormula( Plot2d_AnalyticalCurve* = 0, const QString& def = "" );
  int                   propIntervals( Plot2d_AnalyticalCurve* = 0, int def = 100 );
  Plot2d::MarkerType    propMarkerType( Plot2d_AnalyticalCurve* = 0, Plot2d::MarkerType def = Plot2d::None );
  Plot2d::LineType      propLineType( Plot2d_AnalyticalCurve* = 0, Plot2d::LineType def = Plot2d::NoPen );
  int                   propLineWidth( Plot2d_AnalyticalCurve* = 0, int def = 0 );
  QColor                propColor( Plot2d_AnalyticalCurve* = 0, const QColor& def = QColor() );
  bool                  propAutoAssign( Plot2d_AnalyticalCurve* = 0, bool def = true );

private slots:
  void addCurve();
  void removeCurve();
  void updateCurve();
  void updateState();
  void selectionChanged();
  void help();
  void apply();

private:
  typedef QMap <QString, QVariant> CurveProps;
  typedef QMap <Plot2d_AnalyticalCurve*, CurveProps> PropMap;

  Plot2d_CurveContainer* myContainer;
  PropMap                myProperties;
  Updater*               myUpdater;

  QListWidget*           myCurvesList;
  QGroupBox*             myCurveParams;
  QLineEdit*             myFormula;
  QtxIntSpinBox*         myNbIntervals;
  QGroupBox*             myCurveProps;
  QCheckBox*             myAutoAssign;
  QWidget*               myPropsGrp;
  QComboBox*             myMarkerType;
  QComboBox*             myLineType;
  QtxIntSpinBox*         myLineWidth;
  QtxColorButton*        myColor;
  QPushButton*           myAddButton;
  QPushButton*           myRemoveButton;
};

class Plot2d_AnalyticalCurveDlg::Updater : public QObject
{
  Q_OBJECT
public:
  Updater( QWidget* parent );
  ~Updater();
signals:
  void update();
};

#endif //PLOT2D_ANALYTICAL_CURVE_DLG_H
