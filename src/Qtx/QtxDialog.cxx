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
// File:      QtxDialog.cxx
// Author:    Sergey TELKOV

#include "QtxDialog.h"

#include "QtxGroupBox.h"

#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qapplication.h>

/*!
	Class: QtxDialog::Area
	Level: Internal
*/
class QtxDialog::Area : public QFrame
{
public:
	Area( Orientation, QtxDialog*, QWidget* = 0 );
	virtual ~Area();

	bool                     isBorderEnabled() const;
	void                     setBorderEnabled( const bool );

	void                     setBorderWidget( QLabel* );

	void                     insertButton( QButton* );
	void                     removeButton( QButton* );
	bool                     contains( QButton* ) const;

	int                      policy() const;
	void                     setPolicy( const int );

	void                     layoutButtons();

	const QPtrList<QButton>& buttons() const;

private:
	void                     updateBorder();

private:
	QtxDialog*               myDlg;
	QLabel*                  myLine;
	bool                     myBorder;
	int                      myPolicy;
	QPtrList<QButton>        myButtons;
	Orientation              myOrientation;
};

/*!
  Contructor
*/
QtxDialog::Area::Area( Orientation o, QtxDialog* dlg, QWidget* parent )
: QFrame( parent ),
myDlg( dlg ),
myLine( 0 ),
myBorder( false ),
myPolicy( Position ),
myOrientation( o )
{
	if ( myOrientation == Qt::Horizontal )
		setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum ) );
	else
		setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred ) );

	hide();
}

/*!
  Destructor
*/
QtxDialog::Area::~Area()
{
}

/*!
  Inserts button to area
  \param b - button
*/
void QtxDialog::Area::insertButton( QButton* b )
{
	if ( !b || myButtons.findRef( b ) != -1 )
		return;

	QWidget* parent = b->parentWidget();
	if ( parent != this )
		b->reparent( this, 0, QPoint( 0, 0 ), b->isVisibleTo( parent ) );

	myButtons.append( b );

	if ( myDlg )
		myDlg->adjustButtons();
	layoutButtons();

	show();

	updateBorder();
}

/*!
  Removes button from area
  \param b - button
*/
void QtxDialog::Area::removeButton( QButton* b )
{
	if ( !b )
		return;

	myButtons.removeRef( b );

	if ( myButtons.isEmpty() )
		hide();

	updateBorder();

	if ( myDlg )
		myDlg->adjustButtons();

	layoutButtons();
}

/*!
  \return true if area contains button
  \param b - button
*/
bool QtxDialog::Area::contains( QButton* b ) const
{
	return myButtons.containsRef( b );
}

/*!
  \return policy of button layouting.
*/
int QtxDialog::Area::policy() const
{
	return myPolicy;
}

/*!
  Changes policy of button layouting.
  \param p - new policy
*/
void QtxDialog::Area::setPolicy( const int p )
{
	if ( myPolicy == p )
		return;

	myPolicy = p;
	layoutButtons();
}

/*!
  \return true if border enabled
*/
bool QtxDialog::Area::isBorderEnabled() const
{
	return myLine && myBorder;
}

/*!
  Enables/disable separator between main frame and button frame
  \param on - new state
*/
void QtxDialog::Area::setBorderEnabled( const bool on )
{
	if ( !myLine || myBorder == on )
		return;

	myBorder = on;
	updateBorder();
}

/*!
  Sets label as separator between main frame and button frame
  \param line - new separator
*/
void QtxDialog::Area::setBorderWidget( QLabel* line )
{
	if ( myLine == line )
		return;

	delete myLine;
	myLine = line;
	updateBorder();
}

/*!
  \return const reference to list of buttons
*/
const QPtrList<QButton>& QtxDialog::Area::buttons() const
{
	return myButtons;
}

/*!
  Updates visibility of border
*/
void QtxDialog::Area::updateBorder()
{
	if ( !myLine )
		return;

	bool isVis = isVisibleTo( parentWidget() );
#if QT_VER >= 3    
	if ( isVis )
#else
	if ( isVis && myBorder )
#endif
		myLine->show();
	else
		myLine->hide();

	myLine->setLineWidth( myBorder ? 1 : 0 );
}

