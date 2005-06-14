#include "SUIT_Study.h"

#include "SUIT_Desktop.h"
#include "SUIT_Operation.h"
#include "SUIT_DataObject.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Application.h"

SUIT_Study::SUIT_Study( SUIT_Application* app )
: QObject(),
myApp( app ),
myIsSaved( false ),
myIsModified( false ),
myName( "" )
{
  static int _id = 0;

  myId = ++_id;

  myRoot = new SUIT_DataObject();
  myOperations.setAutoDelete( false );
}

SUIT_Study::~SUIT_Study()
{
  delete myRoot;
  myRoot = 0;
}

int SUIT_Study::id() const
{
  return myId;
}

SUIT_DataObject* SUIT_Study::root() const
{
  return myRoot;
}

SUIT_Application* SUIT_Study::application() const
{
  return myApp;
}

QString SUIT_Study::studyName() const
{
  return myName;
}

SUIT_Operation* SUIT_Study::activeOperation() const
{
  return myOperations.current();
}

bool SUIT_Study::isSaved() const
{
  return myIsSaved;
}

bool SUIT_Study::isModified() const
{
  return myIsModified;
}

void SUIT_Study::closeDocument()
{
}

void SUIT_Study::createDocument()
{
  // Custom document initialization to be performed
  // within onNewDoc() handler can be put here
}

bool SUIT_Study::openDocument( const QString& fileName )
{
  myName = fileName;
  myIsSaved = true;
  myIsModified = false;

  return true;
}

bool SUIT_Study::saveDocumentAs( const QString& fileName )
{
  myName = fileName;
  myIsSaved = true;
  myIsModified = false;

  return true;
}

bool SUIT_Study::saveDocument()
{
  return saveDocumentAs( myName );
}

void SUIT_Study::abortAllOperations()
{
  SUIT_Operation* aOperation = 0;
  while ( aOperation = myOperations.current() )
  {
    aOperation->abort();
    myOperations.pop();
  }
}

void SUIT_Study::update()
{
}

void SUIT_Study::sendChangesNotification()
{
  emit studyModified( this );
}

void SUIT_Study::setIsSaved( const bool on )
{
  myIsSaved = on;
}

void SUIT_Study::setIsModified( const bool on )
{
  myIsModified = on;
}

void SUIT_Study::setRoot( SUIT_DataObject* obj )
{
  if ( myRoot == obj )
    return;

  delete myRoot;
  myRoot = obj;
}

void SUIT_Study::setStudyName( const QString& name )
{
  myName = name;
}

void SUIT_Study::stopOperation()
{
  myOperations.pop();
  if ( myOperations.current() )
    myOperations.current()->resume();
  myIsModified = true;
}

bool SUIT_Study::canStartOperation( SUIT_Operation* theOperation )
{
  SUIT_Operation* anActiveOperation = (SUIT_Operation*)activeOperation();
  if ( anActiveOperation )
  {
    if ( !theOperation->isGranted() )
    {
      if ( !anActiveOperation->isValid( theOperation ) )
      {
        // Ask user about existing operation
        int anAnsw = SUIT_MessageBox::warn2( application()->desktop(), tr( "Operation launch" ), 
                                             tr( "Previous operation is not finished and will be aborted." ),
                                             tr( "Continue" ), tr( "Cancel" ), 0, 1, 1 );
        if ( anAnsw == 1 )
          return false;

        anActiveOperation->abort();
        myOperations.pop();
        myOperations.push( theOperation );
        return true;
      }
    }
    anActiveOperation->suspend();
  }
  myOperations.push( theOperation );

  return true;
}
