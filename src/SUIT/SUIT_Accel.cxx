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
// See http://www.salome-platform.org/
//
// SUIT_Accel.cxx: implementation of the SUIT_Accel class.
//
//////////////////////////////////////////////////////////////////////

#include "SUIT_Accel.h"
#include "SUIT_Desktop.h"
#include "SUIT_ViewManager.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewModel.h"

#include <qaccel.h>

/*!\class SUIT_Accel
 * Class handles keyboard accelerator bindings.
 */

/*! Constructor.*/
SUIT_Accel::SUIT_Accel(SUIT_Desktop* theDesktop)
  : QObject( theDesktop, "SUIT_Accel" ),
    myDesktop( theDesktop )
{
  myAccel = new QAccel( theDesktop, "SUIT_Accel_interal_qaccel" );
  connect( myAccel, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
}

/*! Destructor.*/
SUIT_Accel::~SUIT_Accel()
{
}

/*! setActionKey  assign a ceratain action for a key accelerator */
void SUIT_Accel::setActionKey( const int action, const int key, const QString& type )
{    
  // 1. get or generate interal "id" of action
  int id = myAccel->findKey( key );
  if ( id == -1 )
    id = myAccel->insertItem( key );

  IdActionMap idActionMap;
  if ( myMap.contains( type ) )
    idActionMap = myMap[type];

  idActionMap[id] = action;
  myMap[type] = idActionMap;
}

/*! onActivated  slot called when a registered key accelerator was activated */
void SUIT_Accel::onActivated( int id )
{
  if ( myDesktop ) {
    if ( SUIT_ViewWindow* vw = myDesktop->activeWindow() ) {
      QString type = vw->getViewManager()->getViewModel()->getType();
      if (  myMap.contains( type ) ) {
	IdActionMap idActionMap = myMap[type];
	if ( idActionMap.contains( id ) ) {
	  vw->onAccelAction( idActionMap[id] );
	}
      }
    }
  }
}