/*!
  Installs buttons into layout
*/
void QtxDialog::Area::layoutButtons()
{
	int aPolicy = policy();

	QMap<QButton*, int> buttonId;
	for ( QPtrListIterator<QButton> it1( myButtons ); it1.current(); ++it1 )
		buttonId.insert( it1.current(), 0 );

	QPtrList<QButton> src;
	for ( ButtonMap::Iterator mit = myDlg->myButton.begin();
		  mit != myDlg->myButton.end(); ++mit )
	{
		if ( buttonId.contains( mit.data() ) )
		{
			buttonId[mit.data()] = mit.key();
			if ( mit.key() >= 0 )
				src.append( mit.data() );
		}
	}

	for ( QPtrListIterator<QButton> it2( myButtons ); it2.current(); ++it2 )
		if ( buttonId[it2.current()] < 0 )
			src.append( it2.current() );

	QPtrList<QButton> left, right, center, other;
	for ( QPtrListIterator<QButton> it( src ); it.current(); ++it )
	{
		if ( !it.current()->isVisibleTo( this ) )
			continue;

		int aPosition = myDlg->buttonPosition( it.current() );
		if ( aPosition == -1 )
			continue;

		if ( aPolicy != QtxDialog::Position )
			other.append( it.current() );
		else if ( aPosition == QtxDialog::Left )
			left.append( it.current() );
		else if ( aPosition == QtxDialog::Right )
			right.append( it.current() );
		else if ( aPosition == QtxDialog::Center )
			center.append( it.current() );
	}

  delete layout();

	QBoxLayout* buttonLayout = 0;
	if ( myOrientation == Qt::Vertical )
		buttonLayout = new QVBoxLayout( this, 0, 5 );
	else
		buttonLayout = new QHBoxLayout( this, 0, 5 );

	if ( !buttonLayout )
		return;

	if ( aPolicy == QtxDialog::Position )
	{
		for ( QPtrListIterator<QButton> lit( left ); lit.current(); ++lit )
			buttonLayout->addWidget( lit.current() );
		buttonLayout->addStretch( 1 );
		for ( QPtrListIterator<QButton> cit( center ); cit.current(); ++cit )
			buttonLayout->addWidget( cit.current() );
		buttonLayout->addStretch( 1 );
		for ( QPtrListIterator<QButton> rit( right ); rit.current(); ++rit )
			buttonLayout->addWidget( rit.current() );
	}
	else
	{
		for ( QPtrListIterator<QButton> oit( other ); oit.current(); ++oit )
		{
			buttonLayout->addWidget( oit.current() );
			if ( aPolicy == QtxDialog::Uniform && !oit.atLast() )
				buttonLayout->addStretch( 1 );
		}
	}

  QWidgetList wids;
  if ( layout() )
  {
    for ( QLayoutIterator it = layout()->iterator(); it.current(); ++it )
    {
      if ( !it.current()->widget() )
        continue;

      if ( QApplication::reverseLayout() )
        wids.prepend( it.current()->widget() );
      else
        wids.append( it.current()->widget() );
    }
  }
  Qtx::setTabOrder( wids );
}


/*!
  \class QtxDialog::Border

  Special label used as separator between main frame and button frame
*/
class QtxDialog::Border : public QLabel
{
public:
	Border( QWidget* = 0 );
	virtual ~Border();

	virtual void setLineWidth( int );

	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
};

/*!
  Constructor
*/
QtxDialog::Border::Border( QWidget* parent )
: QLabel( parent )
{
    setAlignment( Qt::AlignCenter );
}

/*!
  Destructor
*/
QtxDialog::Border::~Border()
{
}

/*!
  Set line width of separator
  \param lw - new line width
*/
void QtxDialog::Border::setLineWidth( int lw )
{
  bool isOn = lineWidth() > 0;

  QLabel::setLineWidth( lw );
    
  if ( isOn != ( lineWidth() > 0 ) )
    updateGeometry();
}

/*!
  \return the recommended size for the widget
*/
QSize QtxDialog::Border::sizeHint() const
{
  QSize sz( 5, 5 );

#if QT_VER >= 3    
  if ( lineWidth() > 0 )
	{
    if ( frameShape() == VLine )
      sz += QSize( 5 + lineWidth(), 0 );
		else if ( frameShape() == HLine )
      sz += QSize( 0, 5 + lineWidth() );
	}
#endif

  return sz;
}

/*!
  \return the recommended minimum size for the widget
*/
QSize QtxDialog::Border::minimumSizeHint() const
{
	return sizeHint();
}

/*!
  Constructor
  Construct a dialog with specified parent and name.
  \param modal define modal status of dialog (default non modal dialog created).
  \param allowResize - if it is true then dialog can be resize by user (default non resizable dialog created).
  \param Button flags specified control buttons for dialog (default buttons is OK, Cancel and Help).
  \param Widget flags used as in any widget.
*/
QtxDialog::QtxDialog( QWidget* parent, const char* name,
					  bool modal, bool allowResize, const int f, WFlags wf )
: QDialog( parent, name, modal,
#ifdef WIN32
           wf | WStyle_Customize | WStyle_Title | WStyle_SysMenu |
           ( allowResize ? WStyle_NormalBorder : WStyle_NoBorderEx ) ),
#else
           wf | WStyle_NormalBorder | WStyle_Customize | WStyle_Title | WStyle_SysMenu ),
