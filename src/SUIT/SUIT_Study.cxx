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
myName( "" ),
myIsSaved( false ),
myIsModified( false ),
myBlockChangeState( false )
{
  static int _id = 0;

  myId = ++_id;

  myRoot = new SUIT_DataObject();
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
  return myOperations.count() > 0 ? myOperations.last() : 0;
}

/*!
 *\retval \c true - if study saved, else \c false.
 */
bool SUIT_Study::isSaved() const
{
  return myIsSaved;
}

/*!
 *\retval \c true - if study modified, else \c false.
 */
bool SUIT_Study::isModified() const
{
  return myIsModified;
}

/*!
 *Close document. NOT IMPLEMENTED.
 */
void SUIT_Study::closeDocument(bool /*permanently*/)
{
}

/*!
  Custom document initialization to be performed \n
   within onNewDoc() handler can be put here
*/
bool SUIT_Study::createDocument( const QString& )
{
  return true;
}

/*!
 * Open document. Sets file name. return \c true.
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
 *\retval \c true - if document saved successful, else \c false.
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
  myBlockChangeState = true;
  while ( !myOperations.isEmpty() ) {
    Operations::iterator it = myOperations.begin();
    (*it)->abort();
  }
  myBlockChangeState = false;
  myOperations.clear();
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
  Set study modified to \a on.
 */
void SUIT_Study::Modified()
{
  setIsModified( true );
  sendChangesNotification();
}

/*!
  Set root object.
 */
