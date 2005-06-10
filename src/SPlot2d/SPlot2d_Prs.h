//  File   : SPlot2d_Prs.h
//  Author : Sergey RUIN
//  Module : SUIT

#ifndef SPLOT2D_PRS_H
#define SPLOT2D_PRS_H

#include "SPlot2d.h"
#include "SALOME_Prs.h"
#include "Plot2d_Prs.h"

#include <qptrlist.h>

class Plot2d_Curve;

class SPLOT2D_EXPORT SPlot2d_Prs : public SALOME_Prs2d, public Plot2d_Prs
{
public:

  // Default constructor 
  SPlot2d_Prs();

  // Standard constructor    
  SPlot2d_Prs( const Plot2d_Curve* obj );

  SPlot2d_Prs( const Plot2d_Prs* prs );

  // Destructor
  ~SPlot2d_Prs();

  virtual bool IsNull() const { return Plot2d_Prs::IsNull(); }

  // Get curves list
  // Note: Depricated method, use method getCurves     
  curveList GetObjects() const { return getCurves(); }

};

#endif
