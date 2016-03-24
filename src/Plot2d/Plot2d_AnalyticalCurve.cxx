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
//  File   : Plot2d_AnalyticalCurve.cxx
//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)

#include "Plot2d_AnalyticalParser.h"
#include "Plot2d_AnalyticalCurve.h"
#include "Plot2d_PlotItems.h"
#include "Plot2d_Object.h"

#include <qwt_scale_div.h>


//Init static data;

int Plot2d_AnalyticalCurve::myNbCurves = 0;

/*!
  Constructor
*/
Plot2d_AnalyticalCurve::Plot2d_AnalyticalCurve() : 
  myAutoAssign(true),
  myColor( 0, 0, 0 ), 
  myMarker( Plot2d::Circle ), 
  myMarkerSize( 0 ), 
  myLine( Plot2d::Solid ), 
  myLineWidth( 0 ),
  myRangeBegin(0.0),
  myRangeEnd(100.0),
  myNbIntervals(100),
  myExpression(""),
  myAction(Plot2d_AnalyticalCurve::ActAddInView),
  myState(Plot2d_AnalyticalCurve::StateNeedUpdate),
  myCurve(0),
  myActive(true),
  myIsSelected(false)
{
  myName = QString("Analytical Curve %1").arg(++myNbCurves);
}


/*!
  Destructor
*/
Plot2d_AnalyticalCurve::~Plot2d_AnalyticalCurve()
{
}

/*!
  Copy constructor. Makes deep copy of data
*/
Plot2d_AnalyticalCurve::Plot2d_AnalyticalCurve( const Plot2d_AnalyticalCurve& curve )
{
  myAutoAssign = curve.isAutoAssign();
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  myRangeBegin = curve.getRangeBegin();
  myRangeEnd   = curve.getRangeEnd();
  myNbIntervals= curve.getNbIntervals();
  myPoints     = curve.myPoints;
  myAction     = curve.getAction();
  myName       = curve.getName();
  myExpression = curve.getExpression();
  myState      = curve.state();
  myCurve      = curve.myCurve;
  myActive     = curve.isActive();
}

/*!
  operator=. Makes deep copy of data
*/
Plot2d_AnalyticalCurve& Plot2d_AnalyticalCurve::operator=( const Plot2d_AnalyticalCurve& curve )
{
  myAutoAssign = curve.isAutoAssign();
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myMarkerSize = curve.getMarkerSize();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  myRangeBegin = curve.getRangeBegin();
  myRangeEnd   = curve.getRangeEnd();
  myNbIntervals= curve.getNbIntervals();
  myPoints     = curve.myPoints;
  myAction     = curve.getAction();
  myName       = curve.getName();
  myExpression = curve.getExpression();
  myState      = curve.state();
  myCurve      = curve.myCurve;
  myActive     = curve.isActive();
  return *this;
}

/*!
  Create plot object for the curve
*/
QwtPlotItem* Plot2d_AnalyticalCurve::plotItem()
{
  if(!myCurve) {
    myCurve = new Plot2d_QwtPlotCurve(QString(""));
    updatePlotItem();
  }
  return myCurve;
}

/*!
  Auto fill parameters of object by plot view
*/
void Plot2d_AnalyticalCurve::autoFill( const QwtPlot* thePlot )
{
  QwtSymbol::Style typeMarker;
  QColor           color;
  Qt::PenStyle     typeLine;
  Plot2d::getNextMarker( QwtPlotItem::Rtti_PlotCurve, thePlot, typeMarker, color, typeLine );
  
  setColor( color );
  setLine( Plot2d::qwt2plotLine( typeLine ));
  setLineWidth(1);
  setMarker( Plot2d::qwt2plotMarker( typeMarker ) );
}