void SUIT_Study::setRoot( SUIT_DataObject* obj )
{
  if ( myRoot == obj )
    return;

  // This is necessary in order not to destroy the complete tree of objects
  if ( obj )
    obj->reparentChildren( myRoot );

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
 * \brief Verifies whether operation can be activated above already started ones
  * \param theOp - operation to be checked
  * \return NULL if operation can be activated, pointer to operation which denies
  * starting tested operation
*
* Verifies whether operation can be activated above already started ones. This method
* is called from SUIT_Study::start() and SUIT_Study::resume() methods.
*/
SUIT_Operation* SUIT_Study::blockingOperation( SUIT_Operation* theOp ) const
{
  if( theOp->isGranted() )
    return 0;

  Operations tmpOps( myOperations );
  for ( Operations::const_iterator it = tmpOps.end(); it != tmpOps.begin(); )
  {
    it--;
    SUIT_Operation* anOp = *it;
    if ( anOp != 0 && anOp!= theOp && !anOp->isValid( theOp ) )
      return anOp;
  }

  return 0;
}

/*!
 * \brief Starts operation
  * \param theOp - operation to be started
  * \param toCheck - if parameters is equal \c true then checking performed whether
  * all already started operations allow to start this operation above them (default
  * value is \c true
  * \return \c true if operation is started, \c false otherwise
*
* Verifies whether theOp operation can be started above already started ones (if toCheck
* parameter is equal \c true) and starts it
*/
bool SUIT_Study::start( SUIT_Operation* theOp, const bool toCheck )
{
  if ( !theOp || myOperations.contains( theOp ) )
    return false;

  theOp->setExecStatus( SUIT_Operation::Rejected );
  theOp->setStudy( this );

  if ( !theOp->isReadyToStart() )
    return false;

  if ( toCheck )
  {
    while ( SUIT_Operation* anOp = blockingOperation( theOp ) )
    {
      int anAnsw = SUIT_MessageBox::question( application()->desktop(),
                                              tr( "OPERATION_LAUNCH" ), tr( "PREVIOUS_NOT_FINISHED" ),
                                              tr( "CONTINUE" ), tr( "CANCEL" ) );

      if ( anAnsw == 1 )
        return false;
      else
        anOp->abort();
    }
  }

  SUIT_Operation* anOp = activeOperation();
  if ( anOp )
  {
    activeOperation()->suspendOperation();
    anOp->setState( SUIT_Operation::Suspended );
  }

  theOp->setState( SUIT_Operation::Running );
  myOperations.append( theOp );
 
  emit theOp->started( theOp );
  operationStarted( theOp );
  theOp->startOperation();

  return true;
}

/*!
 * \brief Aborts operation
  * \param theOp - operation to be aborted
  * \return \c true if operation is aborted successfully
*
* Verifies whether operation already started and aborts it in this case (sets execution
* status to Rejected and stops operation)
*/
bool SUIT_Study::abort( SUIT_Operation* theOp )
{
  if ( !theOp || !myOperations.contains( theOp ) )
    return false;

  theOp->setExecStatus( SUIT_Operation::Rejected );

  theOp->abortOperation();
  operationAborted( theOp );
  emit theOp->aborted( theOp );

  stop( theOp );

  return true;
}

/*!
 * \brief Commits operation
  * \param theOp - operation to be committed
  * \return \c true if operation is committed successfully
*
* Verifies whether operation already started and commits it in this case (sets execution
* status to Accepted and stops operation)
*/
bool SUIT_Study::commit( SUIT_Operation* theOp )
{
  if ( !theOp || !myOperations.contains( theOp ) )
    return false;

  theOp->setExecStatus( SUIT_Operation::Accepted );

  theOp->commitOperation();
  operationCommited( theOp );
  emit theOp->committed( theOp );

  stop( theOp );

  emit studyModified( this );

  return true;
}

/*!
 * \brief Commits operation
  * \param theOp - operation to be committed
  * \return \c true if operation is suspended successfully
*
* Verifies whether operation already started and suspends it in this case. Operations
* ususlly are suspended to start other one above them.
*/
bool SUIT_Study::suspend( SUIT_Operation* theOp )
{
  if ( !theOp || !myOperations.contains( theOp ) || theOp->state() == SUIT_Operation::Suspended )
    return false;

  theOp->setState( SUIT_Operation::Suspended );
  theOp->suspendOperation();
  emit theOp->suspended( theOp );
  return true;
}


/*!
 * \brief Resumes operation
  * \param theOp - operation to be resumed
  * \return \c true if operation is aborted successfully
*
* Verifies whether operation already started but suspended and resumesit in this case.
*/
bool SUIT_Study::resume( SUIT_Operation* theOp )
{
  if ( !theOp || !myOperations.contains( theOp ) ||
       theOp->state() == SUIT_Operation::Running ||
       blockingOperation( theOp ) != 0 )
    return false;

  if ( myOperations.count() > 0 )
    suspend( myOperations.last() );

  theOp->setState( SUIT_Operation::Running );
  theOp->resumeOperation();

  // Move operation at the end of list in order to sort it in the order of activation.
  // As result active operation is a last operation of list, operation which was active
  // before currently active operation is located before it and so on
  myOperations.removeAll( theOp );
  myOperations.append( theOp );

  emit theOp->resumed( theOp );
  return true;
}

/*!
 * \brief Stops operation
  * \param theOp - operation to be stopped
*
* Stops operation. This private method is called from abort() and commit() ones to perform
* common actions when operation is stopped
*/
void SUIT_Study::stop( SUIT_Operation* theOp )
{
  theOp->setState( SUIT_Operation::Waiting );
  myOperations.removeAll( theOp );

  // get last operation which can be resumed
  SUIT_Operation* aResultOp = 0;

  QListIterator<SUIT_Operation*> it (myOperations);
  it.toBack();
  while( it.hasPrevious() )
  {
    SUIT_Operation* anOp = it.previous();
    if ( anOp && anOp != theOp && blockingOperation( anOp ) == 0 )
    {
      aResultOp = anOp;
      break;
    }
  }

  theOp->stopOperation();
  operationStopped( theOp );
  emit theOp->stopped( theOp );

  if ( aResultOp )
    resume( aResultOp );
}

/*!
 * \brief Get all started operations
  * \return List of all started operations
*/
const QList<SUIT_Operation*>& SUIT_Study::operations() const
{
  return myOperations;
}

/*!
 * \brief Perform some actions when operation starting
*/
void SUIT_Study::operationStarted( SUIT_Operation* op )
{
  if ( !op )
    return;

  if ( op->testFlags( SUIT_Operation::Transaction ) )
    op->openTransaction();
}

/*!
 * \brief Perform some actions when operation aborted
*/
void SUIT_Study::operationAborted( SUIT_Operation* op )
{
  if ( op->testFlags( SUIT_Operation::Transaction ) )
    op->abortTransaction();
}

/*!
 * \brief Perform some actions when operation commited
*/
void SUIT_Study::operationCommited( SUIT_Operation* op )
{
  if ( op->testFlags( SUIT_Operation::Transaction ) )
    op->commitTransaction( op->operationName() );
}

/*!
 * \brief Perform some actions when operation stopped
*/
void SUIT_Study::operationStopped( SUIT_Operation* )
{
}

/*!
 * \brief Opens transaction for data modifications.
*/
bool SUIT_Study::openTransaction()
{
  return true;
}

/*!
 * \brief Aborts transaction and all performed data modifications.
*/
bool SUIT_Study::abortTransaction()
{
  return true;
}

/*!
 * \brief Commits transaction and all performed data modifications.
*/
bool SUIT_Study::commitTransaction( const QString& )
{
  return true;
}

/*!
 * \brief Returns \c true if transaction is opened.
*/
bool SUIT_Study::hasTransaction() const
{
  return false;
}

/*!
 * \brief Restores the study state.
 */
void SUIT_Study::restoreState(int /*savePoint*/)
{
}
