// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File:      QtxDialog.cxx
// Author:    Sergey TELKOV
//
#include "QtxDialog.h"

#include <QLabel>
#include <QLayout>
#include <QKeyEvent>
#include <QFrame>
#include <QTabWidget>
#include <QPushButton>
#include <QApplication>

/*!
  \class QtxDialog::Area
  \internal
  \brief Area containing dialog box buttons.
*/

class QtxDialog::Area : public QFrame
{
public:
  Area( Qt::Orientation, QtxDialog*, QWidget* = 0 );
  virtual ~Area();

  bool                     isBorderEnabled() const;
  void                     setBorderEnabled( const bool );

  void                     setBorderWidget( QLabel* );

  void                     insertButton( QAbstractButton* );
  void                     removeButton( QAbstractButton* );
  bool                     contains( QAbstractButton* ) const;

  int                      policy() const;
  void                     setPolicy( const int );

  void                     layoutButtons();

  const QList<QAbstractButton*>& buttons() const;

private:
  void                     updateBorder();

private:
  QtxDialog*               myDlg;          //!< parent dialog box
  QLabel*                  myLine;         //!< border widget 
  bool                     myBorder;       //!< "has border" flag
  int                      myPolicy;       //!< button layout type (QtxDialog::PlacePolicy)
  QList<QAbstractButton*>  myButtons;      //!< buttons list
  Qt::Orientation          myOrientation;  //!< buttons orientation (Qt::Orientation)
};

/*!
  \brief Constructor.
  \param o buttons orientation
  \param dlg dialog box owning this area
  \param parent parent widget
*/
QtxDialog::Area::Area( Qt::Orientation o, QtxDialog* dlg, QWidget* parent )
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
  \brief Destructor.
*/
QtxDialog::Area::~Area()
{
}

/*!
  \brief Insert button to the area.
  \param b button to be added
  \sa removeButton()
*/
void QtxDialog::Area::insertButton( QAbstractButton* b )
{
  if ( !b || myButtons.contains( b ) )
    return;

  QWidget* parent = b->parentWidget();
  if ( parent != this )
    b->setParent( this );

  myButtons.append( b );

  if ( myDlg )
    myDlg->adjustButtons();
  layoutButtons();

  show();

  updateBorder();
}

/*!
  \brief Remove button from the area.
  \param b button to be removed
  \sa insertButton()
*/
void QtxDialog::Area::removeButton( QAbstractButton* b )
{
  if ( !b )
    return;

  myButtons.removeAll( b );

  if ( myButtons.isEmpty() )
    hide();

  updateBorder();

  if ( myDlg )
    myDlg->adjustButtons();

  layoutButtons();
}

/*!
  \brief Check if area owns the button specified.
  \param b button to be checked
  \return \c true if area contains button
*/
bool QtxDialog::Area::contains( QAbstractButton* b ) const
{
  return myButtons.contains( b );
}

/*!
  \brief Get buttons layout policy.
  \return policy of button layouting (Qtx::PlacePolicy)
  \sa setPolicy()
*/
int QtxDialog::Area::policy() const
{
  return myPolicy;
}

/*!
  \brief Set buttons layout policy.
  \param p new policy
*/
void QtxDialog::Area::setPolicy( const int p )
{
  if ( myPolicy == p )
    return;

  myPolicy = p;
  layoutButtons();
}

/*!
  \brief Check of the border is enabled.
  \return \c true if border is enabled
  \sa setBorderEnabled(), setBorderWidget()
*/
bool QtxDialog::Area::isBorderEnabled() const
{
  return myLine && myBorder;
}

/*!
  \brief Enable/disable border (separator between main frame and button frame)
  \param on new state
*/
void QtxDialog::Area::setBorderEnabled( const bool on )
{
  if ( !myLine || myBorder == on )
    return;

  myBorder = on;
  updateBorder();
}

/*!
  \brief Set border widget (separator between main frame and button frame).
  \param line new separator widget
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
  \brief Get all area buttons.
  \return const reference to the list of buttons
*/
const QList<QAbstractButton*>& QtxDialog::Area::buttons() const
{
  return myButtons;
}

/*!
  \brief Update border visibility.
*/
void QtxDialog::Area::updateBorder()
{
  if ( !myLine )
    return;

  bool isVis = isVisibleTo( parentWidget() );
  myLine->setVisible( isVis );

  myLine->setLineWidth( myBorder ? 1 : 0 );
}

