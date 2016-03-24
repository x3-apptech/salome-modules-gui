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
//  File   : Plot2d_Curve.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "Plot2d_Curve.h"
#include "Plot2d_PlotItems.h"
#include <qwt_plot_curve.h>

const int DEFAULT_LINE_WIDTH  =  0;     // (default) line width
const int DEFAULT_MARKER_SIZE =  9;     // default marker size

/*!
  Constructor
*/
Plot2d_Curve::Plot2d_Curve()
: Plot2d_Object(),
  myColor( 0, 0, 0 ), 
  myMarker( Plot2d::Circle ), 
  myMarkerSize( 0 ), 
  myLine( Plot2d::Solid ), 
  myLineWidth( 0 )
{
}

/*!
  Destructor
*/
Plot2d_Curve::~Plot2d_Curve()
{
}

/*!
  Copy constructor. Makes deep copy of data
*/
Plot2d_Curve::Plot2d_Curve( const Plot2d_Curve& curve )
: Plot2d_Object( curve )
{
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
}

/*!
  operator=. Makes deep copy of data
*/
Plot2d_Curve& Plot2d_Curve::operator=( const Plot2d_Curve& curve )
{
  Plot2d_Object::operator=(curve);
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  return *this;
}

/*!
  Get typeid for the plot2d curve class
*/
int Plot2d_Curve::rtti()
{
  return QwtPlotItem::Rtti_PlotCurve;
}

/*!
  Create plot object for the curve
*/
QwtPlotItem* Plot2d_Curve::createPlotItem()
{
  QwtPlotCurve* aCurve = new Plot2d_QwtPlotCurve( getVerTitle(), getYAxis() );
  updatePlotItem( aCurve );
  return aCurve;
}

/*!
  Auto fill parameters of object by plot view
*/
void Plot2d_Curve::autoFill( const QwtPlot* thePlot )
{
  QwtSymbol::Style typeMarker;
  QColor           color;
  Qt::PenStyle     typeLine;
  Plot2d::getNextMarker( rtti(), thePlot, typeMarker, color, typeLine );

  setColor( color );
  setLine( Plot2d::qwt2plotLine( typeLine ), DEFAULT_LINE_WIDTH );
  setMarker( Plot2d::qwt2plotMarker( typeMarker ) );
}

/*!
  Updates curve fields
*/
void Plot2d_Curve::updatePlotItem( QwtPlotItem* theItem )
{
  if ( theItem->rtti() != rtti() )
    return;

  Plot2d_QwtPlotCurve* aCurve = dynamic_cast<Plot2d_QwtPlotCurve*>( theItem );
  if ( !aCurve )
    return;

  Plot2d_Object::updatePlotItem( theItem );

  Qt::PenStyle     ps = Plot2d::plot2qwtLine( getLine() );
  QwtSymbol::Style ms = Plot2d::plot2qwtMarker( getMarker() );
  
  QColor aColor = isSelected() ?  Plot2d_Object::selectionColor() : getColor();
  int lineW = getLineWidth(); 
  if ( isSelected() ) lineW += (lineW == 0 ? 3 : 2);

  int markerS = isSelected() ? getMarkerSize() + 2 : getMarkerSize();

  aCurve->setSelected(isSelected());

  aCurve->setPen( QPen(aColor , lineW, ps ) );
  aCurve->setSymbol( new QwtSymbol( ms, QBrush( aColor ), 
                                    QPen( aColor ),
                                    QSize( markerS , markerS ) ) );

  aCurve->setLegendPen(QPen(getColor(), getLineWidth(), ps ));
  aCurve->setLegendSymbol( new QwtSymbol( ms, QBrush( getColor() ),
                                          QPen( getColor() ),
                                          QSize( getMarkerSize() , getMarkerSize() )));
  
  double *x, *y, *min, *max;
  long nb = getData( &x, &y );
  if(nb > 0 && x && y) {
    aCurve->setSamples( x, y, nb );
    delete [] x;
    delete [] y;
    QList<int> idx;
    getDeviationData(min, max, idx);
    if(idx.size() > 0 && min && max) {
      aCurve->setDeviationData(min,max,idx);
      delete min;
      delete max;
    } else {
      aCurve->clearDeviationData();
    }
  } else {
    aCurve->setSamples( NULL, NULL, 0 );
  }
}

/*!
  Sets curve's color ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setColor( const QColor& color )
{
  myColor = color;
  setAutoAssign( false );
}

/*!
  Gets curve's color
*/
QColor Plot2d_Curve::getColor() const
{
  return myColor;
}

