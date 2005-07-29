#include "SUIT_Desktop.h"

#include "SUIT_Tools.h"
#include "SUIT_ViewWindow.h"

#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <qtoolbar.h>
#include <qmenubar.h>
#include <qdockarea.h>
#include <qstatusbar.h>
#include <qapplication.h>

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
    ((QWidget*)e->child())->reparent( parentArea(), QPoint( 0, 0 ), true );
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
