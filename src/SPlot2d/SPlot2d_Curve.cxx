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
//  File   : SPlot2d_Curve.cxx
//  Author : Sergey RUIN, Open CASCADE S.A.S. (sergey.ruin@opencascade.com)

#include "SPlot2d_Curve.h"

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
: Plot2d_Curve( curve )
{
  myIO      = curve.getIO();
  myTableIO = curve.getTableIO();
}

/*!
  operator=. Makes deep copy of data.
*/
SPlot2d_Curve& SPlot2d_Curve::operator=( const SPlot2d_Curve& curve )
{
  Plot2d_Curve::operator=(curve);
  myIO         = curve.getIO();
  myTableIO    = curve.getTableIO();
  return *this;
}

/*!
  \return corresponding SALOME_InteractiveObject
*/
Handle(SALOME_InteractiveObject) SPlot2d_Curve::getIO() const
{
  return myIO;
}

/*!
  Sets corresponding SALOME_InteractiveObject
  \param io - SALOME_InteractiveObject
*/
void SPlot2d_Curve::setIO( const Handle(SALOME_InteractiveObject)& io )
{
  myIO = io;
}

/*!
  \return true if curve has table interactive object
*/
bool SPlot2d_Curve::hasTableIO() const
{
  return !myTableIO.IsNull();
}

/*!
  \return table interactive object of curve
*/
Handle(SALOME_InteractiveObject) SPlot2d_Curve::getTableIO() const
{
  return myTableIO;
}

/*!
  Sets table interactive object of curve
  \param io - SALOME_InteractiveObject
*/
void SPlot2d_Curve::setTableIO( const Handle(SALOME_InteractiveObject)& io )
{
  myTableIO = io;
}

/*!
  \return SALOME_InteractiveObject
*/
bool SPlot2d_Curve::hasIO() const
{
  return !myIO.IsNull();
}

/*!
  \return table title
*/
QString SPlot2d_Curve::getTableTitle() const
{
  QString title;
  if( hasTableIO() )
    title = getTableIO()->getName();
  return title;
}

/*!
  Add owner of the curve.
  \param owner  - owner of the curve
*/
void SPlot2d_Curve::addOwner(const QString& owner) {
  myOwners.insert(owner);
}

/*!
  Remove owner of the curve.
  \param owner  - owner of the curve
*/
void SPlot2d_Curve::removeOwner(const QString& owner) {
  myOwners.insert(owner);
}

/*!
  Get all owners of the curve.
  \return owners of the curve.
*/
OwnerSet SPlot2d_Curve::getOwners() const {
  return myOwners;
}


/*!
  Add owners of the curve.
  \param owners  - owners of the curve
*/
void SPlot2d_Curve::addOwners(OwnerSet& owners) {
  myOwners = myOwners|=owners;
}

