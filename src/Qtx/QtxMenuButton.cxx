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
// File:      QtxMenuButton.cxx
// Author:    Sergey TELKOV

#include "QtxMenuButton.h"

#include <qstyle.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qpointarray.h>
#include <qapplication.h>

class QtxMenuButton::PopupMenu : public QPopupMenu
{
public:
    PopupMenu( QtxMenuButton* mb ) : QPopupMenu( mb ), myMenuButton( mb ) {};
    virtual ~PopupMenu() {};

    virtual void setMinimumSize( int, int );

private:
    QtxMenuButton* myMenuButton;
};

/*!
  Sets the widget's minimum size
  \param w - width
  \param h - height
*/
void QtxMenuButton::PopupMenu::setMinimumSize( int w, int h )
{
    if ( myMenuButton->isAlignWidth() &&
         ( myMenuButton->position() == Top || myMenuButton->position() == Bottom ) )
        w = QMAX( w, myMenuButton->width() );

    QPopupMenu::setMinimumSize( w, h );
}


/*!
  Constructor
  \param pos - position
  \param parent - parent widget
  \param name - name
*/
QtxMenuButton::QtxMenuButton( int pos, QWidget* parent, const char* name )
: QPushButton( parent, name ),
myPos( pos )
{
	initialize();
}

/*!
  Constructor
  \param text - button text
  \param parent - parent widget
  \param name - name
*/
QtxMenuButton::QtxMenuButton( const QString& text, QWidget* parent, const char* name )
: QPushButton( parent, name ),
myPos( Bottom )
{
	setText( text );
	initialize();
}

/*!
  Constructor
  \param pos - position
  \param text - button text
  \param parent - parent widget
  \param name - name
*/
QtxMenuButton::QtxMenuButton( int pos, const QString& text, QWidget* parent, const char* name )
: QPushButton( parent, name ),
myPos( pos )
{
	setText( text );
	initialize();
}

/*!
  Constructor
  \param parent - parent widget
  \param name - name
*/
QtxMenuButton::QtxMenuButton( QWidget* parent, const char* name )
: QPushButton( parent, name ),
myPos( Bottom )
{
	initialize();
}

/*!
  Destructor
*/
QtxMenuButton::~QtxMenuButton()
{
}

/*!
  Initialization
*/
void QtxMenuButton::initialize()
{
	myArrow = true;
    myAlign = true;

	setAutoDefault( false );
	myPopup = new PopupMenu( this );
    myPopup->hide();

	connect( myPopup, SIGNAL( activated( int ) ), this, SIGNAL( activated( int ) ) );
	connect( this, SIGNAL( clicked() ), this, SLOT( onShowPopup() ) );
}

/*!
  \return position
*/
int QtxMenuButton::position() const
{
	return myPos;
}

/*!
  \return true if align is enabled
*/
bool QtxMenuButton::isAlignWidth() const
{
    return myAlign;
}

/*!
  \return true if arrow is shown
*/
bool QtxMenuButton::isArrowEnabled() const
{
	return myArrow;
}

/*!
  Changes position
  \param pos - new position
*/
void QtxMenuButton::setPosition( const int pos )
{
	if ( myPos == pos )
		return;

    myPos = pos;
    if ( myPopup->isVisible() )
        onShowPopup();
}

/*!
  Changes align state
  \param on - new align state
*/
void QtxMenuButton::setAlignWidth( const bool on )
{
    if ( myAlign == on )
        return;

    myAlign = on;
    updateGeometry();
}

/*!
  Enables/disable arrow
  \param on - new enabled state
*/
void QtxMenuButton::setArrowEnabled( const bool on )
{
	if ( myArrow == on )
		return;

    myArrow = on;
    repaint();
}

/*!
  Clears popup
*/
void QtxMenuButton::clear()
{
	if ( myPopup )
		myPopup->clear();
	onShowPopup();
	updateGeometry();
}

/*!
  Removes item from popup
  \param id - item id
*/
void QtxMenuButton::removeItem( int id )
{
	if ( myPopup )
		myPopup->removeItem( id );
	updateGeometry();
}

/*!
  Inserts separator into popup
  \param id - position
*/
int QtxMenuButton::insertSeparator( int id )
{
	int res = -1;
	if ( myPopup )
		res = myPopup->insertSeparator( id );
    return res;
}

/*!
  Inserts item into popup
  \param t - menu text
  \param id - item id
  \param index - position
*/
int QtxMenuButton::insertItem( const QString& t, int id, int index )
{
	int resId = -1;
	if ( myPopup )
		resId = myPopup->insertItem( t, id, index );

	if ( resId != -1 )
		updateGeometry();

	return resId;
}

/*!
  Inserts item into popup
  \param is - icons
  \param t - menu text
  \param id - item id
  \param index - position
*/
int QtxMenuButton::insertItem( const QIconSet& is, const QString& t, int id, int index )
{
	int resId = -1;
	if ( myPopup )
		resId = myPopup->insertItem( is, t, id, index );

	if ( resId != -1 )
		updateGeometry();

	return resId;
}