#endif
mySender( 0 ),
myAlignment( 0 ),
myInited( false ),
myDialogFlags( Accept | SetFocus )
{
	QVBoxLayout* base = new QVBoxLayout( this, 5, 0 );
	QtxGroupBox* main = new QtxGroupBox( 1, Qt::Horizontal, "", this );
	main->setFrameStyle( QFrame::NoFrame );
	main->setInsideMargin( 0 );
	main->setInsideSpacing( 0 );
	base->addWidget( main );

	Area* topArea = new Area( Qt::Horizontal, this, main );
	QLabel* topLine = new Border( main );
	QtxGroupBox* midGroup = new QtxGroupBox( 1, Qt::Vertical, "", main );
	QLabel* botLine = new Border( main );
	Area* botArea = new Area( Qt::Horizontal, this, main );

	midGroup->setFrameStyle( QFrame::NoFrame );
	midGroup->setInsideMargin( 0 );
	midGroup->setInsideSpacing( 0 );

	Area* leftArea = new Area( Qt::Vertical, this, midGroup );
	QLabel* leftLine = new Border( midGroup );
	myMainFrame = new QFrame( midGroup );
	QLabel* rightLine = new Border( midGroup );
	Area* rightArea = new Area( Qt::Vertical, this, midGroup );

	myMainFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding,
											        QSizePolicy::Expanding ) );

	topLine->setFrameStyle( QFrame::Sunken | QFrame::HLine );
	botLine->setFrameStyle( QFrame::Sunken | QFrame::HLine );
	leftLine->setFrameStyle( QFrame::Sunken | QFrame::VLine );
	rightLine->setFrameStyle( QFrame::Sunken | QFrame::VLine );
	topArea->setBorderWidget( topLine );
	botArea->setBorderWidget( botLine );
	leftArea->setBorderWidget( leftLine );
	rightArea->setBorderWidget( rightLine );

	myArea.insert( TopArea,    topArea );
	myArea.insert( BottomArea, botArea );
	myArea.insert( LeftArea,   leftArea );
	myArea.insert( RightArea,  rightArea );

	for ( AreaMap::Iterator itr = myArea.begin(); itr != myArea.end(); ++ itr )
		itr.data()->setBorderEnabled( false );

	myButton.insert( OK,     new QPushButton( tr( "&OK" ),     this ) );
	myButton.insert( Cancel, new QPushButton( tr( "&Cancel" ), this ) );
	myButton.insert( Close,  new QPushButton( tr( "C&lose" ),  this ) );
	myButton.insert( Help,   new QPushButton( tr( "&Help" ),   this ) );
	myButton.insert( Apply,  new QPushButton( tr( "&Apply" ),  this ) );
	myButton.insert( Yes,    new QPushButton( tr( "&Yes" ),    this ) );
	myButton.insert( No,     new QPushButton( tr( "&No" ),     this ) );

	for ( ButtonMap::Iterator it = myButton.begin(); it != myButton.end(); ++it )
	{
		((QPushButton*)it.data())->setAutoDefault( false );
#if QT_VER >= 3
		connect( it.data(), SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
#endif
	}

	setButtonPosition( Left,   OK | Cancel | Apply );
	setButtonPosition( Center, Yes | No | Close );
	setButtonPosition( Right,  Help );
	setButtonPlace( BottomArea, All );

	connect( myButton[Apply],  SIGNAL( clicked() ), this, SIGNAL( dlgApply() ) );
	connect( myButton[Help],   SIGNAL( clicked() ), this, SIGNAL( dlgHelp() ) );

	connect( myButton[OK],     SIGNAL( clicked() ), this, SLOT( onAccept() ) );
	connect( myButton[Cancel], SIGNAL( clicked() ), this, SLOT( onReject() ) );
	connect( myButton[Yes],    SIGNAL( clicked() ), this, SLOT( onAccept() ) );
	connect( myButton[No],     SIGNAL( clicked() ), this, SLOT( onReject() ) );
	connect( myButton[Close],  SIGNAL( clicked() ), this, SLOT( onReject() ) );

	QPixmap icon;
	if ( qApp && qApp->mainWidget() && qApp->mainWidget()->icon() )
		setIcon( *qApp->mainWidget()->icon() );

	myButtonFlags = f;

#ifndef WIN32
	if ( !allowResize )
		setMaximumSize( minimumSize() );
#endif

	update();
}

/*!
	Name: ~QtxDialog [public]
	Desc: Destructor
*/

QtxDialog::~QtxDialog()
{
}

/*!
	Name: setButtonFlags [public]
	Desc: Allow to set specified control button(s) into dialog.
*/

void QtxDialog::setButtonFlags( const int f )
{
	int old = myButtonFlags;
	myButtonFlags = myButtonFlags | f;
	if ( old != myButtonFlags )
		update();
}

/*!
	Name: clearButtonFlags [public]
	Desc: Allow to unset specified control button(s) from dialog.
*/

