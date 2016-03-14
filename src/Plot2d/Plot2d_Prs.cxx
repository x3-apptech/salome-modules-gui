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

#include "Plot2d_Prs.h"

/*!
  Default constructor
*/
Plot2d_Prs::Plot2d_Prs( bool theDelete )
: mySecondY( false), myIsAutoDel( theDelete )
{
}

/*!
  Standard constructor
*/
Plot2d_Prs::Plot2d_Prs( Plot2d_Object* obj, bool theDelete )
: mySecondY( false), myIsAutoDel( theDelete )
{
  AddObject( obj ); 
}

/*!
  Destructor
*/
Plot2d_Prs::~Plot2d_Prs()
{ 
  if ( myIsAutoDel )
    qDeleteAll( myObjects );
}

/*!
  Get objects list
*/
objectList Plot2d_Prs::getObjects() const
{
  return myObjects;
}

/*!
  Add curve
*/
void Plot2d_Prs::AddObject( Plot2d_Object* obj )
{
  myObjects.append(obj);

  if (obj->getYAxis() == QwtPlot::yRight)
    mySecondY = true;
}

/*!
  \return 0 if list of the curves is empty [ Reimplemented from SALOME_Prs ]
*/
bool Plot2d_Prs::IsNull() const 
{ 
  return myObjects.isEmpty();
}

/*!
  \return 1 if in the list of curves exist at least one curve with axis Qwt::yRight [ Reimplemented from SALOME_Prs ]
*/
bool Plot2d_Prs::isSecondY() const
{
  return mySecondY;
}

/*!
  \return 0 if list of the curves is empty [ Reimplemented from SALOME_Prs ]
*/
void Plot2d_Prs::setAutoDel(bool theDel)
{
  myIsAutoDel = theDel;
}
