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
// File   : PyEditor_Window.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "PyEditor_Window.h"
#include "PyEditor_Editor.h"
#include "PyEditor_Settings.h"
#include "PyEditor_SettingsDlg.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>

/*!
  \class PyEditor_Window
  \brief Python view window.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
PyEditor_Window::PyEditor_Window( QWidget* parent ) :
  QMainWindow( parent )
{
  Q_INIT_RESOURCE( PyEditor );

  // Create editor and set it as a central widget.
  myTextEditor = new PyEditor_Editor( this );
  setCentralWidget( myTextEditor );

  // Create actions.
  QAction* action;

  // . New
  action = new QAction( QIcon( ":/images/py_new.png" ),
                        tr( "ACT_NEW" ), this );
  action->setToolTip( tr( "TTP_NEW" ) );
  action->setStatusTip( tr( "DSC_NEW" ) );
  action->setShortcut( QKeySequence::New );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onNew() ) );
  myActions[ NewId ] = action;

  // . Open
  action = new QAction( QIcon( ":/images/py_open.png" ),
                        tr( "ACT_OPEN" ), this );
  action->setToolTip( tr( "TTP_OPEN" ) );
  action->setStatusTip( tr( "DSC_OPEN" ) );
  action->setShortcut( QKeySequence::Open );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onOpen() ) );
  myActions[ OpenId ] = action;

  // . Save
  action = new QAction( QIcon( ":/images/py_save.png" ),
                        tr( "ACT_SAVE" ), this );
  action->setToolTip( tr( "TTP_SAVE" ) );
  action->setStatusTip( tr( "DSC_SAVE" ) );
  action->setShortcut( QKeySequence::Save );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onSave() ) );
  action->setEnabled( false );
  connect( myTextEditor->document(), SIGNAL( modificationChanged( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  myActions[ SaveId ] = action;

  // . SaveAs
  action = new QAction( QIcon( ":/images/py_save_as.png" ),
                        tr( "ACT_SAVEAS" ), this );
  action->setToolTip( tr( "TTP_SAVEAS" ) );
  action->setStatusTip( tr( "DSC_SAVEAS" ) );
  action->setShortcut( QKeySequence::SaveAs );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onSaveAs() ) );
  myActions[ SaveAsId ] = action;

  // . Exit
  action = new QAction( QIcon( ":/images/py_exit.png" ),
                        tr( "ACT_EXIT" ), this );
  action->setToolTip( tr( "TTP_EXIT" ) );
  action->setStatusTip( tr( "DSC_EXIT" ) );
  action->setShortcut( QKeySequence::Quit );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( close() ) );
  myActions[ ExitId ] = action;

  // . Undo
  action = new QAction( QIcon( ":/images/py_undo.png" ),
                        tr( "ACT_UNDO" ), this );
  action->setToolTip( tr( "TTP_UNDO" ) );
  action->setStatusTip( tr( "DSC_UNDO" ) );
  action->setShortcut( QKeySequence::Undo );
  connect( action, SIGNAL( triggered( bool ) ), myTextEditor, SLOT( undo() ) );
  action->setEnabled( false );
  connect( myTextEditor->document(), SIGNAL( undoAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  myActions[ UndoId ] = action;

  // . Redo
  action = new QAction( QIcon( ":/images/py_redo.png" ),
                        tr( "ACT_REDO" ), this );
  action->setToolTip( tr( "TTP_REDO" ) );
  action->setStatusTip( tr( "DSC_REDO" ) );
  action->setShortcut( QKeySequence::Redo );
  connect( action, SIGNAL( triggered( bool ) ), myTextEditor, SLOT( redo() ) );
  action->setEnabled( false );
  connect( myTextEditor->document(), SIGNAL( redoAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  myActions[ RedoId ] = action;

  // . Cut
  action = new QAction( QIcon( ":/images/py_cut.png" ),
                        tr( "ACT_CUT" ), this );
  action->setToolTip( tr( "TTP_CUT" ) );
  action->setStatusTip( tr( "DSC_CUT" ) );
  action->setShortcut( QKeySequence::Cut );
  connect( action, SIGNAL( triggered( bool ) ), myTextEditor, SLOT( cut() ) );
  action->setEnabled( false );
  connect( myTextEditor, SIGNAL( copyAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  myActions[ CutId ] = action;

  // . Copy
  action = new QAction( QIcon( ":/images/py_copy.png" ),
                        tr( "ACT_COPY" ), this );
  action->setToolTip( tr( "TTP_COPY" ) );
  action->setStatusTip( tr( "DSC_COPY" ) );
  action->setShortcut( QKeySequence::Copy );
  connect( action, SIGNAL( triggered( bool ) ), myTextEditor, SLOT( copy() ) );
  action->setEnabled( false );
  connect( myTextEditor, SIGNAL( copyAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  myActions[ CopyId ] = action;

  // . Paste
  action = new QAction( QIcon( ":/images/py_paste.png" ),
                        tr( "ACT_PASTE" ), this );
  action->setToolTip( tr( "TTP_PASTE" ) );
  action->setStatusTip( tr( "DSC_PASTE" ) );
  action->setShortcut( QKeySequence::Paste );
  connect( action, SIGNAL( triggered( bool ) ), myTextEditor, SLOT( paste() ) );
  myActions[ PasteId ] = action;

  // . Delete
  action = new QAction( QIcon( ":/images/py_delete.png" ),
                        tr( "ACT_DELETE" ), this );
  action->setToolTip( tr( "TTP_DELETE" ) );
  action->setStatusTip( tr( "DSC_DELETE" ) );
  action->setShortcut( QKeySequence::Delete );
  connect( action, SIGNAL( triggered( bool ) ), myTextEditor, SLOT( deleteSelected() ) );
  action->setEnabled( false );
  connect( myTextEditor, SIGNAL( copyAvailable( bool ) ),
           action, SLOT( setEnabled( bool ) ) );
  myActions[ DeleteId ] = action;

  // . SelectAll
  action = new QAction( QIcon( ":/images/py_select_all.png" ),
                        tr( "ACT_SELECT_ALL" ), this );
  action->setToolTip( tr( "TTP_SELECT_ALL" ) );
  action->setStatusTip( tr( "DSC_SELECT_ALL" ) );
  action->setShortcut( QKeySequence::SelectAll );
  connect( action, SIGNAL( triggered( bool ) ), myTextEditor, SLOT( selectAll() ) );
  myActions[ SelectAllId ] = action;

  // . Preferences
  action = new QAction( QIcon( ":/images/py_preferences.png" ),
                        tr( "ACT_PREFERENCES" ), this );
  action->setToolTip( tr( "TTP_PREFERENCES" ) );
  action->setStatusTip( tr( "DSC_PREFERENCES" ) );
  connect( action, SIGNAL( triggered( bool ) ), this, SLOT( onPreferences() ) );
  myActions[ PreferencesId ] = action;

  // . Help
  action = new QAction( QIcon( ":/images/py_help.png" ),
                        tr( "ACT_HELP" ), this );
  action->setToolTip( tr( "TTP_HELP" ) );
  action->setStatusTip( tr( "DSC_HELP" ) );
  connect( action, SIGNAL( triggered() ), this, SLOT( onHelp() ) );
  myActions[ HelpId ] = action;

  // Create menu.
  QMenu* menu = menuBar()->addMenu( tr( "MNU_FILE" ) );
  menu->addAction( myActions[ NewId ] );
  menu->addAction( myActions[ OpenId ] );
  menu->addSeparator();
  menu->addAction( myActions[ SaveId ] );
  menu->addAction( myActions[ SaveAsId ] );
  menu->addSeparator();
  menu->addAction( myActions[ ExitId ] );

  menu = menuBar()->addMenu( tr( "MNU_EDIT" ) );
  menu->addAction( myActions[ UndoId ] );
  menu->addAction( myActions[ RedoId ] );
  menu->addSeparator();
  menu->addAction( myActions[ CutId ] );
  menu->addAction( myActions[ CopyId ] );
  menu->addAction( myActions[ PasteId ] );
  menu->addAction( myActions[ DeleteId ] );
  menu->addSeparator();
  menu->addAction( myActions[ SelectAllId ] );
  menu->addSeparator();
  menu->addAction( myActions[ PreferencesId ] );

  menu = menuBar()->addMenu( tr( "MNU_HELP" ) );
  menu->addAction( myActions[ HelpId ] );

  // Create toolbar.
  QToolBar* toolbar = addToolBar( tr( "TOOLBAR_LABEL" ) );
  toolbar->setObjectName("PythonEditor");
  toolbar->addAction( myActions[ NewId ] );
  toolbar->addAction( myActions[ OpenId ] );
  toolbar->addAction( myActions[ SaveId ] );
  toolbar->addAction( myActions[ SaveAsId ] );
  toolbar->addSeparator();
  toolbar->addAction( myActions[ ExitId ] );
  toolbar->addSeparator();
  toolbar->addAction( myActions[ UndoId ] );
  toolbar->addAction( myActions[ RedoId ] );
  toolbar->addSeparator();
  toolbar->addAction( myActions[ CutId ] );
  toolbar->addAction( myActions[ CopyId ] );
  toolbar->addAction( myActions[ PasteId ] );
  toolbar->addAction( myActions[ DeleteId ] );
  toolbar->addAction( myActions[ SelectAllId ] );
  toolbar->addSeparator();
  toolbar->addAction( myActions[ PreferencesId ] );
  toolbar->addSeparator();
  toolbar->addAction( myActions[ HelpId ] );

  // Set current file.
  setCurrentFile( QString() );

  // Additional set-up for main window.
  connect( myTextEditor->document(), SIGNAL( modificationChanged( bool ) ),
           this, SLOT( setWindowModified( bool ) ) );

  // Initialize status bar.
  statusBar()->showMessage( tr( "STS_READY" ) );
}

/*!
  \brief Destructor.
*/
PyEditor_Window::~PyEditor_Window()
{
}

