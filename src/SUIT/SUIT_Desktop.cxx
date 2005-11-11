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
 Class: SUIT_Desktop::LogoMgr
 Level: Internal
*/

class SUIT_Desktop::LogoMgr : public QObject
{
public:
  LogoMgr( QMenuBar* );
  virtual ~LogoMgr();

  int                    count() const;

  void                   insert( const QString&, const QPixmap&, const int = -1 );
  void                   remove( const QString& );
  void                   clear();

  QMenuBar*              menuBar() const;

private:
  void                   generate();
  int                    find( const QString& ) const;

private:
  typedef struct { QString id; QPixmap pix; } LogoInfo;
  typedef QValueList<LogoInfo>                LogoList;

private:
  int                    myId;
  QMenuBar*              myMenus;
  LogoList               myLogos;
};

SUIT_Desktop::LogoMgr::LogoMgr( QMenuBar* mb )
: QObject( mb ),
myMenus( mb ),
myId( 0 )
{
} 

SUIT_Desktop::LogoMgr::~LogoMgr()
{
}

QMenuBar* SUIT_Desktop::LogoMgr::menuBar() const
{
  return myMenus;
}

int SUIT_Desktop::LogoMgr::count() const
{
  return myLogos.count();
}

void SUIT_Desktop::LogoMgr::insert( const QString& id, const QPixmap& pix, const int index )
{
  if ( pix.isNull() )
    return;

  LogoInfo* inf = 0;

  int idx = find( id );
  if ( idx < 0 )
  {
    idx = index < (int)myLogos.count() ? index : -1;
    if ( idx < 0 )
      inf = &( *myLogos.append( LogoInfo() ) );
    else
      inf = &( *myLogos.insert( myLogos.at( idx ), LogoInfo() ) );
  }
  else
    inf = &( *myLogos.at( idx ) );


  inf->id = id;
  inf->pix = pix;

  generate();
}

void SUIT_Desktop::LogoMgr::remove( const QString& id )
{
  int idx = find( id );
  if ( idx < 0 )
    return;

  myLogos.remove( myLogos.at( idx ) );

  generate();
}

void SUIT_Desktop::LogoMgr::clear()
{
  myLogos.clear();
  generate();
}

void SUIT_Desktop::LogoMgr::generate()
{
  if ( !menuBar() )
    return;

  if ( myId ) 
    menuBar()->removeItem( myId );

  myId = 0;

  if ( myLogos.isEmpty() )
    return;

  class LogoBox : public QHBox
  {
  public:
    LogoBox( QWidget* parent = 0, const char* name = 0, WFlags f = 0 ) : QHBox( parent, name, f ) {};

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

  LogoBox* cnt = new LogoBox( menuBar() );
  cnt->setSpacing( 2 );

  for ( LogoList::const_iterator it = myLogos.begin(); it != myLogos.end(); ++it )
  {
    QLabel* logoLab = new QLabel( cnt );
    logoLab->setPixmap( (*it).pix );
    logoLab->setScaledContents( false );
    logoLab->setAlignment( QLabel::AlignCenter ); 
  }
  QApplication::sendPostedEvents( cnt, QEvent::ChildInserted );
  cnt->addSpacing( 2 );

  myId = menuBar()->insertItem( cnt );

  QApplication::sendPostedEvents( menuBar()->parentWidget(), QEvent::LayoutHint );
  QApplication::postEvent( menuBar()->parentWidget(), new QEvent( QEvent::LayoutHint ) );
}

int SUIT_Desktop::LogoMgr::find( const QString& id ) const
{
  int idx = -1;
  for ( uint i = 0; i < myLogos.count() && idx < 0; i++ )
  {
    if ( (*myLogos.at( i ) ).id == id )
      idx = i;
  }
  return idx;
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
: QtxMainWindow()
{
  myMenuMgr = new QtxActionMenuMgr( this );
  myToolMgr = new QtxActionToolMgr( this );
  myLogoMgr = new LogoMgr( menuBar() );
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


