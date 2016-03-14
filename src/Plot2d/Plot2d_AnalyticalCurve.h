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
//  File   : Plot2d_AnalyticalCurve.h
//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)

#ifndef PLOT2D_ANALYTICAL_CURVE_H
#define PLOT2D_ANALYTICAL_CURVE_H

#include "Plot2d.h"


class QwtPlot;
class QwtPlotItem;
class QwtPlotCurve;


class PLOT2D_EXPORT Plot2d_AnalyticalCurve
{
public:
  
  /*
    Action enumeration.
   */
  enum {
    ActAddInView = 0,        //! Add curve in view
    ActRemoveFromView,       //! Remove curve from view
    ActUpdateInView,         //! Update curve in view 
    ActNothing               //! Do nothing
  };

  /*
    State enumeration.
   */
  enum {
    StateOk = 0,
    StateNeedUpdate
  };
  
  Plot2d_AnalyticalCurve();
  Plot2d_AnalyticalCurve( const Plot2d_AnalyticalCurve& );
  Plot2d_AnalyticalCurve& operator= ( const Plot2d_AnalyticalCurve& );

  virtual ~Plot2d_AnalyticalCurve();

  virtual QwtPlotItem* plotItem();
  virtual void         autoFill( const QwtPlot* );
  virtual void         updatePlotItem();
  virtual bool         checkCurve( const QwtPlot* );

  virtual void         calculate();

  long                 getData( double** , double** ) const;

  void                 setSelected(const bool);
  bool                 isSelected() const;



  void                 setAutoAssign( bool );
  bool                 isAutoAssign( ) const;

  void                 setColor( const QColor& );
  QColor               getColor() const;

  void                 setMarker( Plot2d::MarkerType );
  Plot2d::MarkerType   getMarker() const;

  void                 setMarkerSize( const int );
  int                  getMarkerSize() const;

  void                 setLine( Plot2d::LineType );
  Plot2d::LineType     getLine() const;

  void                 setLineWidth( const int );
  int                  getLineWidth() const;

  void                 setNbIntervals( const long );
  long                 getNbIntervals() const;

  void                 setRangeBegin( const double );
  double               getRangeBegin() const;

  void                 setRangeEnd( const double );
  double               getRangeEnd() const;

  void                 setExpression( const QString& );
  QString              getExpression() const;

  void                 setName( const QString& );
  QString              getName() const;

  void                 setActive(const bool);
  bool                 isActive() const;

  void                 setAction(const int);
  int                  getAction() const;
  int                  state() const;


protected:
  
  bool                 myAutoAssign;
  QColor               myColor;
  Plot2d::MarkerType   myMarker;
  int                  myMarkerSize;
  Plot2d::LineType     myLine;
  int                  myLineWidth;
  long                 myNbIntervals;
  pointList            myPoints;
  double               myRangeBegin;
  double               myRangeEnd;
  QString              myExpression;
  QString              myName; 
  int                  myAction;
  int                  myState;
  QwtPlotCurve*        myCurve;
  bool                 myActive;
  bool                 myIsSelected;

private:
  static int           myNbCurves;

};

typedef QList<Plot2d_AnalyticalCurve*> AnalyticalCurveList;

class PLOT2D_EXPORT Plot2d_CurveContainer
{
public:
  virtual void              addAnalyticalCurve( Plot2d_AnalyticalCurve* ) = 0;
  virtual void              removeAnalyticalCurve( Plot2d_AnalyticalCurve* ) = 0;
  virtual void              updateAnalyticalCurve( Plot2d_AnalyticalCurve*, bool = false ) = 0;
  virtual void              updateAnalyticalCurves() = 0;
  virtual AnalyticalCurveList getAnalyticalCurves() const = 0;
};

#endif //PLOT2D_ANALYTICAL_CURVE_H