/*!
  \brief Layout buttons in the area.
*/
void QtxDialog::Area::layoutButtons()
{
  int aPolicy = policy();

  QMap<QAbstractButton*, int> buttonId;
  for ( QList<QAbstractButton*>::iterator it1 = myButtons.begin(); it1 != myButtons.end(); ++it1 )
    buttonId.insert( *it1, 0 );

  QList<QAbstractButton*> src;
  for ( ButtonMap::Iterator mit = myDlg->myButton.begin(); mit != myDlg->myButton.end(); ++mit )
  {
    if ( buttonId.contains( mit.value() ) )
    {
      buttonId[mit.value()] = mit.key();
      if ( mit.key() >= 0 )
        src.append( mit.value() );
    }
  }

  for ( QList<QAbstractButton*>::iterator it2 = myButtons.begin(); it2 != myButtons.end(); ++it2 )
  {
    if ( buttonId[*it2] < 0 )
      src.append( *it2 );
  }

  QList<QAbstractButton*> left, right, center, other;
  for ( QList<QAbstractButton*>::iterator it = src.begin(); it != src.end(); ++it )
  {
    if ( !(*it)->isVisibleTo( this ) )
      continue;

    int aPosition = myDlg->buttonPosition( *it );
    if ( aPosition == -1 )
      continue;

    if ( aPolicy != QtxDialog::Position )
      other.append( *it );
    else if ( aPosition == QtxDialog::Left )
      left.append( *it );
    else if ( aPosition == QtxDialog::Right )
      right.append( *it );
    else if ( aPosition == QtxDialog::Center )
      center.append( *it );
  }

  delete layout();

  QBoxLayout* buttonLayout = 0;
  if ( myOrientation == Qt::Vertical )
    buttonLayout = new QVBoxLayout( this );
  else
    buttonLayout = new QHBoxLayout( this );

  if ( !buttonLayout )
    return;

  buttonLayout->setMargin( 0 );
  buttonLayout->setSpacing( 5 );

  if ( aPolicy == QtxDialog::Position )
  {
    for ( QList<QAbstractButton*>::iterator lit = left.begin(); lit != left.end(); ++lit )
      buttonLayout->addWidget( *lit );
    buttonLayout->addStretch( 1 );
    for ( QList<QAbstractButton*>::iterator cit = center.begin(); cit != center.end(); ++cit )
      buttonLayout->addWidget( *cit );
    buttonLayout->addStretch( 1 );
    for ( QList<QAbstractButton*>::iterator rit = right.begin(); rit != right.end(); ++rit )
      buttonLayout->addWidget( *rit );
  }
  else
  {
    for ( int i = 0; i < (int)other.count(); i++ )
    {
      buttonLayout->addWidget( other[i] );
      if ( aPolicy == QtxDialog::Uniform && i < (int)other.count() - 1  )
        buttonLayout->addStretch( 1 );
    }
  }

  QWidgetList wids;
  if ( layout() )
  {
    for ( int i = 0; i < layout()->count(); i++ )
    {
      if ( !layout()->itemAt( i ) || layout()->itemAt( i )->widget() )
        continue;

      if ( QApplication::layoutDirection() == Qt::RightToLeft )
        wids.prepend( layout()->itemAt( i )->widget() );
      else
        wids.append( layout()->itemAt( i )->widget() );
    }
  }
  Qtx::setTabOrder( wids );
}


/*!
  \class QtxDialog::Border
  \internal
  \brief Special label used as border widget (separator
         between main frame and button frame).
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
  \brief Constructor.
  \param parent parent widget
*/
QtxDialog::Border::Border( QWidget* parent )
: QLabel( parent )
{
  setAlignment( Qt::AlignCenter );
}

/*!
  \brief Destructor.
*/
QtxDialog::Border::~Border()
{
}

/*!
  \brief Set separator line width.
  \param lw new line width
*/
void QtxDialog::Border::setLineWidth( int lw )
{
  bool isOn = lineWidth() > 0;

  QLabel::setLineWidth( lw );
    
  if ( isOn != ( lineWidth() > 0 ) )
    updateGeometry();
}

/*!
  \brief Get recommended size for the widget.
  \return recommended size for the widget
*/
QSize QtxDialog::Border::sizeHint() const
{
  QSize sz( 5, 5 );

  if ( lineWidth() > 0 )
  {
    if ( frameShape() == VLine )
      sz += QSize( 5 + lineWidth(), 0 );
    else if ( frameShape() == HLine )
      sz += QSize( 0, 5 + lineWidth() );
  }

  return sz;
}

/*!
  \brief Get recommended minimum size for the widget.
  \return recommended minimum size for the widget
*/
QSize QtxDialog::Border::minimumSizeHint() const
{
  return sizeHint();
}

/*!
  \class QtxDialog
  \brief Generic dialog box class.
*/

