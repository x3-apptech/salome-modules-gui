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
//  File   : SPlot2d_Prs.h
//  Author : Sergey RUIN, Open CASCADE S.A.S. (sergey.ruin@opencascade.com)

#ifndef SPLOT2D_PRS_H
#define SPLOT2D_PRS_H

#include "SPlot2d.h"
#include "SALOME_Prs.h"
#include "Plot2d_Prs.h"

class Plot2d_Curve;

class SPLOT2D_EXPORT SPlot2d_Prs : public SALOME_Prs2d, public Plot2d_Prs
{
public:
  // Default constructor 
  explicit SPlot2d_Prs( const char* entry );

  // Standard constructor    
  SPlot2d_Prs( const char* entry , const Plot2d_Object* obj );

  // Copy constructor
  SPlot2d_Prs( const Plot2d_Prs* prs );

  // Destructor
  ~SPlot2d_Prs();

  virtual bool IsNull() const;

  // Get curves list
  // Note: Deprecated method, use method getCurves     
  objectList GetObjects() const;
};

#endif
