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

// File:      QtxListAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxListAction.h"

#include <QMenu>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QListWidget>
#include <QToolButton>
#include <QApplication>

/*!
  \class QtxListAction::ScrollEvent
  \internal
  \brief Event for the scrolling in the list of actions.
*/

class QtxListAction::ScrollEvent : public QEvent
{
public:
  enum { Scroll = User + 1 };

  ScrollEvent( bool down ) : QEvent( (QEvent::Type)Scroll ), myDown( down ) {}
  virtual ~ScrollEvent() {}

  bool isDown() const { return myDown; }

private:
  bool myDown;
};

/*!
  \class QtxListAction::ListWidget
  \internal
  \brief List of actions.
*/

class QtxListAction::ListWidget : public QListWidget
{
public:
  ListWidget( QWidget* parent = 0 ) : QListWidget( parent ) {}
  virtual ~ListWidget() {}

protected:
  virtual void scrollContentsBy( int dx, int dy )
  {
    QListWidget::scrollContentsBy( dx, dy );
    if ( dy != 0 )
      QApplication::postEvent( viewport(), new ScrollEvent( dy <= 0 ) );
  }
};

/*!
  \class QtxListAction::ListFrame
  \internal
  \brief Expanding frame with action list and comment.
*/

class QtxListAction::ListFrame: public QMenu
{
public:
  ListFrame( QtxListAction*, QWidget* parent );
  virtual ~ListFrame();

  void                    clear();
  const QStringList       names() const;
  void                    addNames( const QStringList& );

  void                    setSingleComment( const QString& );
  void                    setMultipleComment( const QString& );

  int                     selected() const;
  void                    setSelected( const int );

  int                     linesNumber() const;
  int                     charsNumber() const;

  void                    setLinesNumber( const int );
  void                    setCharsNumber( const int );

  virtual QSize           sizeHint() const;
  virtual QSize           minimumSizeHint() const;

  virtual bool            eventFilter( QObject*, QEvent* );

  virtual void            setVisible( bool );

protected:
  virtual void            keyPressEvent( QKeyEvent* );

private:
  void                    accept();
  void                    updateComment();
  void                    setNames( const QStringList& );
  void                    removePostedEvens( QObject*, int );

private:
  QListWidget*            myList;
  QStringList             myNames;
  QtxListAction*          myAction;
  QLabel*                 myComment;

  int                     myLines;
  int                     myChars;

  QString                 mySingleComment;
  QString                 myMultipleComment;
};

/*!
  \brief Constructor.
  \param a list action
  \param parent parent widget
*/
QtxListAction::ListFrame::ListFrame( QtxListAction* a, QWidget* parent )
: QMenu( parent ),
  myList( 0 ),
  myAction( a ),
  myComment( 0 ),
  myLines( 5 ),
  myChars( 5 )
{
  QVBoxLayout* top = new QVBoxLayout( this );
  top->setMargin( 0 );
  QFrame* main = new QFrame( this );
  main->setFrameStyle( QFrame::Panel | QFrame::Raised );
  top->addWidget( main );

  QVBoxLayout* base = new QVBoxLayout( main );
  base->setMargin( 3 );
  base->setSpacing( 2 );

  myList = new ListWidget( main );
  myList->setSelectionMode( QListWidget::MultiSelection );
  myList->setVerticalScrollMode( QListWidget::ScrollPerItem );
  myList->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
  myList->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  myList->viewport()->installEventFilter( this );
  myList->viewport()->setMouseTracking( true );
  myList->setFocusPolicy( Qt::NoFocus );

  myComment = new QLabel( main );
  myComment->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  myComment->setAlignment( Qt::AlignCenter );
  myMultipleComment = "%1";

  base->addWidget( myList );
  base->addWidget( myComment );
}

/*!
  \brief Destructor.
*/
QtxListAction::ListFrame::~ListFrame()
{
}

/*!
  \brief Clear list of names.
*/
void QtxListAction::ListFrame::clear()
{
  myNames.clear();
  setNames( myNames );
}

/*!
  \brief Add names to the list.

  Truncates each name to fit the frame width.
  Method QtxListAction::setCharsNumber(int) can be used to change
  the frame width (in characters).

  \param names list of names to be added
  \sa setNames(), QtxListAction::setCharsNumber(int)
*/
void QtxListAction::ListFrame::addNames( const QStringList& names )
{
  for ( QStringList::ConstIterator it = names.begin(); it != names.end(); ++it )
    myNames.append( *it );
  setNames( myNames );
}

