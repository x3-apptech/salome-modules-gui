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
#include "SUPERVGraph_ViewManager.h"

int SUPERVGraph_ViewManager::myMaxId = 0;

/*!
  Constructor
*/
SUPERVGraph_ViewManager::SUPERVGraph_ViewManager( SUIT_Study* theStudy, 
						  SUIT_Desktop* theDesktop, 
						  SUIT_ViewModel* theViewModel )
  : SUIT_ViewManager( theStudy, theDesktop, theViewModel )
{
  myId = ++myMaxId;
}

/*!
  Destructor
*/
SUPERVGraph_ViewManager::~SUPERVGraph_ViewManager()
{
}

/*!
  Sets view name
  \param theView - view to assign name
*/
void SUPERVGraph_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption( QString( "SUPERVISION  scene:%1 - viewer:%2" ).arg( myId ).arg(aPos+1));
}

/*!
  Fills popup menu with custom actions
 \param popup - popup menu to be filled with
*/
void SUPERVGraph_ViewManager::contextMenuPopup( QPopupMenu* thePopup)
{
  SUIT_ViewManager::contextMenuPopup( thePopup );
  // to be implemented
}
