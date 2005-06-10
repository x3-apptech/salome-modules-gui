//  File   : SPlot2d_Prs.cxx
//  Author : Sergey RUIN
//  Module : SUIT

#include "SPlot2d_Prs.h"
using namespace std;

//==========================================================
/*!
 *  SPlot2d_Prs::SPlot2d_Prs
 *  Default constructor
 */
//==========================================================
SPlot2d_Prs::SPlot2d_Prs()
:Plot2d_Prs()  
{
}

//==========================================================
/*!
 *  SPlot2d_Prs::SPlot2d_Prs
 *  Standard constructor
 */
//==========================================================
SPlot2d_Prs::SPlot2d_Prs( const Plot2d_Curve* obj )
:Plot2d_Prs(obj)
{ 
}

SPlot2d_Prs::SPlot2d_Prs( const Plot2d_Prs* prs )
{
  mySecondY = prs->isSecondY();
  myCurves = prs->getCurves();
}

//==========================================================
/*!
 *  SPlot2d_Prs::~SPlot2d_Prs
 *  Destructor
 */
//==========================================================
SPlot2d_Prs::~SPlot2d_Prs()
{ 
}