void QtxDialog::clearButtonFlags( const int f )
{
	int old = myButtonFlags;
	myButtonFlags = myButtonFlags & ~f;
	if ( old != myButtonFlags )
		update();
}

/*!
	Name: testButtonFlags [public]
	Desc: Return true if specified control button is used in dialog.
*/

bool QtxDialog::testButtonFlags( const int f ) const
{
    return ( myButtonFlags & f ) == f;
}

/*!
	Name: setDialogFlags [public]
	Desc: Allow to set the dialog flags.

		  Following flags can be used:
			Accept    - Allow to control dialog accepting. See also acceptData().
			Reject    - Allow to control dialog rejecting. See also rejectData().
			AlignOnce - Allow to align dialog only when it first time shown.
			SetFocus  - Allow to set focus on dialog when it shown. User can use
                  setFocusProxy() and specify own initial focus widget.
*/

void QtxDialog::setDialogFlags( const int f )
{
	myDialogFlags = myDialogFlags | f;
}

/*!
	Name: clearDialogFlags [public]
	Desc: Allow to clear the dialog flags. See also setDialogFlags().
*/

void QtxDialog::clearDialogFlags( const int f )
{
	myDialogFlags = myDialogFlags & ~f;
}

/*!
	Name: testDialogFlags [public]
	Desc: Returns true if specified dialog flag is setted (see setDialogFlags()).
*/

bool QtxDialog::testDialogFlags( const int f ) const
{
    return ( myDialogFlags & f ) == f;
}

/*!
	Name: mainFrame [public]
	Desc: Returns main frame of dialog. Main frame should contains all
          elements of dialog except control buttons.
*/

QFrame* QtxDialog::mainFrame() const
{
    return myMainFrame;
}

/*!
	Name: buttonPosition [public]
	Desc: Returns position of specified button.
*/

int QtxDialog::buttonPosition( const int id ) const
{
	int pos = -1;
	if ( myPosition.contains( id ) )
		pos = myPosition[id];
	return pos;
}

/*!
	Name: setButtonPosition [public]
	Desc: Sets the position for specified button(s). Following positions
		  may be used: Left, Right, Center, Top, Bottom.
*/

void QtxDialog::setButtonPosition( const int pos, const int id )
{
	ButtonMap map = buttons( id );

	QMap<QObject*, int> changed;
	for ( ButtonMap::Iterator it = map.begin(); it != map.end(); ++it )
	{
		if ( myPosition[it.key()] == pos )
			continue;
      
		myPosition[it.key()] = pos;
		if ( button( it.key() ) )
			changed.insert( button( it.key() )->parent(), 0 );
    }

	for ( AreaMap::Iterator itr = myArea.begin(); itr != myArea.end(); ++itr )
		if ( changed.contains( itr.data() ) )
			itr.data()->layoutButtons();
}

/*!
	Name: setPlacePosition [public]
	Desc: Sets button position for all buttons in given place.
*/

void QtxDialog::setPlacePosition( const int pos, const int area )
{
	if ( !myArea.contains( area ) )
		return;

	Area* anArea = myArea[area];

	bool changed = false;
	for ( ButtonMap::Iterator it = myButton.begin(); it != myButton.end(); ++it )
	{
		if ( !anArea->contains( it.data() ) )
			continue;

		changed = changed &&  myPosition[it.key()] != pos;

		myPosition[it.key()] = pos;
    }

	if ( changed )
		anArea->layoutButtons();
}

/*!
	Name: placePolicy [public]
	Desc: Returns policy of button layouting for specified place.

		  Following place may be used:
			TopArea    - Horizontal area in the top side of dialog.
			BottomArea - Horizontal area in the bottom side of dialog.
			LeftArea   - Vertical area in the left side of dialog.
			RightArea  - Vertical area in the right side of dialog.

		  Following policy may be used:
		    Position - Buttons placed according their position.
			Expand   - Buttons fills all available space.
			Uniform  - Buttons uniformly placed in area.
*/

int QtxDialog::placePolicy( const int area ) const
{
	int res = -1;
	if ( myArea.contains( area ) )
		res = myArea[area]->policy();
	return res;
}

/*!
	Name: setPlacePolicy [public]
	Desc: Sets the policy of button layouting for specified place.
		  See also placePolicy().
*/

void QtxDialog::setPlacePolicy( const int policy, const int area )
{
	if ( area < 0 )
		for ( AreaMap::Iterator itr = myArea.begin(); itr != myArea.end(); ++itr )
			itr.data()->setPolicy( policy );
	else if ( myArea.contains( area ) )
		myArea[area]->setPolicy( policy );
}

/*!
	Name: setButtonPlace [public]
	Desc: Move given button(s) into specified place.
*/

