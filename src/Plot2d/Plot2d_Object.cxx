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
//  File   : Plot2d_Object.cxx
//  Author : Natalia ERMOLAEVA, Open CASCADE S.A.S. (natalia.donis@opencascade.com)
//

#include "Plot2d_Object.h"

// Static members
QColor Plot2d_Object::mySelectionColor;
QColor Plot2d_Object::myHighlightedLegendTextColor;


/*!
  Constructor
*/
Plot2d_Object::Plot2d_Object()
: myAutoAssign( true ),
  myHorTitle( "" ), myVerTitle( "" ),
  myHorUnits( "" ), myVerUnits( "" ),
  myName( "" ),
  myXAxis( QwtPlot::xBottom ),
  myYAxis( QwtPlot::yLeft ),
  myIsSelected(false),
  myScale ( 1.0 )
{
}

/*!
  Destructor
*/
Plot2d_Object::~Plot2d_Object()
{
}

/*!
  Copy constructor. Makes deep copy of data.
*/
Plot2d_Object::Plot2d_Object( const Plot2d_Object& object )
{
  myAutoAssign = object.isAutoAssign();
  myHorTitle   = object.getHorTitle();
  myVerTitle   = object.getVerTitle();
  myHorUnits   = object.getHorUnits();
  myVerUnits   = object.getVerUnits();
  myName       = object.getName();
  myXAxis      = object.getXAxis();
  myYAxis      = object.getYAxis();
  myPoints     = object.getPointList();
  myScale      = object.getScale();
}

/*!
  operator=. Makes deep copy of data.
*/
Plot2d_Object& Plot2d_Object::operator=( const Plot2d_Object& object )
{
  myAutoAssign = object.isAutoAssign();
  myHorTitle   = object.getHorTitle();
  myVerTitle   = object.getVerTitle();
  myHorUnits   = object.getHorUnits();
  myVerUnits   = object.getVerUnits();
  myName       = object.getName();
  myXAxis      = object.getXAxis();
  myYAxis      = object.getYAxis();
  myPoints     = object.getPointList();
  myScale      = object.getScale();
  return *this;
}

/*!
  Auto fill parameters of object by plot view
*/
void Plot2d_Object::autoFill( const QwtPlot* )
{
}

/*!
 * Updates object fields
 */
void Plot2d_Object::updatePlotItem( QwtPlotItem* theItem )
{
  if ( !theItem || theItem->rtti() != rtti() )
    return;

  if ( theItem->yAxis() != getYAxis() || theItem->xAxis() != getXAxis() ) {
    theItem->setAxes( getXAxis(), getYAxis() );

    QwtPlot* aPlot = theItem->plot();
    if ( aPlot ) {
      theItem->detach();
      theItem->attach( aPlot );
    }
  }
  QString name = !getName().isEmpty() ? getName() : getVerTitle();
  if( myScale != 1.0 )
      name = name + QString("( *%1 )").arg(myScale);
  theItem->setTitle( name );
}

/*!
  \return title of table
*/
QString Plot2d_Object::getTableTitle() const
{
  return QString();
}

/*!
  Sets object's horizontal title
*/
void Plot2d_Object::setHorTitle( const QString& title )
{
  myHorTitle = title;
}

/*!
  Gets object's horizontal title
*/
QString Plot2d_Object::getHorTitle() const
{
  return myHorTitle;
}

/*!
  Sets object's vertical title
*/
void Plot2d_Object::setVerTitle( const QString& title )
{
  myVerTitle = title;
}

/*!
  Gets object's vertical title
*/
QString Plot2d_Object::getVerTitle() const
{
  return myVerTitle;
}

/*!
  Sets object's horizontal units
*/
void Plot2d_Object::setHorUnits( const QString& units )
{
  myHorUnits = units;
}

/*!
  Gets object's horizontal units
*/
QString Plot2d_Object::getHorUnits() const
{
  return myHorUnits;
}

/*!
  Sets object's vertical units
*/
void Plot2d_Object::setVerUnits( const QString& units )
{
  myVerUnits = units;
}

/*!
  Gets object's vertical units
*/
QString Plot2d_Object::getVerUnits() const
{
  return myVerUnits;
}

/*!
  Sets object's name
 */
void Plot2d_Object::setName( const QString& theName )
{
  myName = theName;
}
/*!
  Gets object's name
 */
QString Plot2d_Object::getName() const
{
  return myName;
}

/*!
  Sets object's scale factor
 */
void Plot2d_Object::setScale( double theScale )
{
  myScale = theScale;
}
/*!
  Gets object's scale factor
 */
double Plot2d_Object::getScale() const
{
  return myScale;
}

/*!
  Adds one point for object.
*/
void Plot2d_Object::addPoint( double theX, double theY, const QString& theText )
{
  addPoint( Plot2d_Point( theX, theY, theText ) );
}

/*!
  Adds one point for object.
*/
void Plot2d_Object::addPoint( const Plot2d_Point& thePoint )
{
  myPoints.append( thePoint );
}

/*!
  Insert one point for object on some position.
*/
void Plot2d_Object::insertPoint( int thePos, double theX, double theY,
				 const QString& theText )
{
  insertPoint( thePos, Plot2d_Point( theX, theY, theText ) );
}

/*!
  Insert one point for object on some position.
*/
void Plot2d_Object::insertPoint( int thePos, const Plot2d_Point& thePoint )
{
  if ( thePos < 0 )
    myPoints.append( thePoint );
  else
    myPoints.insert( thePos, thePoint );
}

/*!
  Delete one point for object on some position.
*/
void Plot2d_Object::deletePoint(int thePos)
{
  if ( thePos >= 0 && thePos < myPoints.count() )
    myPoints.removeAt( thePos );
}

