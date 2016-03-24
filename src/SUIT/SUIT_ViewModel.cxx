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

// SUIT_ViewModel.cxx: implementation of the SUIT_ViewModel class.
//
#include "SUIT_ViewModel.h"
#include "SUIT_ViewWindow.h"

SUIT_ViewModel::InteractionStyle2StatesMap SUIT_ViewModel::myStateMap;
SUIT_ViewModel::InteractionStyle2ButtonsMap SUIT_ViewModel::myButtonMap;

static bool isInitialized = false;

/*!Constructor.*/
SUIT_ViewModel::SUIT_ViewModel()
{
  if ( !isInitialized )
  {
    isInitialized = true;

    // standard interaction style
    SUIT_ViewModel::myStateMap[STANDARD][ZOOM]  = Qt::ControlModifier;
    SUIT_ViewModel::myButtonMap[STANDARD][ZOOM] = Qt::LeftButton;

    SUIT_ViewModel::myStateMap[STANDARD][PAN]   = Qt::ControlModifier;
    SUIT_ViewModel::myButtonMap[STANDARD][PAN]  = Qt::MidButton;

    SUIT_ViewModel::myStateMap[STANDARD][ROTATE]  = Qt::ControlModifier;
    SUIT_ViewModel::myButtonMap[STANDARD][ROTATE] = Qt::RightButton;

    SUIT_ViewModel::myStateMap[STANDARD][FIT_AREA]  = Qt::ControlModifier;
    SUIT_ViewModel::myButtonMap[STANDARD][FIT_AREA] = Qt::RightButton;

    // "key free" interaction style
    SUIT_ViewModel::myStateMap[KEY_FREE][ZOOM]  = Qt::NoModifier;
    SUIT_ViewModel::myButtonMap[KEY_FREE][ZOOM] = Qt::RightButton;

    SUIT_ViewModel::myStateMap[KEY_FREE][PAN]   = Qt::NoModifier;
    SUIT_ViewModel::myButtonMap[KEY_FREE][PAN]  = Qt::MidButton;

    SUIT_ViewModel::myStateMap[KEY_FREE][ROTATE]  = Qt::NoModifier;
    SUIT_ViewModel::myButtonMap[KEY_FREE][ROTATE] = Qt::LeftButton;

    SUIT_ViewModel::myStateMap[KEY_FREE][FIT_AREA]  = Qt::NoModifier; // unused
    SUIT_ViewModel::myButtonMap[KEY_FREE][FIT_AREA] = Qt::NoButton;   // unused
  }
  myViewManager = 0;
}

/*!Destructor..*/
SUIT_ViewModel::~SUIT_ViewModel()
{
}

/*!Create new instance of view window on desktop \a theDesktop.
 *\retval SUIT_ViewWindow* - created view window pointer.
 */
SUIT_ViewWindow* SUIT_ViewModel::createView(SUIT_Desktop* theDesktop)
{
  return new SUIT_ViewWindow(theDesktop);
}

/*!Set view manager.
  \param theViewManager view manager
 */
void SUIT_ViewModel::setViewManager(SUIT_ViewManager* theViewManager)
{
  myViewManager = theViewManager;
}

/*!Get view manager.
  \return view manager
 */
SUIT_ViewManager* SUIT_ViewModel::getViewManager() const
{
  return myViewManager;
}

/*! Sets hot button
 *\param theOper - hot operation
 *\param theState - adding state to state map operations.
 *\param theButton - adding state to button map operations.
 */
void SUIT_ViewModel::setHotButton( InteractionStyle theInteractionStyle, HotOperation theOper,
                                   Qt::KeyboardModifiers theState, Qt::MouseButtons theButton )
{
  myStateMap[theInteractionStyle][theOper]  = theState;
  myButtonMap[theInteractionStyle][theOper] = theButton;
}

/*! Gets hot button for operation \a theOper.
 *\param theOper - input hot operation
 *\param theState - output state from state map operations.
 *\param theButton - output state from button map operations.
*/
void SUIT_ViewModel::getHotButton( InteractionStyle theInteractionStyle, HotOperation theOper,
                                   Qt::KeyboardModifiers& theState, Qt::MouseButtons& theButton )
{
  theState  = myStateMap[theInteractionStyle][theOper];
  theButton = myButtonMap[theInteractionStyle][theOper];
}
