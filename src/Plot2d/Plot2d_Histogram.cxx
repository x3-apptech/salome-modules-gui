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
//  File   : Plot2d_Histogram.cxx
//  Author : Natalia ERMOLAEVA, Open CASCADE S.A.S. (natalia.donis@opencascade.com)

#include "Plot2d_Histogram.h"
#include "Plot2d_PlotItems.h"

#include <qwt_plot_curve.h>

const int MAX_ATTEMPTS = 10;     // max attempts

/*!
  Constructor.
*/
Plot2d_Histogram::Plot2d_Histogram()
: Plot2d_Object(),
  myColor( 0, 0, 0 ),
  myWidth( 0 ),
  myDefWidth( 0 )
{
}

/*!
  Destructor.
*/
Plot2d_Histogram::~Plot2d_Histogram()
{
}

/*!
  Copy constructor. Makes deep copy of data.
*/
Plot2d_Histogram::Plot2d_Histogram( const Plot2d_Histogram& hist )
: Plot2d_Object( hist )
{
  myColor      = hist.myColor;
  myWidth      = hist.myWidth;
  myDefWidth   = hist.myDefWidth;
}

/*!
  operator=. Makes deep copy of data.
*/
Plot2d_Histogram& Plot2d_Histogram::operator=( const Plot2d_Histogram& hist )
{
  Plot2d_Object::operator=(hist);
  myColor      = hist.myColor;
  myWidth      = hist.myWidth;
  myDefWidth   = hist.myDefWidth;
  return *this;
}

/*!
  Get typeid for the plot2d histogram class
*/
int Plot2d_Histogram::rtti()
{
  return QwtPlotItem::Rtti_PlotHistogram;
}

/*!
  Create plot object for the histogram
*/
QwtPlotItem* Plot2d_Histogram::createPlotItem()
{
  Plot2d_HistogramItem* anItem = new Plot2d_HistogramItem();
  updatePlotItem( anItem );
  return anItem;
}

/*!
  Auto fill parameters of object by plot view
*/
void Plot2d_Histogram::autoFill( const QwtPlot* thePlot )
{
  setColor( getNextColor( thePlot ) );
}

/*!
  Updates histogram fields
*/
void Plot2d_Histogram::updatePlotItem( QwtPlotItem* theItem )
{
  if ( theItem->rtti() != rtti() )
    return;

  Plot2d_HistogramItem* anItem = dynamic_cast<Plot2d_HistogramItem*>( theItem );
  if ( !anItem )
    return;

  Plot2d_Object::updatePlotItem( theItem );

  anItem->setData( getData() );
  anItem->setLegendPen(getColor());
  anItem->setSelected(isSelected());
  anItem->setColor( isSelected() ? Plot2d_Object::selectionColor() : getColor() );
}

/*!
  Sets data to object
*/
void Plot2d_Histogram::setData( const QList<double>& theXVals,
				const QList<double>& theYVals )
{
  pointList aPoints;
  int aSize = theXVals.size();
  for ( int i = 0; i < aSize; i++ )
    aPoints.append( Plot2d_Point( theXVals[i], theYVals[i] ) );
  setPointList( aPoints );

  myDefWidth = getMinInterval( theXVals )*(2./3.);
  myWidth = 0; // myDefWidth // VSR: width should not be automatically reset to myDefWidth
}

/*!
  Gets data
*/
QwtIntervalSeriesData Plot2d_Histogram::getData() const
{
  pointList aPoints = getPointList();
  int aSize = aPoints.size();

  QwtArray<QwtIntervalSample> anIntervals( aSize );
  double aX;
  double aWidth = myWidth <= 0 ? myDefWidth : myWidth; // VSR: width is either manually assigned or auto-calculated
  for ( int i = 0; i < aSize; i++ ) {
    aX = aPoints[i].x;
    anIntervals[i] = QwtIntervalSample( aPoints[i].y, aX - aWidth/2, aX + aWidth/2 );
  }

  return QwtIntervalSeriesData( anIntervals );
}

/*!
  Sets color of histogram
*/
void Plot2d_Histogram::setColor( const QColor& theColor )
{
  myColor = theColor;
  setAutoAssign( false );
}

/*!
  Returns color of histogram
*/
QColor Plot2d_Histogram::getColor() const
{
  return myColor;
}

/*!
  Sets custom width of a histogram bar
*/
void Plot2d_Histogram::setWidth( const double theWidth )
{
  myWidth = theWidth;
  //setAutoAssign( false ); // VSR: width attribute is not auto-assigned
}

/*!
  Returns custom or automatic width for a histogram bar
*/
double Plot2d_Histogram::getWidth( const bool isDef ) const
{
  return isDef ? myDefWidth : myWidth;
}

/*!
  Gets new unique marker for item if possible
*/
QColor Plot2d_Histogram::getNextColor( const QwtPlot* thePlot )
{
  bool bOk = false;
  int cnt = 0;
  QColor aColor;
  while ( !bOk ) {
    int aRed    = (int)( 256.0 * rand() / RAND_MAX);  // generate random color
    int aGreen  = (int)( 256.0 * rand() / RAND_MAX);  // ...
    int aBlue   = (int)( 256.0 * rand() / RAND_MAX);  // ...
    aColor      = QColor( aRed, aGreen, aBlue );
    bOk = ( ++cnt == MAX_ATTEMPTS ) || !existColor( thePlot, aColor );
  }
  return aColor;
}

/*!
  Checks if color is already user by other histogram entity
*/
bool Plot2d_Histogram::existColor( const QwtPlot* thePlot, const QColor& theColor )
{
  bool ok = false;
  
  QColor bgColor = thePlot->palette().color( QPalette::Background );
  if ( Plot2d::closeColors( theColor, bgColor ) ) {
    ok = true;
  }
  else {
    QwtPlotItemList anItems = thePlot->itemList();
    QwtPlotItemIterator anIt = anItems.begin(), aLast = anItems.end();
    QwtPlotItem* anItem;
    for( ; anIt != aLast && !ok; anIt++ ) {
      anItem = *anIt;
      if ( !anItem )
	continue;
      if ( anItem->rtti() == rtti() ) {
	Plot2d_HistogramItem* aHItem = dynamic_cast<Plot2d_HistogramItem*>( anItem );
	ok = aHItem && Plot2d::closeColors( theColor, aHItem->color() );
      }
      else if ( anItem->rtti() == QwtPlotItem::Rtti_PlotCurve ) {
	QwtPlotCurve* aCurve = dynamic_cast<QwtPlotCurve*>( anItem );
	ok = aCurve && Plot2d::closeColors( theColor, aCurve->pen().color() );
      }
    }
  }
  return ok;
}

/*!
  Return min interval from values
*/
double Plot2d_Histogram::getMinInterval( const QList<double>& theVals )
{
  double aValue = -1;
  int aSize = theVals.size();
  if ( aSize > 1 ) {
    aValue = qAbs( theVals[1] - theVals[0] );
    double aDelta;
    for ( int i = 2; i < aSize; i++ ) {
      aDelta = qAbs( theVals[i] - theVals[i-1] );
      aValue = qMin( aValue, qMax( aDelta, 0. ) );
    }
    aValue = aValue/2;
  }
  return aValue;
}

