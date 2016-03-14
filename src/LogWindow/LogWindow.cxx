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

//  KERNEL SALOME_Event : Define event posting mechanism
// File   : LogWindow.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "LogWindow.h"

#include <QAction>
#include <QApplication>
#include <QDate>
#include <QFile>
#include <QMenu>
#include <QTextEdit>
#include <QTextStream>
#include <QTime>
#include <QVBoxLayout>

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Tools.h>

#define DEFAULT_SEPARATOR "***"

/*!
  \brief Convert rich text to plain text.
  \internal
  \param richText rich text string
  \return converted plain text string
*/
static QString plainText( const QString& richText )
{
  QString aText = richText;
  int startTag = aText.indexOf( '<' );
  while ( true )
  {
    if ( startTag < 0 )
      break;

    int finishTag = aText.indexOf( '>', startTag );
    if ( finishTag < 0 )
      break;

    aText = aText.remove( startTag, finishTag - startTag + 1 );
    startTag = aText.indexOf( '<' );
  }
  return aText;
}

/*!
  \class LogWindow
  \brief Widget, displaying log messages.

  The log messages window provides operations like:
  - show messages
  - display timestamps at the message beginning
  - color messages according to their purposes (e.g., errors/warning)
  - clear log output
  - copy messages to clipvoard
  - save message log to to the text file
*/

/*!
  \brief Constructor.

  Creates new messages log window widget.
  \param parent parent widget
*/
LogWindow::LogWindow( QWidget* parent )
: QWidget( parent ),
  SUIT_PopupClient()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QString fntSet = resMgr ? resMgr->stringValue( "Log Window", "font", QString() ) : QString();

  setFont( SUIT_Tools::stringToFont( fntSet ) );

  myView = new QTextEdit( this );
  myView->setReadOnly( true );
  myView->viewport()->installEventFilter( this );

  QVBoxLayout* main = new QVBoxLayout( this );
  main->setMargin( 0 );
  main->addWidget( myView );

  myBannerSize = 0;
  myBanner = "<b>Message Log</b>\n********************";
  mySeparator = DEFAULT_SEPARATOR;

  clear();

  createActions();
}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
LogWindow::~LogWindow()
{
}

/*!
  \brief Get current banner (message log window header text).
  \return string representing the current banner
*/
QString LogWindow::banner() const
{
  return myBanner;
}

/*!
  \brief Get current separator (text which is printed between messages).
  \return string representing the current separator
*/
QString LogWindow::separator() const
{
  return mySeparator;
}

/*!
  \brief Set current banner (message log window header text).
  \param banner new banner
*/
void LogWindow::setBanner( const QString& banner )
{
  myBanner = banner;

  clear( false );
}

/*!
  Set current separator (text which is printed between messages).
  \param separator new separator
*/
void LogWindow::setSeparator( const QString& separator )
{
  mySeparator = separator;

  clear( false );
}

/*!
  \brief Custom event handler.

  Process context popup menu request event.
  
  \param o object
  \param e event
  \return True if the event is processed and further processing should be stopped
*/
bool LogWindow::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myView->viewport() && e->type() == QEvent::ContextMenu )
  {
    contextMenuRequest( (QContextMenuEvent*)e );
    return true;
  }
  return QWidget::eventFilter( o, e );
}

/*!
  \brief Put new message to the log window.
  \param message text of the message
  \param flags ORed flags which define how the message should be printed
*/
void LogWindow::putMessage( const QString& message, const int flags )
{
  putMessage( message, QColor(), flags );
}

/*!
  \brief Put new message to the log window.
  \param message text of the message
  \param color text color of the message
  \param flags ORed flags which define how the message should be printed
*/
void LogWindow::putMessage( const QString& message, const QColor& color, const int flags )
{
  QString msg = message;
  if ( msg.isEmpty() )
    return;

  bool noColor = flags & DisplayNoColor;

  if ( color.isValid() )
    msg = QString( "<font color=\"%1\">%2</font>" ).arg( color.name() ).arg( msg );

  QString dStr;
  if ( flags & DisplayDate )
  {
    dStr = QDate::currentDate().toString( Qt::SystemLocaleDate );
    if ( !noColor )
      dStr = QString( "<font color=\"#003380\">%1</font>" ).arg( dStr );
  }

  QString tStr;
  if ( flags & DisplayTime )
  {
    tStr = QTime::currentTime().toString( Qt::SystemLocaleDate );
    if ( !noColor )
      tStr = QString( "<font color=\"#008033\">%1</font>" ).arg( tStr );
  }

  QString dateTime = QString( "%1 %2" ).arg( dStr ).arg( tStr ).trimmed();
  if ( !dateTime.isEmpty() )
    msg = QString( "[%1] %2" ).arg( dateTime ).arg( msg );

  myView->append( msg );
  myHistory.append( plainText( message ) );

  if ( flags & DisplaySeparator && !mySeparator.isEmpty() )
  {
    myView->append( mySeparator );   // add separator
    myHistory.append( plainText( mySeparator ) );
  }
  myView->moveCursor( QTextCursor::End );
}