void QtxDialog::setButtonPlace( const int area, const int ids )
{
	if ( !myArea.contains( area ) )
		return;

	Area* anArea = myArea[area];

	ButtonMap map = buttons( ids );

	QMap<Area*, int> areaMap;
	for ( AreaMap::ConstIterator aIt = myArea.begin(); aIt != myArea.end(); ++aIt )
		areaMap.insert( aIt.data(), 0 );

	for ( ButtonMap::Iterator it = map.begin(); it != map.end(); ++it )
	{
		Area* old = (Area*)it.data()->parent();
		if ( old == anArea )
			continue;

		if ( areaMap.contains( old ) )
			old->removeButton( it.data() );
		anArea->insertButton( it.data() );
    }
}

/*!
	Name: isBorderEnabled [public]
	Desc: Returns true if border is shown for specified button area.
*/

bool QtxDialog::isBorderEnabled( const int area ) const
{
	bool res = false;
	if ( myArea.contains( area ) )
		res  = myArea[area]->isBorderEnabled();
	return res;
}

/*!
	Name: setBorderEnabled [public]
	Desc: Show/hide border for specified button area. Border are
	      line which separate main frame and control buttons.
*/

void QtxDialog::setBorderEnabled( const bool on, const int area )
{
	if ( !myArea.contains( area ) )
		return;

	if ( myArea[area]->isBorderEnabled() == on )
		return;

	myArea[area]->setBorderEnabled( on );

	if ( isVisible() )
	{
		QApplication::sendPostedEvents();
		adjustSize();
	}
}

/*!
	Name: isButtonEnabled [public]
	Desc: Returns true if all specified button(s) is enabled.
*/

bool QtxDialog::isButtonEnabled( const int f ) const
{
	ButtonMap map = buttons( f );
	bool result = !map.isEmpty();
	for ( ButtonMap::Iterator it = map.begin(); it != map.end(); ++it )
		result = result && it.data()->isEnabled();
	return result;
}

/*!
	Name: setButtonEnabled [public]
	Desc: Enable/disable specified button(s).
*/

void QtxDialog::setButtonEnabled( const bool on, const int ids )
{
	ButtonMap map = buttons( ids );
	for ( ButtonMap::Iterator it = map.begin(); it != map.end(); ++it )
		it.data()->setEnabled( on );
}

/*!
	Name: hasButtonFocus [public]
	Desc: Retruns true if specified button has keyboard focus.
*/

bool QtxDialog::hasButtonFocus( const int id ) const
{
	bool res = false;
	QButton* pb = button( id );
	if ( pb )
        res = pb->hasFocus();
	return res;
}

/*!
	Name: setButtonFocus [public]
	Desc: Sets the keyboard focus on specified button.
*/

void QtxDialog::setButtonFocus( const int id )
{
	QButton* pb = button( id );
	if ( pb )
	    pb->setFocus();
}

/*!
	Name: buttonText [public]
	Desc: Return text of specified button.
*/

QString QtxDialog::buttonText( const int id )
{
	QString retval;
	QButton* but = button( id );
	if ( but )
		retval = but->text();
	return retval;
}

/*!
	Name: setButtonText [public]
	Desc: Sets text to specified button.
*/

void QtxDialog::setButtonText( const int id, const QString& text )
{
	QButton* but = button( id );
	if ( but && but->text() != text )
	{
		but->setText( text );
		adjustButtons();
	}
}

/*!
	Name: setAlignment [public]
	Desc: Sets alignment policy. Returns the previous alignment.
		  Use the function before the first show of the dialog.
		  If dialog flag AlignOnce is setted then align will performed
		  only one time otherwise dialog will be aligned every time
		  when it shown. Dialog will be aligned relative to it parent.

		  Following align flags may be used:
			Qtx::AlignAuto      - Align to center of desktop (default).
			Qtx::AlignLeft      - Align left side of dialog to left side of parent.
			Qtx::AlignRight     - Align right side of dialog to right side of parent.
			Qtx::AlignTop       - Align top side of dialog to top side of parent.
			Qtx::AlignBottom    - Align bottom side of dialog to bottom side of parent.
			Qtx::AlignHCenter   - Align dialog to center of parent in horizontal dimension.
			Qtx::AlignVCenter   - Align dialog to center of parent in vertical dimension.
			Qtx::AlignCenter    - Align dialog to center of parent in both dimensions.
			Qtx::AlignOutLeft   - Align right side of dialog to left side of parent.
			Qtx::AlignOutRight  - Align left side of dialog to right side of parent.
			Qtx::AlignOutTop    - Align bottom side of dialog to top side of parent.
			Qtx::AlignOutBottom - Align top side of dialog to bottom side of parent.
*/

uint QtxDialog::setAlignment( uint align )
{
  uint oldAlign = myAlignment;
  myAlignment = align;
  return oldAlign;
}

/*!
	Name: update [virtual public slot]
	Desc: Updates dialog, show selected buttons and hide unselected.
*/

