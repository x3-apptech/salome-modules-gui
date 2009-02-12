//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME OCCViewer : build OCC Viewer into Salome desktop
//  File   : Plot2d_Prs.cxx
//  Author : Sergey ANIKIN
//  Module : SALOME
//  $Header$
//
#include <qwt_plot.h>
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
Plot2d_Prs::Plot2d_Prs( const Plot2d_Curve* obj, bool theDelete )
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
    qDeleteAll( myCurves );
}

/*!
  Get curves list
*/
curveList Plot2d_Prs::getCurves() const
{
  return myCurves;
}

/*!
  Add curve
*/
void Plot2d_Prs::AddObject( const Plot2d_Curve* obj )
{
  myCurves.append((Plot2d_Curve*)obj);

  if (obj->getYAxis() == QwtPlot::yRight)
    mySecondY = true;
}

/*!
  \return 0 if list of the curves is empty [ Reimplemented from SALOME_Prs ]
*/
bool Plot2d_Prs::IsNull() const 
{ 
  return myCurves.isEmpty();
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
