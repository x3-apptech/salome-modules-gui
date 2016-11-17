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

#include "SUIT_Application.h"

#include "SUIT_Study.h"
#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_ShortcutMgr.h"

#include <QTimer>
#include <QLabel>
#include <QStatusBar>
#include <QApplication>
#include <QSize>

#include <QtxAction.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>


/*!
  \class StatusLabel
  \brief Status bar customization label. Used to workaroubd desktop resizing bug.
  \internal
*/
class StatusLabel : public QLabel
{
public:
  StatusLabel( QWidget* parent ) : QLabel( parent ) {}
  QSize minimumSizeHint () const { return QSize( 0, QLabel::minimumSizeHint().height() ); }
};

/*!
  Default constructor
*/
SUIT_Application::SUIT_Application()
: QObject( 0 ),
  myStudy( 0 ),
  myDesktop( 0 ),
  myStatusLabel( 0 ),
  myPostRoutines( QList<PostRoutine>() )
{
  if ( SUIT_Session::session() )
    SUIT_Session::session()->insertApplication( this );
}

/*!
  Destructor
*/
SUIT_Application::~SUIT_Application()
{
  SUIT_Study* s = myStudy;
  setActiveStudy( 0 );
  delete s;

  setDesktop( 0 );

  foreach ( PostRoutine routine, myPostRoutines )
    routine();
}

/*!
  \return main window of application (desktop)
*/
SUIT_Desktop* SUIT_Application::desktop()
{
  return myDesktop;
}

/*!
   \return \c false if application can not be closed (because of non saved data for example).
   This method called by SUIT_Session when closing of application was requested.
*/
bool SUIT_Application::isPossibleToClose( bool& )
{
  return true;
}

/*!
  Performs some finalization of life cycle of this application.
  For instance, the application can force its documents(s) to close.
*/
void SUIT_Application::closeApplication()
{
  emit applicationClosed( this );
}

/*!
  \return active Study. If Application supports wirking with several studies this method should be redefined
*/
SUIT_Study* SUIT_Application::activeStudy() const
{
  return myStudy;
}

/*!
  \return version of application
*/
QString SUIT_Application::applicationVersion() const
{
  return QString();
}

/*!
  Shows the application's main widget. For non GUI application must be redefined.
*/
void SUIT_Application::start()
{
  if ( desktop() )
    desktop()->show();

  // Initialize shortcut manager
  SUIT_ShortcutMgr::Init();
}

/*!
  Opens document into active Study. If Study is empty - creates it.
  \param theFileName - name of document file
*/
bool SUIT_Application::useFile( const QString& theFileName )
{
  createEmptyStudy();
  SUIT_Study* study = activeStudy();

  bool status = study ? study->openDocument( theFileName ) : false;

  if ( !status )
  {
    setActiveStudy( 0 );
    delete study;
  }

  return status;
}

/*!
  Creates new empty Study if active Study = 0
*/
void SUIT_Application::createEmptyStudy()
{
  if ( !activeStudy() )
    setActiveStudy( createNewStudy() );
}

/*!
  \return number of Studies.
  Must be redefined in Applications which support several studies for one Application instance.
*/
int SUIT_Application::getNbStudies() const
{
  return activeStudy() ? 1 : 0;
}

/*!
  \return global resource manager
*/
SUIT_ResourceMgr* SUIT_Application::resourceMgr() const
{
  if ( !SUIT_Session::session() )
    return 0;

  return SUIT_Session::session()->resourceMgr();
}

/*!
  \brief Get access to shortcut manager.
  \return global shortcut manager
*/
SUIT_ShortcutMgr* SUIT_Application::shortcutMgr() const
{
  return SUIT_ShortcutMgr::getShortcutMgr();
}

#define DEFAULT_MESSAGE_DELAY 3000

/*!
  Puts the message to the status bar
  \param msg - text of message
  \param msec - time in milliseconds, after that the status label will be cleared
*/
void SUIT_Application::putInfo( const QString& msg, const int msec )
{
  if ( !desktop() )
    return;

  if ( !myStatusLabel )
  {
    myStatusLabel = new StatusLabel( desktop()->statusBar() );
    desktop()->statusBar()->addWidget( myStatusLabel, 1 );
    myStatusLabel->show();
  }

  QString prev = myStatusLabel->text();

  myStatusLabel->setText( msg );
  if ( msec != -1 )
    QTimer::singleShot( msec <= 0 ? DEFAULT_MESSAGE_DELAY : msec, this, SLOT( onInfoClear() ) );

  if ( prev != msg )
    emit infoChanged( msg );
}