/*!
  \brief Set names to the list.

  Truncates each name to fit the frame width.
  Method QtxListAction::setCharsNumber(int) can be used to change
  the frame width (in characters).

  \param names list of names to be set
  \sa addNames(), QtxListAction::setCharsNumber(int)
*/
void QtxListAction::ListFrame::setNames( const QStringList& names )
{
  if ( !myList )
    return;

  myList->clear();
  QStringList strList;
  for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
  {
    QString s = *it;
    QFontMetrics fm = myList->fontMetrics();
    int maxW = charsNumber() * fm.maxWidth();
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
    strList.append( s );
  }
  myList->addItems( strList );
}

/*!
  \brief Get list of names.
  \return list of names
*/
const QStringList QtxListAction::ListFrame::names() const
{
  return myNames;
}

/*!
  \brief Get maximum numer of lines shown without activation of vertical scroll bar.
  \return number of lines
  \sa setLinesNumber(), charsNumber(), setCharsNumber()
*/
int QtxListAction::ListFrame::linesNumber() const
{
  return myLines;
}

/*!
  \brief Get maximum numer of characters in the line.

  If the name length is greater than this value, it will be truncated.

  \return number of characters
  \sa setCharsNumber(), linesNumber(), setLinesNumber()
*/
int QtxListAction::ListFrame::charsNumber() const
{
  return myChars;
}

/*!
  \brief Set maximum numer of lines shown without activation of vertical scroll bar.
  \param maxLines number of lines
  \sa linesNumber(), charsNumber(), setCharsNumber()
*/
void QtxListAction::ListFrame::setLinesNumber( const int maxLines )
{
  myLines = maxLines;
}

/*!
  \brief Set maximum numer of characters in the line.

  If the name length is greater than this value, it will be truncated.

  \param maxChars number of characters
  \sa charsNumber(), linesNumber(), setLinesNumber()
*/
void QtxListAction::ListFrame::setCharsNumber( const int maxChars )
{
  if ( myChars == maxChars )
    return;

  myChars = maxChars;
  setNames( myNames );
}

/*!
  \brief Set comment which is displayed when single name is selected.
  \param comment comment format
*/
void QtxListAction::ListFrame::setSingleComment( const QString& comment )
{
  mySingleComment = comment;
  setNames( myNames );
  updateComment();
}

/*!
  \brief Set comment which is displayed when multiple names are selected.
  \param comment comment format
*/
void QtxListAction::ListFrame::setMultipleComment( const QString& comment )
{
  myMultipleComment = comment;
  setNames( myNames );
  updateComment();
}

/*!
  \brief Update displayed comment.
*/
void QtxListAction::ListFrame::updateComment()
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
  \brief Get preferable size for the list widget.
  \return preferable size
*/
QSize QtxListAction::ListFrame::sizeHint() const
{
  return QSize( myList->fontMetrics().maxWidth() * charsNumber() + 10,
                qMax( 1, linesNumber() ) * ( myList->fontMetrics().height() + 2 ) +
                myComment->sizeHint().height() );
}

/*!
  \brief Get preferable minimum size for the list widget.
  \return preferable minimum size
*/
QSize QtxListAction::ListFrame::minimumSizeHint() const
{
  return QSize( myList->fontMetrics().maxWidth() * charsNumber() + 10,
                qMax( 1, linesNumber() ) * ( myList->fontMetrics().height() + 2 ) +
                myComment->sizeHint().height() );
}

/*!
  \brief Validate the action.
*/
void QtxListAction::ListFrame::accept()
{
  int sel = selected();
  if ( sel && myAction )
    myAction->onMultiple( sel );
}

/*!
  \brief Called when list widget is shown/hidden.
  \param on if \c true, widget is shown, otherswise it is hidden
*/
void QtxListAction::ListFrame::setVisible( bool on )
{
  if ( on )
  {
    myList->setFocus();
    myList->scrollToItem( myList->item( 0 ), QListWidget::PositionAtTop );
    setSelected( 0 );
    updateComment();
  }

  QMenu::setVisible( on );
}

