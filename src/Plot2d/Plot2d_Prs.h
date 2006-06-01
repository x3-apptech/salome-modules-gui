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
#ifndef PLOT2D_PRS_H
#define PLOT2D_PRS_H

#include "Plot2d.h"
#include "Plot2d_Curve.h"

class PLOT2D_EXPORT Plot2d_Prs
{
public:
  Plot2d_Prs( bool theDelete = false );
  Plot2d_Prs( const Plot2d_Curve* obj, bool theDelete = false );
  ~Plot2d_Prs();

  curveList getCurves() const;
  void AddObject( const Plot2d_Curve* obj );

  bool IsNull() const;

  bool isSecondY() const;

  void setAutoDel(bool theDel);

protected:
  curveList myCurves;
  bool      mySecondY;
};

#endif
