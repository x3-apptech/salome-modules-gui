// Copyright (C) 2015-2020  OPEN CASCADE
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

#include "PyEditor_Widget.h"
#include "PyEditor_SettingsDlg.h"

#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"

#include "QtxAction.h"
#include "QtxActionToolMgr.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>

/*!
  \class PyViewer_ViewWindow
  \brief Python view window.
*/

/*!
  \brief Constructor.
  \param desktop SALOME desktop window
*/
PyViewer_ViewWindow::PyViewer_ViewWindow( SUIT_Desktop* desktop ) :
  SUIT_ViewWindow( desktop )
{
  // Create central widget.
  myEditor = new PyEditor_Widget( this );
  setCentralWidget( myEditor );

  // Create actions.
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QtxAction* action;

  // . New
  action = new QtxAction( tr( "TTP_NEW" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_NEW" ) ),
                          tr( "ACT_NEW" ), 0, this );
  action->setStatusTip( tr( "DSC_NEW" ) );
  action->setShortcut( QKeySequence::New );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onNew() ) );
  toolMgr()->registerAction( action, NewId );
  
  // . Open
  action = new QtxAction( tr( "TTP_OPEN" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_OPEN" ) ),
                          tr( "ACT_OPEN" ), 0, this );
  action->setStatusTip( tr( "DSC_OPEN" ) );
  action->setShortcut( QKeySequence::Open );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onOpen() ) );
  toolMgr()->registerAction( action, OpenId );
  
  // . Save
  action = new QtxAction( tr( "TTP_SAVE" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_SAVE" ) ),
                          tr( "ACT_SAVE" ), 0, this );
  action->setStatusTip( tr( "DSC_SAVE" ) );
  action->setShortcut( QKeySequence::Save );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onSave() ) );
  action->setEnabled( false );
  connect( myEditor, SIGNAL( modificationChanged( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  toolMgr()->registerAction( action, SaveId );
  
  // . SaveAs
  action = new QtxAction( tr( "TTP_SAVEAS" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_SAVEAS" ) ),
                          tr( "ACT_SAVEAS" ), 0, this );
  action->setStatusTip( tr( "DSC_SAVEAS" ) );
  action->setShortcut( QKeySequence::SaveAs );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onSaveAs() ) );
  toolMgr()->registerAction( action, SaveAsId );

  // . Undo
  action = new QtxAction( tr( "TTP_UNDO" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_UNDO" ) ),
                          tr( "ACT_UNDO" ), 0, this );
  action->setStatusTip( tr( "DSC_UNDO" ) );
  action->setShortcut( QKeySequence::Undo );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( undo() ) );
  action->setEnabled( false );
  connect( myEditor, SIGNAL( undoAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  toolMgr()->registerAction( action, UndoId );

  // . Redo
  action = new QtxAction( tr( "TTP_REDO" ), 
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_REDO" ) ),
                          tr( "ACT_REDO" ), 0, this );
  action->setStatusTip( tr( "DSC_REDO" ) );
  action->setShortcut( QKeySequence::Redo );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( redo() ) );
  action->setEnabled( false );
  connect( myEditor, SIGNAL( redoAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  toolMgr()->registerAction( action, RedoId );

  // . Cut
  action = new QtxAction( tr( "TTP_CUT" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_CUT" ) ),
                          tr( "ACT_CUT" ), 0, this );
  action->setStatusTip( tr( "DSC_CUT" ) );
  action->setShortcut( QKeySequence::Cut );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( cut() ) );
  action->setEnabled( false );
  connect( myEditor, SIGNAL( copyAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  toolMgr()->registerAction( action, CutId );

  // . Copy
  action = new QtxAction( tr( "TTP_COPY" ), 
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_COPY" ) ),
                          tr( "ACT_COPY" ), 0, this );
  action->setStatusTip( tr( "DSC_COPY" ) );
  action->setShortcut( QKeySequence::Copy );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( copy() ) );
  action->setEnabled( false );
  connect( myEditor, SIGNAL( copyAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  toolMgr()->registerAction( action, CopyId );

  // . Paste
  action = new QtxAction( tr( "TTP_PASTE" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_PASTE" ) ),
                          tr( "ACT_PASTE" ), 0, this );
  action->setStatusTip( tr( "DSC_PASTE" ) );
  action->setShortcut( QKeySequence::Paste );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( paste() ) );
  toolMgr()->registerAction( action, PasteId );

  // . Delete
  action = new QtxAction( tr( "TTP_DELETE" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_DELETE" ) ),
                          tr( "ACT_DELETE" ), 0, this );
  action->setStatusTip( tr( "DSC_DELETE" ) );
  action->setShortcut( QKeySequence::Delete );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( deleteSelected() ) );
  action->setEnabled( false );
  connect( myEditor, SIGNAL( copyAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  toolMgr()->registerAction( action, DeleteId );

  // . SelectAll
  action = new QtxAction( tr( "TTP_SELECT_ALL" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_SELECT_ALL" ) ),
                          tr( "ACT_SELECT_ALL" ), 0, this );
  action->setStatusTip( tr( "DSC_SELECT_ALL" ) );
  action->setShortcut( QKeySequence::SelectAll );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( selectAll() ) );
  toolMgr()->registerAction( action, SelectAllId );

  // . Find
  action = new QtxAction( tr( "TTP_FIND" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_FIND" ) ),
                          tr( "ACT_FIND" ), 0, this );
  action->setStatusTip( tr( "DSC_FIND" ) );
  action->setShortcut( QKeySequence::Find );
  action->setShortcutContext( Qt::WidgetShortcut );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( find() ) );
  toolMgr()->registerAction( action, FindId );

  // . Replace
  action = new QtxAction( tr( "TTP_REPLACE" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_REPLACE" ) ),
                          tr( "ACT_REPLACE" ), 0, this );
  action->setStatusTip( tr( "DSC_REPLACE" ) );
  action->setShortcuts( QList<QKeySequence>() << QKeySequence( "Ctrl+H" ) << QKeySequence( QKeySequence::Replace ) );
  action->setShortcutContext( Qt::WidgetShortcut );
  connect( action, SIGNAL( triggered( bool ) ), myEditor, SLOT( replace() ) );
  toolMgr()->registerAction( action, ReplaceId );

  // . Preferences
  action = new QtxAction( tr( "TTP_PREFERENCES" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_PREFERENCES" ) ),
                          tr( "ACT_PREFERENCES" ), 0, this );
  action->setStatusTip( tr( "DSC_PREFERENCES" ) );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onPreferences() ) );
  toolMgr()->registerAction( action, PreferencesId );

  // . Help
  action = new QtxAction( tr( "TTP_HELP" ),
                          resMgr->loadPixmap( "PyViewer", tr( "ICON_HELP" ) ),
                          tr( "ACT_HELP" ), 0, this );
  action->setStatusTip( tr( "DSC_HELP" ) );
  connect( action, SIGNAL( triggered() ), this, SLOT( onHelp() ) );
  toolMgr()->registerAction( action, HelpId );

  // Create toolbar.
  int idTB = toolMgr()->createToolBar( tr("TOOLBAR_LABEL"), QString( "PythonEditor" ), false );
  toolMgr()->append( NewId, idTB );
  toolMgr()->append( OpenId, idTB );
  toolMgr()->append( SaveId, idTB );
  toolMgr()->append( SaveAsId, idTB );
  toolMgr()->append( toolMgr()->separator(), idTB );
  toolMgr()->append( UndoId, idTB );
  toolMgr()->append( RedoId, idTB );
  toolMgr()->append( toolMgr()->separator(), idTB );
  toolMgr()->append( CutId, idTB );
  toolMgr()->append( CopyId, idTB );
  toolMgr()->append( PasteId, idTB );
  toolMgr()->append( DeleteId, idTB );
  toolMgr()->append( SelectAllId, idTB );
  toolMgr()->append( toolMgr()->separator(), idTB );
  toolMgr()->append( FindId, idTB );
  toolMgr()->append( ReplaceId, idTB );
  toolMgr()->append( toolMgr()->separator(), idTB );
  toolMgr()->append( PreferencesId, idTB );
  toolMgr()->append( toolMgr()->separator(), idTB );
  toolMgr()->append( HelpId, idTB );

  // Set current file.
  setCurrentFile( QString() );
}

/*!
  \brief Destructor.
*/
PyViewer_ViewWindow::~PyViewer_ViewWindow()
{
}

/*!
  \brief Manage window close request.
  \param event close event
*/
void PyViewer_ViewWindow::closeEvent( QCloseEvent* event )
{
  if ( whetherSave() )
    event->accept();
  else
    event->ignore();
}

/*!
  SLOT: Create new document
*/
void PyViewer_ViewWindow::onNew()
{
  if ( whetherSave() )
  {
    myEditor->clear();
    setCurrentFile( QString() );
  }
}

/*!
  SLOT: Open existing Python file
*/
void PyViewer_ViewWindow::onOpen()
{
  if ( whetherSave() )
  {
    QString filter = tr( "TIT_PY_FILES" );
    filter += " (*.py)";
    QString aFilePath = QFileDialog::getOpenFileName( this,
                                                      tr( "TIT_DLG_OPEN" ),
                                                      QDir::currentPath(),
                                                      filter );

    if ( !aFilePath.isEmpty() )
      loadFile( aFilePath );
  }
}

/*!
  SLOT: Save current document
*/
bool PyViewer_ViewWindow::onSave()
{
  if ( myURL.isEmpty() )
    return onSaveAs();
  else
    return saveFile( myURL );
}


/*!
  SLOT: Save current document under a new name
*/
bool PyViewer_ViewWindow::onSaveAs()
{
  QString filter = tr( "TIT_PY_FILES" );
  filter += " (*.py)";
  QString url = myURL.isEmpty() ? defaultName() : myURL;
  QString aFilePath = QFileDialog::getSaveFileName( this,
                                                    tr( "TIT_DLG_SAVE" ),
                                                    url,
                                                    filter );

  if ( !aFilePath.isEmpty() )
    return saveFile( aFilePath );

  return false;
}

/*!
  SLOT: Open preferences dialog
*/
void PyViewer_ViewWindow::onPreferences()
{
  PyEditor_SettingsDlg dlg( myEditor->editor(), true, this );
  connect( &dlg, SIGNAL( help() ), this, SLOT( onHelp() ) );
  dlg.exec();
}

/*!
  \brief Associate \a filePath with the current document
  \param filePath document's file path
*/
void PyViewer_ViewWindow::setCurrentFile( const QString& filePath )
{
  myURL = filePath;
  myEditor->setModified( false );
}

/*!
  \brief Check whether the file is modified.
  If it has the modifications then ask the user to save it.
  \return true if the document is saved.
*/
bool PyViewer_ViewWindow::whetherSave()
{
  if ( myEditor->isModified() )
  {
    QMessageBox::StandardButton answer =  QMessageBox::warning( this,
                                                                tr( "NAME_PYEDITOR" ),
                                                                tr( "WRN_SAVE_FILE" ),
                                                                QMessageBox::Save |
                                                                QMessageBox::Discard |
                                                                QMessageBox::Cancel );
    switch( answer )
    {
    case QMessageBox::Save:
      return onSave();
    case QMessageBox::Cancel:
      return false;
    default:
      break;
    }
  }
  return true;
}

/*!
  \brief Open file.
  \param filePath file path
*/
void PyViewer_ViewWindow::loadFile( const QString& filePath )
{
  QFile aFile( filePath );
  if ( !aFile.open(QFile::ReadOnly | QFile::Text) )
  {
    QMessageBox::warning( this, tr( "NAME_PYEDITOR" ),
                          tr( "WRN_READ_FILE" ).arg( filePath ).arg( aFile.errorString() ) );
    return;
  }

  QTextStream anInput( &aFile );
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myEditor->setText( anInput.readAll() );
  QApplication::restoreOverrideCursor();

  setCurrentFile( filePath );
  aFile.close();

}

/*!
  \brief Saves file.
  \param theFilePath file path
*/
bool PyViewer_ViewWindow::saveFile( const QString& filePath )
{
  QFile aFile( filePath );
  if ( !aFile.open( QFile::WriteOnly | QFile::Text ) )
  {
    QMessageBox::warning( this, tr( "NAME_PYEDITOR" ),
                          tr( "WRN_WRITE_FILE" ).arg( filePath ).arg( aFile.errorString() ) );
    return false;
  }

  QTextStream anOutput( &aFile );
  QApplication::setOverrideCursor( Qt::WaitCursor );
  anOutput << myEditor->text();
  QApplication::restoreOverrideCursor();

  setCurrentFile( filePath );
  aFile.close();

  return true;
}

/*!
  Slot, called when user clicks "Help" button in "Preferences" dialog box.
*/
void PyViewer_ViewWindow::onHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app ) {
    QString page = "python_viewer.html";
    if ( qobject_cast<QWidget*>( sender() ) )
      page += "#custom-python-preferences";
    app->onHelpContextModule( "GUI", page );
  }
}

/*!
  Get default name for Python file 
  \return default name
*/
QString PyViewer_ViewWindow::defaultName() const
{
  return tr( "NONAME" );
}
