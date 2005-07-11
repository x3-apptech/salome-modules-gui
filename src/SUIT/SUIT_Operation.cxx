#include "SUIT_Operation.h"

#include "SUIT_Study.h"
#include "SUIT_Application.h"

/*! Constructor. Initialize myApp, myStudy, myState.*/
SUIT_Operation::SUIT_Operation( SUIT_Application* app )
: QObject(),
myApp( app ),
myStudy( 0 ),
myState( Waiting )
{
}

/*! Destructor*/
SUIT_Operation::~SUIT_Operation()
{
}

/*! \retval Return myStudy.*/
SUIT_Study* SUIT_Operation::study() const
{
  return myStudy;
}

/*! \retval Return myApp*/
SUIT_Application* SUIT_Operation::application() const
{
  return myApp;
}

/*! \retval Return myState*/
SUIT_Operation::OperationState SUIT_Operation::state() const
{
  return myState;
}

/*! Set started operation status*/
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

/*! Set aborted operation status*/
void SUIT_Operation::abort()
{
	abortOperation();
	myStudy->stopOperation();
	myState = Waiting;

  emit aborted( this );
}

/*! Set commited operation status*/
void SUIT_Operation::commit()
{
	commitOperation();
	myStudy->stopOperation();
	myState = Waiting;

	emit commited( this );

  myStudy->sendChangesNotification();
}

/*! Set resumed operation status*/
void SUIT_Operation::resume()
{
	resumeOperation();
	myState = Running;

	emit resumed( this );
}

/*! Set suspended operation status*/
void SUIT_Operation::suspend()
{
	suspendOperation();
	myState = Suspended;

	emit suspended( this );
}

/*! \retval Return true*/
bool SUIT_Operation::isReadyToStart()
{
	return true;
}

/*! start operation.\n
 *  emitting callSlot() signal \n
 *  calling commit() function.
 */
void SUIT_Operation::startOperation()
{
	emit callSlot();

	commit();
}

/*! Do nothing*/
void SUIT_Operation::abortOperation()
{
}

/*! Do nothing*/
void SUIT_Operation::resumeOperation()
{
}

/*! Do nothing*/
void SUIT_Operation::suspendOperation()
{
}

/*! Do nothing*/
void SUIT_Operation::commitOperation()
{
}

/*! Setting slot.*/
bool SUIT_Operation::setSlot( const QObject* theReceiver, const char* theSlot )
{
	return connect( this, SIGNAL( callSlot() ), theReceiver, theSlot );
}

/*! \retval Return false*/
bool SUIT_Operation::isValid( SUIT_Operation* ) const
{
  return false;
}

/*! \retval Return false*/
bool SUIT_Operation::isGranted() const
{
  return false;
}

/*! Setting study.*/
void SUIT_Operation::setStudy( SUIT_Study* s )
{
  myStudy = s;
}

/*! Setting application.*/
void SUIT_Operation::setApplication( SUIT_Application* app )
{
  myApp = app;
}
