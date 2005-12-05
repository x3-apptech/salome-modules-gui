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
// See http://www.salome-platform.org/
//
#include "SUIT_Application.h"

#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SUIT_ResourceMgr.h"

#include <qlabel.h>
#include <qtimer.h>
#include <qstatusbar.h>

#include <QtxAction.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

SUIT_Application::SUIT_Application()
: QObject( 0 ),
myStudy( 0 ),
myDesktop( 0 ),
myStatusLabel( 0 )
{ 
}

SUIT_Application::~SUIT_Application() 
{
  delete myStudy;
  myStudy = 0;

  setDesktop( 0 );
}

SUIT_Desktop* SUIT_Application::desktop()
{
  return myDesktop;
}

bool SUIT_Application::isPossibleToClose()
{
  return true;
}

void SUIT_Application::closeApplication()
{
  emit applicationClosed( this );
}

SUIT_Study* SUIT_Application::activeStudy() const
{
  return myStudy;
}

QString SUIT_Application::applicationVersion() const
{
  return QString::null;
}

void SUIT_Application::start()
{
  if ( desktop() )
    desktop()->show();
}

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

bool SUIT_Application::useStudy( const QString& theName )
{
  return false;
}

void SUIT_Application::createEmptyStudy()
{
  if ( !activeStudy() )
    setActiveStudy( createNewStudy() );
}

int SUIT_Application::getNbStudies() const
{
  return activeStudy() ? 1 : 0;
}

SUIT_ResourceMgr* SUIT_Application::resourceMgr() const
{
  if ( !SUIT_Session::session() )
    return 0;

  return SUIT_Session::session()->resourceMgr();
}

#define DEFAULT_MESSAGE_DELAY 3000
void SUIT_Application::putInfo ( const QString& msg, const int msec )
{
  if ( desktop() ) {
    //desktop()->statusBar()->message( msg, msec <= 0 ? DEFAULT_MESSAGE_DELAY : msec );
    if ( !myStatusLabel ) {
      myStatusLabel = new QLabel (desktop()->statusBar());
      desktop()->statusBar()->addWidget(myStatusLabel, /*int stretch = */1);
    }
    myStatusLabel->setText(msg);
    if( msec != -1 )
      QTimer::singleShot(msec <= 0 ? DEFAULT_MESSAGE_DELAY : msec, myStatusLabel, SLOT(clear()));
  }
}

SUIT_Application* SUIT_Application::startApplication( int argc, char** argv ) const
{
  return startApplication( name(), argc, argv );
}

SUIT_Application* SUIT_Application::startApplication( const QString& name, int argc, char** argv ) const
{
  SUIT_Session* session = SUIT_Session::session();
  if ( !session )
    return 0;

  return session->startApplication( name, argc, argv );
}

void SUIT_Application::setDesktop( SUIT_Desktop* desk )
{
  if ( myDesktop == desk )
    return;

  delete myDesktop;
  myDesktop = desk;
  if ( myDesktop )
    connect( myDesktop, SIGNAL( activated() ), this, SLOT( onDesktopActivated() ) );
}

SUIT_Study* SUIT_Application::createNewStudy()
{
  return new SUIT_Study( this );
}

void SUIT_Application::setActiveStudy( SUIT_Study* study )
{
  if ( myStudy == study )
    return;

  myStudy = study;
}

int SUIT_Application::createTool( const QString& name )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  return desktop()->toolMgr()->createToolBar( name );
}

int SUIT_Application::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  int regId = desktop()->toolMgr()->registerAction( a, id );
  return desktop()->toolMgr()->insert( regId, tBar, idx );
}

int SUIT_Application::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  return desktop()->toolMgr()->insert( a, tBar, idx );
}

int SUIT_Application::createTool( const int id, const int tBar, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  return desktop()->toolMgr()->insert( id, tBar, idx );
}

int SUIT_Application::createTool( const int id, const QString& tBar, const int idx )
{
  if ( !desktop() || !desktop()->toolMgr() )
    return -1;

  return desktop()->toolMgr()->insert( id, tBar, idx );
}