/*!
  \brief Manage window close request.
  \param event close event
*/
void PyEditor_Window::closeEvent( QCloseEvent* event )
{
  if ( whetherSave() )
    event->accept();
  else
    event->ignore();
}

/*!
  SLOT: Create new document
*/
void PyEditor_Window::onNew()
{
  if ( whetherSave() )
  {
    myTextEditor->clear();
    setCurrentFile( QString() );
  }
}

/*!
  SLOT: Open existing Python file
*/
void PyEditor_Window::onOpen()
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
bool PyEditor_Window::onSave()
{
  if ( myURL.isEmpty() )
    return onSaveAs();
  else
    return saveFile( myURL );
}


/*!
  SLOT: Save current document under a new name
*/
bool PyEditor_Window::onSaveAs()
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
void PyEditor_Window::onPreferences()
{
  PyEditor_SettingsDlg dlg( myTextEditor, true, this );
  connect( &dlg, SIGNAL( help() ), this, SLOT( onHelp() ) );
  dlg.exec();
}

/*!
  \brief Associate \a filePath with the current document
  \param filePath document's file path
*/
void PyEditor_Window::setCurrentFile( const QString& filePath )
{
  myURL = filePath;
  myTextEditor->document()->setModified( false );

  setWindowModified( false );

  setWindowFilePath( myURL.isEmpty() ? defaultName() : myURL );
}

