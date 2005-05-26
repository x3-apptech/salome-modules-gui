//  File   : SPlot2d_CurveContainer.cxx
//  Author : Sergey RUIN
//  Module : SUIT

#include "SPlot2d_CurveContainer.h"
using namespace std;

/*!
  Constructor
*/
SPlot2d_CurveContainer::SPlot2d_CurveContainer()
{
  myCurves.setAutoDelete( false );
}
/*!
  Destructor
*/
SPlot2d_CurveContainer::~SPlot2d_CurveContainer()
{
  
}
/*!
  Adds curve if not exist yet in the container
*/
void SPlot2d_CurveContainer::addCurve( Plot2d_Curve* curve )
{
  if ( myCurves.find( curve ) < 0 )
    myCurves.append( curve );
}
/*!
  Removes curve form the container ( and deletes it if <alsoDelete> is true )
*/
void SPlot2d_CurveContainer::removeCurve( const int index, bool alsoDelete )
{
  Plot2d_Curve* curve = myCurves.take( index );
  if ( curve && alsoDelete )
    delete curve;
}
/*!
  Clears container contents ( removes and deletes all curves )
*/
void SPlot2d_CurveContainer::clear( bool alsoDelete )
{
  while( myCurves.count() > 0 ) {
    Plot2d_Curve* curve = myCurves.take( 0 );
    if ( curve && alsoDelete )
      delete curve;
  }
}
/*!
  Gets nb of curves in container
*/
int SPlot2d_CurveContainer::count()
{
  return myCurves.count();
}
/*!
  Returns true if contains no curves
*/
bool SPlot2d_CurveContainer::isEmpty() const
{
  return myCurves.isEmpty();
}
/*!
  Gets curve by index
*/
Plot2d_Curve* SPlot2d_CurveContainer::curve( const int index )
{
  return myCurves.at( index );
}


