//  File   : SPlot2d_CurveContainer.h
//  Author : Sergey RUIN
//  Module : SUIT

#ifndef SPlot2d_CurveContainer_h
#define SPlot2d_CurveContainer_h

#include "SPlot2d.h"  

#include "Plot2d_Curve.h"
#include <qlist.h>

class SPLOT2D_EXPORT SPlot2d_CurveContainer 
{
public:
  SPlot2d_CurveContainer();
  ~SPlot2d_CurveContainer();

  void                addCurve( Plot2d_Curve* curve );
  void                removeCurve( const int index, bool alsoDelete = false ) ;
  void                clear( bool alsoDelete = false );
  int                 count();
  bool                isEmpty() const;
  Plot2d_Curve*       curve( const int index );

private:
  QList<Plot2d_Curve>  myCurves;
};

#endif // SPlot2d_CurveContainer_h


