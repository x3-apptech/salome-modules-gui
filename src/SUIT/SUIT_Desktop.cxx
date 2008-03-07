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
#include "SUIT_Desktop.h"

#include "SUIT_Tools.h"
#include "SUIT_ViewWindow.h"

#include <QtxLogoMgr.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

/*!\class SUIT_Desktop
 * Provide desktop management:\n
 * \li menu manager
 * \li tool manager
 * \li windows
 */

/*!
  Constructor.
*/
SUIT_Desktop::SUIT_Desktop()
: QtxMainWindow()
{
  myMenuMgr = new QtxActionMenuMgr( this );
  myToolMgr = new QtxActionToolMgr( this );
  myLogoMgr = new QtxLogoMgr( menuBar() );
}

/*!
  Destructor.
*/
SUIT_Desktop::~SUIT_Desktop()
{
}

/*!
  Emit on event \a e.
*/
bool SUIT_Desktop::event( QEvent* e )
{
  if ( !e )
    return false;

  switch ( e->type() )
  {
  case QEvent::WindowActivate:
    emit activated();
    break;
  case QEvent::WindowDeactivate:
    emit deactivated();
    break;
  }

  return QMainWindow::event( e );
}

/*!
  Close event \a e.
*/
void SUIT_Desktop::closeEvent( QCloseEvent* e )
{
  emit closing( this, e );
  e->ignore();
}

/*!
  Child event.
*/
void SUIT_Desktop::childEvent( QChildEvent* e )
{
  if ( e->type() == QEvent::ChildInserted && parentArea() &&
       e->child()->isWidgetType() && e->child()->inherits( "SUIT_ViewWindow" ) )
  {
    QWidget* wid = (QWidget*)e->child();
    bool vis = wid->isVisibleTo( wid->parentWidget() );
    wid->reparent( parentArea(), QPoint( 0, 0 ), vis );
    wid->setShown( vis );
  }
  else
    QtxMainWindow::childEvent( e );
}

/*!
  Gets menu manager.
*/
QtxActionMenuMgr* SUIT_Desktop::menuMgr() const
{
  return myMenuMgr;
}

/*!
  Gets tool manager.
*/
QtxActionToolMgr* SUIT_Desktop::toolMgr() const
{
  return myToolMgr;
}

/*!
  Returns the count of the existed logos.
*/
int SUIT_Desktop::logoCount() const
{
  if ( !myLogoMgr )
    return 0;
  else
    return myLogoMgr->count();
}

/*!
  Adds new logo to the menu bar area.
  Obsolete. Not should be used.
  Use SUIT_Desktop::logoInsert();
*/
void SUIT_Desktop::addLogo( const QString& id, const QPixmap& pix )
{
  logoInsert( id, pix );
}

/*!
  Removes a logo.
  Obsolete. Not should be used.
  Use SUIT_Desktop::logoRemove();
*/
void SUIT_Desktop::removeLogo( const QString& id )
{
  logoRemove( id );
}

/*!
  Adds new logo to the menu bar area
*/
void SUIT_Desktop::logoInsert( const QString& logoID, const QPixmap& logo, const int idx )
{
  if ( myLogoMgr )
    myLogoMgr->insert( logoID, logo, idx );
}

/*!
  Removes a logo
*/
void SUIT_Desktop::logoRemove( const QString& logoID )
{
  if ( myLogoMgr )
    myLogoMgr->remove( logoID );
}

/*!
  Removes all logos 
*/
void SUIT_Desktop::logoClear()
{
  if ( myLogoMgr )
    myLogoMgr->clear();
}

/*!
  Emits activated signal
*/
void SUIT_Desktop::emitActivated()
{
  emit activated();
}

/*!
  Emits message signal
*/
void SUIT_Desktop::emitMessage( const QString& theMessage )
{
  emit message( theMessage );
}
