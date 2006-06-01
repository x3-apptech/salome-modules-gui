// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

/*!
  Constructor
*/
CAF_Study::CAF_Study(SUIT_Application* theApp)
: SUIT_Study( theApp ),
myModifiedCnt( 0 )
{
}

/*!
  Constructor
*/
CAF_Study::CAF_Study(SUIT_Application* theApp, Handle (TDocStd_Document)& aStdDoc)
: SUIT_Study( theApp ),
myStdDoc( aStdDoc ),
myModifiedCnt( 0 )
{
}

/*!
  Destructor
*/
CAF_Study::~CAF_Study()
{
}

/*!
  \return OCAF document
*/
Handle(TDocStd_Document) CAF_Study::stdDoc() const
{
  return myStdDoc;
}

/*!
  Sets new OCAF document
  \param aStdDoc - new OCAF document
*/
void CAF_Study::setStdDoc( Handle(TDocStd_Document)& aStdDoc )
{
  myStdDoc = aStdDoc;
}

/*!
  Custom document initialization
*/
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

/*!
  Close document
*/
void CAF_Study::closeDocument( bool permanent )
{
  Handle(TDocStd_Application) app = stdApp();
  if ( !app.IsNull() && !stdDoc().IsNull() )
    app->Close( stdDoc() );

  SUIT_Study::closeDocument( permanent );
}

/*!
  Open document
  \param fname - name of file
*/
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

/*!
  Save document with other name
  \param fname - name of file
*/
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

  if ( status )
    status = SUIT_Study::saveDocumentAs( fname );

  if ( status )
    myModifiedCnt = 0;

  return status;
}

/*!
  Open OCAF transaction
*/
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

/*!
  Abort OCAF transaction
*/
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

/*!
  Commit OCAF transaction
*/
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

/*!
  \return true, if there is opened OCAF transaction
*/
bool CAF_Study::hasTransaction() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->HasOpenCommand();
}

/*!
  \return whether the document was saved in file. [ public ]
*/
bool CAF_Study::isSaved() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->IsSaved();
}

/*!
  \return whether the document is modified. [ public ]
*/
bool CAF_Study::isModified() const
{
  if ( myStdDoc.IsNull() )
    return false;

//  return myStdDoc->IsModified();
  return myModifiedCnt;
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
  \return true if possible to perform 'undo' command. [ public ]
*/
bool CAF_Study::canUndo() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->GetAvailableUndos() > 0;
}

/*!
  \return true if possible to perform 'redo' command. [ public ]
*/
bool CAF_Study::canRedo() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->GetAvailableRedos() > 0;
}

/*!
  \return the list of names of 'undo' actions available. [ public ]
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
  \return the list of names of 'redo' actions available. [ public ]
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
  \return the standard OCAF application from owner application. [ protected ]
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
  \return the application casted to type CAF_Application. [ protected ]
*/
CAF_Application* CAF_Study::cafApplication() const
{
  return ::qt_cast<CAF_Application*>( application() );
}
