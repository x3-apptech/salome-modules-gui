// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "QxScene_ViewManager.h"
#include "QxScene_ViewWindow.h"

//#define _DEVDEBUG_
#include "DebTrace.hxx"

/*!
  Constructor
*/
QxScene_ViewManager::QxScene_ViewManager( SUIT_Study* theStudy, 
                                          SUIT_Desktop* theDesktop, 
                                          SUIT_ViewModel* theViewModel )
  : SUIT_ViewManager( theStudy, theDesktop, theViewModel )
{
  DEBTRACE("QxScene_ViewManager::QxScene_ViewManager");
  setTitle( tr( "QXSCENE_VIEW_TITLE" ) );
}

/*!
  Destructor
*/
QxScene_ViewManager::~QxScene_ViewManager()
{
  DEBTRACE("QxScene_ViewManager::~QxScene_ViewManager");
}


/*!
  Adds new view
  \param theView - view to be added
*/
bool QxScene_ViewManager::insertView( SUIT_ViewWindow* theView )
{
  DEBTRACE("QxScene_ViewManager::insertView");
  bool res = SUIT_ViewManager::insertView( theView );
  return res;
}

/*!
  Creates new view
*/
void QxScene_ViewManager::createView()
{
  DEBTRACE("QxScene_ViewManager::createView");
  createViewWindow();
}

/*!
 *  Method redefined here to allow some housekeeping before closing
 *  the view (delete a loded schema and everything related).
 *  If this is not possible, the view is not closed 
 */
void QxScene_ViewManager::closeView(SUIT_ViewWindow* theView)
{
  DEBTRACE("QxScene_ViewManager::closeView");
  QxScene_ViewWindow *qxview = dynamic_cast<QxScene_ViewWindow*>(theView);
  bool OKToClose = true;
  if (!qxview)
    OKToClose = true;
  else
    OKToClose = qxview->closeRequested();
   if (OKToClose)
     {
       DEBTRACE("SUIT View to close");
       SUIT_ViewManager::closeView(theView);
       DEBTRACE("SUIT View closed");
     }
}
