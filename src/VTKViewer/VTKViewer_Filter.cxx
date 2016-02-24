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

//  VTKViewer_Filter : Filter for VTK viewer
//  File   : VTKViewer_Filter.cxx
//  Author : Sergey LITONIN

#include "VTKViewer_Filter.h"

OCCT_IMPLEMENT_STANDARD_RTTIEXT(VTKViewer_Filter, MMgt_TShared)

/*!
 * \class       VTKViewer_Filter
 * Description : Base class of filters of for <a href="http://www.vtk.org/">VTK</a> viewer. Method IsValid \n
 *               should be redefined in derived classes
 */

/*!Constructor.*/
VTKViewer_Filter::VTKViewer_Filter()
{
  myActor = 0;
}

/*!Virtual Destructor.*/
VTKViewer_Filter::~VTKViewer_Filter()
{
}

/*!Check correctness of \a theCellId for actor \a theActor by
 * call virtual method IsValid( const int theId ).
 * \param theActor - actor
 * \param theCellId - cell id.
 * \retval \c true - if cell id is valid, else \c false.
 */
bool VTKViewer_Filter::IsValid( VTKViewer_Actor* theActor, const int theCellId )
{
  SetActor( theActor );
  return IsValid( theCellId );
}

/*!Virtual method.Set actor to \a theActor.
 * \param theActor - actor.
 */
void VTKViewer_Filter::SetActor( VTKViewer_Actor* theActor )
{
  myActor = theActor;
}
