
#include "SalomeApp_EventFilter.h"
#include <SALOME_Event.hxx>

#include <qapplication.h>

SalomeApp_EventFilter* SalomeApp_EventFilter::myFilter = NULL;

SalomeApp_EventFilter::SalomeApp_EventFilter()
: QObject()
{
  /* VSR 13/01/03 : installing global event filter for the application */
  qApp->installEventFilter( this );
}

SalomeApp_EventFilter::~SalomeApp_EventFilter()
{
  qApp->removeEventFilter( this );
}

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

void SalomeApp_EventFilter::processEvent( SALOME_Event* theEvent )
{
  if(theEvent){
    theEvent->Execute();
    // Signal the calling thread that the event has been processed
    theEvent->processed();
  }
}

void SalomeApp_EventFilter::Init()
{
  if( myFilter==NULL )
    myFilter = new SalomeApp_EventFilter();
}

void SalomeApp_EventFilter::Destroy()
{
  if( myFilter )
  {
    delete myFilter;
    myFilter = NULL;
  }
}
