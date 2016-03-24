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
//  File   : Plot2d_HistogramItem.h
//  Author : Natalia ERMOLAEVA, Open CASCADE S.A.S. (natalia.donis@opencascade.com)

#ifndef PLOT2D_PLOTITEMS_H
#define PLOT2D_PLOTITEMS_H

#include "Plot2d.h"

#include <qwt_legend_label.h>
#include <qwt_legend.h>
#include <qwt_plot_item.h>
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_series_data.h>
#include <qwt_compat.h>

class PLOT2D_EXPORT Plot2d_QwtLegendLabel : public QwtLegendLabel
{
public:
  enum YAxisIdentifierMode { IM_None = 0, IM_Left, IM_Right };

public:
  Plot2d_QwtLegendLabel( QWidget* = 0 );
  virtual ~Plot2d_QwtLegendLabel();

public:
  void             setYAxisIdentifierMode( const int );
  void             updateHighlit();
  void             setSelected( const bool on );
  bool             isSelected() const;
  QColor           getColorFromPalette( QPalette::ColorRole role );
  void             drawIdentifier( QPainter*, const QRect& );
  void             setSymbol( const QwtSymbol* );
  void             setSymbolType( const int );
  void             setPen( const QPen& );

protected:
  virtual void     drawText( QPainter*, const QRectF& );
  virtual void     paintEvent( QPaintEvent* );

private:

private:
  int              myYAxisIdentifierMode;
  QPixmap          myYAxisLeftIcon;
  QPixmap          myYAxisRightIcon;
  int              mySpacingCollapsed;
  int              mySpacingExpanded;
  bool             myIsSelected;
  QwtSymbol*       mySymbol;
  int              mySymbolType;
  QPen             myPen;
};

class PLOT2D_EXPORT Plot2d_SelectableItem {
public:
  Plot2d_SelectableItem();
  ~Plot2d_SelectableItem();

  void             setSelected( const bool );
  bool             isSelected() const;

  void             setLegendPen( const QPen& );
  QPen             legendPen() const;

  void             setLegendSymbol( const QwtSymbol* );
  QwtSymbol*       legendSymbol() const;

private:
  bool             myIsSelected;
  QPen             myLegendPen;
  QwtSymbol*       myLegendSymbol;
};

class PLOT2D_EXPORT Plot2d_QwtPlotCurve : public QwtPlotCurve, public Plot2d_SelectableItem
{
public:
  Plot2d_QwtPlotCurve( const QString&, QwtPlot::Axis = QwtPlot::yLeft );
  virtual ~Plot2d_QwtPlotCurve();

public:
  virtual void     setYAxisIdentifierEnabled( const bool );
  virtual void     drawSeries( QPainter* p,
                               const QwtScaleMap& xMap,
                               const QwtScaleMap& yMap,
                               const QRectF& canvasRect,
                               int from, int to) const;

  void             setDeviationData( const double* min, const double* max, const QList<int>& idx );
  bool             hasDeviationData() const;
  void             clearDeviationData();

protected:
  virtual void     updateLegend( const QwtPlotItem*,
                                 const QList<QwtLegendData>& );
  virtual void     itemChanged();

  QColor           deviationMarkerColor() const;
  int              deviationMarkerLineWidth() const;
  int              deviationMarkerTickSize() const;

private:
  QwtPlot::Axis    myYAxis;
  bool             myYAxisIdentifierEnabled;
  
  class Plot2d_DeviationData;
  Plot2d_DeviationData* myDeviationData;
};

class PLOT2D_EXPORT Plot2d_HistogramQwtItem: public QwtPlotItem
{
public:
  enum HistogramAttribute 
  {
    Auto = 0,
    Xfy  = 1
  };

  explicit Plot2d_HistogramQwtItem( const QString& = QString() );
  explicit Plot2d_HistogramQwtItem( const QwtText& );
  virtual ~Plot2d_HistogramQwtItem();

  void                   setData( const QwtIntervalSeriesData& );
  const QwtIntervalSeriesData& data() const;

  void                   setColor( const QColor& );
  QColor                 color() const;

  virtual QwtDoubleRect  boundingRect() const;
  virtual int            rtti() const;
  virtual void           draw( QPainter*, const QwtScaleMap&, 
                               const QwtScaleMap&, const QRectF& ) const;

  void                   setBaseline( double );
  double                 baseline() const;

  void                   setHistogramAttribute( HistogramAttribute, bool = true );
  bool                   testHistogramAttribute( HistogramAttribute ) const;

protected:
  virtual void           drawBar( QPainter*, Qt::Orientation, const QRect& ) const;

private:
  void                   init();

private:
  int                    myAttributes;
  QwtIntervalSeriesData  myData;
  QColor                 myColor;
  double                 myReference;
};

class PLOT2D_EXPORT Plot2d_HistogramItem : public Plot2d_HistogramQwtItem, public Plot2d_SelectableItem
{
public:
  explicit Plot2d_HistogramItem( const QString& = QString() );
  explicit Plot2d_HistogramItem( const QwtText& );
  virtual ~Plot2d_HistogramItem();

  QList<QRect>     getBars() const;

  virtual void     updateLegend( QwtPlotItem*,
                                 QList<QwtLegendData>& );
  virtual void     draw( QPainter*, const QwtScaleMap&,
                         const QwtScaleMap&, const QRectF& ) const;

  void             setCrossItems( bool theCross );
  bool             isCrossItems() const;

protected:
  void             drawRectAndLowers( QPainter*, Qt::Orientation,
                                      const QRect& ) const;
  int              getCrossedTop( const QRect& ) const;

protected:
  QList<QRect>     myBarItems;
  bool             myCrossed;
};

class PLOT2D_EXPORT Plot2d_QwtLegend : public QwtLegend
{
public:
  explicit Plot2d_QwtLegend( QWidget *parent = NULL );
  virtual ~Plot2d_QwtLegend();

protected:
  virtual QWidget* createWidget( const QwtLegendData& ) const;
};

#endif // PLOT2D_PLOTITEMS_H
