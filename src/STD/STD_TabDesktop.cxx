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

#include "STD_TabDesktop.h"

#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ResourceMgr.h>

#include <QtxWorkstack.h>
#include <QtxActionMenuMgr.h>
#include <QtxWorkstackAction.h>

#include <QFrame>
#include <QVBoxLayout>

#include <stdarg.h>

/*!Constructor.Create new instances of QVBox and QtxWorkstack.*/
STD_TabDesktop::STD_TabDesktop()
: SUIT_Desktop(),
myWorkstack( 0 ),
myWorkstackAction( 0 )
{
  QFrame* base = new QFrame( this );
  base->setFrameStyle( QFrame::Panel | QFrame::Sunken );

  QVBoxLayout* main = new QVBoxLayout( base );
  main->setMargin( 0 );

  setCentralWidget( base );

  myWorkstack = new QtxWorkstack( base );
  main->addWidget( myWorkstack );
  // setting Expanding size policy for central workstack.  If there are several widgets
  // in central area of Desktop, other widgets will be added below the workstack (CATHARE, TRIPOLI modules).
  // But the workstack must occupy as much space as possible -- set Expanding for it.
  myWorkstack->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  myWorkstack->setAccel( QtxWorkstack::SplitVertical,   Qt::ALT + Qt::SHIFT + Qt::Key_V );
  myWorkstack->setAccel( QtxWorkstack::SplitHorizontal, Qt::ALT + Qt::SHIFT + Qt::Key_H );
  //myWorkstack->setAccel( QtxWorkstack::Close,           Qt::CTRL + Qt::Key_F4 );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if ( resMgr ) {
    myWorkstack->setIcon( QtxWorkstack::SplitVertical,
                          resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_VSPLIT" ) ) );
    myWorkstack->setIcon( QtxWorkstack::SplitHorizontal,
                          resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_HSPLIT" ) ) );
    myWorkstack->setIcon( QtxWorkstack::Close,
                          resMgr->loadPixmap( "STD", tr( "ICON_FILE_CLOSE" ) ));
  }

  connect( myWorkstack, SIGNAL( windowActivated( QWidget* ) ),
           this, SLOT( onWindowActivated( QWidget* ) ) );

  createActions();
}

/*!
  Destructor.
*/
STD_TabDesktop::~STD_TabDesktop()
{
}

/*!
  \retval SUIT_ViewWindow - return const active window.
*/
SUIT_ViewWindow* STD_TabDesktop::activeWindow() const
{
  SUIT_ViewWindow* wnd = 0;

  QWidget* wid = myWorkstack->activeWindow();
  if ( wid && wid->inherits( "SUIT_ViewWindow" ) )
    wnd = (SUIT_ViewWindow*)wid;

  return wnd;
}

/*!
  Set active window
  \param wnd - view window
*/
void STD_TabDesktop::setActiveWindow(SUIT_ViewWindow* wnd)
{
  if (wnd) {
    myWorkstack->setActiveWindow(wnd);
    wnd->setFocus();
  }
}

/*!
  \retval QPtrList<SUIT_ViewWindow> - return const active window list.
*/
QList<SUIT_ViewWindow*> STD_TabDesktop::windows() const
{
  QList<SUIT_ViewWindow*> winList;

  QWidgetList children = myWorkstack->windowList();
  for ( QWidgetList::iterator it = children.begin(); it != children.end(); ++it )
  {
    if ( (*it)->inherits( "SUIT_ViewWindow" ) )
      winList.append( (SUIT_ViewWindow*)*it );
  }

  return winList;
}

/*!
  Insert new widget into desktop.
*/
void STD_TabDesktop::addWindow( QWidget* w )
{
  if ( !w || !workstack() )
    return;

  workstack()->addWindow( w );
}

/*!
  Call method perform for operation \a type.
*/
void STD_TabDesktop::windowOperation( const int type )
{
  myWorkstackAction->perform( operationFlag( type ) );
}

/*!
  Sets window operations by \a first ... parameters.
*/
void STD_TabDesktop::setWindowOperations( const int first, ... )
{
  va_list ints;
        va_start( ints, first );

        QList<int> typeList;

        int cur = first;
        while ( cur )
        {
          typeList.append( cur );
                cur = va_arg( ints, int );
  }

        setWindowOperations( typeList );
}

/*!
  Sets window operations by variable \a opList - operation list.
*/
void STD_TabDesktop::setWindowOperations( const QList<int>& opList )
{
  int flags = 0;

  for ( QList<int>::const_iterator it = opList.begin(); it != opList.end(); ++it )
    flags = flags | operationFlag( *it );

//  myWorkstackAction->setItems( flags );
}

/*!
  \retval QtxWorkstack pointer - Qt work stack.
*/
QtxWorkstack* STD_TabDesktop::workstack() const
{
  return myWorkstack;
}

/*!
  Emit window activated.
*/
void STD_TabDesktop::onWindowActivated( QWidget* w )
{
  if ( w && w->inherits( "SUIT_ViewWindow" ) )
    emit windowActivated( (SUIT_ViewWindow*)w );
}

/*!
  Create actions for window.
*/
void STD_TabDesktop::createActions()
{
  if ( myWorkstackAction )
    return;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if ( !resMgr )
    return;

  myWorkstackAction = new QtxWorkstackAction( workstack(), this );

  myWorkstackAction->setMenuActions( QtxWorkstackAction::Split | QtxWorkstackAction::Windows );

  // Split Horizontal
  myWorkstackAction->setIcon( QtxWorkstackAction::SplitHorizontal,
                              resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_HSPLIT" ) ) );
  myWorkstackAction->setText( QtxWorkstackAction::SplitHorizontal, tr( "MEN_DESK_WINDOW_HSPLIT" ) );
  myWorkstackAction->setStatusTip( QtxWorkstackAction::SplitHorizontal, tr( "PRP_DESK_WINDOW_HSPLIT" ) );
  myWorkstackAction->setAccel( QtxWorkstackAction::SplitHorizontal, Qt::ALT + Qt::SHIFT + Qt::Key_H );

  // Split Vertical
  myWorkstackAction->setIcon( QtxWorkstackAction::SplitVertical,
                              resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_VSPLIT" ) ) );
  myWorkstackAction->setText( QtxWorkstackAction::SplitVertical, tr( "MEN_DESK_WINDOW_VSPLIT" ) );
  myWorkstackAction->setStatusTip( QtxWorkstackAction::SplitVertical, tr( "PRP_DESK_WINDOW_VSPLIT" ) );
  myWorkstackAction->setAccel( QtxWorkstackAction::SplitVertical,   Qt::ALT + Qt::SHIFT + Qt::Key_V );

  QAction* anArrangeViewsAction = myWorkstackAction->getArrangeViewsAction();
  if( anArrangeViewsAction )
    connect( anArrangeViewsAction, SIGNAL( triggered() ), this, SLOT( onArrangeViews() ) );

  QtxActionMenuMgr* mMgr = menuMgr();
  if ( !mMgr )
    return;

  int winMenuId = mMgr->insert( tr( "MEN_DESK_WINDOW" ), -1, 100 );
  mMgr->insert( anArrangeViewsAction, winMenuId, -1 );
  mMgr->insert( myWorkstackAction, winMenuId, -1 );
  mMgr->insert( QtxActionMenuMgr::separator(), winMenuId, -1 );
}

/*!
  Emit Arrange Views menu activated.
*/
void STD_TabDesktop::onArrangeViews()
{
  QtxSplitDlg ArrangeViewsDlg( this, workstack(), ArrangeViews );
  ArrangeViewsDlg.exec();
}

/*!
  Convert STD_TabDesktop enumerations to QtxWorkstackAction
*/
int STD_TabDesktop::operationFlag( const int type ) const
{
  int res = 0;
  switch ( type )
  {
  case SplitVertical:
    res = QtxWorkstackAction::SplitVertical;
    break;
  case SplitHorizontal:
    res = QtxWorkstackAction::SplitHorizontal;
    break;
  }

  return res;
}
