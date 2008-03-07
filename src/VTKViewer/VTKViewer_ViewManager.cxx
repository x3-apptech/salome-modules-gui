// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "VTKViewer_ViewManager.h"
#include "VTKViewer_ViewModel.h"

/*!Constructor.Initialize SIUT_ViewManager by \a study and \a theDesktop.
 * Create new instance of VTKViewer_Viewer and set view model by it.
 */
VTKViewer_ViewManager::VTKViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop ) 
: SUIT_ViewManager( study, theDesktop, new VTKViewer_Viewer() )
{
  setTitle( tr( "VTK_VIEW_TITLE" ) );
}

/*!Destructor.*/
VTKViewer_ViewManager::~VTKViewer_ViewManager()
{
  /*!Do nothing.*/
}