/*!
  Updates curve fields
*/
void Plot2d_AnalyticalCurve::updatePlotItem()
{
  if ( !myCurve )
    return;

  Plot2d_QwtPlotCurve* aCurve = dynamic_cast<Plot2d_QwtPlotCurve*>(myCurve);

  if(!aCurve)
    return;
  
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

  double *x, *y;
  long nb = getData( &x, &y );
  aCurve->setSamples( x, y, nb );
  aCurve->setTitle(getName());
}


/*!
  Calculate the curve points.
*/
void Plot2d_AnalyticalCurve::calculate() {
  if( state() == Plot2d_AnalyticalCurve::StateOk )
    return;

  if(myRangeBegin > myRangeEnd)
    return;

  Plot2d_AnalyticalParser* parser = Plot2d_AnalyticalParser::parser();
  double* x = 0;
  double* y = 0;
  int nb = parser->calculate(getExpression(), getRangeBegin(), getRangeEnd(),
			     getNbIntervals(),&x,&y);
  if( nb > 0 ) {
    myPoints.clear();
    for( int i = 0; i < nb; i++ ) {
      Plot2d_Point pnt(x[i], y[i]);
      myPoints.append(pnt);
    }
    delete x;
    delete y;
    myState = Plot2d_AnalyticalCurve::StateOk;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}
/*!
  Checks that this curve can be computed for the input QwtPlot
*/
bool Plot2d_AnalyticalCurve::checkCurve( const QwtPlot* thePlot) {
  if( !myExpression.isEmpty() && thePlot ) {
    const QwtScaleDiv div = thePlot->axisScaleDiv(QwtPlot::xBottom);
    setRangeBegin(div.lowerBound());
    setRangeEnd(div.upperBound());
    calculate();
  }
  return myState == Plot2d_AnalyticalCurve::StateOk;
}


/*!
  Gets object's data
*/
long Plot2d_AnalyticalCurve::getData( double** theX, double** theY ) const
{
  int aNPoints = myPoints.size();
  *theX = new double[aNPoints];
  *theY = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    (*theX)[i] = myPoints[i].x;
    (*theY)[i] = myPoints[i].y;
  }
  return aNPoints;
}