/*!
  \brief Constructor.

  Construct a dialog with specified parent and name.
  By default non-modal, non-resizable with the OK, Cancel and Help buttons
  dialog box is created.

  \param parent parent widget
  \param modal if \c true dialog box is modal
  \param allowResize if \c true then dialog can be resized by user
  \param f specified control buttons for dialog box (QtxDialog::ButtonFlags)
  \param wf dialog box flags (Qt::WindowFlags)
*/
QtxDialog::QtxDialog( QWidget* parent, bool modal, bool allowResize, const int f, Qt::WindowFlags wf )
: QDialog( parent, (Qt::WindowFlags)( wf | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::Dialog |
#ifdef WIN32
           ( allowResize ? 0 : Qt::FramelessWindowHint ) |
#endif
           ( ( allowResize 
#ifdef WIN32 
               // in qwidget_win.cpp flag WStyle_ContextHelp will be unset in WStyle_MinMax in switched ON
               && !( wf & Qt::WindowContextHelpButtonHint )
#endif
               ) ? Qt::WindowMaximizeButtonHint : 0 ) ) ),
  myInited( false ),
  mySender( 0 ),
  myAlignment( 0 ),
  myDialogFlags( Accept | SetFocus )
{
  setModal( modal );

  QVBoxLayout* base = new QVBoxLayout( this );
  base->setMargin( 5 );
  base->setSpacing( 0 );

  QWidget* main = new QWidget( this );
  base->addWidget( main );

  QVBoxLayout* lMain = new QVBoxLayout( main );
  lMain->setMargin( 0 );
  lMain->setSpacing( 0 );

  Area* topArea = new Area( Qt::Horizontal, this, main );
  QLabel* topLine = new Border( main );
  lMain->addWidget( topArea );
  lMain->addWidget( topLine );

  QWidget* midGroup = new QWidget( main );
  lMain->addWidget( midGroup );

  QVBoxLayout* midLyout = new QVBoxLayout( midGroup );
  midLyout->setMargin( 0 );
  midLyout->setSpacing( 0 );

  QLabel* botLine = new Border( main );
  Area* botArea = new Area( Qt::Horizontal, this, main );
  lMain->addWidget( botLine );
  lMain->addWidget( botArea );

  Area* leftArea = new Area( Qt::Vertical, this, midGroup );
  QLabel* leftLine = new Border( midGroup );
  midLyout->addWidget( leftArea );
  midLyout->addWidget( leftLine );

  myMainFrame = new QFrame( midGroup );
  midLyout->addWidget( myMainFrame );

  QLabel* rightLine = new Border( midGroup );
  Area* rightArea = new Area( Qt::Vertical, this, midGroup );
  midLyout->addWidget( rightLine );
  midLyout->addWidget( rightArea );

  myMainFrame->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );

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
    itr.value()->setBorderEnabled( false );

  myButton.insert( OK,     new QPushButton( tr( "&OK" ),     this ) );
  myButton.insert( Cancel, new QPushButton( tr( "&Cancel" ), this ) );
  myButton.insert( Close,  new QPushButton( tr( "C&lose" ),  this ) );
  myButton.insert( Help,   new QPushButton( tr( "&Help" ),   this ) );
  myButton.insert( Apply,  new QPushButton( tr( "&Apply" ),  this ) );
  myButton.insert( Yes,    new QPushButton( tr( "&Yes" ),    this ) );
  myButton.insert( No,     new QPushButton( tr( "&No" ),     this ) );

  for ( ButtonMap::Iterator it = myButton.begin(); it != myButton.end(); ++it )
  {
    ((QPushButton*)it.value())->setAutoDefault( false );
    connect( it.value(), SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );
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

  QIcon icon;
  QWidget* p = parentWidget();
  while( p && p->parentWidget() )
    p = p->parentWidget();

  if ( p )
    setWindowIcon( p->windowIcon() );

  myButtonFlags = f;

#ifndef WIN32
  if ( !allowResize )
    setMaximumSize( minimumSize() );
#endif

  update();
}

/*!
  \brief Destructor.
*/
QtxDialog::~QtxDialog()
{
}

/*!
  \brief Add specified control button(s) to the dialog box.
  \param f ORed buttons flags (Qtx::ButtonFlags)
  \sa clearButtonFlags(), testButtonFlags()
*/
void QtxDialog::setButtonFlags( const int f )
{
  int old = myButtonFlags;
  myButtonFlags = myButtonFlags | f;
  if ( old != myButtonFlags )
    update();
}

/*!
  \brief Remove specified control button(s) from the dialog box.
  \param f ORed buttons flags (Qtx::ButtonFlags)
  \sa setButtonFlags(), testButtonFlags()
*/
void QtxDialog::clearButtonFlags( const int f )
{
  int old = myButtonFlags;
  myButtonFlags = myButtonFlags & ~f;
  if ( old != myButtonFlags )
    update();
}

