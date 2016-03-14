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

#include "LightApp_EventFilter.h"

#include <SUIT_Desktop.h>

#include <QApplication>

#include <SALOME_Event.h>

LightApp_EventFilter* LightApp_EventFilter::myFilter = NULL;

/*!Constructor.*/
LightApp_EventFilter::LightApp_EventFilter()
: QObject()
{
  qApp->installEventFilter( this );
}

/*!Destructor.*/
LightApp_EventFilter::~LightApp_EventFilter()
{
  qApp->removeEventFilter( this );
}

/*!
  Custom event filter
*/
bool LightApp_EventFilter::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::WindowActivate && o->inherits("QDialog") )
    {
      QWidget* parent = ((QWidget*)o)->parentWidget();
      
      SUIT_Desktop* aDesktop = 0;
      
      while( parent )
        {
          aDesktop = dynamic_cast<SUIT_Desktop*>(parent);
          if ( aDesktop )
            break;
          parent = parent->parentWidget();
        }
      
      if ( aDesktop )
        aDesktop->emitActivated();
    }

  else if(e->type() == SALOME_EVENT)
    {
      SALOME_Event* aSE = (SALOME_Event*)((SALOME_CustomEvent*)e)->data();
      processEvent(aSE);
      ((SALOME_CustomEvent*)e)->setData( 0 );
      return true;
    }
  
  return QObject::eventFilter( o, e );
}

/*!Process event.*/
void LightApp_EventFilter::processEvent( SALOME_Event* theEvent )
{
  if(theEvent)
    theEvent->ExecutePostedEvent();
}


/*!Create new instance of LightApp_EventFilter*/
void LightApp_EventFilter::Init()
{
  if( myFilter==NULL )
    myFilter = new LightApp_EventFilter();
}

/*!Destroy filter.*/
void LightApp_EventFilter::Destroy()
{
  if( myFilter )
  {
    delete myFilter;
    myFilter = NULL;
  }
}
