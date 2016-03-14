// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyViewer_ViewWindow.cxx
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#include "PyViewer_ViewWindow.h"

#include "PyEditor_Editor.h"
#include "PyEditor_Settings.h"
#include "PyEditor_SettingsDlg.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QtxAction.h>
#include <QtxActionToolMgr.h>
#include <QtxMultiAction.h>

#include <QLocale>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QProcess>
#include <QStatusBar>
#include <QTextStream>

/*!
  \class PyViewer_ViewWindow
  \brief Python view window.
*/

/*!
  \brief Constructor.
  \param theParent parent widget
*/
PyViewer_ViewWindow::PyViewer_ViewWindow( SUIT_Desktop* theDesktop , PyViewer_Viewer* theModel ) :
  SUIT_ViewWindow(theDesktop),
  myModel(theModel)
{
  my_IsExternal = (theDesktop == NULL);

  if( isExternal() )
    initLayout();
}

void PyViewer_ViewWindow::initLayout()
{
  my_TextEditor = new PyEditor_Editor( my_IsExternal ,SUIT_Session::session()->resourceMgr(), this );
  setCentralWidget( my_TextEditor );

  createActions();
  createToolBar();
  setCurrentFile( QString() );
    
  if ( isExternal() )
    {
      connect( my_TextEditor->document(), SIGNAL( modificationChanged( bool ) ),
	       this, SLOT( setWindowModified( bool ) ) );
      
      statusBar()->showMessage( tr("STS_READY") );
    }  
}

/*!
  \brief Destructor.
 */
PyViewer_ViewWindow::~PyViewer_ViewWindow()
{
  my_CurrentFile.clear();
  delete my_TextEditor;
}

/*!
  \return \c true if the application is external
 */
bool PyViewer_ViewWindow::isExternal()
{
  return my_IsExternal;
}

