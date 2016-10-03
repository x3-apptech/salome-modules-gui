// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxMenu.cxx
// Author:    Sergey TELKOV
//
#include "QtxMenu.h"

#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QTextDocument>
#include <QWidgetAction>
#include <QLinearGradient>
#include <QAbstractTextDocumentLayout>

/*!
  \class QtxMenu::Title
  \brief Popup menu title item.
  \internal
*/

class QtxMenu::Title : public QWidget
{
public:
  Title( QWidget* = 0 );
  virtual ~Title();

  QIcon            icon() const;
  void             setIcon( const QIcon& );

  QString          text() const;
  void             setText( const QString& );

  Qt::Alignment    alignment() const;
  void             setAlignment( const Qt::Alignment );

  virtual QSize    sizeHint() const;
  virtual QSize    minimumSizeHint() const;

protected:
  virtual void     paintEvent( QPaintEvent* );

private:
  QIcon            myIcon;
  QString          myText;
  Qt::Alignment    myAlignment;
};

/*!
  \brief Constructor.
  \param parent parent widget
  \internal
*/
QtxMenu::Title::Title( QWidget* parent )
: QWidget( parent ),
  myAlignment( 0 )
{
}

/*!
  \brief Destructor.
  \internal
*/
QtxMenu::Title::~Title()
{
}

/*!
  \brief Get title icon.
  \return title item icon
  \internal
*/
QIcon QtxMenu::Title::icon() const
{
  return myIcon;
}

/*!
  \brief Set title icon.
  \param ico title item icon
  \internal
*/
void QtxMenu::Title::setIcon( const QIcon& ico )
{
  myIcon = ico;
}

/*!
  \brief Get title menu text.
  \return menu text for the title item
  \internal
*/
QString QtxMenu::Title::text() const
{
  return myText;
}

/*!
  \brief Set title menu text.
  \param txt menu text to be used for the title item
  \internal
*/
void QtxMenu::Title::setText( const QString& txt )
{
  myText = txt;
}

/*!
  \brief Get title alignment flags.
  \return title alignment flags
  \internal
*/
Qt::Alignment QtxMenu::Title::alignment() const
{
  return myAlignment;
}

/*!
  \brief Set title alignment flags.
  \param a title alignment flags
  \internal
*/
void QtxMenu::Title::setAlignment( const Qt::Alignment a )
{
  myAlignment = a;
}

/*!
  \brief Get recommended size for the title item widget.
  \return title item widget size
  \internal
*/
QSize QtxMenu::Title::sizeHint() const
{
  int m = 5;
  QTextDocument doc;
  doc.setHtml( text() );

  QSize sz = icon().isNull() ? QSize( 0, 0 ) : icon().actualSize( QSize( 16, 16 ) );
  sz.setWidth( 2 * m + sz.width() + (int)doc.size().width() );
  sz.setHeight( 2 * m + qMax( sz.height(), (int)doc.size().height() ) );
  return sz;
}

/*!
  \brief Get recommended minimum size for the title item widget.
  \return title item widget minimum size
  \internal
*/
QSize QtxMenu::Title::minimumSizeHint() const
{
  return sizeHint();
}

/*!
  \brief Paint the title item widget.
  \param e paint event (not used)
  \internal
*/
void QtxMenu::Title::paintEvent( QPaintEvent* /*e*/ )
{
  int m = 5;
  QIcon ico = icon();
  QString txt = text();
  Qt::Alignment align = alignment();

  QRect base = rect();
  base.setTop( base.top() + 1 );
  base.setLeft( base.left() + 1 );
  base.setRight( base.right() -1 );
  base.setBottom( base.bottom() - 1 );

  QTextDocument doc;
  doc.setHtml( txt );

  QSize isz = ico.isNull() ? QSize( 0, 0 ) : ico.actualSize( QSize( 16, 16 ) );
  QSize sz( (int)doc.size().width(), (int)doc.size().height() );

  QPainter p( this );
  QAbstractTextDocumentLayout::PaintContext ctx;
  ctx.palette.setColor( QPalette::Text, palette().color( QPalette::Light ) );

  QLinearGradient linearGrad( base.topLeft(), base.topRight() );
  linearGrad.setColorAt( 0, palette().color( QPalette::Highlight ) );
  linearGrad.setColorAt( 1, palette().color( QPalette::Window ) );

  p.fillRect( base, linearGrad );

  QPoint start = base.topLeft() + QPoint( m, m );
  if ( align & Qt::AlignLeft )
    start.setX( base.left() + m );
  else if ( align & Qt::AlignRight )
    start.setX( base.right() - m - isz.width() - sz.width() );
  else if ( align & Qt::AlignHCenter )
    start.setX( base.left() + ( base.width() - isz.width() - sz.width() ) / 2 );

  if ( align & Qt::AlignTop )
    start.setY( base.top() + m );
  else if ( align & Qt::AlignBottom )
    start.setY( base.bottom() - m - qMax( isz.height(), - sz.height() ) );
  else if ( align & Qt::AlignVCenter )
    start.setY( base.top() + ( base.height() - qMax( isz.height(), sz.height() ) ) / 2 );

  if ( !ico.isNull() )
  {
    ico.paint( &p, QRect( start, isz ) );
    start.setX( start.x() + isz.width() );
  }

  p.save();
  p.translate( start );
  doc.documentLayout()->draw( &p, ctx );
  p.restore();
}

