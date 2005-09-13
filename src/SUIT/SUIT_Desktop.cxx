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
#include <qhbox.h>
#include <qlabel.h>
#include <qstyle.h>

/*!
 Class: LogoBox
 Level: Internal
*/

class LogoBox : public QHBox
{
public:
  LogoBox( QWidget* parent = 0, const char* name = 0, WFlags f = 0 ) : QHBox( parent, name, f )
  {
    setFrameStyle( Plain | NoFrame );
    setMargin( 0 ); setSpacing( 2 );
  }
  void addSpacing( int spacing )
  {
    QApplication::sendPostedEvents( this, QEvent::ChildInserted );
    ((QHBoxLayout*)layout())->addSpacing( spacing );
  }
protected:
  void drawContents( QPainter* p )
  {
    if ( parentWidget()->inherits( "QMenuBar" ) )
      style().drawControl( QStyle::CE_MenuBarEmptyArea, p, this, contentsRect(), colorGroup() );
    else
      QHBox::drawContents( p );
  }
};

/*!
 Class: SUIT_Desktop::LogoManager
 Level: Internal
*/

SUIT_Desktop::LogoManager::LogoManager( SUIT_Desktop* desktop)
: myDesktop( desktop ), myId( 0 ) 
{
} 

void SUIT_Desktop::LogoManager::addLogo( const QString& logoID, const QPixmap& logo )
{
  if ( !myDesktop || logo.isNull() )
    return;
  myLogoMap[ logoID ] = logo;
  generateLogo();
}

void SUIT_Desktop::LogoManager::removeLogo( const QString& logoID )
{
  if ( !myDesktop || myLogoMap.find( logoID ) == myLogoMap.end() )
    return;
  myLogoMap.remove( logoID );
  generateLogo();
}

void SUIT_Desktop::LogoManager::clearLogo()
{
  myLogoMap.clear();
  generateLogo();
}

void SUIT_Desktop::LogoManager::generateLogo()
{
  if ( !myDesktop ) return;

  if ( myId ) 
    myDesktop->menuBar()->removeItem( myId );
  myId = 0;

  if ( !myLogoMap.count() )
    return;

  LogoBox* cnt = new LogoBox( myDesktop );
  
  QMap<QString, QPixmap>::Iterator it;
  for ( it = myLogoMap.begin(); it != myLogoMap.end(); ++it ) {
    QLabel* logoLab = new QLabel( cnt );
    logoLab->setPixmap( *it );
    logoLab->setAlignment( QLabel::AlignCenter ); 
    logoLab->setScaledContents( false );
  }
  cnt->addSpacing( 2 );

  myId = myDesktop->menuBar()->insertItem( cnt );
  QApplication::sendPostedEvents( myDesktop->menuBar()->parentWidget(), QEvent::LayoutHint );
  QApplication::postEvent( myDesktop->menuBar()->parentWidget(), new QEvent( QEvent::LayoutHint ) );
}


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
: QtxMainWindow(), myLogoMan( this )
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

/*!
  Adds new logo to the menu bar area
*/
void SUIT_Desktop::addLogo( const QString& logoID, const QPixmap& logo )
{
  myLogoMan.addLogo( logoID, logo );
}

/*!
  Removes a logo
*/
void SUIT_Desktop::removeLogo( const QString& logoID )
{
  myLogoMan.removeLogo( logoID );
}

/*!
  Removes all logos 
*/
void SUIT_Desktop::clearLogo()
{
  myLogoMan.clearLogo();
}


