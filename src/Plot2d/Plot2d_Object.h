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
//  File   : Plot2d_Object.h
//  Author : Natalia ERMOLAEVA, Open CASCADE S.A.S. (natalia.donis@opencascade.com)
//

#ifndef PLOT2D_OBJECT_H
#define PLOT2D_OBJECT_H

#include "Plot2d.h"

#include <QList>
#include <qwt_plot.h>


class PLOT2D_EXPORT Plot2d_Object
{
public:
  Plot2d_Object();
  Plot2d_Object( const Plot2d_Object& );

  virtual ~Plot2d_Object();
  Plot2d_Object& operator= ( const Plot2d_Object& );

  virtual int          rtti() = 0;
  virtual QwtPlotItem* createPlotItem() = 0;
  virtual void         autoFill( const QwtPlot* );
  virtual void         updatePlotItem( QwtPlotItem* );

  virtual QString      getTableTitle() const;
  
  void                 setHorTitle( const QString& );
  QString              getHorTitle() const;
  void                 setVerTitle( const QString& );
  QString              getVerTitle() const;

  void                 setHorUnits( const QString& );
  QString              getHorUnits() const;
  void                 setVerUnits( const QString& );
  QString              getVerUnits() const;

  void                 setName( const QString& );
  QString              getName() const;

  void                 setScale( double );
  double               getScale() const;

  void                 addPoint( double, double, const QString& = QString() );
  void                 addPoint( const Plot2d_Point& );
  void                 insertPoint( int, double, double, const QString& = QString() );
  void                 insertPoint( int, const Plot2d_Point& );
  void                 deletePoint( int );
  void                 clearAllPoints();
  pointList            getPointList() const;
  void                 setPointList( const pointList& points );
  Plot2d_Point&        getPoint(int index);

  void                 setData( const double*, const double*, 
				long, const QStringList& = QStringList() );
  double*              horData() const;
  double*              verData() const;
  long                 getData( double**, double** ) const;

  void                 setText( const int, const QString& );
  QString              text( const int ) const;

  int                  nbPoints() const;
  bool                 isEmpty() const;

  void                 setAutoAssign( bool );
  bool                 isAutoAssign() const;

  void                 setXAxis( QwtPlot::Axis );
  QwtPlot::Axis        getXAxis() const;
  void                 setYAxis( QwtPlot::Axis );
  QwtPlot::Axis        getYAxis() const;

  // Protection against QwtObject::drawLines() bug in Qwt 0.4.x: 
  // it crashes if switched to X/Y logarithmic mode, when one or more points have
  // non-positive X/Y coordinate
  virtual double          getMinX() const;
  virtual double          getMaxX() const;
  virtual double          getMinY() const;
  virtual double          getMaxY() const;
  
  void                 setSelected(const bool);
  bool                 isSelected() const;
  
  static void   setSelectionColor(const QColor& c);
  static QColor selectionColor();

  static void   setHighlightedLegendTextColor(const QColor& c);
  static QColor highlightedLegendTextColor();

protected:
  bool                 myAutoAssign;
  QString              myHorTitle;
  QString              myVerTitle;
  QString              myHorUnits;
  QString              myVerUnits;
  QString              myName;
  QwtPlot::Axis        myXAxis;
  QwtPlot::Axis        myYAxis;

  double               myScale;

  pointList            myPoints;
  bool                 myIsSelected;

 private:
  static QColor mySelectionColor;            //!< Color of the selected curve or histogram
  static QColor myHighlightedLegendTextColor;  //!< Color of the selected legend item font
};

typedef QList<Plot2d_Object*> objectList;

#endif
