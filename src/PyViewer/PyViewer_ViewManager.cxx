// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyViewer_ViewManager.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyViewer_ViewManager.h"

#include "PyViewer_ViewModel.h"

/*!
  \class PyViewer_ViewManager
  \brief Python viewer view manager.
*/

/*!
  \brief Constructor.
  \param theStudy study
  \param theDesktop parent desktop window
*/
PyViewer_ViewManager::PyViewer_ViewManager( SUIT_Study* theStudy,
                                            SUIT_Desktop* theDesktop )
: SUIT_ViewManager( theStudy, theDesktop, new PyViewer_Viewer() )
{
  setTitle( tr( "PYVIEWER_VIEW_TITLE" ) );
}

/*!
  \brief Destructor.
*/
PyViewer_ViewManager::~PyViewer_ViewManager()
{
}