/*!
  \class QtxMenu
  \brief The class QtxMenu represents the popup menu with the title.

  The title for the popup menu can be set via setTitleText() method.
  In addition, title item can contain the icon, which can be set using
  setTitleIcon() method. Current title text and icon can be retrieved with
  titleText() and titleIcon() methods.

  The title text alignment flags can be changed using setTitleAlignment()
  method and retrieved with titleAlignment() method.

  By default, QtxMenu::TitleAuto mode is used. In this mode, the title item
  is shown only if it is not empty. To show title always (even empty), pass
  QtxMenu::TitleOn to the setTitleMode() method. To hide the title, use 
  setTitleMode() method with QtxMenu::TitleOff parameter.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxMenu::QtxMenu( QWidget* parent )
: QMenu( parent ),
  myMode( TitleAuto )
{
  myTitle = new Title( this );
  myAction = new QWidgetAction( this );
  myAction->setDefaultWidget( myTitle );
}

/*!
  \brief Destructor.
*/
QtxMenu::~QtxMenu()
{
}

/*!
  \brief Get title menu text.
  \return menu text for the title item
*/
QString QtxMenu::titleText() const
{
  return myTitle->text();
}

/*!
  \brief Get title icon.
  \return title item icon
*/
QIcon QtxMenu::titleIcon() const
{
  return myTitle->icon();
}

/*!
  \brief Get title item display mode.
  \return popup menu title display mode (QtxMenu::TitleMode)
*/
QtxMenu::TitleMode QtxMenu::titleMode() const
{
  return myMode;
}

/*!
  \brief Get title alignment flags.
  \return title alignment flags
*/
Qt::Alignment QtxMenu::titleAlignment() const
{
  return myTitle->alignment();
}

/*!
  \brief Set title menu text.
  \param txt menu text to be used for the title item
*/
void QtxMenu::setTitleText( const QString& txt )
{
  if ( titleText() == txt )
    return;

  myTitle->setText( txt );

  updateTitle();
}

/*!
  \brief Set title icon.
  \param ico title item icon
*/
void QtxMenu::setTitleIcon( const QIcon& ico )
{
  myTitle->setIcon( ico );

  updateTitle();
}

/*!
  \brief Set title item display mode.
  \param m popup menu title display mode (QtxMenu::TitleMode)
*/
void QtxMenu::setTitleMode( const QtxMenu::TitleMode m )
{
  if ( myMode == m )
    return;

  myMode = m;

  updateTitle();
}

/*!
  \brief Set title alignment flags.
  \param a title alignment flags
*/
void QtxMenu::setTitleAlignment( const Qt::Alignment a )
{
  if ( titleAlignment() == a )
    return;

  myTitle->setAlignment( a );

  updateTitle();
}

/*!
 * \brief Append a title to the and of the menu
 *  \param [in] text - title text
 */
void QtxMenu::appendGroupTitle( const QString& text )
{
  Title* aTitle = new Title( this );
  aTitle->setText( text );

  QWidgetAction* anAction = new QWidgetAction( this );
  anAction->setDefaultWidget( aTitle );

  addAction( anAction );
}

/*!
  \brief Customize show/hide menu operation.
  \param on new popup menu visibility state
*/
void QtxMenu::setVisible( bool on )
{
  if ( on )
    insertTitle();

  QMenu::setVisible( on );

  if ( !on )
    removeTitle();
}

/*!
  \brief Insert title item to the popup menu.
*/
void QtxMenu::insertTitle()
{
  if ( titleMode() == TitleOff || ( titleMode() == TitleAuto && titleText().trimmed().isEmpty() ) )
    return;

  if ( actions().isEmpty() )
    addAction( myAction );
  else
    insertAction( actions().first(), myAction );
}

/*!
  \brief Remove title item from the popup menu.
*/
void QtxMenu::removeTitle()
{
  if ( actions().contains( myAction ) )
    removeAction( myAction );
}

/*!
  \brief Update title item.
*/
void QtxMenu::updateTitle()
{
  if ( !actions().contains( myAction ) )
    return;

  removeTitle();
  insertTitle();
}