/*!
  Sets curves's AutoAssign flag - in this case attributes will be set automatically
*/
void Plot2d_AnalyticalCurve::setAutoAssign( bool on )
{
  if( myAutoAssign != on ) {
    myAutoAssign = on;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}

/*!
  Gets curve's AutoAssign flag state
*/
bool Plot2d_AnalyticalCurve::isAutoAssign() const
{
  return myAutoAssign;
}

/*!
  Sets curve's color.
*/
void Plot2d_AnalyticalCurve::setColor( const QColor& color )
{
  if(myColor != color) {
    myColor = color;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}

/*!
  Gets curve's color
*/
QColor Plot2d_AnalyticalCurve::getColor() const
{
  return myColor;
}


/*!
  Sets marker type ( and resets AutoAssign flag )
*/
void Plot2d_AnalyticalCurve::setMarker( Plot2d::MarkerType marker )
{
  if(myMarker != marker) {
    myMarker = marker;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}

/*!
  Gets marker type
*/
Plot2d::MarkerType Plot2d_AnalyticalCurve::getMarker() const
{
  return myMarker;
}

/*!
  Sets new marker size
*/
void Plot2d_AnalyticalCurve::setMarkerSize( const int theSize )
{
  if( myMarkerSize != theSize ) {
    myMarkerSize = theSize < 0 ? 0 : theSize;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}

/*!
  Gets marker size
*/
int Plot2d_AnalyticalCurve::getMarkerSize() const
{
  return myMarkerSize;
}

/*!
  Sets line type
*/
void Plot2d_AnalyticalCurve::setLine( Plot2d::LineType line )
{
  if(myLine != line) {
    myLine = line;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}

/*!
  Gets line type
*/
Plot2d::LineType Plot2d_AnalyticalCurve::getLine() const
{
  return myLine;
}


/*!
  Sets line width
*/
void Plot2d_AnalyticalCurve::setLineWidth( const int lineWidth )
{
  if( myLineWidth != lineWidth ) {
    myLineWidth = lineWidth < 0 ? 0 : lineWidth;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}

/*!
  Gets line width
*/
int Plot2d_AnalyticalCurve::getLineWidth() const
{
  return myLineWidth;
}

/*!
  Sets number of points
*/
void Plot2d_AnalyticalCurve::setNbIntervals( const long nb )
{
  if( myNbIntervals != nb ) {
    myNbIntervals = nb < 1 ? 1 : nb;
    myState = Plot2d_AnalyticalCurve::StateNeedUpdate;
  }
}

/*!
  Gets number of points
*/
long Plot2d_AnalyticalCurve::getNbIntervals() const
{
  return myNbIntervals;
}

/*!
  Sets X coordinate of the first curve points
*/
void Plot2d_AnalyticalCurve::setRangeBegin( const double coord) {
  if( myRangeBegin != coord ) {
    myRangeBegin = coord;
    myState = Plot2d_AnalyticalCurve::StateNeedUpdate;
  }
}

/*!
  Gets X coordinate of the first curve points
*/
double Plot2d_AnalyticalCurve::getRangeBegin() const {
  return myRangeBegin;
}

/*!
  Sets X coordinate of the last curve points
*/
void Plot2d_AnalyticalCurve::setRangeEnd( const double coord) {
  if( myRangeEnd != coord ) {
    myRangeEnd = coord;
    myState = Plot2d_AnalyticalCurve::StateNeedUpdate;
  }
}

/*!
  Gets X coordinate of the last curve points
*/
double Plot2d_AnalyticalCurve::getRangeEnd() const {
  return myRangeEnd;
}

/*!
  Sets the curve expression.
*/
void Plot2d_AnalyticalCurve::setExpression( const QString& expr ) {
  if( myExpression != expr ) {
    myExpression = expr;
    myState = Plot2d_AnalyticalCurve::StateNeedUpdate;
  }
}

/*!
  Gets the curve expression.
*/
QString Plot2d_AnalyticalCurve::getExpression() const {
  return  myExpression;
}

/*!
  Sets the curve name.
*/
void Plot2d_AnalyticalCurve::setName( const QString& name ) {
  if( myName != name ) {    
    myName = name;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }
}

/*!
  Gets the curve name.
*/
QString Plot2d_AnalyticalCurve::getName() const {
  return myName;
}


/*!
  Sets the curve action.
*/
void Plot2d_AnalyticalCurve::setAction(const int act) {
  if( act == Plot2d_AnalyticalCurve::ActNothing ) {
    myAction = act;
    return;
  }
  
  if(myAction != Plot2d_AnalyticalCurve::ActAddInView && 
     myAction != Plot2d_AnalyticalCurve::ActRemoveFromView) {
    myAction = act;  
  }
}

/*!
  Gets the curve action.
*/
int Plot2d_AnalyticalCurve::getAction() const {
  return myAction;
}

/*!
  Gets the curve state.
*/
int Plot2d_AnalyticalCurve::state() const {
  return myState;
}

/*!
  Sets the curve active status.
*/
void Plot2d_AnalyticalCurve::setActive(const bool on) {
  if( myActive != on ) {    
    if(myActive && !on)
      setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
    else if(!myActive && on) {
      setAction(Plot2d_AnalyticalCurve::ActAddInView);
      myState = Plot2d_AnalyticalCurve::StateNeedUpdate;
    }
    myActive = on;
  }  
}

/*!
  Gets the curve active status.
*/
bool Plot2d_AnalyticalCurve::isActive() const {
  return myActive;
}


/*!
  Sets curve's selected property.
*/
void Plot2d_AnalyticalCurve::setSelected(const bool on) {
  if(myIsSelected != on) {
	myIsSelected  = on;
    setAction(Plot2d_AnalyticalCurve::ActUpdateInView);
  }

}

/*!
  Gets curve's selected property.
*/
bool Plot2d_AnalyticalCurve::isSelected() const {
  return myIsSelected;
}