/*!
  \brief Creates actions of Python view window.
*/
void PyViewer_ViewWindow::createActions()
{
  QtxActionToolMgr* aMgr = toolMgr();
  QtxAction* anAction;

  // 1. File operations
  // 1.1. Create New action
  anAction = new QtxAction( tr( "MNU_PY_NEW" ), QIcon( ":/images/py_new.png" ),
                                 tr( "MNU_PY_NEW" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_NEW" ) );
  anAction->setShortcuts( QKeySequence::New );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onNew() ) );
  aMgr->registerAction( anAction, NewId );

  // 1.2 Create Open action
  anAction = new QtxAction( tr( "MNU_PY_OPEN" ), QIcon( ":/images/py_open.png" ),
                                  tr( "MNU_PY_OPEN" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_OPEN" ) );
  anAction->setShortcuts( QKeySequence::Open );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onOpen() ) );
  aMgr->registerAction( anAction, OpenId );

  // 1.3. Create Save action
  anAction = new QtxAction( tr( "MNU_PY_SAVE" ), QIcon( ":/images/py_save.png" ),
                                  tr( "MNU_PY_SAVE" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_SAVE" ) );
  anAction->setShortcuts( QKeySequence::Save );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onSave() ) );
  aMgr->registerAction( anAction, SaveId );
  // Set default statement for Save action
  anAction->setEnabled( my_TextEditor->document()->isModified() );
  connect( my_TextEditor->document(), SIGNAL( modificationChanged( bool ) ),
    anAction, SLOT( setEnabled( bool ) ) );

  // 1.4. Create SaveAs action
  anAction = new QtxAction( tr( "MNU_PY_SAVEAS" ), QIcon( ":/images/py_save_as.png" ),
                                    tr( "MNU_PY_SAVEAS" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_SAVEAS" ) );
  anAction->setShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_S );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onSaveAs() ) );
  aMgr->registerAction( anAction, SaveAsId );

  // 1.5 Create multi-action for file operations
  /*QtxMultiAction* aFileAction = new QtxMultiAction( this );
  aFileAction->insertAction( aMgr->action( NewId ) );
  aFileAction->insertAction( aMgr->action( OpenId ) );
  aFileAction->insertAction( aMgr->action( SaveId ) );
  aFileAction->insertAction( aMgr->action( SaveAsId ) );
  aMgr->registerAction( aFileAction, FileOpId );*/

  // 1.6. Create Close action
  if (isExternal())
  {
    anAction = new QtxAction( tr( "MNU_PY_CLOSE" ), QIcon( ":/images/py_close.png" ),
                              tr( "MNU_PY_CLOSE" ), 0, this );
    anAction->setStatusTip( tr( "DSC_PY_CLOSE" ) );
    anAction->setShortcut( Qt::CTRL + Qt::Key_Q );
    connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( close() ) );
    aMgr->registerAction( anAction, CloseId );
  }

  // 2. Edit operations
  // 2.1. Create Undo action
  anAction = new QtxAction( tr( "MNU_PY_UNDO" ), QIcon( ":/images/py_undo.png" ),
                            tr( "MNU_PY_UNDO" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_UNDO" ) );
  anAction->setShortcuts( QKeySequence::Undo );
  connect( anAction, SIGNAL( triggered( bool ) ), my_TextEditor, SLOT( undo() ) );
  aMgr->registerAction( anAction, UndoId );
  // Set default statement for Undo action
  anAction->setEnabled( my_TextEditor->document()->isUndoAvailable() );
  connect( my_TextEditor->document(), SIGNAL( undoAvailable( bool ) ),
           anAction, SLOT( setEnabled( bool ) ) );

  // 2.2. Create Redo action
  anAction = new QtxAction( tr( "MNU_PY_REDO" ), QIcon( ":/images/py_redo.png" ),
                            tr( "MNU_PY_REDO" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_REDO" ) );
  anAction->setShortcuts( QKeySequence::Redo );
  connect( anAction, SIGNAL( triggered( bool ) ), my_TextEditor, SLOT( redo() ) );
  aMgr->registerAction( anAction, RedoId );
  // Set default statement for Redo action
  anAction->setEnabled( my_TextEditor->document()->isRedoAvailable() );
  connect( my_TextEditor->document(), SIGNAL( redoAvailable( bool ) ),
           anAction, SLOT( setEnabled( bool ) ) );

  // 2.3. Create Cut action
  anAction = new QtxAction( tr( "MNU_PY_CUT" ), QIcon( ":/images/py_cut.png" ),
                            tr( "MNU_PY_CUT" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_CUT" ) );
  anAction->setShortcuts( QKeySequence::Cut );
  connect( anAction, SIGNAL( triggered( bool ) ), my_TextEditor, SLOT( cut() ) );
  aMgr->registerAction( anAction, CutId );
  // Set default statement for Cut action
  anAction->setEnabled( false );
  connect( my_TextEditor, SIGNAL( copyAvailable( bool ) ),
           anAction, SLOT( setEnabled( bool ) ) );

  // 2.4. Create Copy action
  anAction = new QtxAction( tr( "MNU_PY_COPY" ), QIcon( ":/images/py_copy.png" ),
                            tr( "MNU_PY_COPY" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_COPY" ) );
  anAction->setShortcuts( QKeySequence::Copy );
  connect( anAction, SIGNAL( triggered( bool ) ), my_TextEditor, SLOT( copy() ) );
  aMgr->registerAction( anAction, CopyId );
  // Set default statement for Copy action
  anAction->setEnabled( false );
  connect( my_TextEditor, SIGNAL( copyAvailable( bool ) ),
           anAction, SLOT( setEnabled( bool ) ) );

  // 2.5. Create Paste action
  anAction = new QtxAction( tr( "MNU_PY_PASTE" ), QIcon( ":/images/py_paste.png" ),
                            tr( "MNU_PY_PASTE" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_PASTE" ) );
  anAction->setShortcuts( QKeySequence::Paste );
  connect( anAction, SIGNAL( triggered( bool ) ), my_TextEditor, SLOT( paste() ) );
  aMgr->registerAction( anAction, PasteId );

  // 2.6. Create Delete action
  anAction = new QtxAction( tr( "MNU_PY_DELETE" ), QIcon( ":/images/py_delete.png" ),
                            tr( "MNU_PY_DELETE" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_DELETE" ) );
  anAction->setShortcuts( QKeySequence::Delete );
  connect( anAction, SIGNAL( triggered( bool ) ), my_TextEditor, SLOT( deleteSelected() ) );
  aMgr->registerAction( anAction, DeleteId );
  // Set default statement for Delete action
  anAction->setEnabled( false );
  connect( my_TextEditor, SIGNAL( copyAvailable( bool ) ),
           anAction, SLOT( setEnabled( bool ) ) );

  // 2.7. Create SelectAll action
  anAction = new QtxAction( tr( "MNU_PY_SELECTALL" ), QIcon( ":/images/py_select_all.png" ),
                            tr( "MNU_PY_SELECTALL" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_SELECT_ALL" ) );
  anAction->setShortcuts( QKeySequence::SelectAll );
  connect( anAction, SIGNAL( triggered( bool ) ), my_TextEditor, SLOT( selectAll() ) );
  aMgr->registerAction( anAction, SelectAllId );

  // 2.8. Create multi-action for edit operations
  /*QtxMultiAction* anEditAction = new QtxMultiAction( this );
  anEditAction->insertAction( aMgr->action( UndoId ) );
  anEditAction->insertAction( aMgr->action( RedoId ) );
  anEditAction->insertAction( aMgr->action( CutId ) );
  anEditAction->insertAction( aMgr->action( CopyId ) );
  anEditAction->insertAction( aMgr->action( PasteId ) );
  anEditAction->insertAction( aMgr->action( DeleteId ) );
  anEditAction->insertAction( aMgr->action( SelectAllId ) );
  aMgr->registerAction( anEditAction, EditOpId );*/

  // 3. Create Preference action
  anAction = new QtxAction( tr( "MNU_PY_PREFERENCES" ), QIcon( ":/images/py_preferences.png" ),
                            tr( "MNU_PY_PREFERENCES" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_PREFERENCES" ) );
  connect( anAction, SIGNAL( triggered( bool ) ), this, SLOT( onPreferences() ) );
  aMgr->registerAction( anAction, PreferencesId );

  // 4. Help operations

  // 4.1. Create Help action
  anAction = new QtxAction( tr( "MNU_PY_BROWSER" ), QIcon( ":/images/py_browser.png" ),
                            tr( "MNU_PY_BROWSER" ), 0, this );
  anAction->setStatusTip( tr( "DSC_PY_BROWSER" ) );
  connect( anAction, SIGNAL( triggered() ), this, SLOT( onBrowser() ) );
  //aMgr->registerAction( anAction, BrowserId );

  // 4.2. Create multi-action for help operations
  /*QtxMultiAction* aHelpAction = new QtxMultiAction( this );
  aHelpAction->insertAction( aMgr->action( BrowserId ) );
  aMgr->registerAction( aHelpAction, HelpOpId );*/
}

/*!
  \brief Create toolbar for the python view window.
*/
void PyViewer_ViewWindow::createToolBar()
{
  QtxActionToolMgr* aMgr = toolMgr();
  int idTB = aMgr->createToolBar( tr("LBL_TOOLBAR_LABEL"),         // title (language-dependent)
                                  QString( "PyEditorOperations" ), // name (language-independent)
                                  false );                         // disable floatable toolbar
  aMgr->append( NewId, idTB );
  aMgr->append( OpenId, idTB );
  aMgr->append( SaveId, idTB );
  aMgr->append( SaveAsId, idTB );
  if ( isExternal() )
    aMgr->append( CloseId, idTB );
  aMgr->append( aMgr->separator(), idTB );
  aMgr->append( UndoId, idTB );
  aMgr->append( RedoId, idTB );
  aMgr->append( aMgr->separator(), idTB );
  aMgr->append( CutId, idTB );
  aMgr->append( CopyId, idTB );
  aMgr->append( PasteId, idTB );
  aMgr->append( DeleteId, idTB );
  aMgr->append( SelectAllId, idTB );
  aMgr->append( aMgr->separator(), idTB );
  aMgr->append( PreferencesId, idTB );
  aMgr->append( aMgr->separator(), idTB );
  aMgr->append( BrowserId, idTB );

}

/*!
  \brief Reimplemented class is to receive a window close request.
  \param theEvent event
*/
void PyViewer_ViewWindow::closeEvent( QCloseEvent* theEvent )
{
  if ( whetherSave() )
    theEvent->accept();
  else
    theEvent->ignore();
}

/*!
  SLOT: Creates a new document
 */
void PyViewer_ViewWindow::onNew()
{
  if ( whetherSave() )
  {
    my_TextEditor->clear();
    setCurrentFile( QString() );
  }
}

/*!
  SLOT: Open an existing python file
 */
void PyViewer_ViewWindow::onOpen()
{
  if ( whetherSave() )
  {
    QString aFilePath = QFileDialog::getOpenFileName( 
      this, tr( "TIT_DLG_OPEN" ), QDir::currentPath(), "Python Files (*.py)" );

    if ( !aFilePath.isEmpty() )
      loadFile( aFilePath );
  }
}

/*!
  SLOT: Save the current file
 */
bool PyViewer_ViewWindow::onSave()
{
  if ( my_CurrentFile.isEmpty() )
    return onSaveAs();
  else
    return saveFile( my_CurrentFile );
}


/*!
  SLOT: Save the current file under a new name
 */
bool PyViewer_ViewWindow::onSaveAs()
{
  QString aFilePath = QFileDialog::getSaveFileName(
    this, tr( "TIT_DLG_SAVEAS" ), QDir::currentPath(), "Python Files (*.py)" );

  if ( !aFilePath.isEmpty() )
    return saveFile( aFilePath );

  return false;
}

/*!
  SLOT: Open preferences dialog
 */
void PyViewer_ViewWindow::onPreferences()
{
  PyEditor_SettingsDlg* aPage = new PyEditor_SettingsDlg( my_TextEditor, this );
  connect( aPage, SIGNAL( onHelpClicked() ),
	   this, SLOT( onHelp() ) );
  aPage->exec();
  delete aPage;
}

/*!
  \brief Set preferece values for view.
 */
void PyViewer_ViewWindow::setPreferences()
{
  my_TextEditor->settings()->readSettings();
  my_TextEditor->updateStatement();
}

/*!
  \brief Associates the theFilePath with the python view.
  \param theFilePath file path
 */
void PyViewer_ViewWindow::setCurrentFile( const QString &theFilePath )
{
  my_CurrentFile = theFilePath;
  my_TextEditor->document()->setModified( false );

  if ( isExternal() )
  {
    setWindowModified( false );

    QString aShownName = my_CurrentFile;
    if ( my_CurrentFile.isEmpty() )
      aShownName = "untitled.py";
    setWindowFilePath( aShownName );

    // Set window title with associated file path
    QFileInfo anInfo( aShownName );
    setWindowTitle( "Python Viewer - " + anInfo.fileName() + "[*]" );
  }
}

/*!
  \brief Checks whether the file is modified.
  If it has the modifications then ask the user to save it.
  \return true if the document is saved.
 */
bool PyViewer_ViewWindow::whetherSave()
{
  if ( my_TextEditor->document()->isModified() )
  {
    QMessageBox::StandardButton aReturn;
    aReturn = QMessageBox::warning(
      this, tr( "TIT_DLG_SAVE" ),tr( "WRN_PY_SAVE_FILE" ),
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );

    if ( aReturn == QMessageBox::Save )
      return onSave();
    else if ( aReturn == QMessageBox::Cancel )
      return false;
  }
  return true;
}

/*!
  \brief Opens file.
  \param theFilePath file path
 */
void PyViewer_ViewWindow::loadFile( const QString &theFilePath )
{
  QFile aFile( theFilePath );
  if ( !aFile.open(QFile::ReadOnly | QFile::Text) )
  {
    QMessageBox::warning( this, tr( "NAME_PYEDITOR" ),
      tr( "WRN_PY_READ_FILE" ).arg( theFilePath ).arg( aFile.errorString() ) );
    return;
  }

  QTextStream anInput( &aFile );
  QApplication::setOverrideCursor( Qt::WaitCursor );
  my_TextEditor->setPlainText( anInput.readAll() );
  QApplication::restoreOverrideCursor();

  setCurrentFile( theFilePath );
  aFile.close();
  if ( isExternal() )
    statusBar()->showMessage( tr( "STS_F_LOADED" ), 2000 );
}

/*!
  \brief Saves file.
  \param theFilePath file path
 */
bool PyViewer_ViewWindow::saveFile( const QString &theFilePath )
{
  QFile aFile( theFilePath );
  if ( !aFile.open( QFile::WriteOnly | QFile::Text ) )
  {
    QMessageBox::warning( this, tr( "NAME_PYEDITOR" ),
      tr( "WRN_PY_WRITE_FILE" ).arg( theFilePath ).arg( aFile.errorString() ) );
    return false;
  }

  QTextStream anOutput( &aFile );
  QApplication::setOverrideCursor( Qt::WaitCursor );
  anOutput << my_TextEditor->toPlainText();
  QApplication::restoreOverrideCursor();

  setCurrentFile( theFilePath );
  aFile.close();

  if ( isExternal() )
    statusBar()->showMessage( tr( "STS_F_SAVED" ), 2000 );

  return true;
}

/*!
  \brief Opens help browser with python view help information.
 */
void PyViewer_ViewWindow::onBrowser()
{
  QDir appDir = QApplication::applicationDirPath();
  QStringList parameters;
  parameters << QString( "--file=%1" ).arg( appDir.filePath( "pyeditor.html" ) );
  QProcess::startDetached( "HelpBrowser", parameters );
}

/*!
  Slot, called when user clicks "Help" button in "Preferences" dialog box.
*/
void PyViewer_ViewWindow::onHelp()
{
#ifndef NO_SUIT
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "python_viewer_page.html", "custom_python_preferences" );
#endif
}
