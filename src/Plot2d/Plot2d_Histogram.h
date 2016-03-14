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
//  File   : Plot2d_Histogram.h
//  Author : Natalia ERMOLAEVA, Open CASCADE S.A.S. (natalia.donis@opencascade.com)

#ifndef PLOT2D_HISTOGRAM_H
#define PLOT2D_HISTOGRAM_H

#include "Plot2d.h"
#include "Plot2d_Object.h"

#include <qwt_series_data.h>

class PLOT2D_EXPORT Plot2d_Histogram : public Plot2d_Object
{
public:
  Plot2d_Histogram();
  Plot2d_Histogram( const Plot2d_Histogram& );

  virtual ~Plot2d_Histogram();
  Plot2d_Histogram& operator= ( const Plot2d_Histogram& );

  virtual int           rtti();
  virtual QwtPlotItem*  createPlotItem();
  virtual void          autoFill( const QwtPlot* );
  virtual void          updatePlotItem( QwtPlotItem* );

  void                  setData( const QList<double>&, const QList<double>& );
  QwtIntervalSeriesData getData() const;

  void                  setColor( const QColor& );
  QColor                getColor() const;

  void                  setWidth( const double );
  double                getWidth( const bool ) const;

  static double         getMinInterval( const QList<double>& );

protected:
  QColor                getNextColor( const QwtPlot* );
  bool                  existColor( const QwtPlot*, const QColor& );

private:
  QColor                myColor;
  double                myWidth;
  double                myDefWidth;
};

#endif // PLOT2D_HISTOGRAM_H