/*!
  SLOT: calls when button is clicked, shows popup
*/
void QtxMenuButton::onShowPopup()
{
	if ( !myPopup || !myPopup->count() )
	{
		myPopup->hide();
		return;
	}

	QPoint p = mapToGlobal( QPoint( 0, 0 ) );
	int x = p.x();
	int y = p.y() + 1;
	int margin = 0;
	int xoffset = 0;
	int yoffset = 0;
	switch ( position() )
	{
	case Left:
		xoffset = -1 * ( myPopup->sizeHint().width() + margin );
		break;
	case Right:
		xoffset = width() + margin;
		break;
	case Top:
		yoffset = -1 * ( myPopup->sizeHint().height() + margin );
		break;
	case Bottom:
	default:
		yoffset = height() + margin;
		break;
	}
	int dw = QApplication::desktop()->width();
	int dh = QApplication::desktop()->height();
	x = QMIN( QMAX( x + xoffset, 0 ), dw );
	y = QMIN( QMAX( y + yoffset, 0 ), dh );

	myPopup->exec( QPoint( x, y ) );
}

/*!
  Custom event handler
*/
bool QtxMenuButton::event( QEvent* e )
{
	if ( e->type() == QEvent::MouseButtonPress ||
		 e->type() == QEvent::MouseButtonDblClick ||
		 e->type() == QEvent::MouseButtonRelease )
	{
		onShowPopup();
		return false;
	}

	return QPushButton::event( e );
}

/*!
  \return the recommended size for the widget
*/
QSize QtxMenuButton::sizeHint() const
{
	QSize sz = QPushButton::sizeHint();
	if ( ( position() == Top || position() == Bottom ) && myPopup && myAlign )
		sz = QSize( QMAX( sz.width(), myPopup->sizeHint().width() ), sz.height() );

	return sz;
}

/*!
  \return the recommended minimum size for the widget
*/
QSize QtxMenuButton::minimumSizeHint() const
{
	QSize sz = QPushButton::minimumSizeHint();
	if ( ( position() == Top || position() == Bottom ) && myPopup && myAlign )
		sz = QSize( QMAX( sz.width(), myPopup->sizeHint().width() ), sz.height() );

	return sz;
}

/*!
  Custom resize event handler
*/
void QtxMenuButton::resizeEvent( QResizeEvent* re )
{
	if ( re )
		QPushButton::resizeEvent( re );

	if ( ( position() == Top || position() == Bottom ) && myPopup && myAlign )
        myPopup->setMinimumWidth( re ? re->size().width() : width() );
}

/*!
  \return corresponding popup
*/
QPopupMenu* QtxMenuButton::popup() const
{
    return myPopup;
}

/*!
  Draws label
*/
void QtxMenuButton::drawButtonLabel( QPainter* p )
{
    QPushButton::drawButtonLabel( p );
/*
	QStyle::SFlags flags = QStyle::Style_Default;
	if ( isEnabled() )
		flags |= QStyle::Style_Enabled;
	if ( hasFocus() )
		flags |= QStyle::Style_HasFocus;
*/
#if QT_VER < 3
    QRect r = rect();
#else
	QRect r = style().subRect( QStyle::SR_PushButtonContents, this );
#endif

	if ( myArrow && myPopup && myPopup->count() )
	{
		int w = 7;
		int h = 7;
		int margin = 5;

		QRect ar( 0, 0, w, h );
		if ( position() == Left || position() == Top )
			r.moveBy( ar.width() + 2 * margin, 0 );
		else
			ar.moveBy( r.width() - ar.width() - 2 * margin, 0 );

		r.setWidth( r.width() - ar.width() - 2 * margin );
    
		ar.moveBy( margin, ( height() - h ) / 2 );

		QPointArray arrow( 3 );
		switch ( position() )
		{
		case Left:
			arrow.putPoints( 0, 3, ar.left(), ar.top() + ar.height() / 2, ar.right(), ar.top(), ar.right(), ar.bottom() );
			break;
		case Right:
			arrow.putPoints( 0, 3, ar.left(), ar.top(), ar.left(), ar.bottom(), ar.right(), ar.top() + ar.height() / 2 );
			break;
		case Top:
			arrow.putPoints( 0, 3, ar.left(), ar.bottom(), ar.right(), ar.bottom(), ar.left() + ar.width() / 2, ar.top() );
			break;
		case Bottom:
		default:
			arrow.putPoints( 0, 3, ar.left(), ar.top(), ar.right(), ar.top(), ar.left() + ar.width() / 2, ar.bottom() );
			break;
		}

		p->setPen( colorGroup().text() );
		p->setBrush( colorGroup().text() );
		p->drawPolygon( arrow, true );
	}

//	style().drawControl( QStyle::CE_PushButtonLabel, p, this, r, colorGroup(), flags );
}
