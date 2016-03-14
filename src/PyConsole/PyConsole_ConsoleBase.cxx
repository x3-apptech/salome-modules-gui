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

// File   : PyConsole_ConsoleBase.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
/*!
  \class PyConsole_Console
  \brief Python console widget.
*/  

#include "PyConsole_Interp.h"   /// !!! WARNING !!! THIS INCLUDE MUST BE VERY FIRST !!!
#include "PyConsole_ConsoleBase.h"
#include "PyConsole_EnhEditorBase.h"
#include "PyConsole_EnhInterp.h"

#include "Qtx.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QMenu>
#include <QContextMenuEvent>
#include <QVBoxLayout>

PyConsole_EditorBase *PyConsole_ConsoleBase::PyConsole_Interp_CreatorBase::createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const
{ return new PyConsole_EditorBase(interp,console); }

PyConsole_Interp *PyConsole_ConsoleBase::PyConsole_Interp_CreatorBase::createInterp( ) const
{ return new PyConsole_Interp; }

/*!
  \brief Constructor.

  Creates new python console widget.
  \param parent parent widget
  \param interp python interpreter
*/
PyConsole_ConsoleBase::PyConsole_ConsoleBase( QWidget* parent, PyConsole_Interp* interp )
: QWidget( parent )
{
  PyConsole_ConsoleBase::PyConsole_Interp_CreatorBase crea;
  defaultConstructor(interp,crea);
}

/*!
 * MUST BE NON VIRTUAL ! (called from constructor !!!!)
 */
void PyConsole_ConsoleBase::defaultConstructor( PyConsole_Interp* interp, const PyConsole_Interp_CreatorBase& crea )
{
  PyConsole_Interp *anInterp = interp ? interp : crea.createInterp();
  
  // initialize Python interpretator
  anInterp->initialize();
  
  // create editor console
  QVBoxLayout* lay = new QVBoxLayout( this );
  lay->setMargin( 0 );
  myEditor = crea.createEditor( anInterp, this );
  char* synchronous = getenv("PYTHON_CONSOLE_SYNC");
  if (synchronous && atoi(synchronous))
  {
      myEditor->setIsSync(true);
  }
  myEditor->viewport()->installEventFilter( this );
  lay->addWidget( myEditor );

  createActions();
}

/**
 * Protected constructor.
 */
PyConsole_ConsoleBase::PyConsole_ConsoleBase( QWidget* parent, PyConsole_Interp* /*i*/,  PyConsole_EditorBase* e )
  : QWidget (parent), myEditor(e)
{}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
PyConsole_ConsoleBase::~PyConsole_ConsoleBase()
{
}

PyConsole_Interp* PyConsole_ConsoleBase::getInterp() const
{
  return myEditor ? myEditor->getInterp() : 0;
} 

/*!
  \brief Execute python command in the interpreter.
  \param command string with command and arguments
*/
void PyConsole_ConsoleBase::exec( const QString& command )
{
  if ( myEditor )
    myEditor->exec( command );
}

/*!
  \brief Execute python command in the interpreter 
         and wait until it is finished.
  
  Block execution of main application until the python command is executed.
  \param command string with command and arguments
*/
void PyConsole_ConsoleBase::execAndWait( const QString& command )
{
  if ( myEditor )
    myEditor->execAndWait( command );
}

/*!
  \brief Get synchronous mode flag value.
  
  \sa setIsSync()
  \return True if python console works in synchronous mode
*/
bool PyConsole_ConsoleBase::isSync() const
{
  return myEditor->isSync();
}

/*!
  \brief Set synchronous mode flag value.

  In synhronous mode the Python commands are executed in the GUI thread
  and the GUI is blocked until the command is finished. In the asynchronous
  mode each Python command is executed in the separate thread that does not
  block the main GUI loop.

  \param on synhronous mode flag
*/
void PyConsole_ConsoleBase::setIsSync( const bool on )
{
  myEditor->setIsSync( on );
}

