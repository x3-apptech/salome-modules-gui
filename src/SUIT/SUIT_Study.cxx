#include "SUIT_Study.h"

#include "SUIT_Desktop.h"
#include "SUIT_Operation.h"
#include "SUIT_DataObject.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Application.h"

/*!\class SUIT_Study
 * Support study management. Object management. Operation management.
 */

/*!Constructor.*/
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

/*!Destructor.*/
SUIT_Study::~SUIT_Study()
{
  delete myRoot;
  myRoot = 0;
}

/*!
 *\retval study id.
 */
int SUIT_Study::id() const
{
  return myId;
}

/*!
 *\retval root data object.
 */
SUIT_DataObject* SUIT_Study::root() const
{
  return myRoot;
}

/*!
 *\retval Application.
 */
SUIT_Application* SUIT_Study::application() const
{
  return myApp;
}

/*!
 *\retval study name
 */
QString SUIT_Study::studyName() const
{
  return myName;
}

/*!
 *\retval active operation.
 */
SUIT_Operation* SUIT_Study::activeOperation() const
{
  return myOperations.current();
}

/*!
 *\retval TRUE - if study saved, else FALSE.
 */
bool SUIT_Study::isSaved() const
{
  return myIsSaved;
}

/*!
 *\retval TRUE - if study modified, else FALSE.
 */
bool SUIT_Study::isModified() const
{
  return myIsModified;
}

/*!
 *Close document. NOT IMPLEMENTED.
 */
void SUIT_Study::closeDocument(bool permanently)
{
}

void SUIT_Study::createDocument()
{
  /*! Custom document initialization to be performed \n
   *  within onNewDoc() handler can be put here
   */
}

/*!
 * Open document. Sets file name. return true.
 */
bool SUIT_Study::openDocument( const QString& fileName )
{
  myName = fileName;
  myIsSaved = true;
  myIsModified = false;

  return true;
}

/*!
 * Save document as \a fileName. Set file name.
 */
bool SUIT_Study::saveDocumentAs( const QString& fileName )
{
  myName = fileName;
  myIsSaved = true;
  myIsModified = false;

  return true;
}

/*!
 *\retval TRUE - if document saved successful, else FALSE.
 */
bool SUIT_Study::saveDocument()
{
  return saveDocumentAs( myName );
}

/*!
 *Abort all operations.
 */
void SUIT_Study::abortAllOperations()
{
  SUIT_Operation* aOperation = 0;
  while ( aOperation = myOperations.current() )
  {
    aOperation->abort();
    myOperations.pop();
  }
}

/*!
  Update study. NOT IMPLEMENTED HERE.
 */
void SUIT_Study::update()
{
}

/*!
  Emit study modified.
 */
void SUIT_Study::sendChangesNotification()
{
  emit studyModified( this );
}

/*!
  Set study saved to \a on.
 */
void SUIT_Study::setIsSaved( const bool on )
{
  myIsSaved = on;
}

/*!
  Set study modified to \a on.
 */
void SUIT_Study::setIsModified( const bool on )
{
  myIsModified = on;
}

/*!
  Set root object.
 */
void SUIT_Study::setRoot( SUIT_DataObject* obj )
{
  if ( myRoot == obj )
    return;

  delete myRoot;
  myRoot = obj;
}

/*!
  Set study name.
 */
void SUIT_Study::setStudyName( const QString& name )
{
  myName = name;
}

/*!
  Stop operation.
 */
void SUIT_Study::stopOperation()
{
  myOperations.pop();
  if ( myOperations.current() )
    myOperations.current()->resume();
  myIsModified = true;
}

/*!
 * Set can start operation \a theOperation.
 *\retval FALSE - if can't start.
 */
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