int SUIT_Application::createMenu( const QString& subMenu, const int menu,
                                  const int id, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  return desktop()->menuMgr()->insert( subMenu, menu, group, index );
}

int SUIT_Application::createMenu( const QString& subMenu, const QString& menu,
                                  const int id, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  return desktop()->menuMgr()->insert( subMenu, menu, group, index );
}

int SUIT_Application::createMenu( QAction* a, const int menu, const int id, const int group, const int index )
{
  if ( !a || !desktop() || !desktop()->menuMgr() )
    return -1;

  int regId = desktop()->menuMgr()->registerAction( a, id );
  return desktop()->menuMgr()->insert( regId, menu, group, index );
}

int SUIT_Application::createMenu( QAction* a, const QString& menu, const int id, const int group, const int index )
{
  if ( !a || !desktop() || !desktop()->menuMgr() )
    return -1;

  int regId = desktop()->menuMgr()->registerAction( a, id );
  return desktop()->menuMgr()->insert( regId, menu, group, index );
}

int SUIT_Application::createMenu( const int id, const int menu, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  return desktop()->menuMgr()->insert( id, menu, group, index );
}

int SUIT_Application::createMenu( const int id, const QString& menu, const int group, const int index )
{
  if ( !desktop() || !desktop()->menuMgr() )
    return -1;

  return desktop()->menuMgr()->insert( id, menu, group, index );
}

void SUIT_Application::setMenuShown( QAction* a, const bool on )
{
  setMenuShown( actionId( a ), on );
}

void SUIT_Application::setMenuShown( const int id, const bool on )
{
  if ( desktop() && desktop()->menuMgr() )
    desktop()->menuMgr()->setShown( id, on );
}

void SUIT_Application::setToolShown( QAction* a, const bool on )
{
  setToolShown( actionId( a ), on );
}

void SUIT_Application::setToolShown( const int id, const bool on )
{
  if ( desktop() && desktop()->toolMgr() )
    desktop()->toolMgr()->setShown( id, on );
}

void SUIT_Application::setActionShown( QAction* a, const bool on )
{
  setMenuShown( a, on );
  setToolShown( a, on );
}

void SUIT_Application::setActionShown( const int id, const bool on )
{
  setMenuShown( id, on );
  setToolShown( id, on );
}

QAction* SUIT_Application::action( const int id ) const
{
  SUIT_Application* that = (SUIT_Application*)this;
  SUIT_Desktop* desk = that->desktop();
  if ( !desk )
    return 0;

  QAction* a = 0;
  if ( desk->menuMgr() )
    a = desk->menuMgr()->action( id );
  if ( !a && desk->toolMgr() )
    a = desk->toolMgr()->action( id );
  return a;
}

int SUIT_Application::actionId( const QAction* a ) const
{
  SUIT_Application* that = (SUIT_Application*)this;
  SUIT_Desktop* desk = that->desktop();
  if ( !desk )
    return 0;

  int id = -1;
  if ( desk->menuMgr() )
    id = desk->menuMgr()->actionId( a );
  if ( id == -1 && desk->toolMgr() )
    id = desk->toolMgr()->actionId( a );
  return id;
}

QAction* SUIT_Application::createAction( const int id, const QString& text, const QIconSet& icon,
                                         const QString& menu, const QString& tip, const int key,
                                         QObject* parent, const bool toggle, QObject* reciever, const char* member )
{
  QtxAction* a = new QtxAction( text, icon, menu, key, parent, 0, toggle );
  a->setStatusTip( tip );

  if ( reciever && member )
    connect( a, SIGNAL( activated() ), reciever, member );

  registerAction( id, a );

  return a;
}

void SUIT_Application::registerAction( const int id, QAction* a )
{
  if ( desktop() && desktop()->menuMgr() )
    desktop()->menuMgr()->registerAction( a, id );

  if ( desktop() && desktop()->toolMgr() )
    desktop()->toolMgr()->registerAction( a, id );
}

QAction* SUIT_Application::separator()
{
  return QtxActionMgr::separator();
}

void SUIT_Application::onDesktopActivated()
{
  emit activated( this );
}
