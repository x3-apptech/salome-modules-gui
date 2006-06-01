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
#include "STD_TabDesktop.h"

#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ResourceMgr.h>

#include <QtxAction.h>
#include <QtxWorkstack.h>
#include <QtxActionMenuMgr.h>
#include <QtxWorkstackAction.h>

#include <qvbox.h>
#include <qmenubar.h>
#include <qworkspace.h>
#include <qobjectlist.h>

#include <stdarg.h>

/*!Constructor.Create new instances of QVBox and QtxWorkstack.*/
STD_TabDesktop::STD_TabDesktop()
: SUIT_Desktop(),
myWorkstack( 0 ),
myWorkstackAction( 0 )
{
  QVBox* base = new QVBox( this );
  base->setFrameStyle( QFrame::Panel | QFrame::Sunken );

  setCentralWidget( base );

  myWorkstack = new QtxWorkstack( base );
  // setting Expanding size policy for central workstack.  If there are several widgets
  // in central area of Desktop, other widgets will be added below the workstack (CATHARE, TRIPOLI modules).  
  // But the workstack must occupy as much space as possible -- set Expanding for it.
  myWorkstack->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

  myWorkstack->setAccel(QtxWorkstack::SplitVertical,   SHIFT + Key_V);
  myWorkstack->setAccel(QtxWorkstack::SplitHorizontal, SHIFT + Key_H);
  myWorkstack->setAccel(QtxWorkstack::Close,           SHIFT + Key_C);

  connect( myWorkstack, SIGNAL( windowActivated( QWidget* ) ),
           this, SLOT( onWindowActivated( QWidget* ) ) );

  createActions();
}

/*!Destructor.*/
STD_TabDesktop::~STD_TabDesktop()
{
}

/*!\retval SUIT_ViewWindow - return const active window.*/
SUIT_ViewWindow* STD_TabDesktop::activeWindow() const
{
  SUIT_ViewWindow* wnd = 0;

  QWidget* wid = myWorkstack->activeWindow();
  if ( wid && wid->inherits( "SUIT_ViewWindow" ) )
    wnd = (SUIT_ViewWindow*)wid;

  return wnd;
}

/*!\retval QPtrList<SUIT_ViewWindow> - return const active window list.*/
QPtrList<SUIT_ViewWindow> STD_TabDesktop::windows() const
{
  QPtrList<SUIT_ViewWindow> winList;

  QWidgetList children = myWorkstack->windowList();
  for ( QWidgetListIt it( children ); it.current(); ++it )
  {
    if ( it.current()->inherits( "SUIT_ViewWindow" ) )
      winList.append( (SUIT_ViewWindow*)it.current() );
  }

  return winList;
}

/*!\retval QWidget pointer - QT work stack.*/
QWidget* STD_TabDesktop::parentArea() const
{
  return workstack();
}

/*!Call method perform for operation \a type.*/
void STD_TabDesktop::windowOperation( const int type )
{
  myWorkstackAction->perform( operationFlag( type ) );
}

/*!Sets window operations by \a first ... parameters.*/
void STD_TabDesktop::setWindowOperations( const int first, ... )
{
  va_list ints;
	va_start( ints, first );

	QValueList<int> typeList;

	int cur = first;
	while ( cur )
	{
	  typeList.append( cur );
		cur = va_arg( ints, int );
  }

	setWindowOperations( typeList );
}

/*!Sets window operations by variable \a opList - operation list.*/
void STD_TabDesktop::setWindowOperations( const QValueList<int>& opList )
{
  int flags = 0;

  for ( QValueList<int>::const_iterator it = opList.begin(); it != opList.end(); ++it )
    flags = flags | operationFlag( *it );

  myWorkstackAction->setItems( flags );
}

/*!\retval QtxWorkstack pointer - QT work stack.*/
QtxWorkstack* STD_TabDesktop::workstack() const
{
  return myWorkstack;
}

/*!Emit window activated.*/
void STD_TabDesktop::onWindowActivated( QWidget* w )
{
  if ( w && w->inherits( "SUIT_ViewWindow" ) )
    emit windowActivated( (SUIT_ViewWindow*)w );
}

/*!Create actions for window.*/
void STD_TabDesktop::createActions()
{
  if ( myWorkstackAction )
    return;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  if ( !resMgr )
    return;

  myWorkstackAction = new QtxWorkstackAction( workstack(), this );

  myWorkstackAction->setItems( QtxWorkstackAction::Split | QtxWorkstackAction::Windows );

  // Split Horizontal
  myWorkstackAction->setIconSet( QtxWorkstackAction::HSplit,
                                 resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_HSPLIT" ) ) );
  myWorkstackAction->setMenuText( QtxWorkstackAction::HSplit, tr( "MEN_DESK_WINDOW_HSPLIT" ) );
  myWorkstackAction->setStatusTip( QtxWorkstackAction::HSplit, tr( "PRP_DESK_WINDOW_HSPLIT" ) );

  // Split Vertical
  myWorkstackAction->setIconSet( QtxWorkstackAction::VSplit,
                                 resMgr->loadPixmap( "STD", tr( "ICON_DESK_WINDOW_VSPLIT" ) ) );
  myWorkstackAction->setMenuText( QtxWorkstackAction::VSplit, tr( "MEN_DESK_WINDOW_VSPLIT" ) );
  myWorkstackAction->setStatusTip( QtxWorkstackAction::VSplit, tr( "PRP_DESK_WINDOW_VSPLIT" ) );

  QtxActionMenuMgr* mMgr = menuMgr();
  if ( !mMgr )
    return;

  int winMenuId = mMgr->insert( tr( "MEN_DESK_WINDOW" ), -1, 100, MenuWindowId );
  mMgr->insert( myWorkstackAction, winMenuId, -1 );
  mMgr->insert( QtxActionMenuMgr::separator(), winMenuId, -1 );
}

/*!Convert STD_TabDesktop enumerations to QtxWorkstackAction*/
int STD_TabDesktop::operationFlag( const int type ) const
{
  int res = 0;
  switch ( type )
  {
  case VSplit:
    res = QtxWorkstackAction::VSplit;
    break;
  case HSplit:
    res = QtxWorkstackAction::HSplit;
    break;
  }
  return res;
}