void QtxDialog::update()
{
	for ( ButtonMap::Iterator it = myButton.begin(); it != myButton.end(); ++it )
		if ( it.key() >= 0 )
			testButtonFlags( it.key() ) ? it.data()->show() : it.data()->hide();

	for ( AreaMap::Iterator itr = myArea.begin(); itr != myArea.end(); ++itr )
		itr.data()->layoutButtons();

	adjustButtons();

	QDialog::update();
}

/*!
	Name: show [virtual public]
	Desc: Show dialog, set keyboard focus on dialog.
*/

void QtxDialog::show()
{
	QDialog::show();

	if ( testDialogFlags( SetFocus ) )
	  setFocus();

	myInited = true;
}

/*!
	Name: hide [virtual public]
	Desc: Hides dialog, processed all posted events.
*/

void QtxDialog::hide()
{
	QDialog::hide();
	qApp->processEvents();
}

/*!
	Name: userButton [public]
	Desc: Return user dialog button using specified identificator.
*/

QButton* QtxDialog::userButton( const int id ) const
{
	QButton* b = 0;
	if ( id < -1 && myButton.contains( id ) )
		b = myButton[id];
    return b;
}

/*!
	Name: userButtonIds [public]
	Desc: Return list of user dialog button identificators.
*/

QValueList<int> QtxDialog::userButtonIds() const
{
	QValueList<int> retlist;
	for ( ButtonMap::ConstIterator it = myButton.begin(); it != myButton.end(); ++it )
		if ( it.key() < 0 )
			retlist.append( it.key() );
	return retlist;
}

/*!
	Name: insertButton [public]
	Desc: Add new user dialog button. Function return identificator of
		  newly added button in successfull case otherwise -1 will be returned.
*/