/*!
  \brief Process key press event.

  The following keys are supported:
  - Up/Down
  - PageUp/PageDown
  - Enter
  - Escape

  \param e key press event
*/
void QtxListAction::ListFrame::keyPressEvent( QKeyEvent* e )
{
  if ( e->type() == QEvent::KeyRelease )
    return;

  e->accept();

  int selNum = selected();
  switch( e->key() )
  {
  case Qt::Key_Up:
    setSelected( qMax( 1, selNum - 1 ) );
    break;
  case Qt::Key_Down:
    setSelected( qMax( 1, selNum + 1 ) );
    break;
  case Qt::Key_PageUp:
    setSelected( qMax( 1, selNum - linesNumber() ) );
    break;
  case Qt::Key_PageDown:
    setSelected( selNum += linesNumber() );
    break;
  case Qt::Key_Home:
    setSelected( 1 );
    break;
  case Qt::Key_End:
    setSelected( myList->count() );
    break;
  case Qt::Key_Return:
    accept();
    break;
  case Qt::Key_Escape:
    hide();
    break;
  }
}

/*!
  \brief Process mouse events on the viewport of the list widget.
  \param o object recieving event (viewport)
  \param e event
  \return \c true if further event processing should be stopped.
*/
bool QtxListAction::ListFrame::eventFilter( QObject* o, QEvent* e )
{
  bool res = true;

  switch( e->type() )
  {
  case QEvent::MouseMove:
    {
      QMouseEvent* me = (QMouseEvent*)e;
      if ( !myList->viewport()->rect().contains( me->pos() ) )
        setSelected( 0 );
      else if ( myList->itemAt( me->pos() ) )
        setSelected( myList->row( myList->itemAt( me->pos() ) ) + 1 );
    }
    break;
  case QEvent::MouseButtonRelease:
    accept();
  case QEvent::MouseButtonPress:
  case QEvent::MouseButtonDblClick:
    break;
  case ScrollEvent::Scroll:
    {
      ScrollEvent* se = (ScrollEvent*)e;
      QPoint pos = myList->viewport()->mapFromGlobal( QCursor::pos() );
      if ( myList->viewport()->rect().contains( pos ) )
      {
        if ( myList->itemAt( pos ) )
          setSelected( myList->row( myList->itemAt( pos ) ) + 1 );
      }
      else if ( se->isDown() )
        setSelected( myList->row( myList->itemAt( myList->viewport()->rect().bottomLeft() -
                                                  QPoint( 0, myList->fontMetrics().height() / 2 ) ) ) + 1 );
      else
        setSelected( myList->row( myList->itemAt( myList->viewport()->rect().topLeft() +
                                                  QPoint( 0, myList->fontMetrics().height() / 2 ) ) ) + 1 );
    }
    break;
  default:
    res = false;
    break;
  }

  if ( res )
    return true;
  else
    return QMenu::eventFilter( o, e );
}

/*!
  \brief Get number of selected names.
  \return number of selected items
*/
int QtxListAction::ListFrame::selected() const
{
  int sel = 0;
  QModelIndexList indexes = myList->selectionModel()->selectedRows();
  for ( QModelIndexList::const_iterator it = indexes.begin(); it != indexes.end(); ++it )
    sel = qMax( sel, (*it).row() + 1 );
  
  return sel;
}

/*!
  \brief Set number of selected names.
  \param lastSel number of items to be selected
*/
void QtxListAction::ListFrame::setSelected( const int lastSel )
{
  int last = qMin( lastSel, (int)myList->count() );

  QItemSelection selection;
  QItemSelectionModel* selModel = myList->selectionModel();

  for ( int i = 0; i < last; i++ )
    selection.select( selModel->model()->index( i, 0 ), selModel->model()->index( i, 0 ) );

  selModel->select( selection, QItemSelectionModel::ClearAndSelect );
  
  int item = last - 1;

  myList->scrollToItem( myList->item( item ) );
  myList->clearFocus();

  removePostedEvens( myList->viewport(), ScrollEvent::Scroll );

  updateComment();
}

/*!
  \brief Filter all events of specified type sent to specified object.
  \param o object
  \param type event type to be filtered
*/
void QtxListAction::ListFrame::removePostedEvens( QObject* o, int type )
{
  class Filter : public QObject
  {
  public:
    Filter() : QObject( 0 ) {}
    virtual bool eventFilter( QObject*, QEvent* )
    {
      return true;
    }
  };

  Filter f;
  o->installEventFilter( &f );
  QApplication::sendPostedEvents( o, type );
}

