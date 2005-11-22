#include "CAF_Study.h"

#include "CAF_Tools.h"
#include "CAF_Operation.h"
#include "CAF_Application.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Application.h>

#include <qdir.h>

#include <TDF_Delta.hxx>
#include <TDF_ListIteratorOfDeltaList.hxx>

#include <Standard_ErrorHandler.hxx>

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

Handle(TDocStd_Document) CAF_Study::stdDoc() const
{
  return myStdDoc;
}

void CAF_Study::setStdDoc( Handle(TDocStd_Document)& aStdDoc )
{
  myStdDoc = aStdDoc;
}

void CAF_Study::createDocument()
{
  SUIT_Study::createDocument();

  CAF_Application* app = cafApplication();
  if ( app && !app->stdApp().IsNull() )
  {
    try {
      TColStd_SequenceOfExtendedString formats;
	    app->stdApp()->Formats( formats );
      if ( !formats.IsEmpty() )
        app->stdApp()->NewDocument( formats.First(), myStdDoc );
    }
    catch ( Standard_Failure ) {
    }
  }
}

void CAF_Study::closeDocument( bool permanent )
{
  Handle(TDocStd_Application) app = stdApp();
  if ( !app.IsNull() && !stdDoc().IsNull() )
    app->Close( stdDoc() );

  SUIT_Study::closeDocument( permanent );
}

bool CAF_Study::openDocument( const QString& fname )
{
  Handle(TDocStd_Application) app = stdApp();
  if ( app.IsNull() )
    return false;

  bool status = false;
  try {
    status = app->Open( CAF_Tools::toExtString( fname ), myStdDoc ) == CDF_RS_OK;
  }
  catch ( Standard_Failure ) {
    status = false;
  }

  return status && SUIT_Study::openDocument( fname );
}

bool CAF_Study::saveDocumentAs( const QString& fname )
{
  Handle(TDocStd_Application) app = stdApp();
  if ( app.IsNull() )
    return false;

  bool save = false;
  if ( !stdDoc().IsNull() && stdDoc()->IsSaved() )
  {
    QString path = QDir::convertSeparators( CAF_Tools::toQString( stdDoc()->GetPath() ) );
    save = path == QDir::convertSeparators( fname );
  }

  bool status = false;
  try {
    if ( save )
      status = app->Save( stdDoc() ) == CDF_SS_OK;
    else
    {
      TCollection_ExtendedString format, path( CAF_Tools::toExtString( fname ) );
      app->Format( path, format );

      if ( format.Length() )
        stdDoc()->ChangeStorageFormat( format );

      status = app->SaveAs( stdDoc(), path ) == CDF_SS_OK;
    }
  }
  catch ( Standard_Failure ) {
    status = false;
  }

  return status && SUIT_Study::saveDocumentAs( fname );
}

bool CAF_Study::openTransaction()
{
	if ( myStdDoc.IsNull() )
    return false;

  bool res = true;
  try {
    if ( myStdDoc->HasOpenCommand() )
      myStdDoc->AbortCommand();

    myStdDoc->OpenCommand();
  }
  catch ( Standard_Failure ) {
    res = false;
  }

  return res;
}

bool CAF_Study::abortTransaction()
{
	if ( myStdDoc.IsNull() )
    return false;

  bool res = true;
	try {
    myStdDoc->AbortCommand();
		update();
  }
  catch ( Standard_Failure ) {
    res = false;
  }
  return res;
}

bool CAF_Study::commitTransaction( const QString& name )
{
	if ( myStdDoc.IsNull() )
    return false;

  bool res = true;
	try {
    myStdDoc->CommitCommand();

    if ( canUndo() )
    {
      Handle(TDF_Delta) d = myStdDoc->GetUndos().Last();
			if ( !d.IsNull() )
        d->SetName( CAF_Tools::toExtString( name ) );
    }
  }
  catch ( Standard_Failure ) {
    res = false;
  }
  return res;
}

bool CAF_Study::hasTransaction() const
{
	if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->HasOpenCommand();
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

/*!
    Returns the standard OCAF application from owner application. [ protected ]
*/
Handle(TDocStd_Application) CAF_Study::stdApp() const
{
  Handle(TDocStd_Application) stdApp;
  CAF_Application* app = cafApplication();
  if ( app )
    stdApp = app->stdApp();
  return stdApp;
}

/*!
    Returns the application casted to type CAF_Application. [ protected ]
*/
CAF_Application* CAF_Study::cafApplication() const
{
  return ::qt_cast<CAF_Application*>( application() );
}