/*!
  \brief Get suppress output flag value.
  
  \sa setIsSuppressOutput()
  \return True if python console output is suppressed.
*/
bool PyConsole_ConsoleBase::isSuppressOutput() const
{
  return myEditor->isSuppressOutput();
}

/*!
  \brief Set suppress output flag value.

  In case if suppress output flag is true, the python 
  console output suppressed.

  \param on suppress output flag
*/
void PyConsole_ConsoleBase::setIsSuppressOutput( const bool on )
{
  myEditor->setIsSuppressOutput(on);
}

/*!
  \brief Get 'show banner' flag value.
  
  \sa setIsShowBanner()
  \return \c true if python console shows banner
*/
bool PyConsole_ConsoleBase::isShowBanner() const
{
  return myEditor->isShowBanner();
}

/*!
  \brief Set 'show banner' flag value.

  The banner is shown in the top of the python console window.

  \sa isShowBanner()
  \param on 'show banner' flag
*/
void PyConsole_ConsoleBase::setIsShowBanner( const bool on )
{
  myEditor->setIsShowBanner( on );
}

/*!
  \brief Change the python console's font.
  \param f new font
*/
void PyConsole_ConsoleBase::setFont( const QFont& f )
{
  if( myEditor )
    myEditor->setFont( f );
}

/*!
  \brief Get python console font.
  \return current python console's font
*/
QFont PyConsole_ConsoleBase::font() const
{
  QFont res;
  if( myEditor )
    res = myEditor->font();
  return res;
}

/*!
  \brief Set actions to be visible in the context popup menu.
  
  Actions, which IDs are set in \a flags parameter, will be shown in the 
  context popup menu. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void PyConsole_ConsoleBase::setMenuActions( const int flags )
{
  myActions[CopyId]->setVisible( flags & CopyId );
  myActions[PasteId]->setVisible( flags & PasteId );
  myActions[ClearId]->setVisible( flags & ClearId );
  myActions[SelectAllId]->setVisible( flags & SelectAllId );
  myActions[DumpCommandsId]->setVisible( flags & DumpCommandsId );
  myActions[StartLogId]->setVisible( flags & StartLogId );
  myActions[StopLogId]->setVisible( flags & StopLogId );
}

/*!
  \brief Get menu actions which are currently visible in the context popup menu.
  \return ORed together actions flags
  \sa setMenuActions()
*/
int PyConsole_ConsoleBase::menuActions() const
{
  int ret = 0;
  ret = ret | ( myActions[CopyId]->isVisible() ? CopyId : 0 );
  ret = ret | ( myActions[PasteId]->isVisible() ? PasteId : 0 );
  ret = ret | ( myActions[ClearId]->isVisible() ? ClearId : 0 );
  ret = ret | ( myActions[SelectAllId]->isVisible() ? SelectAllId : 0 );
  ret = ret | ( myActions[DumpCommandsId]->isVisible() ? DumpCommandsId : 0 );
  ret = ret | ( myActions[StartLogId]->isVisible() ? StartLogId : 0 );
  ret = ret | ( myActions[StopLogId]->isVisible() ? StopLogId : 0 );
  return ret;
}