/*!
  \class QtxListAction
  \brief Action with associated list of items.
  
  This class can be helpuful, for example, for creation of Undo/Redo
  toolbar items which show list of available commands in the popup list box.
*/

/*!
  \brief Constructor.
  \param parent parent object
*/
QtxListAction::QtxListAction( QObject* parent )
: QtxAction( parent ),
  myFrame( 0 )
{
  initialize();
}

/*!
  \brief Constructor.
  \param icon action icon
  \param menuText menu text
  \param accel key accelerator
  \param parent parent object
*/
QtxListAction::QtxListAction( const QIcon& icon, const QString& menuText, 
                              int accel, QObject* parent )
: QtxAction( menuText, icon, menuText, accel, parent ),
  myFrame( 0 )
{
  initialize();
}

/*!
  \brief Constructor.
  \param menuText menu text
  \param accel key accelerator
  \param parent parent object
*/
QtxListAction::QtxListAction( const QString& menuText, int accel, QObject* parent )
: QtxAction( menuText, menuText, accel, parent ),
  myFrame( 0 )
{
  initialize();
}

/*!
  \brief Constructor.
  \param text action description text (tooltip)
  \param menuText menu text
  \param accel key accelerator
  \param parent parent object
*/
QtxListAction::QtxListAction( const QString& text, const QString& menuText, 
                              int accel, QObject* parent )
: QtxAction( text, menuText, accel, parent ),
  myFrame( 0 )
{
  initialize();
}

/*!
  \brief Constructor.
  \param text action description text (tooltip)
  \param icon action icon
  \param menuText menu text
  \param accel key accelerator
  \param parent parent object
*/
QtxListAction::QtxListAction( const QString& text, const QIcon& icon, 
                              const QString& menuText, int accel, QObject* parent )
: QtxAction( text, icon, menuText, accel, parent ),
  myFrame( 0 )
{
  initialize();
}

/*!
  \brief Destructor.
*/
QtxListAction::~QtxListAction()
{
  delete myFrame;
  myFrame = 0;
}

/*!
  \brief Get popup mode.
  \return current popup mode (QtxListAction::PopupMode)
  \sa setPopupMode()
*/
int QtxListAction::popupMode() const
{
  return menu() ? SubMenu : Item;
}

/*!
  \brief Set popup mode.
  \param mode new popup mode (QtxListAction::PopupMode)
  \sa popupMode()
*/
void QtxListAction::setPopupMode( const int mode )
{
  if ( mode == popupMode() )
    return;

  if ( mode == Item )
  {
    delete menu();
    setMenu( 0 );
  }
  else
    setMenu( new QMenu( 0 ) );

  onChanged();
}

/*!
  \brief Get current list of names.
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
  \brief Add names to the list.

  Truncates each name to fit the frame width.
  Method setCharsNumber() can be used to change
  the frame width (in characters).

  \param names list of names to be added
  \param clear if \c true, remove the old contents from the list
  \sa setCharsNumber()
*/
void QtxListAction::addNames( const QStringList& names, bool clear )
{
  if ( !myFrame )
    return;

  if ( clear )
    myFrame->clear();

  myFrame->addNames( names );

  onChanged();
}

/*!
  \brief Get maximum numer of lines shown without activation of vertical scroll bar.
  \return number of lines
  \sa setLinesNumber(), charsNumber(), setCharsNumber()
*/
int QtxListAction::linesNumber() const
{
  return myFrame->linesNumber();
}

/*!
  \brief Get maximum numer of characters in the line.

  If the name length is greater than this value, it will be truncated.

  \return number of characters
  \sa setCharsNumber(), linesNumber(), setLinesNumber()
*/
int QtxListAction::charsNumber() const
{
  return myFrame->charsNumber();
}

/*!
  \brief Set maximum numer of lines shown without activation of vertical scroll bar.
  \param nlines number of lines (5 by default)
  \sa linesNumber(), charsNumber(), setCharsNumber()
*/
void QtxListAction::setLinesNumber( const int nlines )
{
  myFrame->setLinesNumber( nlines );
}

/*!
  \brief Set maximum numer of characters in the line.

  If the name length is greater than this value, it will be truncated.

  \param maxChars number of characters (5 by default)
  \sa charsNumber(), linesNumber(), setLinesNumber()
*/

