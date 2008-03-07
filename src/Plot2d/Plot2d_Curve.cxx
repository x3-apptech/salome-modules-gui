// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "Plot2d_Curve.h"
#include <qcolor.h>

/*!
  Constructor
*/
Plot2d_Curve::Plot2d_Curve()
: myHorTitle( "" ), myVerTitle( "" ), 
myHorUnits( "" ), myVerUnits( "" ), 
myAutoAssign( true ), myColor( 0,0,0 ), myMarker( Circle ), myLine( Solid ), myLineWidth( 0 ),
myYAxis( QwtPlot::yLeft )
{
}

/*!
  Destructor
*/
Plot2d_Curve::~Plot2d_Curve()
{
}

/*!
  Copy constructor. Makes deep copy of data.
*/
Plot2d_Curve::Plot2d_Curve( const Plot2d_Curve& curve )
{
  myAutoAssign = curve.isAutoAssign();
  myHorTitle   = curve.getHorTitle();
  myVerTitle   = curve.getVerTitle();
  myHorUnits   = curve.getHorUnits();
  myVerUnits   = curve.getVerUnits();
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  myPoints     = curve.getPointList();
}

/*!
  operator=. Makes deep copy of data.
*/
Plot2d_Curve& Plot2d_Curve::operator=( const Plot2d_Curve& curve )
{
  myAutoAssign = curve.isAutoAssign();
  myHorTitle   = curve.getHorTitle();
  myVerTitle   = curve.getVerTitle();
  myHorUnits   = curve.getHorUnits();
  myVerUnits   = curve.getVerUnits();
  myColor      = curve.getColor();
  myMarker     = curve.getMarker();
  myLine       = curve.getLine();
  myLineWidth  = curve.getLineWidth();
  myPoints     = curve.getPointList();
  return *this;
}

/*!
  \return title of table
*/
QString Plot2d_Curve::getTableTitle() const
{
  return QString();
}

/*!
  Sets curve's horizontal title
*/
void Plot2d_Curve::setHorTitle( const QString& title )
{
  myHorTitle = title;
}

/*!
  Gets curve's horizontal title
*/
QString Plot2d_Curve::getHorTitle() const
{
  return myHorTitle;
}

/*!
  Sets curve's vertical title
*/
void Plot2d_Curve::setVerTitle( const QString& title )
{
  myVerTitle = title;
}

/*!
  Gets curve's vertical title
*/
QString Plot2d_Curve::getVerTitle() const
{
  return myVerTitle;
}

/*!
  Sets curve's horizontal units
*/
void Plot2d_Curve::setHorUnits( const QString& units )
{
  myHorUnits = units;
}

/*!
  Gets curve's horizontal units
*/
QString Plot2d_Curve::getHorUnits() const
{
  return myHorUnits;
}

/*!
  Sets curve's vertical units
*/
void Plot2d_Curve::setVerUnits( const QString& units )
{
  myVerUnits = units;
}

/*!
  Gets curve's vertical units
*/
QString Plot2d_Curve::getVerUnits() const
{
  return myVerUnits;
}

/*!
  Adds one point for curve.
*/
void Plot2d_Curve::addPoint(double theX, double theY, const QString& txt )
{
  Plot2d_Point aPoint;
  aPoint.x = theX;
  aPoint.y = theY;
  aPoint.text = txt;
  myPoints.append(aPoint);
}

/*!
  Insert one point for curve on some position.
*/
void Plot2d_Curve::insertPoint(int thePos, double theX, double theY, const QString& txt)
{
  Plot2d_Point aPoint;
  aPoint.x = theX;
  aPoint.y = theY;
  aPoint.text = txt;

  QValueList<Plot2d_Point>::iterator aIt;
  int aCurrent = 0;
  for(aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    if (thePos == aCurrent) {
      myPoints.insert(aIt, aPoint);
      return;
    }
    aCurrent++;  
  }
  myPoints.append(aPoint);
}

/*!
  Delete one point for curve on some position.
*/
void Plot2d_Curve::deletePoint(int thePos)
{
  QValueList<Plot2d_Point>::iterator aIt;
  int aCurrent = 0;
  for(aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    if (thePos == aCurrent) {
      myPoints.remove(aIt);
      return;
    }
    aCurrent++;  
  }
}

/*!
  Remove all points for curve.
*/
void Plot2d_Curve::clearAllPoints()
{
  myPoints.clear();
}

/*!
  Gets curve's data : abscissas of points
*/
pointList Plot2d_Curve::getPointList() const
{
  return myPoints;
}

