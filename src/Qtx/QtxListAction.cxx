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
// File:      QtxListAction.cxx
// Author:    Sergey TELKOV (Based on code by Eugene AKSENOV)

#include "QtxListAction.h"

#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qlistbox.h>
#include <qtoolbar.h>
#include <qwmatrix.h>
#include <qpopupmenu.h>
#include <qtoolbutton.h>
#include <qobjectlist.h>
#include <qapplication.h>

static const char* list_arrow_icon[] = {
"10 6 2 1",
"# c #000000",
"  c none",
"          ",
" #######  ",
"  #####   ",
"   ###    ",
"    #     ",
"          "
};

/*!
  \class QtxListAction::ToolButton
  Custom tool button
*/
class QtxListAction::ToolButton : public QToolButton
{
public:
  ToolButton( QtxListAction*, QWidget* = 0, const char* = 0 );
  virtual ~ToolButton();

  virtual QSize sizeHint() const;

private:
  QtxListAction* myAction;
};

/*!
  Constructor
*/
QtxListAction::ToolButton::ToolButton( QtxListAction* a, QWidget* parent, const char* name )
: QToolButton( parent, name ),
myAction( a )
{
  setIconSet( QPixmap( list_arrow_icon ) );
}

/*!
  Destructor
*/
QtxListAction::ToolButton::~ToolButton()
{
  if ( myAction )
    myAction->controlDeleted( this );
}

/*!
  \return the recommended size for the widget
*/
QSize QtxListAction::ToolButton::sizeHint() const
{
  QSize sz = iconSet().pixmap().size();
  return QSize( sz.width() + 2, sz.height() + 2 );
}

/*!
  Constructs an list action with given parent and name. If toggle is true the
  action will be a toggle action, otherwise it will be a command action.
*/
QtxListAction::QtxListAction( QObject* parent, const char* name, bool toggle )
: QtxAction( parent, name, toggle ),
myFrame( 0 ),
myMode( Item ),
myRaise( false )
{
  initialize();
}

/*!
  This constructor creates an action with the following properties: the
  description text, the icon or iconset icon, the menu text and keyboard
  accelerator. It is a child of given parent and named specified name.
  If toggle is true the action will be a toggle action, otherwise it will
  be a command action.
*/

QtxListAction::QtxListAction( const QString& text, const QIconSet& icon,
                              const QString& menuText, int accel,
                              QObject* parent, const char* name, bool toggle )
: QtxAction( text, icon, menuText, accel, parent, name, toggle ),
myFrame( 0 ),
myMode( Item ),
myRaise( false )
{
  initialize();
}

/*!
  This constructor creates an action with the following properties: the
  description text, the menu text and keyboard accelerator. It is a child
  of given parent and named specified name. If toggle is true the action
  will be a toggle action, otherwise it will be a command action.
*/

QtxListAction::QtxListAction( const QString& text, const QString& menuText,
                              int accel, QObject* parent, const char* name, bool toggle )
: QtxAction( text, menuText, accel, parent, name, toggle ),
myFrame( 0 ),
myMode( Item ),
myRaise( false )
{
  initialize();
}

/*!
  Destructor.
*/

QtxListAction::~QtxListAction()
{
  if ( myFrame ) {
    myFrame->myAction = 0;
    delete myFrame;
    myFrame = 0;
  }
}

/*!
	Name: popupMode [public]
	Desc: Returns popup mode. If popup mode "Item" (default) then action will
	      be added into popup menu as menu item. If popup mode "SubMenu" then
		    action will be added into popup menu as sub menu with list of items.
*/

int QtxListAction::popupMode() const
{
  return myMode;
}

/*!
	Name: setPopupMode [public]
	Desc: Set the popup mode. Popup mode define way in this action will be
	      added into popup menu. This function should be used before addTo.
*/

void QtxListAction::setPopupMode( const int mode )
{
  myMode = mode;
}

/*!
  \return list of names
*/
QStringList QtxListAction::names() const
{
  QStringList lst;
  if ( myFrame )
    lst = myFrame->names();
  return lst;
}

/*!
	Name: addNames [public]
	Desc: Fills the list of actions. Removes the old contents from
	      the list if 'clear' is true.
*/

