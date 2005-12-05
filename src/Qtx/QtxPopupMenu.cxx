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
#include "QtxPopupMenu.h"

#include <qpainter.h>

/*!
    Class: QtxPopupMenu::TitleMenuItem [Internal]
    Descr: Custom menu item for popup title.
*/

class QtxPopupMenu::TitleMenuItem : public QCustomMenuItem
{
public:
  TitleMenuItem( const QString&, const QIconSet&, const int );
  virtual ~TitleMenuItem();

  virtual bool  fullSpan() const;
  virtual bool  isSeparator() const;
  virtual void  setFont( const QFont& );

  virtual void  paint( QPainter*, const QColorGroup&, bool, bool, int, int, int, int );
  virtual QSize sizeHint();

private:
  QString       myText;
  QIconSet      myIcon;
  QFont         myFont;
  int           myAlign;
};

QtxPopupMenu::TitleMenuItem::TitleMenuItem( const QString& txt, const QIconSet& ico, const int align )
: QCustomMenuItem(),
myText( txt ),
myIcon( ico ),
myAlign( align )
{
}

QtxPopupMenu::TitleMenuItem::~TitleMenuItem()
{
}

bool QtxPopupMenu::TitleMenuItem::fullSpan() const
{
  return true;
}

bool QtxPopupMenu::TitleMenuItem::isSeparator() const
{
  return false;
}

void QtxPopupMenu::TitleMenuItem::setFont( const QFont& font )
{
  myFont = font;
  myFont.setBold( true );
}

void QtxPopupMenu::TitleMenuItem::paint( QPainter* p, const QColorGroup& cg,
                                         bool act, bool enabled, int x, int y, int w, int h )
{
  QFont f = p->font();
  p->setFont( myFont );

  p->fillRect( x, y, w, h, cg.brush( QColorGroup::Dark ) );

  p->setPen( cg.shadow() );
  p->drawRect( x, y, w, h );

  int m = 3;
  int s = 3;
  int iw = p->fontMetrics().width( myText ) + ( myIcon.isNull() ? 0 : myIcon.pixmap().width() + s );
  int ih = QMAX( ( myIcon.isNull() ? 0 : myIcon.pixmap().height() ), p->fontMetrics().height() );

  int ix = x;
  int iy = y + m;

  if ( myAlign & AlignLeft )
    ix = x;
  else if ( myAlign & AlignRight )
    ix = x + ( w - iw );
  else if ( myAlign & AlignHCenter )
    ix = x + ( w - iw ) / 2;

  if ( myAlign & AlignTop )
    iy = y;
  else if ( myAlign & AlignBottom )
    iy = y + ( h - ih - m );
  else if ( myAlign & AlignVCenter )
    iy = y + ( h - ih ) / 2;

  if ( !myIcon.isNull() )
  {
    p->drawPixmap( ix, iy + ( ih - myIcon.pixmap().height() ) / 2, myIcon.pixmap() );
    ix += myIcon.pixmap().width() + s;
  }

  p->setPen( cg.brightText() );
  p->drawText( ix, iy + ( ih - p->fontMetrics().height() ) / 2 +
               p->fontMetrics().ascent(), myText, 0, -1 );

  p->setFont( f );
}

QSize QtxPopupMenu::TitleMenuItem::sizeHint()
{
  QFontMetrics fM( myFont );

  int m = 3;
  int s = 3;
  int w = fM.width( myText ) + ( myIcon.isNull() ? 0 : myIcon.pixmap().width() + s );
  int h = QMAX( ( myIcon.isNull() ? 0 : myIcon.pixmap().height() ), fM.height() ) + 2 * m;

  return QSize( w, h );
}

/*!
    Class: QtxPopupMenu [Public]
    Descr: Popup menu item with title.
*/

QtxPopupMenu::QtxPopupMenu( QWidget* parent, const char* name )
: QPopupMenu( parent, name ),
myId( -1 ),
myPolicy( TitleAuto ),
myAlign( AlignCenter )
{
}

QtxPopupMenu::~QtxPopupMenu()
{
}

QString QtxPopupMenu::titleText() const
{
  return myText;
}

QIconSet QtxPopupMenu::titleIcon() const
{
  return myIcon;
}

int QtxPopupMenu::titlePolicy() const
{
  return myPolicy;
}

int QtxPopupMenu::titleAlignment() const
{
  return myAlign;
}

void QtxPopupMenu::setTitleText( const QString& txt )
{
  if ( myText == txt )
    return;

  myText = txt;

  updateTitle();
}

void QtxPopupMenu::setTitleIcon( const QIconSet& ico )
{
  myIcon = ico;

  updateTitle();
}

void QtxPopupMenu::setTitlePolicy( const int p )
{
  if ( myPolicy == p )
    return;

  myPolicy = p;

  updateTitle();
}

void QtxPopupMenu::setTitleAlignment( const int a )
{
  if ( myAlign == a )
    return;

  myAlign = a;

  updateTitle();
}

void QtxPopupMenu::show()
{
  insertTitle();

  QPopupMenu::show();
}

void QtxPopupMenu::hide()
{
  QPopupMenu::hide();

  removeTitle();
}

QtxPopupMenu::TitleMenuItem* QtxPopupMenu::createTitleItem( const QString& txt, const QIconSet& ico,
                                                            const int align ) const
{
  return new TitleMenuItem( txt, ico, align );
}

void QtxPopupMenu::insertTitle()
{
  if ( myId != -1 || titlePolicy() == TitleOff ||
       ( titlePolicy() == TitleAuto && titleText().stripWhiteSpace().isEmpty() ) )
    return;

  TitleMenuItem* item = createTitleItem( titleText(), titleIcon(), titleAlignment() );

  myId = insertItem( item, -1, 0 );
  setItemEnabled( myId, false );
}

void QtxPopupMenu::removeTitle()
{
  if ( myId == -1 )
    return;

  removeItem( myId );
  myId = -1;
}

void QtxPopupMenu::updateTitle()
{
  if ( myId != -1 )
  {
    removeTitle();
    insertTitle();
  }
}