/*!
  Clear the information label in status bar after delay.
*/
void SUIT_Application::onInfoClear()
{
  if ( !myStatusLabel )
    return;

  bool changed = !myStatusLabel->text().isEmpty();
  myStatusLabel->clear();
  if ( changed )
    emit infoChanged( QString() );
}

/*!
  Update status of the registerd actions
*/
void SUIT_Application::updateCommandsStatus()
{
}

/*!
  Initialize with application arguments
  \param argc - number of application arguments
  \param argv - array of application arguments
*/
SUIT_Application* SUIT_Application::startApplication( int argc, char** argv ) const
{
  return startApplication( objectName(), argc, argv );
}

/*!
  Initialize with application name and arguments
  \param name - name of application
  \param argc - number of application arguments
  \param argv - array of application arguments
*/
SUIT_Application* SUIT_Application::startApplication( const QString& name, int argc, char** argv ) const
{
  SUIT_Session* session = SUIT_Session::session();
  if ( !session )
    return 0;

  return session->startApplication( name, argc, argv );
}

/*!
  Sets the main window of application
  \param desk - new main window (desktop)
*/
void SUIT_Application::setDesktop( SUIT_Desktop* desk )
{
  if ( myDesktop == desk )
    return;

  // >> VSR 09/06/2009: workaround about the Qt 4.5.0 bug: SIGSEGV on desktop delete
  myDesktop->deleteLater(); // delete myDesktop;
  // << VSR 09/06/2009
  myDesktop = desk;
  if ( myDesktop ) {
    connect( myDesktop, SIGNAL( activated() ), this, SLOT( onDesktopActivated() ) );
//    connect( myDesktop, SIGNAL( moved() ), this, SLOT( onDesktopMoved() ) );
    // Force desktop activation (NPAL16628)
    QApplication::postEvent(myDesktop, new QEvent(QEvent::WindowActivate));
  }
}

/*!
  Creates new instance of study.
  By default, it is called from createEmptyStudy()
  \sa createEmptyStudy()
*/
SUIT_Study* SUIT_Application::createNewStudy()
{
  return new SUIT_Study( this );
}

/*!
  Sets study as active
  \param study - instance of study to be set as active
*/
void SUIT_Application::setActiveStudy( SUIT_Study* study )
{
  if ( myStudy == study )
    return;

  if ( myStudy )
    disconnect( myStudy, SIGNAL( studyModified( SUIT_Study* ) ),
                this, SLOT( updateCommandsStatus() ) );
  if ( study )
    connect( study, SIGNAL( studyModified( SUIT_Study* ) ),
             this, SLOT( updateCommandsStatus() ) );

  myStudy = study;
}

/*!
  Creates new toolbar
  \return identificator of new toolbar in tool manager
  \param title - title of new toolbar
  \param name - name (identifier) of new toolbar
*/
int SUIT_Application::createTool( const QString& title, const QString& name )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  return desktop()->toolMgr()->createToolBar( title, name );
}

/*!
  Creates new toolbutton
  \return SUIT identificator of new action
  \param a - action
  \param tBar - identificator of toolbar
  \param id - proposed SUIT identificator of action (if it is -1, then must be use any free)
  \param idx - index in toolbar
*/
int SUIT_Application::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = desktop()->toolMgr()->insert( a, tBar, idx );
  return intId != -1 ? regId : -1;
}

/*!
  Creates new toolbutton
  \return SUIT identificator of new action
  \param a - action
  \param tBar - name of toolbar
  \param id - proposed SUIT identificator of action (if it is -1, then must be use any free)
  \param idx - index in toolbar
*/
int SUIT_Application::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = desktop()->toolMgr()->insert( a, tBar, idx );
  return intId != -1 ? regId : -1;
}

/*!
  Creates new toolbutton
  \return "id" if all right or -1 otherwise
  \param id - SUIT identificator of action
  \param tBar - identificator of toolbar
  \param idx - index in toolbar
*/
int SUIT_Application::createTool( const int id, const int tBar, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  int intId = desktop()->toolMgr()->insert( action( id ), tBar, idx );
  return intId != -1 ? id : -1;
}

/*!
  Creates new toolbutton
  \return "id" if all right or -1 otherwise
  \param id - SUIT identificator of action
  \param tBar - name of toolbar
  \param idx - index in toolbar
*/
int SUIT_Application::createTool( const int id, const QString& tBar, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  int intId = desktop()->toolMgr()->insert( action( id ), tBar, idx );
  return intId != -1 ? id : -1;
}

