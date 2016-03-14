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
// File   : PyViewer_ViewModel.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyViewer_ViewModel.h"

#include "PyViewer_ViewWindow.h"

/*!
  \class PyViewer_Viewer
  \brief Python view model.
*/

/*!
  \brief Constructor.
*/
PyViewer_Viewer::PyViewer_Viewer() : SUIT_ViewModel()
{
}

/*!
  \brief Destructor.
*/
PyViewer_Viewer::~PyViewer_Viewer()
{
}

/*!
  Create new instance of view window on desktop \a theDesktop.
  \retval SUIT_ViewWindow* - created view window pointer.
*/
SUIT_ViewWindow* PyViewer_Viewer::createView( SUIT_Desktop* theDesktop )
{
  PyViewer_ViewWindow* aPyViewer = new PyViewer_ViewWindow( theDesktop, this );
  initView( aPyViewer );
  return aPyViewer;
}

/*!
  Start initialization of view window
  \param view - view window to be initialized
*/
void PyViewer_Viewer::initView( PyViewer_ViewWindow* theViewModel )
{
  if ( theViewModel )
    theViewModel->initLayout();
}
