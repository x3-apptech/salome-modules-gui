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

#include "SALOME_EventFilter.h"
#include "SALOME_Event.h"

#include <QApplication>

SALOME_EventFilter* SALOME_EventFilter::myFilter = NULL;

/*!Constructor.*/
SALOME_EventFilter::SALOME_EventFilter()
: QObject()
{
  /* VSR 13/01/03 : installing global event filter for the application */
  qApp->installEventFilter( this );
}

/*!Destructor.*/
SALOME_EventFilter::~SALOME_EventFilter()
{
  qApp->removeEventFilter( this );
}

/*!
  Custom event filter
*/
bool SALOME_EventFilter::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == SALOME_EVENT )
  { 
    SALOME_Event* aSE = (SALOME_Event*)((SALOME_CustomEvent*)e)->data();
    processEvent(aSE);
    ((SALOME_CustomEvent*)e)->setData( 0 );
    return true;
  }
  return QObject::eventFilter( o, e );
}

/*!Process event.*/
void SALOME_EventFilter::processEvent( SALOME_Event* theEvent )
{
  if(theEvent)
    theEvent->ExecutePostedEvent();
}

/*!Create new instance of SALOME_EventFilter*/
void SALOME_EventFilter::Init()
{
  if( myFilter==NULL )
    myFilter = new SALOME_EventFilter();
}

/*!Destroy filter.*/
void SALOME_EventFilter::Destroy()
{
  if( myFilter )
  {
    delete myFilter;
    myFilter = NULL;
  }
}