/*!
  \brief Test specified buttons.
  \return \c true if specified control buttons are used in the dialog box
  \sa setButtonFlags(), clearButtonFlags()
*/
bool QtxDialog::testButtonFlags( const int f ) const
{
  return ( myButtonFlags & f ) == f;
}

/*!
  \brief Set specified dialog box flags.
  \param f dialog box flags (QtxDialog::DialogFlags)
  \sa clearDialogFlags(), testDialogFlags(), acceptData(), rejectData()
*/
void QtxDialog::setDialogFlags( const int f )
{
  myDialogFlags = myDialogFlags | f;
}

/*!
  \brief Clear specified the dialog flags.
  \param f dialog box flags (QtxDialog::DialogFlags)
  \sa setDialogFlags(), testDialogFlags()
*/
void QtxDialog::clearDialogFlags( const int f )
{
  myDialogFlags = myDialogFlags & ~f;
}

/*!
  \brief Test specified dialog flags.
  \return \c true if specified dialog box falgs are set
  \sa setDialogFlags(), clearDialogFlags()
*/
bool QtxDialog::testDialogFlags( const int f ) const
{
  return ( myDialogFlags & f ) == f;
}

/*!
  \brief Get dialog box main frame widget.

  Main frame is an internal widget which should contains all
  elements of dialog box except control buttons.

  \return main frame widget 
*/
QFrame* QtxDialog::mainFrame() const
{
  return myMainFrame;
}

/*!
  \brief Get specified control button position
  \param id control button ID (QtxDialog::ButtonFlags)
  \return button's position (QtxDialog::ButtonPosition) or -1 if it is not set or invalid \a id is given
  \sa setButtonPosition()
*/
int QtxDialog::buttonPosition( const int id ) const
{
  int pos = -1;
  if ( myPosition.contains( id ) )
    pos = myPosition[id];
  return pos;
}
/*!
  \brief Set the specified control button(s) position.
  \param id control button(s) ID (QtxDialog::ButtonFlags)
  \param pos button(s) position (QtxDialog::ButtonPosition)
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
  {
    if ( changed.contains( itr.value() ) )
      itr.value()->layoutButtons();
  }
}

/*!
  \brief Set button position for all buttons in specified \a area.
  \param pos button(s) position (QtxDialog::ButtonPosition)
  \param area buttons area (QtxDialog::ButtonArea)
  \sa setButtonPosition()
*/
void QtxDialog::setPlacePosition( const int pos, const int area )
{
  if ( !myArea.contains( area ) )
    return;

  Area* anArea = myArea[area];

  bool changed = false;
  for ( ButtonMap::Iterator it = myButton.begin(); it != myButton.end(); ++it )
  {
    if ( !anArea->contains( it.value() ) )
      continue;

    changed = changed &&  myPosition[it.key()] != pos;

    myPosition[it.key()] = pos;
  }

  if ( changed )
    anArea->layoutButtons();
}

/*!
  \brief Get buttons layouting policy for the specified \a area.
  \param area buttons area (QtxDialog::ButtonArea)
  \sa setPlacePolicy()
*/
int QtxDialog::placePolicy( const int area ) const
{
  int res = -1;
  if ( myArea.contains( area ) )
    res = myArea[area]->policy();
  return res;
}

/*!
  \brief set buttons layouting policy for the specified \a area.
  \param policy buttons layouting policy (QtxDialog::PlacePolicy)
  \param area buttons area (QtxDialog::ButtonArea)
  \sa placePolicy()
*/
void QtxDialog::setPlacePolicy( const int policy, const int area )
{
  if ( area < 0 )
  {
    for ( AreaMap::Iterator itr = myArea.begin(); itr != myArea.end(); ++itr )
      itr.value()->setPolicy( policy );
  }
  else if ( myArea.contains( area ) )
    myArea[area]->setPolicy( policy );
}

/*!
  \brief Move specified button(s) into specified area.
  \param area buttons area (QtxDialog::ButtonArea)
  \param id control button(s) ID (QtxDialog::ButtonFlags)
*/
void QtxDialog::setButtonPlace( const int area, const int id )
{
  if ( !myArea.contains( area ) )
    return;

  Area* anArea = myArea[area];
  ButtonMap map = buttons( id );
  QMap<Area*, int> areaMap;
  for ( AreaMap::ConstIterator aIt = myArea.begin(); aIt != myArea.end(); ++aIt )
    areaMap.insert( aIt.value(), 0 );

  for ( ButtonMap::Iterator it = map.begin(); it != map.end(); ++it )
  {
    Area* old = (Area*)it.value()->parent();
    if ( old == anArea )
      continue;

    if ( areaMap.contains( old ) )
      old->removeButton( it.value() );
    anArea->insertButton( it.value() );
  }
}

