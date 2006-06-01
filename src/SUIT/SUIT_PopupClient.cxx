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
#include "SUIT_PopupClient.h"

#include <qevent.h>

/*! constructor. initialize mySignal.*/
SUIT_PopupClient::SUIT_PopupClient()
: mySignal( 0 )
{
}

/*! destructor. delete mySignal*/
SUIT_PopupClient::~SUIT_PopupClient()
{
  delete mySignal;
}

/*!
  Connect popup request.
*/
bool SUIT_PopupClient::connectPopupRequest( QObject* reciever, const char* slot )
{
  if ( !reciever || !slot )
    return false;
  if ( !mySignal )
    mySignal = new Signal();
  return QObject::connect( mySignal, SIGNAL( contextMenuRequest( SUIT_PopupClient*, QContextMenuEvent* ) ),
                           reciever, slot );
}

/*!
  Disconnect popup request.
*/
bool SUIT_PopupClient::disconnectPopupRequest( QObject* reciever, const char* slot )
{
  if ( !reciever || !slot || !mySignal )
    return false;
  return QObject::disconnect( mySignal, SIGNAL( contextMenuRequest( SUIT_PopupClient*, QContextMenuEvent* ) ),
                              reciever, slot );
}

/*!
  Send signal on context menu request.
*/
void SUIT_PopupClient::contextMenuRequest( QContextMenuEvent* e )
{
  if ( mySignal )
    mySignal->sendSignal( this, e );
}

/*!
 *  \class SUIT_PopupClient::Signal
 * Descr: invoke signal which is connected to reciever in SUIT_PopupClient
 */

/*! constructor*/
SUIT_PopupClient::Signal::Signal()
: QObject( 0 )
{
}

/*! destructor. do nothing*/
SUIT_PopupClient::Signal::~Signal()
{}

/*! Send signal to \a client on context menu request \a e.
 */
void SUIT_PopupClient::Signal::sendSignal( SUIT_PopupClient* client, QContextMenuEvent* e )
{
  emit contextMenuRequest( client, e );
}
