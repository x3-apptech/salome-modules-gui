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

Handle(SALOME_InteractiveObject) SPlot2d_Curve::getIO() const
{
  return myIO;
}

void SPlot2d_Curve::setIO( const Handle(SALOME_InteractiveObject)& io )
{
  myIO = io;
}

bool SPlot2d_Curve::hasTableIO() const
{
  return !myTableIO.IsNull();
}

Handle(SALOME_InteractiveObject) SPlot2d_Curve::getTableIO() const
{
  return myTableIO;
}

void SPlot2d_Curve::setTableIO( const Handle(SALOME_InteractiveObject)& io )
{
  myTableIO = io;
}

bool SPlot2d_Curve::hasIO() const
{
  return !myIO.IsNull();
}

QString SPlot2d_Curve::getTableTitle() const
{
  QString title;
  if( hasTableIO() )
    title = getTableIO()->getName();
  return title;
}
