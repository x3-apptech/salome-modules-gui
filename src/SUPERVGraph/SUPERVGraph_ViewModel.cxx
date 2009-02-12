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
#include "SUPERVGraph_ViewModel.h"
#include "SUPERVGraph_ViewFrame.h"
#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"

/*!
  Constructor
*/
SUPERVGraph_Viewer::SUPERVGraph_Viewer()
:SUIT_ViewModel() 
{
}

/*!
  Destructor
*/
SUPERVGraph_Viewer::~SUPERVGraph_Viewer() 
{
}

/*!
  Creates new view window
  \param theDesktop - main window of application
*/
SUIT_ViewWindow* SUPERVGraph_Viewer::createView(SUIT_Desktop* theDesktop)
{
  SUPERVGraph_ViewFrame* aRes = new SUPERVGraph_ViewFrame( theDesktop );
  return aRes;
}
