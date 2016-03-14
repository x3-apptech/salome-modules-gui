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
//  File   : SPlot2d_Histogram.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "SPlot2d_Histogram.h"

/*!
  Constructor
*/
SPlot2d_Histogram::SPlot2d_Histogram()
:Plot2d_Histogram() 
{
}

/*!
  Destructor
*/
SPlot2d_Histogram::~SPlot2d_Histogram()
{
}

/*!
  Copy constructor. Makes deep copy of data.
*/
SPlot2d_Histogram::SPlot2d_Histogram( const SPlot2d_Histogram& hist )
: Plot2d_Histogram( hist )
{
  myIO      = hist.getIO();
}

/*!
  operator=. Makes deep copy of data.
*/
SPlot2d_Histogram& SPlot2d_Histogram::operator=( const SPlot2d_Histogram& hist )
{
  Plot2d_Histogram::operator=(hist);
  myIO         = hist.getIO();
  return *this;
}

/*!
  \return corresponding SALOME_InteractiveObject
*/
Handle(SALOME_InteractiveObject) SPlot2d_Histogram::getIO() const
{
  return myIO;
}

/*!
  Sets corresponding SALOME_InteractiveObject
  \param io - SALOME_InteractiveObject
*/
void SPlot2d_Histogram::setIO( const Handle(SALOME_InteractiveObject)& io )
{
  myIO = io;
}

/*!
  \return SALOME_InteractiveObject
*/
bool SPlot2d_Histogram::hasIO() const
{
  return !myIO.IsNull();
}