/*!
  \brief Create menu actions.

  Create context popup menu actions.
*/
void PyConsole_ConsoleBase::createActions()
{
  QAction* a = new QAction( tr( "EDIT_COPY_CMD" ), this );
  a->setStatusTip( tr( "EDIT_COPY_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( copy() ) );
  myActions.insert( CopyId, a );

  a = new QAction( tr( "EDIT_PASTE_CMD" ), this );
  a->setStatusTip( tr( "EDIT_PASTE_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( paste() ) );
  myActions.insert( PasteId, a );

  a = new QAction( tr( "EDIT_CLEAR_CMD" ), this );
  a->setStatusTip( tr( "EDIT_CLEAR_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( clear() ) );
  myActions.insert( ClearId, a );

  a = new QAction( tr( "EDIT_SELECTALL_CMD" ), this );
  a->setStatusTip( tr( "EDIT_SELECTALL_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( selectAll() ) );
  myActions.insert( SelectAllId, a );
  
  a = new QAction( tr( "EDIT_DUMPCOMMANDS_CMD" ), this );
  a->setStatusTip( tr( "EDIT_DUMPCOMMANDS_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( dump() ) );
  myActions.insert( DumpCommandsId, a );

  a = new QAction( tr( "EDIT_STARTLOG_CMD" ), this );
  a->setStatusTip( tr( "EDIT_STARTLOG_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( startLog() ) );
  myActions.insert( StartLogId, a );

  a = new QAction( tr( "EDIT_STOPLOG_CMD" ), this );
  a->setStatusTip( tr( "EDIT_STOPLOG_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), myEditor, SLOT( stopLog() ) );
  myActions.insert( StopLogId, a );
}

/*!
  \brief Update menu actions.

  Update context popup menu action state.
*/
void PyConsole_ConsoleBase::updateActions()
{
  myActions[CopyId]->setEnabled( myEditor->textCursor().hasSelection() );
  myActions[PasteId]->setEnabled( !myEditor->isReadOnly() && !QApplication::clipboard()->text().isEmpty() );
  myActions[SelectAllId]->setEnabled( !myEditor->document()->isEmpty() );
}

/*!
  \brief Start python trace logging
  \param fileName the path to the log file
*/
void PyConsole_ConsoleBase::startLog( const QString& fileName )
{
  myEditor->startLog( fileName );
}

/*!
  \brief Stop python trace logging
*/
void PyConsole_ConsoleBase::stopLog()
{
  myEditor->stopLog();
}

/*!
  \brief Create the context popup menu.

  Fill in the popup menu with the commands.

  \param menu context popup menu
*/
void PyConsole_ConsoleBase::contextMenuPopup( QMenu *menu )
{
  if ( myEditor->isReadOnly() )
    return;

  menu->addAction( myActions[CopyId] );
  menu->addAction( myActions[PasteId] );
  menu->addAction( myActions[ClearId] );
  menu->addSeparator();
  menu->addAction( myActions[SelectAllId] );
  menu->addSeparator();
  menu->addAction( myActions[DumpCommandsId] );
  if ( !myEditor->isLogging() )
    menu->addAction( myActions[StartLogId] );
  else
    menu->addAction( myActions[StopLogId] );

  Qtx::simplifySeparators( menu );

  updateActions();
}

PyConsole_EditorBase *PyConsole_EnhConsoleBase::PyConsole_Interp_EnhCreatorBase::createEditor( PyConsole_Interp *interp, PyConsole_ConsoleBase *console ) const
{ return new PyConsole_EnhEditorBase(interp,console); }

PyConsole_Interp *PyConsole_EnhConsoleBase::PyConsole_Interp_EnhCreatorBase::createInterp( ) const
{ return new PyConsole_EnhInterp; }

/**
 * Similar to constructor of the base class but using enhanced objects.
 * TODO: this should really be done in a factory to avoid code duplication.
 * @param parent
 * @param interp
 */
PyConsole_EnhConsoleBase::PyConsole_EnhConsoleBase( QWidget* parent, PyConsole_Interp* interp )
  : PyConsole_ConsoleBase( parent, interp, 0 )
{
  PyConsole_Interp_EnhCreatorBase crea;
  defaultConstructor(interp,crea);
}

/*!
  \brief Event handler.

  Handles context menu request event.

  \param o object
  \param e event
  \return True if the event is processed and further processing should be stopped
*/
bool PyConsole_EnhConsoleBase::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myEditor->viewport() && e->type() == QEvent::ContextMenu )
  {
    contextMenuRequest( (QContextMenuEvent*)e );
    return true;
  }
  return QWidget::eventFilter( o, e );
}

void PyConsole_EnhConsoleBase::contextMenuRequest( QContextMenuEvent * e )
{
  QMenu *menu(new QMenu(this));
  contextMenuPopup(menu);
  menu->move(e->globalPos());
  menu->show();
}
