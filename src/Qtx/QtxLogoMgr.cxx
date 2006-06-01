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
#include "QtxLogoMgr.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qstyle.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qapplication.h>

/*!
  Constructor
*/
QtxLogoMgr::QtxLogoMgr( QMenuBar* mb )
: QObject( mb ),
myMenus( mb ),
myId( 0 )
{
}

/*!
  Destructor
*/
QtxLogoMgr::~QtxLogoMgr()
{
}

/*!
  Returns the menubar.
*/
QMenuBar* QtxLogoMgr::menuBar() const
{
  return myMenus;
}

/*!
  Returns the count of the existed logos.
*/
int QtxLogoMgr::count() const
{
  return myLogos.count();
}

/*!
  Insert new logo to the menu bar area
*/
void QtxLogoMgr::insert( const QString& id, const QPixmap& pix, const int index )
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

/*!
  Removes a logo
*/
void QtxLogoMgr::remove( const QString& id )
{
  int idx = find( id );
  if ( idx < 0 )
    return;

  myLogos.remove( myLogos.at( idx ) );

  generate();
}

/*!
  Removes all logos 
*/
void QtxLogoMgr::clear()
{
  myLogos.clear();
  generate();
}

/*!
  Inserts logo to menu bar
*/
void QtxLogoMgr::generate()
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
  cnt->setSpacing( 3 );

  for ( LogoList::const_iterator it = myLogos.begin(); it != myLogos.end(); ++it )
  {
    QPixmap pix = (*it).pix;
    if ( !pix.mask() )
    {
      QImage img = pix.convertToImage();
      QBitmap bm;
      if ( img.hasAlphaBuffer() )
        bm = img.createAlphaMask();
      else
        bm = img.createHeuristicMask();
      pix.setMask( bm );
    }

    QLabel* logoLab = new QLabel( cnt );
    logoLab->setPixmap( (*it).pix );
    logoLab->setScaledContents( false );
    logoLab->setAlignment( QLabel::AlignCenter ); 

    if ( pix.mask() )
  	  logoLab->setMask( *pix.mask() );
  }

  QApplication::sendPostedEvents( cnt, QEvent::ChildInserted );
  cnt->addSpacing( 2 );

  myId = menuBar()->insertItem( cnt );

  QApplication::sendPostedEvents( menuBar()->parentWidget(), QEvent::LayoutHint );
  QApplication::postEvent( menuBar()->parentWidget(), new QEvent( QEvent::LayoutHint ) );
}

/*!
  \return index of found logo
  \param id - logo id
*/
int QtxLogoMgr::find( const QString& id ) const
{
  int idx = -1;
  for ( uint i = 0; i < myLogos.count() && idx < 0; i++ )
  {
    if ( (*myLogos.at( i ) ).id == id )
      idx = i;
  }
  return idx;
}