/*!
  Remove all points for object.
*/
void Plot2d_Object::clearAllPoints()
{
  myPoints.clear();
}

/*!
  Gets object's data : abscissas of points
*/
pointList Plot2d_Object::getPointList() const
{
  return myPoints;
}

/*!
  Gets points by index.
*/

Plot2d_Point& Plot2d_Object::getPoint(int index) {
	return myPoints[index];
}

/*!
  Gets object's data : abscissas of points
*/
void Plot2d_Object::setPointList( const pointList& points )
{
  myPoints = points;
}

/*!
  Sets object's data.
*/
void Plot2d_Object::setData( const double* hData, const double* vData, long size, const QStringList& lst )
{
  clearAllPoints();
  QStringList::const_iterator anIt = lst.begin(), aLast = lst.end();
  for ( long i = 0; i < size; i++, anIt++ )
    addPoint( hData[i], vData[i], anIt==aLast ? QString() : *anIt );
}

/*!
  Gets object's data : abscissas of points
*/
double* Plot2d_Object::horData() const
{
  int aNPoints = nbPoints();
  double* aX = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    aX[i] = myPoints[i].x;
  }
  return aX;
}

/*!
  Gets object's data : ordinates of points
*/
double* Plot2d_Object::verData() const
{
  int aNPoints = nbPoints();
  double* aY = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    aY[i] = myScale * myPoints[i].y;
  }
  return aY;
}

/*!
  Gets object's data
*/
long Plot2d_Object::getData( double** theX, double** theY ) const
{
  int aNPoints = nbPoints();
  *theX = new double[aNPoints];
  *theY = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    (*theX)[i] = myPoints[i].x;
    (*theY)[i] = myScale * myPoints[i].y;
  }
  return aNPoints;
}

/*!
  Changes text assigned to point of object
  \param ind -- index of point
  \param txt -- new text
*/
void Plot2d_Object::setText( const int ind, const QString& txt )
{
  if ( ind >= 0 && ind < myPoints.count() )
    myPoints[ind].text = txt;
}

/*!
  \return text assigned to point
  \param ind -- index of point
*/
QString Plot2d_Object::text( const int ind ) const
{
  return ( ind >= 0 && ind < myPoints.count() ) ? myPoints[ind].text : QString();
}

/*!
  Gets object's data : number of points
*/
int Plot2d_Object::nbPoints() const
{
  return myPoints.count();
}

/*!
  Returns true if object has no data
*/
bool Plot2d_Object::isEmpty() const
{
  return myPoints.isEmpty();
}

/*!
  Sets object's AutoAssign flag - in this case attributes will be set automatically
*/
void Plot2d_Object::setAutoAssign( bool on )
{
  myAutoAssign = on;
}

/*!
  Gets object's AutoAssign flag state
*/
bool Plot2d_Object::isAutoAssign() const
{
  return myAutoAssign;
}

/*!
  Sets object's x axis
*/
void Plot2d_Object::setXAxis(QwtPlot::Axis theXAxis)
{
  if (theXAxis == QwtPlot::xBottom || theXAxis == QwtPlot::xTop)
    myXAxis = theXAxis;
}

/*!
  Gets object's x axis
*/
QwtPlot::Axis Plot2d_Object::getXAxis() const
{
  return myXAxis;
}

/*!
  Sets object's y axis
*/
void Plot2d_Object::setYAxis(QwtPlot::Axis theYAxis)
{
  if (theYAxis == QwtPlot::yLeft || theYAxis == QwtPlot::yRight)
    myYAxis = theYAxis;
}

/*!
  Gets object's y axis
*/
QwtPlot::Axis Plot2d_Object::getYAxis() const
{
  return myYAxis;
}

/*!
  Gets object's minimal abscissa
*/
double Plot2d_Object::getMinX() const
{
  double aMinX = 1e150;
  pointList::const_iterator aIt;
  for (aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt)
    aMinX = qMin( aMinX, (*aIt).x );
  return aMinX;
}

/*!
  Gets object's maximal abscissa
*/
double Plot2d_Object::getMaxX() const
{
  double aMaxX = -1e150;
  pointList::const_iterator aIt;
  for (aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt)
    aMaxX = qMax( aMaxX, (*aIt).x );
  return aMaxX;
}

/*!
  Gets object's minimal ordinate
*/
double Plot2d_Object::getMinY() const
{
  double aMinY = 1e150;
  pointList::const_iterator aIt;
  for (aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt)
    aMinY = qMin( aMinY, myScale * (*aIt).y );
  return aMinY;
}

/*!
  Gets object's maximal ordinate
*/
double Plot2d_Object::getMaxY() const
{
  double aMaxY = -1e150;
  pointList::const_iterator aIt;
  for (aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt)
    aMaxY = qMax( aMaxY, myScale * (*aIt).y );
  return aMaxY;
}

/*!
  Sets object's selected property
*/
void Plot2d_Object::setSelected(const bool on) {
  myIsSelected = on;
}

/*!
  Gets object's selected property
*/
bool Plot2d_Object::isSelected() const {
  return myIsSelected;
}

/*!
 * Sets selection color of the object.
*/
void Plot2d_Object::setSelectionColor(const QColor& c) {
  mySelectionColor = c;
}

/*!
 * Return selection color of the object.
*/
QColor Plot2d_Object::selectionColor() {
  return mySelectionColor;
}

/*!
 * Sets font color of the selected legend item.
*/
void Plot2d_Object::setHighlightedLegendTextColor(const QColor& c) {
  myHighlightedLegendTextColor = c;
}

/*!
 * Sets font color of the selected legend item.
*/
QColor Plot2d_Object::highlightedLegendTextColor() {
  return myHighlightedLegendTextColor;
}
