//  File   : SPlot2d_Curve.cxx
//  Author : Sergey RUIN
//  Module : SUIT

#include "SPlot2d_Curve.h"
using namespace std;

/*!
  Constructor
*/
SPlot2d_Curve::SPlot2d_Curve()
:Plot2d_Curve() 
{
}

/*!
  Destructor
*/
SPlot2d_Curve::~SPlot2d_Curve()
{
}

/*!
  Copy constructor. Makes deep copy of data.
*/
SPlot2d_Curve::SPlot2d_Curve( const SPlot2d_Curve& curve )
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
  myIO         = curve.getIO();
  myTableIO    = curve.getTableIO();
}

/*!
  operator=. Makes deep copy of data.
*/
SPlot2d_Curve& SPlot2d_Curve::operator=( const SPlot2d_Curve& curve )
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
  myIO         = curve.getIO();
  myTableIO    = curve.getTableIO();
  return *this;
}

/*!
  Sets curve's data. 
*/
void SPlot2d_Curve::setData( const double* hData, const double* vData, long size )
{
  clearAllPoints();
  for(long i = 0; i < size; i++) addPoint(hData[i], vData[i]);
}