int QtxDialog::insertButton( const QString& text, const int area )
{
	if ( !myArea.contains( area ) )
		return -1;

	int id = -2;
	while ( myButton.contains( id ) )
		id--;

	Area* anArea = myArea[area];
	QButton* b = createButton( this );
	if ( b )
	{
		b->setText( text );
		myButton.insert( id, b );
		myPosition.insert( id, Left );

		connect( b, SIGNAL( clicked() ), this, SLOT( onButton() ) );
#if QT_VER >= 3
		connect( b, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
#endif

		anArea->insertButton( b );
		update();
	}
	else
		id = -1;

	return id;
}

/*!
	Name: removeButton [public]
	Desc: Remove user dialog button with specified identificator. If
		  identificator equal -1 then method will remove all user dialog buttons.
*/

void QtxDialog::removeButton( const int id )
{
	if ( id >= 0 )
		return;

	ButtonMap map;
	if ( id == -1 )
	{
		for ( ButtonMap::Iterator it = myButton.begin(); it != myButton.end(); ++it )
		{
			if ( it.key() < 0 )
				map.insert( it.key(), it.data() );
		}
	}
	else if ( myButton.contains( id ) )
		map.insert( id, myButton[id] );

	for ( ButtonMap::Iterator itr = map.begin(); itr != map.end(); ++itr )
	{
		for ( AreaMap::Iterator it = myArea.begin(); it != myArea.end(); ++it )
			it.data()->removeButton( itr.data() );

		myButton.remove( itr.key() );
		myPosition.remove( itr.key() );

		delete itr.data();
	}
    update();
}

/*!
	Name: setUnits [static public]
	Desc: Sets specified measure units in given label. Measure units close
		  in braces. If measure units not exist then they will be added.
		  For example:
			1. Label contains text 'Radius'.
			   setUnits( aLabel, "mm" )    => aLabel contains 'Radius (mm)'
               setUnits( aLabel, "cm" )    => aLabel contains 'Radius (cm)'
            2. Label "aLabel" contains text 'Radius ():'.
               setUnits( aLabel, "mm" )    => aLabel contains 'Radius (mm):'
               setUnits( aLabel, "cm" )    => aLabel contains 'Radius (cm):'
*/

void QtxDialog::setUnits( QLabel* aLabel, const QString& aUnits )
{
	QString label = aLabel->text();
	int begin;
	int end = label.findRev( ')' );

	QString startLabel = label;
	QString finalLabel;

	if ( end != -1 )
	{
		begin = label.left( end ).findRev( '(' );
		if ( begin != -1 )
		{
			startLabel = label.mid( 0, begin );
			finalLabel = label.mid( end + 1 );
		}
	}
	else
	{
		startLabel = startLabel.stripWhiteSpace();
		if ( startLabel.at( startLabel.length() - 1 ) == ':' )
		{
			finalLabel = startLabel.mid( startLabel.length() - 1 );
			startLabel = startLabel.mid( 0, startLabel.length() - 1 );
		}
	}
	if ( aUnits.length() )
		label = startLabel.stripWhiteSpace() +
		" (" + aUnits + ") " + finalLabel.stripWhiteSpace();
	else
		label = startLabel.stripWhiteSpace() +
		" " + finalLabel.stripWhiteSpace();
	aLabel->setText( label );
}

/*!
	Name: acceptData [virtual protected]
	Desc: If returns true dialog will be accepted and closed. This method
	      called if dialog flag Accept is setted.
*/

bool QtxDialog::acceptData() const
{
  return true;
}

/*!
	Name: rejectData [virtual protected]
	Desc: If returns true dialog will be rejected and closed. This method
	      called if dialog flag Reject is setted.
*/

bool QtxDialog::rejectData() const
{
	return true;
}

/*!
	Name: createButton [virtual protected]
	Desc: Create new user button. Invoked from method "insertButton".
*/

QButton* QtxDialog::createButton( QWidget* parent )
{
	QPushButton* pb = new QPushButton( parent );
	pb->setAutoDefault( false );
	return pb;
}

/*!
	Name: button [protected]
	Desc: Return pointer on control button specified by identifier.
		  If identifier is wrong then null pointer will returned.
*/

QButton* QtxDialog::button( const int f ) const
{
	QButton* retval = 0;
	if ( myButton.contains( f ) )
		retval = myButton[f];
	return retval;
}

/*!
	Name: buttons [protected]
	Desc: Return map with control dialog buttons accordance to given button flags.
*/

QtxDialog::ButtonMap QtxDialog::buttons( const int f ) const
{
	ButtonMap retmap;
	if ( f < -1 )
	{
		if ( myButton.contains( f ) )
			retmap.insert( f, myButton[f] );
	}
	else
	{
		for ( ButtonMap::ConstIterator it = myButton.begin(); it != myButton.end(); ++it )
			if ( f == -1 || ( it.key() >= 0 && f & it.key() ) )
				retmap.insert( it.key(), it.data() );
	}

	return retmap;
}

/*!
	Name: buttonId [protected]
	Desc: Return identifier of specified button.
*/

int QtxDialog::buttonId( const QButton* b ) const
{
	int id = -1;
	for ( ButtonMap::ConstIterator it = myButton.begin(); it != myButton.end() && id == -1; ++it )
		if ( it.data() == b )
			id = it.key();
	return id;
}

/*!
	Name: buttonPosition
	Desc: Returns position of specified button. [protected]
*/

int QtxDialog::buttonPosition( QButton* b ) const
{
	return buttonPosition( buttonId( b ) );
}

/*!
	Name: showEvent [virtual protected]
	Desc: Aligns this dialog according the parent widget and alignment
	      policy before the show.
*/

void QtxDialog::showEvent( QShowEvent* e )
{
	if ( !testDialogFlags( AlignOnce ) || !myInited )
		Qtx::alignWidget( this, parentWidget(), myAlignment );
	QDialog::showEvent( e );
}

/*!
	Name: hideEvent [virtual protected]
	Desc: Process all existing events when dialog is closed.
*/

void QtxDialog::hideEvent( QHideEvent* e )
{
	qApp->processEvents();
	QDialog::hideEvent( e );
}

/*!
	Name: childEvent [virtual protected]
	Desc: Setting up layout when size grip is added.
*/

void QtxDialog::childEvent( QChildEvent* e )
{
	QDialog::childEvent( e );
	if ( layout() && e->inserted() && e->child()->inherits( "QSizeGrip" ) )
	{
		layout()->setMargin( 12 );
#if QT_VER >= 3
		connect( e->child(), SIGNAL( destroyed() ), this, SLOT( onSizeGripDestroyed() ) );
#endif
	}
}

/*!
	Name: keyPressEvent [virtual protected]
	Desc: Calls reject() if key Escape is pressed.
	      Calls accept() if key "Ctrl+Enter" is pressed.
		  Process key "F1" and emit signal dlgHelp().
		  Transfer key "Ctrl+(Shift+)Tab" press event to Tab Widget.
*/

void QtxDialog::keyPressEvent( QKeyEvent* e )
{
	QDialog::keyPressEvent( e );
	if ( e->isAccepted() )
	    return;

	if ( ( e->state() == 0 ) && ( e->key() == Key_Escape ) )
		reject();

	if ( ( e->state() == ControlButton ) && ( e->key() == Key_Return ) )
	{
		if ( testButtonFlags( OK ) || testButtonFlags( Yes ) )
			accept();
		else if ( testButtonFlags( Apply ) && isButtonEnabled( Apply ) )
			emit dlgApply();
		e->accept();
	}

	if ( e->key() == Key_F1 && testButtonFlags( Help ) && isButtonEnabled( Help ) )
	{
		e->accept();
		emit dlgHelp();
	}

	if ( ( e->key() == Key_Tab ) && ( e->state() & ControlButton ) )
	{
		QObject* tab = child( 0, "QTabWidget" );
		if ( tab )
			tab->event( e );
	}
}

/*!
	Name: closeEvent [virtual protected]
	Desc: Reject the dialog.
*/

void QtxDialog::closeEvent( QCloseEvent* )
{
	reject();
}

/*!
	Name: accept [virtual protected slot]
	Desc: Invoke function acceptData() if it needed and if acceptData() return
		  false does nothing. Otherwise hides the dialog and sets the result code
		  to Accepted. Emit signal according to the pressed control button.
*/

void QtxDialog::accept()
{
	if ( !mySender )
	{
		if ( testButtonFlags( OK ) )
			mySender = button( OK );
		else if ( testButtonFlags( Yes ) )
			mySender = button( Yes );
		else
			mySender = button( Close );
	}

  if ( !mySender || !mySender->isWidgetType() ||
       !((QWidget*)mySender)->isEnabled() )
		return;

	if ( testDialogFlags( Accept ) && !acceptData() )
		return;

	QDialog::accept();

	emitSignal();
}

/*!
	Name: reject [virtual protected slot]
	Desc: Invoke function rejectData() if it needed and if rejectData() return
		  false does nothing. Otherwise hides the dialog and sets the result code
		  to Rejected. Emit signal according to the pressed control button. (If
		  dialog was closed by key Escape or by close event emit signal dlgCancel(),
		  or dlgClose(), or dlgNo().
*/

void QtxDialog::reject()
{
	if ( testDialogFlags( Reject ) && !rejectData() )
		return;

	if ( !mySender )
	{
		if ( testButtonFlags( Cancel ) )
			mySender = button( Cancel );
		else if ( testButtonFlags( No ) )
			mySender = button( No );
		else
			mySender = button( Close );
	}

    if ( !mySender || !mySender->isWidgetType() ||
         !((QWidget*)mySender)->isEnabled() )
        return;

	QDialog::reject();

	emitSignal();
}

/*!
	Name: reject [private]
	Desc: Emit signal appropriate to control button.
*/

void QtxDialog::emitSignal()
{
	qApp->processEvents();
	QApplication::syncX();

	int id = buttonId( (QButton*)mySender );
	mySender = 0;

	switch ( id )
	{
	case OK:
		emit dlgOk();
		break;
	case Cancel:
		emit dlgCancel();
		break;
	case Close:
		emit dlgClose();
		break;
	case Yes:
		emit dlgYes();
		break;
	case No:
		emit dlgNo();
		break;
	}
}

/*!
	Name: onAccept [private slot]
	Desc: Process signals "clicked()" from control buttons "OK", "Yes". Invoke accept().
*/

void QtxDialog::onAccept()
{
    const QObject* obj = sender();
    mySender = obj;
    accept();
}

/*!
	Name: onReject [private slot]
	Desc: Process signals "clicked()" from control buttons "Cancel", "No", "Close".
		  Invoke reject().
*/

void QtxDialog::onReject()
{
  const QObject* obj = sender();
  mySender = obj;
  reject();
}

/*!
	Name: onButton [private slot]
	Desc: Receive signal "clicked()" from user buttons and emit signal
		  "dlgButton( int )" with identificator of clicked user button.
*/

void QtxDialog::onButton()
{
	int id = buttonId( (QButton*)sender() );
	if ( id != -1 )
		emit dlgButton( id );
}

/*!
	Name: onDestroyed [private slot]
	Desc: Remove user button if it was destroyed.
*/

void QtxDialog::onDestroyed( QObject* obj )
{
	QButton* b = (QButton*)obj;
	int id = buttonId( b );
	if ( id == -1 )
		return;

	myButton.remove( id );
	myPosition.remove( id );
	for ( AreaMap::Iterator it = myArea.begin(); it != myArea.end(); ++it )
		it.data()->removeButton( b );
}

/*!
	Name: onSizeGripDestroyed [private slot]
	Desc: Setting up layout when size grip is destroyed.
*/

void QtxDialog::onSizeGripDestroyed()
{
	if ( layout() )
		layout()->setMargin( 5 );
}

/*!
	Name: adjustButtons [private]
	Desc: Setting the equal with for all buttons.
*/

void QtxDialog::adjustButtons()
{
	int minWidth = 0;
	for ( AreaMap::Iterator aIt = myArea.begin(); aIt != myArea.end(); ++aIt )
		for ( QPtrListIterator<QButton> bIt( aIt.data()->buttons() ); bIt.current(); ++bIt )
			if ( bIt.current()->isVisibleTo( this ) )
				minWidth = QMAX( minWidth, bIt.current()->sizeHint().width() );

	for ( AreaMap::Iterator aItr = myArea.begin(); aItr != myArea.end(); ++aItr )
		for ( QPtrListIterator<QButton> bItr( aItr.data()->buttons() ); bItr.current(); ++bItr )
			if ( bItr.current()->isVisibleTo( this ) )
				bItr.current()->setMinimumWidth( minWidth );
}
