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
//  File   : Plot2d_Curve.h
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef PLOT2D_CURVE_H
#define PLOT2D_CURVE_H

#include "Plot2d.h"
#include "Plot2d_Object.h"

#include <qwt_symbol.h>

class PLOT2D_EXPORT Plot2d_Curve : public Plot2d_Object
{
public:

  Plot2d_Curve();
  Plot2d_Curve( const Plot2d_Curve& );

  virtual ~Plot2d_Curve();
  Plot2d_Curve& operator= ( const Plot2d_Curve& );

  virtual int          rtti();
  virtual QwtPlotItem* createPlotItem();
  virtual void         autoFill( const QwtPlot* );
  virtual void         updatePlotItem( QwtPlotItem* );

  void                 setColor( const QColor& );
  QColor               getColor() const;

  void                 setMarker( Plot2d::MarkerType, const int );
  void                 setMarker( Plot2d::MarkerType );
  void		             setMarkerStyle( QwtSymbol::Style style);
  Plot2d::MarkerType   getMarker() const;
  QwtSymbol::Style     getMarkerStyle() const;
  void                 setMarkerSize( const int );
  int                  getMarkerSize() const;

  void                 setLine( Plot2d::LineType, const int );
  void                 setLine( Plot2d::LineType );
  Plot2d::LineType     getLine() const;
  void                 setLineWidth( const int );
  int                  getLineWidth() const;
  void                 setDeviationData( const double*, const double*, const QList<int>&);
  void                 getDeviationData( double*&, double*&, QList<int>& ) const;
  void                 clearDeviationData();

  virtual double          getMinY() const;
  virtual double          getMaxY() const;


protected:

  QColor               myColor;
  Plot2d::MarkerType   myMarker;
  QwtSymbol::Style     myMarkerStyle;
  int                  myMarkerSize;
  Plot2d::LineType     myLine;
  int                  myLineWidth;
};

typedef QList<Plot2d_Curve*> curveList;

#endif
