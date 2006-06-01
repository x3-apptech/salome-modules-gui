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

#include "SalomeApp_EventFilter.h"
#include <SALOME_Event.hxx>

#include <qapplication.h>

SalomeApp_EventFilter* SalomeApp_EventFilter::myFilter = NULL;

/*!Constructor.*/
SalomeApp_EventFilter::SalomeApp_EventFilter()
: QObject()
{
  /* VSR 13/01/03 : installing global event filter for the application */
  qApp->installEventFilter( this );
}

/*!Destructor.*/
SalomeApp_EventFilter::~SalomeApp_EventFilter()
{
  qApp->removeEventFilter( this );
}

/*!
  Custom event filter
*/
bool SalomeApp_EventFilter::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == SALOME_EVENT )
  { 
    SALOME_Event* aSE = (SALOME_Event*)((QCustomEvent*)e)->data();
    processEvent(aSE);
    ((QCustomEvent*)e)->setData( 0 );
    return true;
  }
  return QObject::eventFilter( o, e );
}

/*!Process event.*/
void SalomeApp_EventFilter::processEvent( SALOME_Event* theEvent )
{
  if(theEvent){
    theEvent->Execute();
    // Signal the calling thread that the event has been processed
    theEvent->processed();
  }
}

/*!Create new instance of SalomeApp_EventFilter*/
void SalomeApp_EventFilter::Init()
{
  if( myFilter==NULL )
    myFilter = new SalomeApp_EventFilter();
}

/*!Destroy filter.*/
void SalomeApp_EventFilter::Destroy()
{
  if( myFilter )
  {
    delete myFilter;
    myFilter = NULL;
  }
}
