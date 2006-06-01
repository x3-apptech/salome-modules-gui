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
//  File   : SPlot2d_Prs.cxx
//  Author : Sergey RUIN
//  Module : SUIT

#include "SPlot2d_Prs.h"
#ifndef WNT
using namespace std;
#endif

/*!
 Default constructor
*/
SPlot2d_Prs::SPlot2d_Prs()
:Plot2d_Prs()  
{
}

/*!
 Standard constructor
*/
SPlot2d_Prs::SPlot2d_Prs( const Plot2d_Curve* obj )
:Plot2d_Prs(obj)
{ 
}

/*!
 Standard constructor
*/
SPlot2d_Prs::SPlot2d_Prs( const Plot2d_Prs* prs )
{
  mySecondY = prs->isSecondY();
  myCurves = prs->getCurves();
}

/*!
 Destructor
*/
SPlot2d_Prs::~SPlot2d_Prs()
{ 
}