/*!
  \brief Check if border is enabled.
  \param area buttons area (QtxDialog::ButtonArea)
  \return \c true if border is enabled for specified button area.
  \sa setBorderEnabled()
*/
bool QtxDialog::isBorderEnabled( const int area ) const
{
  bool res = false;
  if ( myArea.contains( area ) )
    res  = myArea[area]->isBorderEnabled();
  return res;
}

/*!
  \brief Show/hide border for the specified button area.

  Border is a line which separate main frame and control buttons.

  \param area buttons area (QtxDialog::ButtonArea)
  \param on enable border flag
  \sa isBorderEnabled()
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
  \brief Get "enabled" status of the specified button(s).
  \param id control button(s) ID (QtxDialog::ButtonFlags)
  \return \c true if all specified buttons are enabled.
  \sa setButtonEnabled()
*/
bool QtxDialog::isButtonEnabled( const int id ) const
{
  ButtonMap map = buttons( id );
  bool result = !map.isEmpty();
  for ( ButtonMap::Iterator it = map.begin(); it != map.end(); ++it )
    result = result && it.value()->isEnabled();
  return result;
}

/*!
  \brief Enable/disable specified button(s).
  \param on enable button(s) flag
  \param id control button(s) ID (QtxDialog::ButtonFlags)
  \sa isButtonEnabled()
*/
void QtxDialog::setButtonEnabled( const bool on, const int id )
{
  ButtonMap map = buttons( id );
  for ( ButtonMap::Iterator it = map.begin(); it != map.end(); ++it )
    it.value()->setEnabled( on );
}

/*!
  \brief Check if specified button has keyboard focus.
  \param id control button ID (QtxDialog::ButtonFlags)
  \return \c true if specified button has keyboard focus
  \sa setButtonFocus()
*/
bool QtxDialog::hasButtonFocus( const int id ) const
{
  bool res = false;
  QAbstractButton* pb = button( id );
  if ( pb )
    res = pb->hasFocus();
  return res;
}

/*!
  \brief Sets the keyboard focus to the specified button.
  \param id control button ID (QtxDialog::ButtonFlags)
  \sa hasButtonFocus()
*/
void QtxDialog::setButtonFocus( const int id )
{
  QAbstractButton* pb = button( id );
  if ( pb )
    pb->setFocus();
}

/*!
  \brief Get specified button's text.
  \param id control button ID (QtxDialog::ButtonFlags)
  \return button's text
  \sa setButtonText()
*/
QString QtxDialog::buttonText( const int id )
{
  QString retval;
  QAbstractButton* but = button( id );
  if ( but )
    retval = but->text();
  return retval;
}

/*!
  \brief Set specified button's text.
  \param id control button ID (QtxDialog::ButtonFlags)
  \param text button's text
  \sa buttonText()
*/
void QtxDialog::setButtonText( const int id, const QString& text )
{
  QAbstractButton* but = button( id );
  if ( but && but->text() != text )
  {
    but->setText( text );
    adjustButtons();
  }
}

/*!
  \brief Sets alignment policy.

  Use the function before the the dialog is first time shown.
  If dialog flag AlignOnce is set then alignment is performed
  only once, otherwise the dialog is aligned each time when it
  is shown. 
  Dialog box is aligned relatively to its parent.
  By default, dialog box is aligned to the center of the parent 
  widget (usually desktop or another dialog box).
  
  \param align alignment flag(s) (Qtx::AlignmentFlags)
  \return previous alignment policy
*/
uint QtxDialog::setAlignment( uint align )
{
  uint oldAlign = myAlignment;
  myAlignment = align;
  return oldAlign;
}

/*!
  \brief Update dialog box.
*/
void QtxDialog::update()
{
  for ( ButtonMap::Iterator it = myButton.begin(); it != myButton.end(); ++it )
    if ( it.key() >= 0 )
      it.value()->setVisible( testButtonFlags( it.key() ) );

  for ( AreaMap::Iterator itr = myArea.begin(); itr != myArea.end(); ++itr )
    itr.value()->layoutButtons();

  adjustButtons();

  QDialog::update();
}

/*!
  \brief Show/hide dialog box, set keyboard focus to the dialog.
  
  Re-implemented from Qt.
  
  \param on show/hide flag
*/
void QtxDialog::setVisible( bool on )
{
  resize( sizeHint() );

  QDialog::setVisible( on );

  if ( on )
  {
    if ( testDialogFlags( SetFocus ) )
      setFocus();
    myInited = true;
  }
  else
    QApplication::instance()->processEvents();
}

/*!
  \brief Get user button by the specified \a id.
  \param id user button ID
  \return user button or 0 if it is not found
  \sa insertButton(), removeButton(), userButtonIds()
*/
QAbstractButton* QtxDialog::userButton( const int id ) const
{
  QAbstractButton* b = 0;
  if ( id < -1 && myButton.contains( id ) )
    b = myButton[id];
  return b;
}