/*!
  \brief Clear message log.
  \param clearHistory if True, clear also the messages history
*/
void LogWindow::clear( bool clearHistory )
{
  myView->clear();
  if ( clearHistory )
    myHistory.clear();

  if ( !myBanner.isEmpty() )
  {
    myView->append( myBanner );
    myBannerSize = myView->document()->blockCount();
  }
  else
    myBannerSize = 0;
}

/*!
  \brief Save messages log to the file.
  \param fileName name of the file
  \return \c true on success and \c false on error
*/
bool LogWindow::saveLog( const QString& fileName )
{
  QFile file( fileName );
  if ( !file.open( QFile::WriteOnly ) )
    return false;

  QTextStream stream( &file );

  stream << "*****************************************"   << endl;
  stream << "Message Log"                                 << endl;
  stream << QDate::currentDate().toString( "dd.MM:yyyy" ) << "  ";
  stream << QTime::currentTime().toString( "hh:mm:ss" )   << endl;
  stream << "*****************************************"   << endl;

  for ( int i = 0; i < myHistory.count(); i++ )
    stream << myHistory[ i ] << endl;

  file.close();
  return true;
}

/*!
  \brief Create context popup menu actions.
*/
void LogWindow::createActions()
{
  QAction* a = new QAction( tr( "EDIT_COPY_CMD" ), this );
  a->setStatusTip( tr( "EDIT_COPY_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onCopy() ) );
  myActions.insert( CopyId, a );

  a = new QAction( tr( "EDIT_CLEAR_CMD" ), this );
  a->setStatusTip( tr( "EDIT_CLEAR_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onClear() ) );
  myActions.insert( ClearId, a );

  a = new QAction( tr( "EDIT_SELECTALL_CMD" ), this );
  a->setStatusTip( tr( "EDIT_SELECTALL_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onSelectAll() ) );
  myActions.insert( SelectAllId, a );

  a = new QAction( tr( "EDIT_SAVETOFILE_CMD" ), this );
  a->setStatusTip( tr( "EDIT_SAVETOFILE_CMD" ) );
  connect( a, SIGNAL( triggered( bool ) ), SLOT( onSaveToFile() ) );
  myActions.insert( SaveToFileId, a );
}

/*!
  \brief Create the context popup menu.

  Fill in the popup menu with the commands.

  \param menu context popup menu
*/
void LogWindow::contextMenuPopup( QMenu* popup )
{
  popup->addAction( myActions[ CopyId ] );
  popup->addAction( myActions[ ClearId ] );
  popup->addSeparator();
  popup->addAction( myActions[ SelectAllId ] );
  popup->addSeparator();
  popup->addAction( myActions[ SaveToFileId ] );

  Qtx::simplifySeparators( popup );

  updateActions();
}

/*!
  \brief Update menu actions.

  Update context popup menu action state.
*/
void LogWindow::updateActions()
{
  myActions[CopyId]->setEnabled( myView->textCursor().hasSelection() );
  myActions[ ClearId ]->setEnabled( myView->document()->blockCount() > myBannerSize );
  myActions[SelectAllId]->setEnabled( !myView->document()->isEmpty() );
  myActions[ SaveToFileId ]->setEnabled( myHistory.count() > 0 );
}

/*!
  \brief Called when user selects "Save To File" command in the popup menu.
*/
void LogWindow::onSaveToFile()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( !app )
    return;

  // call application-specific "Save file" dialog box
  QString aName = app->getFileName( false, QString(), QString( "*.log" ), QString(), 0 );
  if ( aName.isNull() )
    return;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  bool bOk = saveLog( aName );

  QApplication::restoreOverrideCursor();

  if ( !bOk )
    SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "ERR_CANT_SAVE_FILE" ) );
}

/*!
  \brief Called when user selects "Select all" command in the popup menu.
*/
void LogWindow::onSelectAll()
{
  if ( myView )
    myView->selectAll();
}

/*!
  \brief Called when user click "Clear" command in the popup menu.
*/
void LogWindow::onClear()
{
  clear( false );
}

/*!
  \brief Called when user click "Copy" command in the popup menu.
*/
void LogWindow::onCopy()
{
  if ( myView )
    myView->copy();
}

/*!
  \brief Set actions to be visible in the context popup menu.
  
  Actions, which IDs are set in \a flags parameter, will be shown in the 
  context popup menu. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void LogWindow::setMenuActions( const int flags )
{
  myActions[CopyId]->setVisible( flags & CopyId );
  myActions[ClearId]->setVisible( flags & ClearId );
  myActions[SelectAllId]->setVisible( flags & SelectAllId );
  myActions[SaveToFileId]->setVisible( flags & SaveToFileId );
}

/*!
  \brief Get menu actions which are currently visible in the context popup menu.
  \return ORed together actions flags
  \sa setMenuActions()
*/
int LogWindow::menuActions() const
{
  int ret = 0;
  ret = ret | ( myActions[CopyId]->isVisible() ? CopyId : 0 );
  ret = ret | ( myActions[ClearId]->isVisible() ? ClearId : 0 );
  ret = ret | ( myActions[SelectAllId]->isVisible() ? SelectAllId : 0 );
  ret = ret | ( myActions[SaveToFileId]->isVisible() ? SaveToFileId : 0 );
  return ret;
}

/*!
  \fn virtual QString LogWindow::popupClientType() const;
  \brief Get popup client symbolic name, used in popup menu management system.
  \return symbolic name
*/