void QtxListAction::setCharsNumber( const int nchars )
{
  myFrame->setCharsNumber( nchars );
}

/*!
  \brief Set the format Qt string for comments displayed under the list
         of actions for one action and for several actions.

  Example: "Undo %1 actions" format string will work as "Undo 3 actions"
  when 3 actions are selected. The default format string is "%1".

  \param c single action comment format
  \param c multiple actions comment format
*/
void QtxListAction::setComment( const QString& c, const QString& sc )
{
  if ( !myFrame )
    return;

  myFrame->setSingleComment( sc.isEmpty() ? c : sc );
  myFrame->setMultipleComment( c );
}

/*!
  \brief Create action widget.

  This function is called whenever the action is added 
  to a container widget that supports custom widgets like menu or toolbar.
  
  \param parent container widget the action is added to
  \return tool button for toolbar and 0 otherwise
*/
QWidget* QtxListAction::createWidget( QWidget* parent )
{
  if ( parent && parent->inherits( "QMenu" ) )
    return 0;

  QToolButton* tb = new QToolButton( parent );
  tb->setText( text() );
  tb->setIcon( icon() );
  tb->setPopupMode( QToolButton::MenuButtonPopup );
  tb->setMenu( myFrame );
  tb->setEnabled( isEnabled() && !names().isEmpty() );
  tb->setToolTip( toolTip() );
  connect( tb, SIGNAL( clicked( bool ) ), this, SLOT( onSingle( bool ) ) );

  return tb;
}

/*!
  \brief Destroy action widget.

  This function is called whenever the action is removed
  from a container widget that supports custom widgets like menu or toolbar.
  
  \param widget container widget the action is removed from
*/
void QtxListAction::deleteWidget( QWidget* widget )
{
  delete widget;
}

/*!
  \brief Initialize the action.
*/
void QtxListAction::initialize()
{
  setPopupMode( Item );
  
  myFrame = new QtxListAction::ListFrame( this, 0 );
  myFrame->setLinesNumber( 7 );
  myFrame->setCharsNumber( 5 );

  myFrame->hide();

  connect( this, SIGNAL( changed() ), this, SLOT( onChanged() ) );
  connect( this, SIGNAL( triggered( bool ) ), this, SLOT( onSingle( bool ) ) );
}

/*!
  \brief Called the action contents is changed.
*/
void QtxListAction::onChanged()
{
  QStringList lst = myFrame->names();

  if ( menu() )
  {
    menu()->clear();
    for ( QStringList::iterator iter = lst.begin(); iter != lst.end(); ++iter )
    {
      QAction* a = new QAction( *iter, menu() );
      menu()->addAction( a );
      connect( a, SIGNAL( triggered( bool ) ), this, SLOT( onTriggered( bool ) ) );
    }
  }

  QList<QWidget*> widList = createdWidgets();
  for ( QList<QWidget*>::iterator it = widList.begin(); it != widList.end(); ++it )
  {
    (*it)->setEnabled( isEnabled() && !lst.isEmpty() );
    QToolButton* tb = ::qobject_cast<QToolButton*>( *it );
    if ( tb )
    {
      tb->setText( text() );
      tb->setIcon( icon() );
      tb->setToolTip( toolTip() );
    }
  }
}

/*!
  \brief Called when a user click action button.
  \param on (not used)
*/

void QtxListAction::onSingle( bool /*on*/ )
{
  onMultiple( 1 );
}

/*!
  \brief Called when multiple items are selected. 
*/
void QtxListAction::onMultiple( const int numActions )
{
  if ( myFrame )
    myFrame->hide();

  if ( numActions > 0 )
    emit triggered( numActions );
}

/*!
  \brief Called when user activates an items in the popup sub menu. 
  \param on (not used)
*/
void QtxListAction::onTriggered( bool /*on*/ )
{
  if ( !menu() )
    return;

  QList<QAction*> actionList = menu()->actions();
  int idx = actionList.indexOf( ::qobject_cast<QAction*>( sender() ) );
  if ( idx < 0 )
    return;

  emit triggered( idx + 1 );
}

/*!
  \fn QtxListAction::activated(int numItems );
  \brief This signal is emitted when an action is activated.
  \param numItems number of items being selected in the action list.
*/