/*!
  Creates new menu item
  \return identificator of new action in menu manager
  \param subMenu - menu text of new item
  \param menu - identificator of parent menu item
  \param id - proposed identificator of action
  \param group - group in menu manager
  \param index - index in menu
*/
int SUIT_Application::createMenu( const QString& subMenu, const int menu,
                                  const int id, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  return desktop()->menuMgr()->insert( subMenu, menu, group, id, index );
}

/*!
  Creates new menu item
  \return identificator of new action in menu manager
  \param subMenu - menu text of new item
  \param menu - menu text of parent menu item
  \param id - proposed identificator of action
  \param group - group in menu manager
  \param index - index in menu
*/
int SUIT_Application::createMenu( const QString& subMenu, const QString& menu,
                                  const int id, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  return desktop()->menuMgr()->insert( subMenu, menu, group, id, index );
}

/*!
  Creates new menu item
  \return SUIT identificator of new action
  \param a - action
  \param menu - identificator of parent menu item
  \param id - proposed SUIT identificator of action
  \param group - group in menu manager
  \param index - index in menu
*/
int SUIT_Application::createMenu( QAction* a, const int menu, const int id, const int group, const int index )
{
  if ( !a || !desktop() || !desktop()->menuMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = desktop()->menuMgr()->insert( a, menu, group, index );
  return intId != -1 ? regId : -1;
}

/*!
  Creates new menu item
  \return SUIT identificator of new action
  \param a - action
  \param menu - menu text of parent menu item
  \param id - proposed SUIT identificator of action
  \param group - group in menu manager
  \param index - index in menu
*/
int SUIT_Application::createMenu( QAction* a, const QString& menu, const int id, const int group, const int index )
{
  if ( !a || !desktop() || !desktop()->menuMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = desktop()->menuMgr()->insert( a, menu, group, index );
  return intId != -1 ? regId : -1;
}

/*!
  Creates new menu item
  \return identificator of new action in menu manager
  \param id - SUIT identificator of action
  \param menu - menu text of parent menu item
  \param group - group in menu manager
  \param index - index in menu
*/
int SUIT_Application::createMenu( const int id, const int menu, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  int intId = desktop()->menuMgr()->insert( action( id ), menu, group, index );
  return intId != -1 ? id : -1;
}

/*!
  Creates new menu item
  \return identificator of new action in menu manager
  \param id - SUIT identificator of action
  \param menu - menu text of parent menu item
  \param group - group in menu manager
  \param index - index in menu
*/
int SUIT_Application::createMenu( const int id, const QString& menu, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  int intId = desktop()->menuMgr()->insert( action( id ), menu, group, index );
  return intId != -1 ? id : -1;
}

/*!
  Show/hide menu item corresponding to action
  \param a - action
  \param on - if it is \c true, the item will be shown, otherwise it will be hidden
*/
void SUIT_Application::setMenuShown( QAction* a, const bool on )
{
  if ( !a || !desktop() )
    return;

  QtxActionMenuMgr* mMgr = desktop()->menuMgr();
  if ( mMgr )
    mMgr->setShown( mMgr->actionId( a ), on );
}

/*!
  Show/hide menu item corresponding to action
  \param id - identificator of action in menu manager
  \param on - if it is \c true, the item will be shown, otherwise it will be hidden
*/
void SUIT_Application::setMenuShown( const int id, const bool on )
{
  setMenuShown( action( id ), on );
}

/*!
  Show/hide tool button corresponding to action
  \param a - action
  \param on - if it is \c true, the button will be shown, otherwise it will be hidden
*/
void SUIT_Application::setToolShown( QAction* a, const bool on )
{
  if ( !a || !desktop() )
    return;

  QtxActionToolMgr* tMgr = desktop()->toolMgr();
  if ( tMgr )
    tMgr->setShown( tMgr->actionId( a ), on );
}

/*!
  Show/hide menu item corresponding to action
  \param id - identificator of action in tool manager
  \param on - if it is \c true, the button will be shown, otherwise it will be hidden
*/
void SUIT_Application::setToolShown( const int id, const bool on )
{
  setToolShown( action( id ), on );
}

/*!
  Show/hide both menu item and tool button corresponding to action
  \param a - action
  \param on - if it is \c true, the item will be shown, otherwise it will be hidden
*/
void SUIT_Application::setActionShown( QAction* a, const bool on )
{
  setMenuShown( a, on );
  setToolShown( a, on );
}

/*!
  Show/hide both menu item and tool button corresponding to action
  \param id - identificator in both menu manager and tool manager
  \param on - if it is \c true, the item will be shown, otherwise it will be hidden
*/
void SUIT_Application::setActionShown( const int id, const bool on )
{
  setMenuShown( id, on );
  setToolShown( id, on );
}

/*!
  \return action by it's SUIT identificator
  \param id - SUIT identificator
*/
QAction* SUIT_Application::action( const int id ) const
{
  QAction* a = 0;
  if ( myActionMap.contains( id ) )
    a = myActionMap[id];
  return a;
}

/*!
  \return SUIT identificator of action
  \param a - action
*/
int SUIT_Application::actionId( const QAction* a ) const
{
  int id = -1;
  for ( QMap<int, QAction*>::ConstIterator it = myActionMap.begin(); it != myActionMap.end() && id == -1; ++it )
  {
    if ( it.value() == a )
      id = it.key();
  }
  return id;
}

QList<QAction*> SUIT_Application::actions() const
{
  return myActionMap.values();
}

QList<int> SUIT_Application::actionIds() const
{
  return myActionMap.keys();
}

/*!
  Creates action and registers it both in menu manager and tool manager
  \return new instance of action
  \param id - proposed SUIT identificator
  \param text - description
  \param icon - icon for toolbar
  \param menu - menu text
  \param tip - tool tip
  \param key - shortcut
  \param parent - parent object
  \param toggle - if it is \c true the action will be a toggle action, otherwise it will be a command action
  \param reciever - object that contains slot
  \param member - slot to be called when action is activated
*/
QAction* SUIT_Application::createAction( const int id, const QString& text, const QIcon& icon,
                                         const QString& menu, const QString& tip, const int key,
                                         QObject* parent, const bool toggle, QObject* reciever, 
					 const char* member, const QString& shortcutAction )
{
  return createAction( id, text, icon, menu, tip, QKeySequence(key), parent, toggle, reciever, member, shortcutAction );
}

/*!
  Creates action and registers it both in menu manager and tool manager
  \return new instance of action
  \param id - proposed SUIT identificator
  \param text - description
  \param icon - icon for toolbar
  \param menu - menu text
  \param tip - tool tip
  \param key - shortcut
  \param parent - parent object
  \param toggle - if it is TRUE the action will be a toggle action, otherwise it will be a command action
  \param reciever - object that contains slot
  \param member - slot to be called when action is activated
*/
QAction* SUIT_Application::createAction( const int id, const QString& text, const QIcon& icon,
                                         const QString& menu, const QString& tip, const QKeySequence& key,
                                         QObject* parent, const bool toggle, QObject* reciever, 
					 const char* member, const QString& shortcutAction )
{
  QtxAction* a = new QtxAction( text, icon, menu, key, parent, toggle, shortcutAction );
  a->setStatusTip( tip );

  if ( reciever && member )
    connect( a, SIGNAL( triggered( bool ) ), reciever, member );

  registerAction( id, a );

  return a;
}

/*!
  Registers action both in menu manager and tool manager
  \param id - proposed SUIT identificator (if it is -1, auto generated one is used)
  \param a - action
*/
int SUIT_Application::registerAction( const int id, QAction* a )
{
  int ident = actionId( a );
  if ( ident != -1 )
    return ident;

  static int generatedId = -1;
  ident = id == -1 ? --generatedId : id;

  if ( action( ident ) )
    qWarning( "Action registration id is already in use: %d", ident );

  myActionMap.insert( ident, a );

  if ( desktop() && desktop()->menuMgr() )
    desktop()->menuMgr()->registerAction( a );

  if ( desktop() && desktop()->toolMgr() )
    desktop()->toolMgr()->registerAction( a );

  if ( desktop() )
    desktop()->addAction( a );

  return ident;
}

/*!
  \return global action used as separator
*/
QAction* SUIT_Application::separator()
{
  return QtxActionMgr::separator();
}

/*!
  SLOT: it is called when desktop is activated
*/

void SUIT_Application::onDesktopActivated()
{
  emit activated( this );
}

/*!
  SLOT: it is called when desktop is moved
*/
/*void SUIT_Application::onDesktopMoved()
{
  emit moving();
}*/
/*!
  SLOT: is used for Help browsing
*/
void SUIT_Application::onHelpContextModule( const QString& /*theComponentName*/,
                                            const QString& /*theFileName*/,
                                            const QString& /*theContext*/ )
{
}

void SUIT_Application::addPostRoutine( PostRoutine theRoutine )
{
  if ( !myPostRoutines.contains( theRoutine ) )
    myPostRoutines << theRoutine;
}
