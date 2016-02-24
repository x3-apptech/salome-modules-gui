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

#include "CAF_Study.h"

#include "CAF_Tools.h"
#include "CAF_Application.h"

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Application.h>

#include <QDir>

#include <TDF_Delta.hxx>
#include <TDF_ListIteratorOfDeltaList.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

/*!
  \class CAF_Study
  \brief Represents study for using in CAF module.

  A study contains reference to OCAF std document and allows using OCAF services.
  Provides necessary functionality for OCC transactions management.
*/

/*!
  \brief Constructor.
  \param theApp application
*/
CAF_Study::CAF_Study(SUIT_Application* theApp)
: SUIT_Study( theApp ),
  myModifiedCnt( 0 )
{
}

/*!
  \brief Constructor.
  \param theApp application
  \param aStdDoc OCAF document
*/
CAF_Study::CAF_Study(SUIT_Application* theApp, Handle (TDocStd_Document)& aStdDoc)
: SUIT_Study( theApp ),
  myStdDoc( aStdDoc ),
  myModifiedCnt( 0 )
{
}

/*!
  \brief Destructor.
*/
CAF_Study::~CAF_Study()
{
}

/*!
  \brief Get OCAF document.
  \return handle to the OCAF document object
*/
Handle(TDocStd_Document) CAF_Study::stdDoc() const
{
  return myStdDoc;
}

/*!
  \brief Set OCAF document.
  \param aStdDoc new OCAF document
*/
void CAF_Study::setStdDoc( Handle(TDocStd_Document)& aStdDoc )
{
  myStdDoc = aStdDoc;
}

/*!
  \brief Customize document initialization.
  \param doc study name
  \return \c true on success and \c false on error
*/
bool CAF_Study::createDocument( const QString& doc )
{
  bool res = SUIT_Study::createDocument( doc );

  CAF_Application* app = cafApplication();
  if ( res && app && !app->stdApp().IsNull() )
  {
    try {
      OCC_CATCH_SIGNALS;
      TColStd_SequenceOfExtendedString formats;
      app->stdApp()->Formats( formats );
      if ( !formats.IsEmpty() )
        app->stdApp()->NewDocument( formats.First(), myStdDoc );
    }
    catch ( Standard_Failure ) {
      res = false;
    }
  }
  return res;
}

/*!
  \brief Close document.
  \param permanently if \c true, a document is closed permanently
*/
void CAF_Study::closeDocument( bool permanently )
{
  Handle(TDocStd_Application) app = stdApp();
  if ( !app.IsNull() && !stdDoc().IsNull() )
    app->Close( stdDoc() );

  SUIT_Study::closeDocument( permanently );
}

/*!
  \brief Open document.
  \param fname study file name
  \return \c true on success and \c false if document cannot be opened
*/
bool CAF_Study::openDocument( const QString& fname )
{
  Handle(TDocStd_Application) app = stdApp();
  if ( app.IsNull() )
    return false;

  bool status = false;
  try {
    OCC_CATCH_SIGNALS;
    status = app->Open( CAF_Tools::toExtString( fname ), myStdDoc ) == PCDM_RS_OK;
  }
  catch ( Standard_Failure ) {
    status = false;
  }

  return status && SUIT_Study::openDocument( fname );
}