/*!
  \brief Check whether the file is modified.
  If it has the modifications then ask the user to save it.
  \return true if the document is saved.
*/
bool PyEditor_Window::whetherSave()
{
  if ( myTextEditor->document()->isModified() )
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
void PyEditor_Window::loadFile( const QString& filePath )
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
  myTextEditor->setPlainText( anInput.readAll() );
  QApplication::restoreOverrideCursor();

  setCurrentFile( filePath );
  aFile.close();

  statusBar()->showMessage( tr( "STS_F_LOADED" ), 2000 );
}

/*!
  \brief Save file.
  \param filePath file path
*/
bool PyEditor_Window::saveFile( const QString& filePath )
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
  anOutput << myTextEditor->toPlainText();
  QApplication::restoreOverrideCursor();

  setCurrentFile( filePath );
  aFile.close();

  statusBar()->showMessage( tr( "STS_F_SAVED" ), 2000 );

  return true;
}

/*!
  Slot, called when user clicks "Help" button in "Preferences" dialog box.
*/
void PyEditor_Window::onHelp()
{
  QWidget* w = qobject_cast<QWidget*>( sender() );
  if ( !w ) w = this;
  QFile file(":/about.txt");
  file.open(QFile::ReadOnly | QFile::Text);
  QTextStream stream( &file );
  QString about = stream.readAll();
  file.close();
  QMessageBox::about( w, tr( "NAME_PYEDITOR" ), about );
}

/*!
  Get default name for Python file 
  \return default name
*/
QString PyEditor_Window::defaultName() const
{
  return tr( "NONAME" );
}