/*!
  \brief Get all user button IDs.
  \return list of user buttons identificators
  \sa insertButton(), removeButton(), userButton()
*/
QIntList QtxDialog::userButtonIds() const
{
  QIntList retlist;
  for ( ButtonMap::ConstIterator it = myButton.begin(); it != myButton.end(); ++it )
    if ( it.key() < 0 )
      retlist.append( it.key() );
  return retlist;
}

/*!
  \brief Add user button to the dialog box.

  The button is inserted to the specified dialog box area.
  if the button is added successfully, the unique identificator of 
  the added button is returned, otherwise -1 is returned.

  \param text text of the added button
  \param area buttons area (QtxDialog::ButtonArea)
  \return button ID
  \sa removeButton(), userButton(), userButtonIds()
*/
int QtxDialog::insertButton( const QString& text, const int area )
{
  if ( !myArea.contains( area ) )
    return -1;

  int id = -2;
  while ( myButton.contains( id ) )
    id--;

  Area* anArea = myArea[area];
  QAbstractButton* b = createButton( this );
  if ( b )
  {
    b->setText( text );
    myButton.insert( id, b );
    myPosition.insert( id, Left );
    
    connect( b, SIGNAL( clicked() ), this, SLOT( onButton() ) );
    connect( b, SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

    anArea->insertButton( b );
    update();
  }
  else
    id = -1;

  return id;
}

/*!
  \brief Remove user button.

  If \c id is -1, all user buttons are removed.

  \param id user button ID
  \sa insertButton(), userButton(), userButtonIds()
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
        map.insert( it.key(), it.value() );
    }
  }
  else if ( myButton.contains( id ) )
    map.insert( id, myButton[id] );

  for ( ButtonMap::Iterator itr = map.begin(); itr != map.end(); ++itr )
  {
    for ( AreaMap::Iterator it = myArea.begin(); it != myArea.end(); ++it )
      it.value()->removeButton( itr.value() );

    myButton.remove( itr.key() );
    myPosition.remove( itr.key() );
    
    delete itr.value();
  }
  update();
}

/*!
  \brief Set measure units to the specified label.

  In the dialog box label the measure units are closed in braces.
  If measure units do not exist they will be added.

  For example:
  \code
  // create label "Radius"
  QLabel* aLabel = new QLabel( "Radius", mainFrame() );
  // set measure units to "mm"
  setUnits( aLabel, "mm" )    // => aLabel contains 'Radius (mm)'
  // set measure units to "cm"
  setUnits( aLabel, "cm" )    // => aLabel contains 'Radius (cm)'

  // create label "Radius" with initially not set measure units
  QLabel* aLabel = new QLabel( "Radius ():", mainFrame() );
  // set measure units to "mm"
  setUnits( aLabel, "mm" )    // => aLabel contains 'Radius (mm):'
  // set measure units to "cm"
  setUnits( aLabel, "cm" )    // => aLabel contains 'Radius (cm):'
  \endcode

  \param aLabel label widget
  \param aUnits measure units
*/
void QtxDialog::setUnits( QLabel* aLabel, const QString& aUnits )
{
  QString label = aLabel->text();

  int begin;
  int end = label.lastIndexOf( ')' );

  QString startLabel = label;
  QString finalLabel;

  if ( end != -1 )
  {
    begin = label.left( end ).lastIndexOf( '(' );
    if ( begin != -1 )
    {
      startLabel = label.mid( 0, begin );
      finalLabel = label.mid( end + 1 );
    }
  }
  else
  {
    startLabel = startLabel.trimmed();
    if ( startLabel.at( startLabel.length() - 1 ) == ':' )
    {
      finalLabel = startLabel.mid( startLabel.length() - 1 );
      startLabel = startLabel.mid( 0, startLabel.length() - 1 );
    }
  }
  if ( aUnits.length() )
    label = startLabel.trimmed() + " (" + aUnits + ") " + finalLabel.trimmed();
  else
    label = startLabel.trimmed() + " " + finalLabel.trimmed();
  aLabel->setText( label );
}

/*!
  \brief Check if data entered by the user is valid.

  This method can be re-implemented in the successor class if it
  requires to check user input consistency.
  Default implementation returns \c true.

  This method is called if dialog flag QtxDialog::Accept is set.
  If this method returns \c true, then dialog will be accepted and closed.

  \return \c true if user input is valid
  \sa accept()
*/
bool QtxDialog::acceptData() const
{
  return true;
}

/*!
  \brief Check if dialog box can be cancelled.

  This method can be re-implemented in the successor class if it
  requires to check possibility to cancel dialog box safely.
  Default implementation returns \c true.

  This method is called if dialog flag QtxDialog::Reject is set.
  If this method returns \c true, then dialog will be rejected and closed.

  \return \c true if dialog box can be cancelled
  \sa reject()
*/
bool QtxDialog::rejectData() const
{
  return true;
}

/*!
  \brief Create new user button.

  This method is invoked from method insertButton().

  \param parent parent widget
  \return new user button
*/
QAbstractButton* QtxDialog::createButton( QWidget* parent )
{
  QPushButton* pb = new QPushButton( parent );
  pb->setAutoDefault( false );
  return pb;
}

/*!
  \brief Get button by the specified ID.
  \param f control button ID (QtxDialog::ButtonFlags)
  \return button or 0 if \a id is invalid
*/
QAbstractButton* QtxDialog::button( const int f ) const
{
  QAbstractButton* retval = 0;
  if ( myButton.contains( f ) )
    retval = myButton[f];
  return retval;
}

/*!
  \brief Get buttons by the specified IDs.
  \param f control button(s) ID(s) (QtxDialog::ButtonFlags)
  \return button map
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
        retmap.insert( it.key(), it.value() );
  }
  return retmap;
}

/*!
  \brief Get specified button's identifier.
  \param b button
  \return button ID
*/
int QtxDialog::buttonId( const QAbstractButton* b ) const
{
  int id = -1;
  for ( ButtonMap::ConstIterator it = myButton.begin(); it != myButton.end() && id == -1; ++it )
    if ( it.value() == b )
      id = it.key();
  return id;
}

/*!
  \brief Get position of specified button.
  \param b button
  \return button position (QtxDialog::ButtonPosition)
*/
int QtxDialog::buttonPosition( QAbstractButton* b ) const
{
  return buttonPosition( buttonId( b ) );
}

/*!
  \brief Align this dialog according to the parent widget and alignment
         policy before the dialog box is shown.

  Re-implemented from Qt.

  \param e show event
*/
void QtxDialog::showEvent( QShowEvent* e )
{
  if ( !testDialogFlags( AlignOnce ) || !myInited )
    Qtx::alignWidget( this, parentWidget(), myAlignment );
  QDialog::showEvent( e );
}

/*!
  \brief Process all existing events when dialog box is hidden.

  Re-implemented from Qt.

  \param e hide event
*/
void QtxDialog::hideEvent( QHideEvent* e )
{
  QApplication::instance()->processEvents();
  QDialog::hideEvent( e );
}

/*!
  \brief Update dialog box layout when the size grip is added.

  Re-implemented from Qt.

  \param e child event
*/
void QtxDialog::childEvent( QChildEvent* e )
{
  QDialog::childEvent( e );
  if ( layout() && e->added() && e->child()->inherits( "QSizeGrip" ) )
  {
    layout()->setMargin( 12 );
    connect( e->child(), SIGNAL( destroyed() ), this, SLOT( onSizeGripDestroyed() ) );
  }
}

/*!
  \brief Process key pressing event.

  Re-implemented from Qt.

  Call reject() if "Escape" key is pressed.
  Call accept() if "Ctrl+Enter" key-sequence is pressed.
  Process "F1" key and emit signal dlgHelp().
  Transfer "Ctrl+(Shift+)Tab" key-sequence press event 
  to the child Tab widget (if there is any).

  \param e key press event
*/
void QtxDialog::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );

  if ( e->isAccepted() )
    return;

  if ( !e->modifiers() && e->key() == Qt::Key_Escape )
    reject();

  if ( e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_Return )
  {
    if ( testButtonFlags( OK ) || testButtonFlags( Yes ) )
      accept();
    else if ( testButtonFlags( Apply ) && isButtonEnabled( Apply ) )
      emit dlgApply();
    e->accept();
  }

  if ( e->key() == Qt::Key_F1 && testButtonFlags( Help ) && isButtonEnabled( Help ) )
  {
    e->accept();
    emit dlgHelp();
  }

  if ( e->key() == Qt::Key_Tab && e->modifiers() & Qt::ControlModifier )
  {
    QObject* tab = this->findChild<QTabWidget*>();
    if ( tab && !property( "in_tab_event" ).toBool() ) {
      setProperty( "in_tab_event", true );
      QApplication::sendEvent( tab, e );
      setProperty( "in_tab_event", false );
    }
  }
}

