#include "SUIT_Operation.h"

#include "SUIT_Study.h"
#include "SUIT_Application.h"

SUIT_Operation::SUIT_Operation( SUIT_Application* app )
: QObject(),
myApp( app ),
myStudy( 0 ),
myState( Waiting )
{
}

SUIT_Operation::~SUIT_Operation()
{
}

SUIT_Study* SUIT_Operation::study() const
{
  return myStudy;
}

SUIT_Application* SUIT_Operation::application() const
{
  return myApp;
}

SUIT_Operation::OperationState SUIT_Operation::state() const
{
  return myState;
}

void SUIT_Operation::start()
{
	myStudy = myApp->activeStudy();
	if ( myStudy )
  {
		if ( myStudy->canStartOperation( this ) )
    {
      if ( !isReadyToStart() )
      {
        myStudy->stopOperation();
        return;
      }
			startOperation();
			myState = Running;

			emit started( this );
		}
	}
}

void SUIT_Operation::abort()
{
	abortOperation();
	myStudy->stopOperation();
	myState = Waiting;

  emit aborted( this );
}

void SUIT_Operation::commit()
{
	commitOperation();
	myStudy->stopOperation();
	myState = Waiting;

	emit commited( this );

  myStudy->sendChangesNotification();
}

void SUIT_Operation::resume()
{
	resumeOperation();
	myState = Running;

	emit resumed( this );
}

void SUIT_Operation::suspend()
{
	suspendOperation();
	myState = Suspended;

	emit suspended( this );
}

bool SUIT_Operation::isReadyToStart()
{
	return true;
}

void SUIT_Operation::startOperation()
{
	emit callSlot();

	commit();
}

void SUIT_Operation::abortOperation()
{
}

void SUIT_Operation::resumeOperation()
{
}

void SUIT_Operation::suspendOperation()
{
}

void SUIT_Operation::commitOperation()
{
}

bool SUIT_Operation::setSlot( const QObject* theReceiver, const char* theSlot )
{
	return connect( this, SIGNAL( callSlot() ), theReceiver, theSlot );
}

bool SUIT_Operation::isValid( SUIT_Operation* ) const
{
  return false;
}

bool SUIT_Operation::isGranted() const
{
  return false;
}

void SUIT_Operation::setStudy( SUIT_Study* s )
{
  myStudy = s;
}

void SUIT_Operation::setApplication( SUIT_Application* app )
{
  myApp = app;
}