/*!
  Sets marker type and size ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarker( Plot2d::MarkerType marker, const int markerSize )
{
  setMarker( marker );
  setMarkerSize( markerSize );
  setAutoAssign( false );
}

/*!
  Sets marker type ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarker( Plot2d::MarkerType marker )
{
  myMarker = marker;
  setAutoAssign( false );
}

/* Sets Qwt marker type
 */
void Plot2d_Curve::setMarkerStyle( QwtSymbol::Style style)
{
  myMarkerStyle = style;
}

/*!
  Gets marker type
*/
Plot2d::MarkerType Plot2d_Curve::getMarker() const
{
  return myMarker;
}


/* Gets Qwt marker type
 */
QwtSymbol::Style Plot2d_Curve::getMarkerStyle() const
{
  return myMarkerStyle;
}



/*!
  Sets new marker size ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarkerSize( const int theSize )
{
  myMarkerSize = theSize < 0 ? 0 : theSize;
  setAutoAssign( false );
}

/*!
  Gets marker size
*/
int Plot2d_Curve::getMarkerSize() const
{
  return myMarkerSize;
}

/*!
  Sets line type and width ( and resets AutoAssign flag )
  NOTE : A line width of 0 will produce a 1 pixel wide line using a fast algorithm for diagonals. 
         A line width of 1 will also produce a 1 pixel wide line, but uses a slower more accurate 
         algorithm for diagonals. 
         For horizontal and vertical lines a line width of 0 is the same as a line width of 1.
*/
void Plot2d_Curve::setLine( Plot2d::LineType line, const int lineWidth )
{
  setLine( line );
  setLineWidth( lineWidth );
  setAutoAssign( false );
}

/*!
  Sets line type ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setLine( Plot2d::LineType line )
{
  myLine = line;
  setAutoAssign( false );
}

/*!
  Gets line type
*/
Plot2d::LineType Plot2d_Curve::getLine() const
{
  return myLine;
}

/*!
  Sets line width ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setLineWidth( const int lineWidth )
{
  myLineWidth = lineWidth < 0 ? 0 : lineWidth;
  setAutoAssign( false );
}

/*!
  Gets line width
*/
int Plot2d_Curve::getLineWidth() const
{
  return myLineWidth;
}
/*!
  Sets deviation data on the curve.
*/
void Plot2d_Curve::setDeviationData( const double* min, const double* max,const QList<int>& idx) {
  for( int i = 0; i < idx.size(); i++ ) {
    if(idx[i] < myPoints.size()) {
      myPoints[idx[i]].setDeviation(min[i], max[i]);
    }
  }
}

/*!
  Gets object's data
*/
void Plot2d_Curve::getDeviationData( double*& theMin, double*& theMax, QList<int>& idx) const
{
  int aNb = 0;
  idx.clear();
  for (int i = 0; i < nbPoints(); i++)
    if(myPoints[i].hasDeviation())
      aNb++;
  if(aNb) {
    double min, max;
    theMin = new double[aNb];
    theMax = new double[aNb];
    for (int i = 0; i < nbPoints(); i++)
      if(myPoints[i].hasDeviation()) {
        myPoints[i].deviation(min,max);
        theMin[i] = min;
        theMax[i] = max;
        idx.push_back(i);
      }
  }
}

/*!
  Clear deviation data on the curve.
*/
void Plot2d_Curve::clearDeviationData() {
  for( int i=0; i < myPoints.size(); i++ )
    myPoints[i].clearDeviation();
}

/*!
  Gets object's minimal ordinate
*/
double Plot2d_Curve::getMinY() const
{
  double aMinY = 1e150;
  pointList::const_iterator aIt;
  double coeff = 0.0;
  for (aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {    
    aMinY = qMin( aMinY, myScale * (*aIt).y );
    if((*aIt).minDeviation(coeff))
      aMinY = qMin( aMinY, coeff );
  }
  return aMinY;
}

/*!
  Gets object's maximal ordinate
*/
double Plot2d_Curve::getMaxY() const
{
  double aMaxY = -1e150;
  pointList::const_iterator aIt;
  double coeff = 0.0;
  for (aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    aMaxY = qMax( aMaxY, myScale * (*aIt).y);
    if((*aIt).maxDeviation(coeff))
      aMaxY = qMax( aMaxY, coeff);
  }
  return aMaxY;
}
