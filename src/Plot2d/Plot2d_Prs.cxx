//  SALOME OCCViewer : build OCC Viewer into Salome desktop
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : Plot2d_Prs.cxx
//  Author : Sergey ANIKIN
//  Module : SALOME
//  $Header$

#include <qwt_plot.h>
#include "Plot2d_Prs.h"

//==========================================================
/*!
 *  Plot2d_Prs::Plot2d_Prs
 *  Default constructor
 */
//==========================================================
Plot2d_Prs::Plot2d_Prs( bool theDelete )
: mySecondY( false)
{
  setAutoDel(theDelete);
}

//==========================================================
/*!
 *  Plot2d_Prs::Plot2d_Prs
 *  Standard constructor
 */
//==========================================================
Plot2d_Prs::Plot2d_Prs( const Plot2d_Curve* obj, bool theDelete )
: mySecondY( false)
{
  setAutoDel(theDelete);
  AddObject( obj ); 
}

//==========================================================
/*!
 *  Plot2d_Prs::~Plot2d_Prs
 *  Destructor
 */
//==========================================================
Plot2d_Prs::~Plot2d_Prs()
{ 
}

//==========================================================
/*!
 *  Plot2d_Prs::GetObjects
 *  Get curves list
 */
//==========================================================
curveList Plot2d_Prs::getCurves() const
{
  return myCurves;
}

//==========================================================
/*!
 *  Plot2d_Prs::AddObject
 *  Add curbe
 */
//==========================================================
void Plot2d_Prs::AddObject( const Plot2d_Curve* obj )
{
  myCurves.append((Plot2d_Curve*)obj);

  if (obj->getYAxis() == QwtPlot::yRight)
    mySecondY = true;
}

//==========================================================
/*!
 *  Plot2d_Prs::IsNull
 *  Return 0 if list of the curves is empty
 *  [ Reimplemented from SALOME_Prs ]
 */
//==========================================================
bool Plot2d_Prs::IsNull() const 
{ 
  return myCurves.isEmpty();
}

//==========================================================
/*!
 *  Plot2d_Prs::IsSecondY
 *  Return 1 if in the list of curves exist at least one curve
 *  with axis Qwt::yRight
 *  [ Reimplemented from SALOME_Prs ]
 */
//==========================================================
bool Plot2d_Prs::isSecondY() const
{
  return mySecondY;
}

//==========================================================
/*!
 *  Plot2d_Prs::IsNull
 *  Return 0 if list of the curves is empty
 *  [ Reimplemented from SALOME_Prs ]
 */
//==========================================================
void Plot2d_Prs::setAutoDel(bool theDel)
{
  myCurves.setAutoDelete(theDel);
}