/*!
  Sets curve's data. 
*/
void Plot2d_Curve::setData( const double* hData, const double* vData, long size, const QStringList& lst )
{
  clearAllPoints();
  QStringList::const_iterator anIt = lst.begin(), aLast = lst.end(); 
  for( long i = 0; i < size; i++, anIt++ )
    addPoint( hData[i], vData[i], anIt==aLast ? QString::null : *anIt );
}

/*!
  Gets curve's data : abscissas of points
*/
double* Plot2d_Curve::horData() const
{
  int aNPoints = nbPoints();
  double* aX = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    aX[i] = myPoints[i].x;
  }
  return aX;
}

/*!
  Gets curve's data : ordinates of points
*/
double* Plot2d_Curve::verData() const
{
  int aNPoints = nbPoints();
  double* aY = new double[aNPoints];
  for (int i = 0; i < aNPoints; i++) {
    aY[i] = myPoints[i].y;
  }
  return aY;
}

/*!
  Gets curve's data : number of points
*/
int Plot2d_Curve::nbPoints() const
{
  return myPoints.count();
}

/*!
  Returns true if curve has no data
*/
bool Plot2d_Curve::isEmpty() const
{
  return myPoints.isEmpty();
}

/*!
  Sets curve's AutoAssign flag - in this case attributes will be set automatically
*/
void Plot2d_Curve::setAutoAssign( bool on )
{
  myAutoAssign = on;
}

/*!
  Gets curve's AutoAssign flag state
*/
bool Plot2d_Curve::isAutoAssign() const
{
  return myAutoAssign;
}

/*!
  Sets curve's color ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setColor( const QColor& color )
{
  myColor = color;
  myAutoAssign = false;
}

/*!
  Gets curve's color
*/
QColor Plot2d_Curve::getColor() const
{
  return myColor;
}

/*!
  Sets curve's marker ( and resets AutoAssign flag )
*/
void Plot2d_Curve::setMarker( MarkerType marker )
{
  myMarker = marker;
  myAutoAssign = false;
}

/*!
  Gets curve's marker
*/
Plot2d_Curve::MarkerType Plot2d_Curve::getMarker() const
{
  return myMarker;
}

/*!
  Sets curve's line type and width ( and resets AutoAssign flag )
  NOTE : A line width of 0 will produce a 1 pixel wide line using a fast algorithm for diagonals. 
         A line width of 1 will also produce a 1 pixel wide line, but uses a slower more accurate 
         algorithm for diagonals. 
         For horizontal and vertical lines a line width of 0 is the same as a line width of 1.
*/
void Plot2d_Curve::setLine( LineType line, const int lineWidth )
{
  myLine = line;
  myLineWidth = lineWidth;
  if ( myLineWidth < 0 ) myLineWidth = 0;
  myAutoAssign = false;
}

/*!
  Gets curve's line type
*/
Plot2d_Curve::LineType Plot2d_Curve::getLine() const
{
  return myLine;
}

/*!
  Gets curve's line width
*/
int Plot2d_Curve::getLineWidth() const
{
  return myLineWidth;
}

/*!
  Sets curve's y axis
*/
void Plot2d_Curve::setYAxis(QwtPlot::Axis theYAxis)
{
  if(theYAxis == QwtPlot::yLeft || theYAxis == QwtPlot::yRight)
    myYAxis = theYAxis;
}

/*!
  Gets curve's y axis
*/
QwtPlot::Axis Plot2d_Curve::getYAxis() const
{
  return myYAxis;
}

/*!
  Gets curve's minimal abscissa
*/
double Plot2d_Curve::getMinX() const
{
  QValueList<Plot2d_Point>::const_iterator aIt;
  double aMinX = 1e150;
  //int aCurrent = 0;
  for(aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    if ( (*aIt).x < aMinX )
      aMinX = (*aIt).x;
  }
  return aMinX;
}

/*!
  Gets curve's minimal ordinate
*/
double Plot2d_Curve::getMinY() const
{
  QValueList<Plot2d_Point>::const_iterator aIt;
  double aMinY = 1e150;
  //int aCurrent = 0;
  for(aIt = myPoints.begin(); aIt != myPoints.end(); ++aIt) {
    if ( (*aIt).y < aMinY )
      aMinY = (*aIt).y;
  }
  return aMinY;
}

/*!
  Changes text assigned to point of curve
  \param ind -- index of point
  \param txt -- new text
*/
void Plot2d_Curve::setText( const int ind, const QString& txt )
{
  if( ind<0 || ind>=myPoints.count() )
    return;

  myPoints[ind].text = txt;
}

/*!
  \return text assigned to point
  \param ind -- index of point
*/
QString Plot2d_Curve::text( const int ind ) const
{
  if( ind<0 || ind>=myPoints.count() )
    return QString::null;
  else
    return myPoints[ind].text;
}