/*!
  \brief Save document with other name.
  \param fname study file name
*/
bool CAF_Study::saveDocumentAs( const QString& fname )
{
  Handle(TDocStd_Application) app = stdApp();
  if ( app.IsNull() )
    return false;

  bool save = false;
  if ( !stdDoc().IsNull() && stdDoc()->IsSaved() )
  {
    QString path = QDir::toNativeSeparators( CAF_Tools::toQString( stdDoc()->GetPath() ) );
    save = path == QDir::toNativeSeparators( fname );
  }

  bool status = false;
  try {
    OCC_CATCH_SIGNALS;
    if ( save )
      status = app->Save( stdDoc() ) == PCDM_SS_OK;
    else
    {
      TCollection_ExtendedString format, path( CAF_Tools::toExtString( fname ) );
      app->Format( path, format );

      if ( format.Length() )
        stdDoc()->ChangeStorageFormat( format );

      status = app->SaveAs( stdDoc(), path ) == PCDM_SS_OK;
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
  \brief Open OCAF transaction.
  \return \c true if transaction is opened successfully
*/
bool CAF_Study::openTransaction()
{
  if ( myStdDoc.IsNull() )
    return false;

  bool res = true;
  try {
    OCC_CATCH_SIGNALS;
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
  \brief Abort OCAF transaction.
  \return \c true if transaction is aborted successfully
*/
bool CAF_Study::abortTransaction()
{
  if ( myStdDoc.IsNull() )
    return false;

  bool res = true;
  try {
    OCC_CATCH_SIGNALS;
    myStdDoc->AbortCommand();
    update();
  }
  catch ( Standard_Failure ) {
    res = false;
  }
  return res;
}

/*!
  \brief Commit OCAF transaction
  \return \c true if transaction is committed successfully
*/
bool CAF_Study::commitTransaction( const QString& name )
{
  if ( myStdDoc.IsNull() )
    return false;

  bool res = true;
  try {
    OCC_CATCH_SIGNALS;
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
  \brief Check if there is any transaction opened.
  \return \c true if there is opened OCAF transaction
*/
bool CAF_Study::hasTransaction() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->HasOpenCommand();
}

/*!
  \brief Check if the study is saved.
  \return \c true if the document has been saved to file
*/
bool CAF_Study::isSaved() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->IsSaved();
}

/*!
  \brief Check if the study is modified.
  \return \c true if the document has been modified
*/
bool CAF_Study::isModified() const
{
  if ( myStdDoc.IsNull() )
    return false;

//  return myStdDoc->IsModified();
  return myModifiedCnt;
}

/*!
  \brief Increment modifications count.

  If \a undoable is \c true, this modification can be rolled back by
  undoModified(), otherwise the document will be marked as \c modified
  until it is saved.

  \param undoable if \c true the operation is undoable
  \sa undoModified(), clearModified()
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
  \brief Decrement modifications count.
  \sa doModified(), clearModified()
*/
void CAF_Study::undoModified()
{
  myModifiedCnt--;
}

/*!
  \brief Clear modifications count.
  \sa doModified(), undoModified()
*/
void CAF_Study::clearModified()
{
  myModifiedCnt = 0;
}

/*!
  \brief Undo the last command.
  \return \c true on success and \c false on error
*/
bool CAF_Study::undo()
{
  if ( myStdDoc.IsNull() )
    return false;

  try {
    OCC_CATCH_SIGNALS;
    myStdDoc->Undo();
    undoModified();     /* decrement modification counter */
  }
  catch ( Standard_Failure ) {
    SUIT_MessageBox::critical(application()->desktop(), tr( "ERR_ERROR" ),
                              tr( "ERR_DOC_UNDO" ));
    return false;
  }
  return true;
}

/*!
  \brief Redo the last undo.
  \return \c true on success and \c false on error
*/
bool CAF_Study::redo()
{
  if ( myStdDoc.IsNull() )
    return false;

  try {
    OCC_CATCH_SIGNALS;
    myStdDoc->Redo();
    doModified();      /* increment modification counter */
  }
  catch ( Standard_Failure ) {
    SUIT_MessageBox::critical( application()->desktop(), tr( "ERR_ERROR" ),
                               tr( "ERR_DOC_REDO" ) );
    return false;
  }
  return true;
}

/*!
  \brief Check if it is possible to undo last command.
  \return \c true if undo is avaiable
*/
bool CAF_Study::canUndo() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->GetAvailableUndos() > 0;
}

/*!
  \brief Check if it is possible to redo last undo.
  \return \c true if redo is avaiable
*/
bool CAF_Study::canRedo() const
{
  if ( myStdDoc.IsNull() )
    return false;

  return myStdDoc->GetAvailableRedos() > 0;
}

/*!
  \brief Get names of available undo commands.
  \return list of commands names
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
  \brief Get names of available redo commands.
  \return list of commands names
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
  \brief Get OCAF application.
  \return handle to the OCAF application object
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
  \brief Get application.
  \return application object (CAF_Application)
*/
CAF_Application* CAF_Study::cafApplication() const
{
  return qobject_cast<CAF_Application*>( application() );
}
