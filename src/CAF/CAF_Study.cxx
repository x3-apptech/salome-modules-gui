#include "CAF_Study.h"

#include "CAF_Tools.h"
#include "CAF_Operation.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Application.h>

#include <TDF_Delta.hxx>
#include <TDF_ListIteratorOfDeltaList.hxx>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAF_Study::CAF_Study(SUIT_Application* theApp)
: SUIT_Study( theApp ),
myModifiedCnt( 0 )
{
}

CAF_Study::CAF_Study(SUIT_Application* theApp, Handle (TDocStd_Document)& aStdDoc)
: SUIT_Study( theApp ),
myStdDoc( aStdDoc ),
myModifiedCnt( 0 )
{
}

CAF_Study::~CAF_Study()
{
}

bool CAF_Study::startOperation()
{
	if ( myStdDoc.IsNull() )
    return false;

  bool res = true;
  try {
    if ( myStdDoc->HasOpenCommand() )
      myStdDoc->AbortCommand();

    myStdDoc->OpenCommand();
  }
  catch( Standard_Failure ) {
    res = false;
  }

  return res;
}

void CAF_Study::abortOperation()
{
	if ( myStdDoc.IsNull() )
    return;

	try {
    myStdDoc->AbortCommand();
		update();
  }
  catch ( Standard_Failure ) {
  }
}

void CAF_Study::commitOperation()
{
	if ( myStdDoc.IsNull() )
    return;

	try {
    myStdDoc->CommitCommand();

    if ( canUndo() )
    {
      CAF_Operation* cafOp = 0;
      if ( activeOperation() && activeOperation()->inherits( "CAF_Operation" ) )
        cafOp = (CAF_Operation*)activeOperation();

      Handle(TDF_Delta) d = myStdDoc->GetUndos().Last();
			if ( cafOp && !d.IsNull() )
        d->SetName( CAF_Tools::toExtString( cafOp->getName() ) );
    }
  }
  catch ( Standard_Failure ) {
  }
}

/*!
    Returns whether the document was saved in file. [ public ]
*/
bool CAF_Study::isSaved() const
{
	if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->IsSaved();
}

/*!
    Returns whether the document is modified. [ public ]
*/
bool CAF_Study::isModified() const
{
  return ( myModifiedCnt != 0 );
}

/*!
    Increments modification count. If 'undoable' is 'true', this modification
    can be rolled back by 'undoModified' otherwise the document will be marked
    as 'modiifed' until saved. [ protected ]
*/
void CAF_Study::doModified( bool undoable )
{
	if ( myStdDoc.IsNull() )
    return;

	myModifiedCnt++;

    /*  Assumed that number of available undos / redos is NOT changed dynamically */
	if ( !undoable )
    myModifiedCnt += myStdDoc->GetAvailableUndos();
}

/*!
    Decrements modification count. [ protected ]
*/
void CAF_Study::undoModified()
{
  myModifiedCnt--;
}

/*!
    Clears modification count. [ public ]
*/
void CAF_Study::clearModified()
{
  myModifiedCnt = 0;
}

/*!
    Undoes the last command. [ public ]
*/
bool CAF_Study::undo()
{
	if ( myStdDoc.IsNull() )
    return false;

  try {
    myStdDoc->Undo();
    undoModified();     /* decrement modification counter */
  }
  catch ( Standard_Failure ) {
		SUIT_MessageBox::error1( application()->desktop(), tr( "ERR_ERROR" ),
                             tr( "ERR_DOC_UNDO" ), tr ( "BUT_OK" ) );
		return false;
	}
  return true;
}

/*!
    Redoes the last undo. [ public ]
*/
bool CAF_Study::redo()
{
	if ( myStdDoc.IsNull() )
    return false;

  try {
    myStdDoc->Redo();
    doModified();      /* increment modification counter */
  }
  catch ( Standard_Failure ) {
    SUIT_MessageBox::error1( application()->desktop(), tr( "ERR_ERROR" ),
                             tr( "ERR_DOC_REDO" ), tr ( "BUT_OK" ) );
    return false;
  }
  return true;
}

/*!
    Check if possible to perform 'undo' command. [ public ]
*/
bool CAF_Study::canUndo() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->GetAvailableUndos() > 0;
}

/*!
    Check if possible to perform 'redo' command. [ public ]
*/
bool CAF_Study::canRedo() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->GetAvailableRedos() > 0;
}

/*!
    Returns the list of names of 'undo' actions available. [ public ]
*/
QStringList CAF_Study::undoNames() const
{
  QStringList names;
  if ( !myStdDoc.IsNull() )
  {
    for ( TDF_ListIteratorOfDeltaList it( myStdDoc->GetUndos() ); it.More(); it.Next() )
      names.prepend( CAF_Tools::toQString( it.Value()->Name() ) );
  }
  return names;
}

/*!
    Returns the list of names of 'redo' actions available. [ public ]
*/
QStringList CAF_Study::redoNames() const
{
  QStringList names;
  if ( !myStdDoc.IsNull() )
  {
    for ( TDF_ListIteratorOfDeltaList it( myStdDoc->GetRedos() ); it.More(); it.Next() )
      names.append( CAF_Tools::toQString( it.Value()->Name() ) );
  }
  return names;
}