/*!
  \brief Called when user closes dialog box.
  
  Call reject() method.
  
  \param e close event (not used)
*/
void QtxDialog::closeEvent( QCloseEvent* /*e*/ )
{
  reject();
}

/*!
  \brief Accept the dialog box.

  This method is used when any accept button is pressed (usually
  "OK", "Yes", etc).

  If dialog flag QtxDialog::Accept is set, this function invokes 
  acceptData() method, which should in this case return \c true to
  allow further processing. 

  If acceptData() returns \c false, this function just returns.

  If acceptData() returns \c true, the Accepted result is set
  and signal according to the pressed control button is emitted.
  Then the default implementation of accept() method is called
  (which hides the dialog box and, depending on the dialog box flags,
  can close and destroy it).
 
  \sa acceptData()
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
  \brief Reject the dialog box.

  This method is used when any reject button is pressed (usually
  "Close", "Cancel", "No", etc).

  If dialog flag QtxDialog::Reject is set, this function invokes 
  rejectData() method, which should in this case return \c true to
  allow further processing. 

  If rejectData() returns \c false, this function just returns.

  If rejectData() returns \c true, the Rejected result is set
  and signal according to the pressed control button is emitted.
  Then the default implementation of reject() method is called
  (which hides the dialog box and, depending on the dialog box flags,
  can close and destroy it).
 
  \sa rejectData()
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
  \brief Emit signal correspondingly to the control button.
*/
void QtxDialog::emitSignal()
{
  QApplication::instance()->processEvents();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::syncX();
#endif
  int id = buttonId( (QAbstractButton*)mySender );
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
  \brief This slot is called when user presses on of the buttons
         "OK", "Yes", etc.

  Call accept() method.
*/
void QtxDialog::onAccept()
{
  const QObject* obj = sender();
  mySender = obj;
  accept();
}

/*!
  \brief This slot is called when user presses on of the buttons
         "Cancel", "No", "Close".

  Call reject() method.
*/

void QtxDialog::onReject()
{
  const QObject* obj = sender();
  mySender = obj;
  reject();
}

/*!
  \brief Process user button click event.
  
  This method is called when user presses one of custom user buttons.
  Emits signal dlgButton(int) with identificator of the clicked user
  button passed as parameter.
*/
void QtxDialog::onButton()
{
  int id = buttonId( (QAbstractButton*)sender() );
  if ( id != -1 )
    emit dlgButton( id );
}

/*!
  \brief Watch for the user button destroying.
  \param obj button being destroyed
*/
void QtxDialog::onDestroyed( QObject* obj )
{
  QAbstractButton* b = (QAbstractButton*)obj;
  int id = buttonId( b );
  if ( id == -1 )
    return;

  myButton.remove( id );
  myPosition.remove( id );
  for ( AreaMap::Iterator it = myArea.begin(); it != myArea.end(); ++it )
    it.value()->removeButton( b );
}

/*!
  \brief Update dialog box layout when the size grip is destroyed.
*/
void QtxDialog::onSizeGripDestroyed()
{
  if ( layout() )
    layout()->setMargin( 5 );
}

/*!
  \brief Adjust buttons (set equal size for all buttons).
*/
void QtxDialog::adjustButtons()
{
  int minWidth = 0;
  for ( AreaMap::Iterator aIt = myArea.begin(); aIt != myArea.end(); ++aIt )
  {
    const QList<QAbstractButton*>& lst = aIt.value()->buttons();
    for ( QList<QAbstractButton*>::const_iterator bIt = lst.begin(); bIt != lst.end(); ++bIt )
      if ( (*bIt)->isVisibleTo( this ) )
        minWidth = qMax( minWidth, (*bIt)->sizeHint().width() );
  }

  for ( AreaMap::Iterator aItr = myArea.begin(); aItr != myArea.end(); ++aItr )
  {
    const QList<QAbstractButton*>& lst = aItr.value()->buttons();
    for ( QList<QAbstractButton*>::const_iterator bItr = lst.begin(); bItr != lst.end(); ++bItr )
      if ( (*bItr)->isVisibleTo( this ) )
        (*bItr)->setMinimumWidth( minWidth );
  }
}

/*!
  \fn void QtxDialog::dlgButton( int id )
  \brief Emitted when the user button is clicked.
  \param id user button identificator
*/
/*!
  \fn void QtxDialog::dlgParamChanged()
  \brief This signal can be used in successor classes to signalize about
         some dialog parameter changing.
*/
/*!
  \fn void QtxDialog::dlgHelp()
  \brief Emitted when the "Help" button is clicked.
*/
/*!
  \fn void QtxDialog::dlgApply()
  \brief Emitted when the "Apply" button is clicked.
*/
/*!
  \fn void QtxDialog::dlgOk()
  \brief Emitted when the "OK" button is clicked.
*/
/*!
  \fn void QtxDialog::dlgNo()
  \brief Emitted when the "No" button is clicked.
*/
/*!
  \fn void QtxDialog::dlgYes()
  \brief Emitted when the "Yes" button is clicked.
*/
/*!
  \fn void QtxDialog::dlgClose()
  \brief Emitted when the "Close" button is clicked.
*/
/*!
  \fn void QtxDialog::dlgCancel()
  \brief Emitted when the "Cancel" button is clicked.
*/
