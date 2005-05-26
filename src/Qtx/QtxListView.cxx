
#include "QtxListView.h"

#include <qheader.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>

static const char* list_xpm[] = {
"16 16 6 1",
". c None",
"a c #E3E9EB",
"b c #798391",
"c c #EBEBEB",
"d c #ABB4BE",
"e c #030E1F",
"................",
"................",
"................",
"...aaaaaaaaaa...",
"..abbbbbbbbbbe..",
"..abecbecbecbe..",
"..abbbbbbbbbbe..",
"..abecbecbecbe..",
"..abecaaaaaaaa..",
"..abeccdbbbbbb..",
"..abecccdbbbbe..",
"..abbbbe.dbbe...",
"...eeeee..de....",
"................",
"................",
"................" };

QtxListView::QtxListView( const int state, QWidget* parent, const char* name, WFlags f )
: QListView( parent, name, f ),
myButton( 0 ),
myHeaderState( state )
{
  initialize();
}

QtxListView::QtxListView( QWidget* parent, const char* name, WFlags f )
: QListView( parent, name, f ),
myButton( 0 ),
myHeaderState( HeaderAuto )
{
  initialize();
}

void QtxListView::initialize()
{
  if ( myHeaderState == HeaderButton )
  {
    QPixmap p( list_xpm );

    QPushButton* but = new QPushButton( this );
    but->setDefault( false );
    but->setFlat( true );
    but->setIconSet( p );
    but->setBackgroundPixmap( p );
    if ( p.mask() )
	    but->setMask( *p.mask() );
    myButton = but;

    connect( myButton, SIGNAL( clicked() ), this, SLOT( onButtonClicked() ) );

    myPopup = new QPopupMenu( this );
    connect( myPopup, SIGNAL( activated( int ) ), this, SLOT( onShowHide( int ) ) );
  }

  connect( header(), SIGNAL( sizeChange( int, int, int ) ), this, SLOT( onHeaderResized() ) );
}

QtxListView::~QtxListView()
{
}

int QtxListView::addColumn( const QString& label, int width )
{
  int res = QListView::addColumn( label, width );
  onHeaderResized();
  return res;
}

int QtxListView::addColumn( const QIconSet& iconset, const QString& label, int width ) 
{
  int res = QListView::addColumn( iconset, label, width );
  onHeaderResized();
  return res;
}

void QtxListView::removeColumn( int index ) 
{
  QListView::removeColumn( index );
  onHeaderResized();
}

void QtxListView::resize( int w, int h )
{
  QListView::resize( w, h );
  onHeaderResized();
}

void QtxListView::show()
{
  QListView::show();
  onHeaderResized();
}

void QtxListView::resizeContents( int w, int h )
{
  QListView::resizeContents( w, h );
  onHeaderResized();
}

void QtxListView::show( int ind )
{
  setShown( ind, true );
}

void QtxListView::hide( int ind )
{
  setShown( ind, false );
}

bool QtxListView::isShown( int ind ) const
{
  if( ind>=0 && ind<header()->count() )
    return columnWidth( ind ) > 0 || header()->isResizeEnabled( ind );
  else
    return false;
}

void QtxListView::setShown( int ind, bool sh )
{
  if( ind<0 || ind>=header()->count() || isShown( ind )==sh )
    return;

  ColumnData& data = myColumns[ ind ];
  if( sh )
  {
    setColumnWidth( ind, data.width );
    header()->setResizeEnabled( data.resizeable, ind );
    myColumns.remove( ind );
  }
  else
  {
    data.width = columnWidth( ind );
    data.resizeable = header()->isResizeEnabled( ind );
    setColumnWidth( ind, 0 );
    header()->setResizeEnabled( false, ind );
  }
  updateContents();
}

QSize QtxListView::sizeHint() const
{
  QSize sz = QListView::sizeHint();

  if ( myButton && myButton->isVisibleTo( myButton->parentWidget() ) )
    sz.setWidth( sz.width() + 2 + myButton->width() );

  return sz;
}

QSize QtxListView::minimumSizeHint() const
{
  QSize sz = QListView::minimumSizeHint();

  if ( myButton && myButton->isVisibleTo( myButton->parentWidget() ) )
    sz.setWidth( sz.width() + 2 + myButton->width() );

  return sz;
}

void QtxListView::onHeaderResized()
{
  if ( myHeaderState == HeaderAuto )
  {
    int c = 0;
    for ( int i = 0; i < columns(); i++ )
    {
      if ( !header()->label( i ).isEmpty() ||
           ( header()->iconSet( i ) && !header()->iconSet( i )->isNull() ) )
        c++;
    }

    if ( c > 1 )
      header()->show();
    else
      header()->hide();
  }

  if ( !myButton || !header()->isVisibleTo( this ) )
    return;

  int lw = lineWidth();
  int h = header()->size().height() - 1;
  myButton->setFixedSize( h, h );

  int x = header()->headerWidth() - header()->offset() + 2;
  if ( x < header()->width() - h )
    x = header()->width() - h;

  if ( myHeaderState == HeaderButton )
  {
    if( header()->orientation() == Qt::Horizontal )
      myButton->move( lw+x, lw );
    else
      myButton->move( lw, lw+x );
  }
}

void QtxListView::onButtonClicked()
{
  if ( myHeaderState != HeaderButton )
    return;

  myPopup->clear();
  for( int i=0, n=header()->count(); i<n; i++ )
  {
    int id = myPopup->insertItem( header()->label( i ) );
    myPopup->setItemChecked( id, isShown( i ) );
  }
  int x = myButton->x(),
      y = myButton->y() + myButton->height();
  myPopup->exec( mapToGlobal( QPoint( x, y ) ) );
}

void QtxListView::onShowHide( int id )
{
  if ( myHeaderState != HeaderButton )
    return;

  int ind = myPopup->indexOf( id );
  setShown( ind, !isShown( ind ) );
}

void QtxListView::viewportResizeEvent( QResizeEvent* e )
{
  QListView::viewportResizeEvent( e );
  onHeaderResized();
}