void QtxListAction::addNames( const QStringList& names, bool clear )
{
  if ( !myFrame )
    return;

  if ( clear )
    myFrame->clear();

	myFrame->addNames( names );

	QStringList lst = myFrame->names();
	for ( PopupsMap::Iterator pit = myPopups.begin(); pit != myPopups.end(); ++pit )
	{
		int i = 1;
		QPopupMenu* pm = (QPopupMenu*)pit.key();
		for ( QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it )
			pit.data().popup->insertItem( *it, i++ );
		pm->setItemEnabled( pit.data().id, isEnabled() && pit.data().popup->count() );
  }

	for ( ButtonsMap::Iterator bit = myButtons.begin(); bit != myButtons.end(); ++bit )
	{
		bit.data().drop->setEnabled( isEnabled() && !lst.isEmpty() );
		bit.data().main->setEnabled( isEnabled() && !lst.isEmpty() );
	}
}

/*!
	Name: addTo [virtual public]
	Desc: Adds this control to 'popup' or 'toolbar'.
*/

bool QtxListAction::addTo( QWidget* w )
{
	if ( myButtons.contains( w ) || myPopups.contains( w ) )
		return false;

	if ( !w->inherits( "QPopupMenu" ) || popupMode() != SubMenu )
		if ( !QtxAction::addTo( w ) )
			return false;

#if QT_VER < 3
  if ( w->children() )
    addedTo( (QWidget*)w->children()->getLast(), w );
#endif

  if ( w->inherits( "QToolBar" ) )
  {
		Buttons& entry = myButtons[w];
		QHBox* dropWrap = new QHBox( w );
    entry.drop = new ToolButton( this, dropWrap, "qt_dockwidget_internal" );

    entry.drop->setTextLabel( text() );
    entry.drop->setToggleButton( true );
    entry.drop->setAutoRaise( entry.main->autoRaise() );

    entry.main->setEnabled( isEnabled() && !myFrame->names().isEmpty() );
    entry.drop->setEnabled( isEnabled() && !myFrame->names().isEmpty() );

		entry.main->installEventFilter( this );
    entry.drop->installEventFilter( this );

		QToolTip::add( entry.drop, toolTip(), myTipGroup, statusTip() );

    connect( entry.drop, SIGNAL( toggled( bool ) ), this, SLOT( onExpand( bool ) ) );
  }
	else if ( w->inherits( "QPopupMenu" ) && popupMode() == SubMenu )
	{
		Popups entry;
		QPopupMenu* pm = (QPopupMenu*)w;

		entry.popup = new QPopupMenu( pm );
		entry.id = pm->insertItem( text(), entry.popup );

		int i = 1;
		QStringList lst = myFrame->names();
		for ( QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it )
		{
			int id = entry.popup->insertItem( *it );
			entry.popup->setItemParameter( id, i++ );
		}
		pm->setItemEnabled( entry.id, isEnabled() && entry.popup->count() );
		myPopups.insert( w, entry );

		connect( entry.popup, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
	}

#if QT_VER >= 3
	connect( w, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
#endif

    return true;
}

/*!
	Name: addTo [virtual public]
	Desc: Adds this control to 'popup' or 'toolbar'. Allow to specify index
	      for adding into 'popup'.
*/

bool QtxListAction::addTo( QWidget* w, const int idx )
{
  return QtxAction::addTo( w, idx );
}

/*!
	Name: removeFrom [virtual public]
	Desc: Removes this control from 'popup' or 'toolbar'.
*/

bool QtxListAction::removeFrom( QWidget* w )
{
  if ( !QtxAction::removeFrom( w ) )
    return false;

  if ( w->inherits( "QToolBar" ) )
  {
    if ( myFrame )
      myFrame->hide();

    if ( myButtons.contains( w ) )
    {
      Buttons& entry = myButtons[w];

      if ( entry.drop->parent() && entry.drop->parent()->parent() == w )
        delete entry.drop->parent();
      else
        delete entry.drop;
    }
    myButtons.remove( w );
  }
  else if ( w->inherits( "QPopupMenu" ) )
    myPopups.remove( w );

#if QT_VER >= 3
	disconnect( w, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
#endif

  return true;
}

/*!
	Name: setEnabled [virtual public slot]
	Desc: Enables/disables this control.
*/

void QtxListAction::setEnabled( bool enable )
{
  QtxAction::setEnabled( enable );

	bool isOn = enable && !myFrame->names().isEmpty();

	for ( ButtonsMap::Iterator bit = myButtons.begin(); bit != myButtons.end(); ++bit )
	{
		bit.data().drop->setEnabled( isOn );
		bit.data().main->setEnabled( isOn );
	}

	for ( PopupsMap::Iterator pit = myPopups.begin(); pit != myPopups.end(); ++pit )
	{
		QPopupMenu* cont = (QPopupMenu*)pit.key();
		cont->setItemEnabled( pit.data().id, isOn );
  }
}

/*!
	Name: setMaxLines [public]
	Desc: Sets max number of lines that list frame shows
		  without vertical scroll bar. Default value is 5.
*/

void QtxListAction::setMaxLines( int nlines )
{
  myFrame->setMaxLines( nlines );
}

/*!
	Name: setMaxLineChars [public]
	Desc: Sets max number of characters in a line which list frame shows
	      without truncation. Default value is 12 (the widest char size is used).
*/

void QtxListAction::setMaxLineChars( int nchars )
{
  myFrame->setMaxLineChars( nchars );
}

/*!
	Name: setComment [public]
	Desc: Sets the format Qt string for comments displayed under the list
	      of actions for one action and for several actions.
		    Ex. "Undo %1 actions" format string will work as "Undo 3 actions"
		    when 3 actions are selected. The default format string is "%1".
*/

void QtxListAction::setComment( const QString& c, const QString& sc )
{
  if ( !myFrame )
    return;

  myFrame->setSingleComment( sc.isEmpty() ? c : sc );
	myFrame->setMultipleComment( c );
}

/*!
	Name: eventFilter [virtual public]
	Desc: Reimplemented to paint the tool buttons in 2D/3D.
*/

bool QtxListAction::eventFilter( QObject* o, QEvent* e )
{
  if ( !myRaise && ( e->type() == QEvent::Enter || e->type() == QEvent::Leave ) )
  {
		QWidget* obj = 0;
		QWidget* wid = widget( (QWidget*)o );
		if ( o == mainButton( wid ) )
			obj = dropButton( wid );
		else if ( o == dropButton( wid ) )
			obj = mainButton( wid );

		if ( obj )
		{
			myRaise = true;
			QApplication::sendEvent( obj, e );
            obj->repaint();
			myRaise = false;
		}
  }
  return QObject::eventFilter( o, e );
}

/*!
	Name: addedTo [protected]
	Desc: Reimplemented for internal reasons.
*/

void QtxListAction::addedTo( QWidget* actionWidget, QWidget* container )
{
#if QT_VER >= 3
	QtxAction::addedTo( actionWidget, container );
#endif

	if ( !container->inherits( "QToolBar" ) )
		return;

	Buttons entry;
	entry.main = (QToolButton*)actionWidget;

	myButtons.insert( container, entry );
}

/*!
	Name: initialize [private]
	Desc: Initialization of object QtxListAction.
*/

void QtxListAction::initialize()
{
	myTipGroup = new QToolTipGroup( this );

	myFrame = new QtxListFrame( this, qApp->mainWidget() );
  myFrame->setMaxLines( 5 );
  myFrame->setMaxLineChars( 7 );

	myFrame->hide();

	connect( myFrame, SIGNAL( hided() ), this, SLOT( onHided() ) );
  connect( this, SIGNAL( activated() ), this, SLOT( onSingle() ) );
	connect( myFrame, SIGNAL( selected( int ) ), this, SLOT( onMultiple( int ) ) );

	connect( myTipGroup, SIGNAL( removeTip() ), this, SLOT( clearStatusText() ) );
	connect( myTipGroup, SIGNAL( showTip( const QString& ) ), this, SLOT( showStatusText( const QString& ) ) );
}

/*!
	Name: onSingle [private slot]
	Desc: Called when a single action is selected.
*/

void QtxListAction::onSingle()
{
  emit activated( 1 );
}

/*!
	Name: onMultiple [private slot]
	Desc: Called when multiple actions are selected.
*/

void QtxListAction::onMultiple( int numActions )
{
  if ( myFrame )
    myFrame->hide();

  if ( numActions > 0 )
    emit activated( numActions );
}

/*!
	Name: onExpand [private slot]
	Desc: Activates the list of actions.
*/

void QtxListAction::onExpand( bool on )
{
	const QObject* obj = sender();
  if ( on )
  {
    QWidget* wid = widget( (QToolButton*)obj );
		QToolButton* main = mainButton( wid );
    myFrame->setOwner( main );
		if ( main )
			myFrame->show();
  }
  else
    myFrame->hide();
}

/*!
  SLOT: called when frame is hidden
*/
void QtxListAction::onHided()
{
  for ( ButtonsMap::Iterator bit = myButtons.begin(); bit != myButtons.end(); ++bit )
	{
    bool block = bit.data().drop->signalsBlocked();
    bit.data().drop->blockSignals( true );
    bit.data().drop->setOn( false );
    bit.data().drop->blockSignals( block );
  }
}

/*!
	Name: onActivated [private slot]
	Desc: Called when a sub menu item is activated.
*/

void QtxListAction::onActivated( int id )
{
	QPopupMenu* pm = (QPopupMenu*)sender();
	int num = pm->itemParameter( id );
	if ( num > 0 )
		emit activated( num );
}

/*!
	Name: onDestroyed [private slot]
	Desc: Called when a container widget is destroyed.
*/

void QtxListAction::onDestroyed( QObject* obj )
{
	if ( !obj->isWidgetType() )
		return;

	myPopups.remove( (QWidget*)obj );
	myButtons.remove( (QWidget*)obj );
}

/*!
	Name: widget [private]
	Desc: Returns container widget for specified control.
*/

QWidget* QtxListAction::widget( QWidget* obj ) const
{
	QWidget* wid = 0;
	for ( PopupsMap::ConstIterator pit = myPopups.begin(); pit != myPopups.end() && !wid; ++pit )
		if ( pit.data().popup == obj )
			wid = pit.key();

	for ( ButtonsMap::ConstIterator bit = myButtons.begin(); bit != myButtons.end() && !wid; ++bit )
		if ( bit.data().main == obj || bit.data().drop == obj )
			wid = bit.key();

	return wid;
}

/*!
	Name: listPopup [private]
	Desc: Returns sub popup menu widget for specified container.
*/

QPopupMenu* QtxListAction::listPopup( QWidget* wid ) const
{
	QPopupMenu* p = 0;
	if ( myPopups.contains( wid ) )
		p = myPopups[wid].popup;
	return p;
}

/*!
	Name: mainButton [private]
	Desc: Returns main tool button for specified container.
*/

QToolButton* QtxListAction::mainButton( QWidget* wid ) const
{
	QToolButton* mb = 0;
	if ( myButtons.contains( wid ) )
		mb = myButtons[wid].main;
	return mb;
}

/*!
	Name: dropButton [private]
	Desc: Returns drop tool button for specified container.
*/

QToolButton* QtxListAction::dropButton( QWidget* wid ) const
{
	QToolButton* db = 0;
	if ( myButtons.contains( wid ) )
		db = myButtons[wid].drop;
	return db;
}

/*!
	Name: controlDeleted [private]
	Desc: Called when action child controls deleted.
*/

void QtxListAction::controlDeleted( QWidget* wid )
{
  QWidget* w = 0;
  for ( ButtonsMap::Iterator it = myButtons.begin(); it != myButtons.end() && !w; ++it )
  {
    if ( it.data().main == wid || it.data().drop == wid )
      w = it.key();
  }

  if ( w )
  {
    if ( myFrame )
      myFrame->hide();

    myButtons.remove( w );
  }
}

/*!
  \class QtxListFrame
  Frame for the list of actions
*/
class QtxListFrame::ScrollEvent : public QCustomEvent
{
public:
	enum { Scroll = User + 1 };

	ScrollEvent( bool down ) : QCustomEvent( Scroll ), myDown( down ) {};
	virtual ~ScrollEvent() {};

	bool isDown() const { return myDown; };

private:
	bool myDown;
};

/*!
	Class: QtxListAction
	Level: Public
*/

/*!
    Constructor
*/
QtxListFrame::QtxListFrame( QtxListAction* a, QWidget* parent, WFlags f )
: QFrame( parent, 0, WStyle_Customize | WStyle_NoBorderEx | WType_Popup | WStyle_Tool | WStyle_StaysOnTop ),
myList( 0 ),
myOwner( 0 ),
myAction( a ),
myComment( 0 ),
myMaxLines( 5 ),
myMaxLineChars( 10 ),
myScrollVal( 0 ),
myScrollBlock( false )
{
  QVBoxLayout* theLayout = new QVBoxLayout( this, 3 );
	theLayout->setResizeMode( QLayout::FreeResize );

  myList = new QListBox( this );
  myList->setSelectionMode( QListBox::Multi );
  myList->setHScrollBarMode( QScrollView::AlwaysOff );
	myList->setFocusPolicy( NoFocus );

	QPalette p = myList->palette();
	p.setColor( QPalette::Inactive, QColorGroup::Highlight,
				      p.color( QPalette::Active, QColorGroup::Highlight ) );
	p.setColor( QPalette::Inactive, QColorGroup::HighlightedText,
				      p.color( QPalette::Active, QColorGroup::HighlightedText ) );
	myList->setPalette( p );

  /*  We'll have the vertical scroll bar only and
      truncate the names which are too wide */
  connect( myList, SIGNAL( contentsMoving( int, int ) ), this, SLOT( onScroll( int, int ) ) );

  myComment = new QLabel( this );
  myComment->setFrameStyle( Panel | Sunken );
  myComment->setAlignment( AlignCenter );
  myMultipleComment = "%1";

  theLayout->addWidget( myList );
  theLayout->addWidget( myComment );

  setFrameStyle( Panel | Raised );
}

/*!
  Destructor
*/
QtxListFrame::~QtxListFrame()
{
  if ( myAction )
    myAction->myFrame = 0;
}

/*!
    Clears list of names [ public ]
*/

void QtxListFrame::clear()
{
	myNames.clear();
	setNames( myNames );
}

/*!
    Adds a names to the list. Truncates the name to fit to the frame width.
    Use QtxListAction::setMaxLineChar( int ) to set the width in characters. [ public ]
*/
  
void QtxListFrame::addNames( const QStringList& names )
{
	for ( QStringList::ConstIterator it = names.begin(); it != names.end(); ++it )
		myNames.append( *it );
	setNames( myNames );
}

/*!
    Sets a names to the list. Truncates the name to fit to the frame width.
    Use QtxListAction::setMaxLineChar( int ) to set the width in characters. [ public ]
*/

void QtxListFrame::setNames( const QStringList& names )
{
  if ( !myList )
		return;

	myList->clear();

	for ( QStringList::ConstIterator it = names.begin(); it != names.end(); ++it )
	{
		QString s = *it;
    QFontMetrics fm = myList->fontMetrics();
    int maxW = myMaxLineChars * fm.maxWidth();
    int w = fm.width( s );
    if ( w > maxW )
    {
      QString extra( "..." );
      int len = s.length();
      int extraLen = fm.width( extra ) + 1;
      while ( true )
      {
        w = fm.width( s, --len );
        if ( w + extraLen < maxW )
        {
          s = s.left( len );
          break;
        }
      }
      s += extra;
    }
    myList->insertItem( s );
  }
}

/*!
  \return list of names
*/
const QStringList QtxListFrame::names() const
{
	return myNames;
}

/*!
    Sets max number of lines shown without activation of vertical scroll bar. [ public ]
*/

void QtxListFrame::setMaxLines( int maxLines )
{
  myMaxLines = maxLines;
}

/*!
    Sets max number of chars in line ( the rest will be truncated ). [ public ]
*/

void QtxListFrame::setMaxLineChars( int maxChars )
{
	if ( myMaxLineChars == maxChars )
		return;

  myMaxLineChars = maxChars;
	setNames( myNames );
}

/*!
    Sets the format of single comment. [ public ]
*/

void QtxListFrame::setSingleComment( const QString& comment )
{
  mySingleComment = comment;
	setNames( myNames );
  updateComment();
}

/*!
    Sets the format of multiple comment. [ public ]
*/

void QtxListFrame::setMultipleComment( const QString& comment )
{
  myMultipleComment = comment;
	setNames( myNames );
  updateComment();
}

/*!
    Updates comment display. [ public ]
*/

void QtxListFrame::updateComment()
{
	QString com;
	int selNum = selected();
	if ( selNum > 1 )
		com = myMultipleComment;
	else if ( selNum > 0 && !mySingleComment.isEmpty() )
		com = mySingleComment;

	if ( !com.isEmpty() )
		com = com.arg( selNum );

  myComment->setText( com );
}

/*!
  Sets owner
  \param wo - new owner
*/
void QtxListFrame::setOwner( QWidget* wo )
{
  myOwner = wo;
  if ( myOwner )
  {
    QPoint lpos;
    if ( myOwner->parentWidget() && myOwner->parentWidget()->inherits( "QToolBar" ) &&
         ((QToolBar*)myOwner->parentWidget())->orientation() == Qt::Vertical )
      lpos = QPoint( myOwner->x() + myOwner->width() + 2, myOwner->y() );
    else
		  lpos = QPoint( myOwner->x(), myOwner->y() + myOwner->height() + 2 );
      QPoint gpos = myOwner->parentWidget() ? myOwner->parentWidget()->mapToGlobal( lpos )
                                            : myOwner->mapToGlobal( lpos );
    if ( parentWidget() )
			move( parentWidget()->mapFromGlobal( gpos ) );
    else
		  move( gpos );
  }
}

/*!
    Validates the action. [ private slot ]
*/

void QtxListFrame::accept()
{
  emit selected( selected() );
}

/*!
    Cancels the action. [ private slot ]
*/

void QtxListFrame::reject()
{
  emit selected( 0 );
}

/*!
    Initializes / shows the frame. [ virtual public slot ]
*/

void QtxListFrame::show()
{
  int cnt = (int)myList->count();
  if ( cnt )
  {
    myScrollVal = 0;
		myList->setTopItem( 0 );
    myList->clearSelection();
		myList->setMinimumSize( 0, ( QMIN( cnt + 1, myMaxLines ) ) * myList->itemHeight() + 1 );
    setSelected( 1 );

    int linstep = myList->itemHeight();
    myList->verticalScrollBar()->setLineStep( linstep );
    myList->verticalScrollBar()->setPageStep( myMaxLines * linstep );

    QFontMetrics fm = myList->fontMetrics();
    layout()->invalidate();
    int maxHeight = layout()->minimumSize().height() + layout()->margin();
    int maxWidth = myMaxLineChars * fm.maxWidth();
		for ( uint i = 0; i <= myList->count(); i++ )
			maxWidth = QMAX( maxWidth, fm.width( myList->text( i ) ) );

		resize( width(), maxHeight );

		myList->updateGeometry();

		QApplication::sendPostedEvents();

    myList->resizeContents( myList->contentsWidth(),
                            myList->itemHeight() * cnt );
    if ( myList->contentsHeight() > myList->visibleHeight() )
         maxWidth += myList->verticalScrollBar()->width();

		QString single = mySingleComment.arg( cnt );
		QString multi = myMultipleComment.arg( cnt );
		int comWidth = QMAX( myComment->fontMetrics().width( single ), myComment->fontMetrics().width( multi ) );
		if ( myComment->frameWidth() )
			comWidth += myComment->fontMetrics().width( "x" );

		maxWidth = QMAX( maxWidth, comWidth );

		resize( maxWidth, maxHeight );
    updateComment();

    qApp->installEventFilter( this );

    QFrame::show();
  }
}

/*!
    Cleanup. [ virtual public slot ]
*/

void QtxListFrame::hide()
{
  qApp->removeEventFilter( this );
  QFrame::hide();
  emit hided();
}

/*!
    Processes KeyUp/KeyDown, PageUp/PageDown, CR and Esc keys.
    Returns 'true' if event is eaten, 'false' otherwise. [ private ]
*/

bool QtxListFrame::handleKeyEvent( QObject* , QKeyEvent* e )
{
  if ( e->type() == QEvent::KeyRelease )
    return true;

  int selNum = selected();
  switch( e->key() )
  {
  case Key_Up:
    setSelected( QMAX( 1, selNum - 1 ) );
    break;
  case Key_Down:
    setSelected( QMAX( 1, selNum + 1 ) );
    break;
  case Key_PageUp:
    setSelected( QMAX( 1, selNum - myMaxLines ) );
    break;
  case Key_PageDown:
	  setSelected( selNum += myMaxLines );
    break;
  case Key_Home:
	  setSelected( 1 );
		break;
  case Key_End:
	  setSelected( myList->count() );
		break;
  case Key_Return:
    accept();
    break;
  case Key_Escape:
    reject();
    break;
  }
  return true;
}

/*!
    Selects items on move, validates on button release. If object 'o' is not our name list,
    we close the frame. Returns 'true' if event is eaten, 'false' otherwise. [ private ]
*/

bool QtxListFrame::handleMouseEvent( QObject* o, QMouseEvent* e )
{
  switch( e->type() )
  {
  case QEvent::MouseButtonPress:
  {
	  if ( o != myList->viewport() && !isPopup() )
		  reject();
    return true;
  }
  case QEvent::MouseMove:
  {
    if ( o == myList->viewport() )
    {
      QListBoxItem* lbi = myList->itemAt( e->pos() );
      if ( lbi )
        setSelected( myList->index( lbi ) + 1 );
    }
    break;
  }
  case QEvent::MouseButtonRelease:
  {
    if ( o == myList->viewport() )
      accept();
    else
		  reject();
    break;
  }
  default:
    break;
  }
  return true;
}

/*!
  Custom event filter
*/
bool QtxListFrame::event( QEvent* e )
{
  if ( e->type() != (int)ScrollEvent::Scroll )
    return QFrame::event( e );

  ScrollEvent* se = (ScrollEvent*)e;
  if ( se->isDown() )
    setSelected( myList->topItem() + myList->numItemsVisible() );
  else
    setSelected( myList->topItem() + 1 );
  
  return true;
}

/*!
    Watches mouse events on the viewport of the list. [ virtual public ]
*/

bool QtxListFrame::eventFilter( QObject* o, QEvent* e )
{
  bool isKeyEvent = ( e->type() == QEvent::KeyPress ||
                      e->type() == QEvent::KeyRelease );
  bool isMouseEvent = ( e->type() == QEvent::MouseMove ||
                        e->type() == QEvent::MouseButtonPress ||
                        e->type() == QEvent::MouseButtonRelease ||
                        e->type() == QEvent::MouseButtonDblClick );

  if ( isKeyEvent )
  {
    if ( handleKeyEvent( o, ( QKeyEvent* )e ) )
      return true;
  }
  else if ( isMouseEvent && o != myList->verticalScrollBar() )
  {
    if ( handleMouseEvent( o, ( QMouseEvent*)e ) )
      return true;
  }

  if ( o != this && ( e->type() == QEvent::Resize || e->type() == QEvent::Move ) )
    setOwner( myOwner );

  return QFrame::eventFilter( o, e );
}

/*!
    Selects operations while scrolling the list. [ private slot ]
*/

void QtxListFrame::onScroll( int x, int y )
{
  int dx = y - myScrollVal;
	if ( !myScrollBlock )
		QApplication::postEvent( this, new ScrollEvent( dx > 0 ) );
  myScrollVal = y;
}

/*!
    Selects the actions [ 0 - lastSel ]. [ public ]
*/

void QtxListFrame::setSelected( const int lastSel )
{
	int last = QMIN( lastSel, (int)myList->count() );

	for ( int i = 0; i < (int)myList->count(); i++ )
		myList->setSelected( i, i < last );

	int item = last - 1;

	myScrollBlock = true;

	if ( item < myList->topItem() )
		myList->setTopItem( item );

	if ( item >= myList->topItem() + myList->numItemsVisible() )
		myList->setTopItem( item - myList->numItemsVisible() + 1 );

	myScrollBlock = false;

  myList->clearFocus();

  updateComment();
}

/*!
  return number of selected items
*/
int QtxListFrame::selected() const
{
	uint sel = 0;
	while ( sel < myList->count() && myList->isSelected( sel ) )
		sel++;
	return sel;
}
