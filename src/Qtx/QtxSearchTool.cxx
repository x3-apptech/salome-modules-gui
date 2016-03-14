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

// File   : QtxSearchTool.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "QtxSearchTool.h"

#include <QApplication>
#include <QCheckBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPersistentModelIndex>
#include <QShortcut>
#include <QTimer>
#include <QToolButton>
#include <QTreeView>

const char* const first_xpm[] = {
"16 16 14 1",
"       c None",
".      c #111111",
"+      c #0A0A0A",
"@      c #161616",
"#      c #ACACAC",
"$      c #FC6D6E",
"%      c #FB6364",
"&      c #F25B5C",
"*      c #EA5859",
"=      c #C1494A",
"-      c #B64545",
";      c #AB4040",
">      c #A03C3C",
",      c #99393A",
"           .    ",
" +@+      ..#   ",
" +$+#    .$.... ",
" +$+#   .$$$$$.#",
" +%+#  .%%%%%%.#",
" +&+# .&&&&&&&.#",
" +*+#.********.#",
" +=+.=========.#",
" +-+#.--------.#",
" +;+##.;;;;;;;.#",
" +>+# #.>>>>>>.#",
" +,+#  #.,,,,,.#",
" +,+#   #.,....#",
" +,+#    #..####",
" +@+#     #.#   ",
"  ###      ##   "};

const char* const last_xpm[] = {
"16 16 14 1",
"       c None",
".      c #111111",
"+      c #0A0A0A",
"@      c #161616",
"#      c #FC6D6E",
"$      c #ACACAC",
"%      c #FB6364",
"&      c #F25B5C",
"*      c #EA5859",
"=      c #C1494A",
"-      c #B64545",
";      c #AB4040",
">      c #A03C3C",
",      c #99393A",
"    .           ",
"    ..      +@+ ",
" ....#.     +#+$",
" .#####.    +#+$",
" .%%%%%%.   +%+$",
" .&&&&&&&.  +&+$",
" .********. +*+$",
" .=========.+=+$",
" .--------.$+-+$",
" .;;;;;;;.$$+;+$",
" .>>>>>>.$$ +>+$",
" .,,,,,.$$  +,+$",
" ....,.$$   +,+$",
" $$$..$$    +,+$",
"    .$$     +@+$",
"    $$       $$$"};

const char* const prev_xpm[] = {
"16 16 12 1",
"       c None",
".      c #111111",
"+      c #ACACAC",
"@      c #FC6D6E",
"#      c #FB6364",
"$      c #F25B5C",
"%      c #EA5859",
"&      c #C1494A",
"*      c #B64545",
"=      c #AB4040",
"-      c #A03C3C",
";      c #99393A",
"        .       ",
"       ..+      ",
"      .@......  ",
"     .@@@@@@@.+ ",
"    .########.+ ",
"   .$$$$$$$$$.+ ",
"  .%%%%%%%%%%.+ ",
" .&&&&&&&&&&&.+ ",
"  .**********.+ ",
"  +.=========.+ ",
"   +.--------.+ ",
"    +.;;;;;;;.+ ",
"     +.;......+ ",
"      +..++++++ ",
"       +.+      ",
"        ++      "};

const char* const next_xpm[] = {
"16 16 12 1",
"       c None",
".      c #111111",
"+      c #FC6D6E",
"@      c #FB6364",
"#      c #F25B5C",
"$      c #EA5859",
"%      c #C1494A",
"&      c #B64545",
"*      c #ACACAC",
"=      c #AB4040",
"-      c #A03C3C",
";      c #99393A",
"       .        ",
"       ..       ",
"  ......+.      ",
"  .+++++++.     ",
"  .@@@@@@@@.    ",
"  .#########.   ",
"  .$$$$$$$$$$.  ",
"  .%%%%%%%%%%%. ",
"  .&&&&&&&&&&.**",
"  .=========.** ",
"  .--------.**  ",
"  .;;;;;;;.**   ",
"  ......;.**    ",
"   ****..**     ",
"       .**      ",
"       **       "};

const char* const close_xpm[] = {
"16 16 8 1",
"       c None",
".      c #D73727",
"+      c #E17765",
"@      c #E7957F",
"#      c #DE6F48",
"$      c #DF7B4F",
"%      c #FAE9E4",
"&      c #FFFFFF",
"                ",
"  ............  ",
" .+@@@@@@@@@@+. ",
" .@#$$$$$$$$#@. ",
" .@$$%$$$$%$$@. ",
" .@$%&%$$%&%$@. ",
" .@$$%&%%&%$$@. ",
" .@$$$%&&%$$$@. ",
" .@$$$%&&%$$$@. ",
" .@$$%&%%&%$$@. ",
" .@$%&%$$%&%$@. ",
" .@$$%$$$$%$$@. ",
" .@#$$$$$$$$#@. ",
" .+@@@@@@@@@@+. ",
"  ............  ",
"                "};

const char* highlightColor = "#FF6666";
const int DefaultAutoHideDelay = 10000;

/*!
  \brief Wrap specified widget by another dumb widget.
  \internal
  \param parent widget to be used as parent for the dumb widget
  \param w widget to be wrapped
  \return wrapper widget
*/
static QWidget* wrapWidget( QWidget* parent, QWidget* w )
{
  QWidget* wrapper = new QWidget( parent );
  w->setParent( wrapper );
  QHBoxLayout* l = new QHBoxLayout( wrapper );
  l->setMargin( 1 );
  l->setSpacing( 0 );
  l->addWidget( w );
  return wrapper;
}

/*!
  \class QtxSearchTool
  \brief Context search tool.

  The QtxSearchTool class implements a specific context search tool widget
  which can be embedded into any GUI element.
  It represents the usual dialog panel with the line edit box used to enter
  text to be searched and set of buttons, like "Find Next", "Find Previous", etc.
  In addition, the search modifiers like "Case sensitive search", "Wrap search"
  are provided.

  Actually the class QtxSearchTool does not perform a serach itself - it is only
  the generic widget. To use this widget, you have to install a searcher depending
  on your needs. This should be a successor of the class QtxSearchTool::Searcher -
  it is the class which will perform actual search of the data in your widget
  according to the widget type.

  For the current moment, only one standard searcher is implemented: it is the
  class QtxTreeViewSearcher, which can be used to search the text data in the
  tree view widget (QTreeView). See this class for more details.

  The usual usage of the searcher widget is the following:
  \code
  QTreeView* tree = new QTreeView( this );
  QtxSearchTool* st = new QtxSearchTool( this, tree, QtxSearchTool::Standard );
  st->setActivators( QtxSearchTool::SlashKey | QtxSearchTool::StandardKey );
  st->setSearcher( new QtxTreeViewSearcher( tree ) );
  \endcode

  Note, that controls to be displayed by the search tool widget are passed as
  ORed flags to the widget's constructor. At any time, the available controls
  can be set/get with setControls() and controls() methods.
  By default, all widgets are displayed (see also QtxSearchTool::Controls
  enumeration).

  The class QtxSearchTool also provides a way to add custom widgets -
  these widgets are displayed at the bottom area of the tool box. Use
  method addCustomWidget() to add custom widget to the search tool box.
  Your searcher class can use custom widgets to perform advanced search.

  The class supports different ways of the activation, all of them can be
  switched on/off with setActivators() method. See QtxSearchTool::Activator
  enumeration for more details.
  By default, all methods are switched on: default hot key is <Ctrl><S> and
  standard key bindings are the platform dependent keyboard shortcuts.
  Shortcuts can be assigned with the setShortcuts() methods.
*/

/*!
  \brief Constructor.

  Creates a search tool widget with parent widget \a parent
  and watched widget \a watched. The controls to be displayed can be passed
  via \a controls parameter. By default, all controls are displayed.

  \param parent parent widget
  \param watched watched widget
  \param controls ORed controls flags (QtxSearchTool::Controls)
  \sa setWatchedWidget(), setControls()
*/
QtxSearchTool::QtxSearchTool( QWidget* parent, QWidget* watched, int controls, Qt::Orientation orientation )
: QFrame( parent ),
  myWatched( watched ? watched : parent ),
  mySearcher( 0 ),
  myControls( controls ),
  myActivators( None ),
  myAutoHideTimer( 0 ),
  myAutoHideEnabled( true )
{
  init( orientation );
}

/*!
  \brief Constructor.

  Creates a search tool widget with parent widget \a parent.
  Parameter \a parent is also used to set watched widget.
  The controls to be displayed can be passed via \a controls parameter.
  By default, all controls are displayed.

  \param parent parent widget
  \param controls ORed controls flags (QtxSearchTool::Controls)
  \sa setWatchedWidget(), setControls()
*/
QtxSearchTool::QtxSearchTool( QWidget* parent, int controls, Qt::Orientation orientation )
: QFrame( parent ),
  myWatched( parent ),
  mySearcher( 0 ),
  myControls( controls ),
  myActivators( None ),
  myAutoHideTimer( 0 ),
  myAutoHideEnabled( true )
{
  init( orientation );
}

/*!
  \brief Destructor.
*/
QtxSearchTool::~QtxSearchTool()
{
  clearShortcuts();
  if ( mySearcher )
    delete mySearcher;
}

/*!
  \brief Get watched widget.
  \return currently used watched widget
  \sa setWatchedWidget(), activators(), setActivators()
*/
QWidget* QtxSearchTool::watchedWidget() const
{
  return myWatched;
}

/*!
  \brief Set watched widget.

  Watched widget is that one for which shortcut bindings are set.
  When this widget has focus and any hot key binbding is pressed by the user,
  the search tool box is activated. The same occurs if slash key is pressed and
  QtxSearchTool::SlashKey activator is enabled. If the QtxSearchTool::PrintKey
  activator is enabled, the tool box is activated if any printed key is pressed
  by the user.

  \param watched a widget to be watched by the search tool
  \sa watchedWidget(), activators(), setActivators()
*/
void QtxSearchTool::setWatchedWidget( QWidget* watched )
{
  if ( myWatched )
  {
    myWatched->removeEventFilter( this );
  }

  myWatched = watched;

  initShortcuts( shortcuts() );

  if ( myWatched )
  {
    myWatched->installEventFilter( this );
  }
}

/*!
  \brief Get current searcher.
  \return currently set searcher (QtxSearchTool::Searcher)
  \sa setSearcher()
*/
QtxSearchTool::Searcher* QtxSearchTool::searcher() const
{
  return mySearcher;
}

/*!
  \brief Assign searcher.

  Note: the search tool takes ownership to the searcher
  and destroys it when deleted.

  \param s searcher to be used (QtxSearchTool::Searcher)
  \sa searcher()
*/
void QtxSearchTool::setSearcher( QtxSearchTool::Searcher* s )
{
  if ( mySearcher )
    delete mySearcher;
  mySearcher = s;
}

/*!
  \brief Get activators.
  \return activators currently enabled (ORed QtxSearchTool::Activator flags)
  \sa setActivators()
*/
int QtxSearchTool::activators() const
{
  return myActivators;
}

/*!
  \brief Set activators.
  \param flags set activators to be used (ORed QtxSearchTool::Activator flags)
  \sa activators()
*/
void QtxSearchTool::setActivators( const int flags )
{
  myActivators = flags;
  updateShortcuts();
}

/*!
  \brief Get controls.
  \return controls currently enabled (ORed QtxSearchTool::Controls flags)
  \sa setControls()
*/
int QtxSearchTool::controls() const
{
  return myControls;
}

/*!
  \brief Set controls.
  \param ctrls controls to be displayed (ORed QtxSearchTool::Controls flags)
  \sa controls()
*/
void QtxSearchTool::setControls( const int ctrls )
{
  if ( myControls == ctrls )
    return;
  myControls = ctrls;
  updateControls();
}

/*!
  \brief Get shortcuts.

  Note: the standard bindings are not include to the resulting list.

  \return list of shortcuts bindings currently set
  \sa setShortcuts()
*/
QList<QKeySequence> QtxSearchTool::shortcuts() const
{
  QList<QKeySequence> ks;

  ShortcutList::ConstIterator it;
  int i;
  for ( it = myShortcuts.begin(), i = 0; it != myShortcuts.end(); ++it, i++ )
  {
    if ( i > 2 ) ks.append( (*it)->key() );
  }

  return ks;
}

/*!
  \brief Set shortcuts.
  \param accel shortcut binding(s) to be used
  \sa shortcuts()
*/
void QtxSearchTool::setShortcuts( const QKeySequence& accel )
{
  QList<QKeySequence> ks;
  ks << accel;
  setShortcuts( ks );
}

/*!
  \brief Set shortcuts.
  \param accel shortcut bindings to be used
  \sa shortcuts()
*/
void QtxSearchTool::setShortcuts( const QList<QKeySequence>& accels )
{
  initShortcuts( accels );
}

/*!
  \brief Add custom widget.
  \param w custom widget to be added
  \param id widget unique ID to be used (if < 0, automatically assigned)
  \return widget unique ID
  \sa customWidget(), customWidgetId()
*/
int QtxSearchTool::addCustomWidget( QWidget* w, int id )
{
  if ( !w ) return -1;

  static int _wid = -1;

  int wid = -1;
  QMap<int, QWidget*>::ConstIterator it;
  for ( it = myWidgets.begin(); it != myWidgets.end() && wid == -1; ++it )
  {
    if ( it.value() == w )
      wid = it.key();
  }

  if ( wid != -1 )
    return wid;

  wid = id < 0 ? --_wid : id;

  QBoxLayout* vbox = qobject_cast<QBoxLayout*>( layout() );
  w->setParent( this );
  vbox->addWidget( w );
  myWidgets.insert( wid, w );

  return wid;
}

/*!
  \brief Get custom widget by ID.
  \param id widget ID
  \return custom widget or 0 if not found
  \sa addCustomWidget(), customWidgetId()
*/
QWidget* QtxSearchTool::customWidget( int id ) const
{
  QWidget* w = 0;
  if ( myWidgets.contains( id ) )
    w = myWidgets[ id ];
  return w;
}

/*!
  \brief Get custom widget ID.
  \param w custom widget
  \return custom widget ID or -1 if widget does not belong to the search tool
  \sa addCustomWidget(), customWidget()
*/
int QtxSearchTool::customWidgetId( QWidget* w ) const
{
  int wid = -1;
  QMap<int, QWidget*>::ConstIterator it;
  for ( it = myWidgets.begin(); it != myWidgets.end() && wid == -1; ++it )
  {
    if ( it.value() == w )
      wid = it.key();
  }
  return wid;
}

/*!
  \brief Check if auto-hide of the tool widget is enabled.

  By default, the search tool widget is automatically hidden
  after 10 seconds of idle (only if watched widget has input focus).

  \return \c true if auto-hide option is set
  \sa enableAutoHide()
*/
bool QtxSearchTool::isAutoHideEnabled() const
{
  return myAutoHideEnabled;
}

/*!
  \brief Set/clear auto-hide option.

  By default, the search tool widget is automatically hidden
  after 10 seconds of idle (only if watched widget has input focus).

  \param enable new option state
  \sa isAutoHideEnabled()
*/
void QtxSearchTool::enableAutoHide( bool enable )
{
  if ( myAutoHideEnabled == enable ) return;

  myAutoHideEnabled = enable;

  if ( myAutoHideEnabled )
  {
    if ( isVisible() && !focused() )
      myAutoHideTimer->start();
  }
  else
  {
    myAutoHideTimer->stop();
  }
}

/*!
  \brief Get 'case sensitive search' option value.

  This method returns \c true if 'case sensitive search' control
  is enabled and switched on.

  \return \c true if case sensitive search is performed
  \sa isRegExpSearch(), isSearchWrapped(), setControls()
  \sa setCaseSensitive(), setRegExpSearch(), setSearchWrapped()
*/
bool QtxSearchTool::isCaseSensitive() const
{
  return myControls & Case && myIsCaseSens->isChecked();
}

/*!
  \brief Get 'regular expression search' option value.

  This method returns \c true if 'regular expression search' control
  is enabled and switched on.

  \return \c true if regular expression search is performed
  \sa isCaseSensitive(), isSearchWrapped(), setControls()
  \sa setCaseSensitive(), setRegExpSearch(), setSearchWrapped()
*/
bool QtxSearchTool::isRegExpSearch() const
{
  return myControls & RegExp && myIsRegExp->isChecked();
}

/*!
  \brief Get 'search wrapping' option value.

  This method returns \c true if 'wrap search' control
  is enabled and switched on.

  \return \c true if search wrapping is enabled
  \sa isCaseSensitive(), isRegExpSearch(), setControls()
  \sa setCaseSensitive(), setRegExpSearch(), setSearchWrapped()
*/
bool QtxSearchTool::isSearchWrapped() const
{
  return myControls & Wrap && myWrap->isChecked();
}

/*!
  \brief Set 'case sensitive search' option value.
  \param on new option state
  \sa setRegExpSearch(), setSearchWrapped(), setControls()
  \sa isCaseSensitive(), isRegExpSearch(), isSearchWrapped()
*/
void QtxSearchTool::setCaseSensitive( bool on )
{
  if ( myControls & Case )
    myIsCaseSens->setChecked( on );
}

/*!
  \brief Set 'regular expression search' option value.
  \param on new option state
  \sa setCaseSensitive(), setSearchWrapped(), setControls()
  \sa isCaseSensitive(), isRegExpSearch(), isSearchWrapped()
*/
void QtxSearchTool::setRegExpSearch( bool on )
{
  if ( myControls & RegExp )
    myIsRegExp->setChecked( on );
}

/*!
  \brief Set 'search wrapping' option value.
  \param on new option state
  \sa setCaseSensitive(), setRegExpSearch(), setControls()
  \sa isCaseSensitive(), isRegExpSearch(), isSearchWrapped()
*/
void QtxSearchTool::setSearchWrapped( bool on )
{
  if ( myControls & Wrap )
    myWrap->setChecked( on );
}

/*!
  \brief Customize event handling.
  \param e event
  \return \c true if event has been handled
*/
bool QtxSearchTool::event( QEvent* e )
{
  if ( e->type() == QEvent::EnabledChange )
  {
    updateShortcuts();
  }
  else if ( e->type() == QEvent::KeyPress )
  {
    QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Qt::Key_Escape )
      hide();
  }
  else if ( e->type() == QEvent::Hide && myWatched )
  {
    myWatched->setFocus();
  }
  return QFrame::event( e );
}

/*!
  \brief Filter events from the watched widget.
  \param o object
  \param e event
  \return \c true if further event processing should be stopped
*/
bool QtxSearchTool::eventFilter( QObject* o, QEvent* e )
{
  switch ( e->type() ) 
  {
  case QEvent::KeyPress:
    if ( myWatched && o == myWatched )
    {
      QKeyEvent* ke = (QKeyEvent*)e;
      int key = ke->key();
      QString ttf = myData->text();
      QString text = ke->text();
      
      if ( isVisible() )
      {
        switch ( key )
        {
        case Qt::Key_Escape:
          hide();
          return true;
        case Qt::Key_Backspace:
          ttf.chop( 1 );
          break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
          findNext();
          return true;
        default:
          if ( text.isEmpty() || !text[0].isPrint() )
            return QFrame::eventFilter( o, e );
          ttf += text;
        }
      }
      else
      {
        if ( text.isEmpty() || ! isEnabled() || !text[0].isPrint() )
          return QFrame::eventFilter( o, e );

        if ( text.startsWith( '/' ) && myActivators & SlashKey )
        {
          myData->clear();
          find();
          return true;
        }
        else if ( !( myActivators & PrintKey ) )
        {
          return QFrame::eventFilter( o, e );
        }
        
        ttf = text;
        show();
      }
      myData->setText( ttf );
      find( ttf );
    }
    break; // case QEvent::KeyPress
  case QEvent::FocusIn:
  case QEvent::FocusOut:
    if ( focused() )
    {
      myAutoHideTimer->stop();
    }
    else if ( isVisible() && isAutoHideEnabled() )
    {
      myAutoHideTimer->start();
    }
    break;
  default:
    break;
  }
  return QFrame::eventFilter( o, e );
}

/*!
  \brief Activate search tool.

  Call this method to start new search.
*/
void QtxSearchTool::find()
{
  show();

  myData->setFocus( Qt::ShortcutFocusReason );
  myData->selectAll();
  myAutoHideTimer->stop();
}

/*!
  \brief Find next appropriate data.

  Call this method to repeat the search in the forward direction.
*/
void QtxSearchTool::findNext()
{
  find( myData->text(), fNext );
}

/*!
  \brief Find previous appropriate data.

  Call this method to repeat the search in the backward direction.
*/
void QtxSearchTool::findPrevious()
{
  find( myData->text(), fPrevious );
}

/*!
  \brief Find first appropriate data.

  Call this method to find the very first appropriate data.
*/
void QtxSearchTool::findFirst()
{
  find( myData->text(), fFirst );
}

/*!
  \brief Find last appropriate data.

  Call this method to find the very last appropriate data.
*/
void QtxSearchTool::findLast()
{
  find( myData->text(), fLast );
}

/*!
  \brief Perform search.
  \internal
  \param what text to be searched
  \param where search flags
*/
void QtxSearchTool::find( const QString& what, int where )
{
  if ( !isVisible() )
    show();

  QPalette p = myData->palette();
  p.setColor( QPalette::Active,
              QPalette::Base,
              QApplication::palette( myData ).color( QPalette::Active,
                                                     QPalette::Base ) );

  bool found = true;
  if ( mySearcher && !what.isEmpty() )
  {
    switch( where )
    {
    case fNext:
      found = mySearcher->findNext( what, this ); break;
    case fPrevious:
      found = mySearcher->findPrevious( what, this ); break;
    case fFirst:
      found = mySearcher->findFirst( what, this ); break;
    case fLast:
      found = mySearcher->findLast( what, this ); break;
    case fAny:
    default:
      found = mySearcher->find( what, this ); break;
    }
  }

  if ( !found )
    p.setColor( QPalette::Active, QPalette::Base, QColor( highlightColor ) );

  if ( !focused() && myAutoHideEnabled )
    myAutoHideTimer->start();

  myData->setPalette( p );
}

/*!
  \brief Called when any search modifier is switched.
  \internal
*/
void QtxSearchTool::modifierSwitched()
{
  find( myData->text() );
}

/*!
  \brief Initialize the search tool widget.
  \internal
*/
void QtxSearchTool::init( Qt::Orientation orientation )
{
  setFrameStyle( QFrame::StyledPanel | QFrame::Plain );

  myBtnWidget = new QWidget( this );
  QHBoxLayout* myBtnWidget_layout = new QHBoxLayout( myBtnWidget );
  myBtnWidget_layout->setSpacing( 0 );
  myBtnWidget_layout->setMargin( 0 );

  myModWidget = new QWidget( this );
  QHBoxLayout* myModWidget_layout = new QHBoxLayout( myModWidget );
  myModWidget_layout->setSpacing( 0 );
  myModWidget_layout->setMargin( 0 );

  myClose = new QToolButton( myBtnWidget );
  myClose->setIcon( QPixmap( close_xpm ) );
  myClose->setAutoRaise( true );
  myBtnWidget_layout->addWidget( wrapWidget( myBtnWidget, myClose ) );
  connect( myClose, SIGNAL( clicked() ), this, SLOT( hide() ) );

  myData = new QLineEdit( myBtnWidget );
  myData->setMinimumWidth( 50 );
  myBtnWidget_layout->addWidget( wrapWidget( myBtnWidget, myData ), 1 );
  connect( myData, SIGNAL( textChanged( const QString& ) ), this, SLOT( find( const QString& ) ) );
  connect( myData, SIGNAL( returnPressed() ), this, SLOT( findNext() ) );
  myData->installEventFilter( this );

  myToFirst = new QToolButton( myBtnWidget );
  myToFirst->setIcon( QPixmap( first_xpm ) );
  myToFirst->setAutoRaise( true );
  myBtnWidget_layout->addWidget( wrapWidget( myBtnWidget, myToFirst ), 0 );
  connect( myToFirst, SIGNAL( clicked() ), this, SLOT( findFirst() ) );
  myToFirst->installEventFilter( this );

  myPrev = new QToolButton( myBtnWidget );
  myPrev->setIcon( QPixmap( prev_xpm ) );
  myPrev->setAutoRaise( true );
  myBtnWidget_layout->addWidget( wrapWidget( myBtnWidget, myPrev ), 0 );
  connect( myPrev, SIGNAL( clicked() ), this, SLOT( findPrevious() ) );
  myPrev->installEventFilter( this );

  myNext = new QToolButton( myBtnWidget );
  myNext->setIcon( QPixmap( next_xpm ) );
  myNext->setAutoRaise( true );
  myBtnWidget_layout->addWidget( wrapWidget( myBtnWidget, myNext ), 0 );
  connect( myNext, SIGNAL( clicked() ), this, SLOT( findNext() ) );
  myNext->installEventFilter( this );

  myToLast = new QToolButton( myBtnWidget );
  myToLast->setIcon( QPixmap( last_xpm ) );
  myToLast->setAutoRaise( true );
  myBtnWidget_layout->addWidget( wrapWidget( myBtnWidget, myToLast ), 0 );
  connect( myToLast, SIGNAL( clicked() ), this, SLOT( findLast() ) );
  myToLast->installEventFilter( this );

  myIsCaseSens = new QCheckBox( tr( "Case sensitive" ), myModWidget );
  myModWidget_layout->addWidget( wrapWidget( myBtnWidget, myIsCaseSens ) );
  connect( myIsCaseSens, SIGNAL( stateChanged( int ) ), this, SLOT( modifierSwitched() ) );
  myIsCaseSens->installEventFilter( this );

  myIsRegExp = new QCheckBox( tr( "Regular expression" ), myModWidget );
  myModWidget_layout->addWidget( wrapWidget( myBtnWidget, myIsRegExp ) );
  connect( myIsRegExp, SIGNAL( stateChanged( int ) ), this, SLOT( modifierSwitched() ) );
  myIsRegExp->installEventFilter( this );

  myWrap = new QCheckBox( tr( "Wrap search" ), myModWidget );
  myModWidget_layout->addWidget( wrapWidget( myBtnWidget, myWrap ) );
  connect( myWrap, SIGNAL( stateChanged( int ) ), this, SLOT( modifierSwitched() ) );
  myWrap->installEventFilter( this );

  setWatchedWidget( myWatched );

  setShortcuts( QKeySequence( "Ctrl+S" ) );
  setActivators( Any );
  
  QBoxLayout* box = orientation == Qt::Vertical ? (QBoxLayout*)( new QVBoxLayout ) : (QBoxLayout*)( new QHBoxLayout );
  box->setSpacing( 0 );
  box->setMargin( 5 );
  box->addWidget( myBtnWidget );
  box->addWidget( myModWidget );
  setLayout( box );

  updateControls();
}

/*!
  \brief Check if any child widget has input focus.
  \internal
  \return \c true if any child widget has input focus
*/
bool QtxSearchTool::focused() const
{
  return isVisible() && isAncestorOf( QApplication::focusWidget() );
}

/*!
  \brief Clear shortcuts.
  \internal
*/
void QtxSearchTool::clearShortcuts()
{
  ShortcutList::Iterator it;
  for ( it = myShortcuts.begin(); it != myShortcuts.end(); ++it )
  {
    if ( !(*it).isNull() )
    {
      QShortcut* sc = (*it);
      delete sc;
    }
  }
  myShortcuts.clear();
}

/*!
  \brief Install shortcuts.
  \internal
  \param accels shortcuts list
*/
void QtxSearchTool::initShortcuts( const QList<QKeySequence>& accels )
{
  clearShortcuts();

  QWidget* p = myWatched ? myWatched : ( parentWidget() ? parentWidget() : this );
  QShortcut* sc;

  sc = new QShortcut( QKeySequence::Find, p );
  connect( sc, SIGNAL( activated() ), this, SLOT( find() ) );
  sc->setContext( Qt::WidgetShortcut );
  myShortcuts.append( sc );

  sc = new QShortcut( QKeySequence::FindNext, p );
  sc->setContext( Qt::WidgetShortcut );
  connect( sc, SIGNAL( activated() ), this, SLOT( findNext() ) );
  myShortcuts.append( sc );

  sc = new QShortcut( QKeySequence::FindPrevious, p );
  sc->setContext( Qt::WidgetShortcut );
  connect( sc, SIGNAL( activated() ), this, SLOT( findPrevious() ) );
  myShortcuts.append( sc );

  QList<QKeySequence>::ConstIterator it;
  for ( it = accels.begin(); it != accels.end(); ++it )
  {
    sc = new QShortcut( *it, p );
    sc->setContext( Qt::WidgetShortcut );
    connect( sc, SIGNAL( activated() ), this, SLOT( find() ) );
    myShortcuts.append( sc );
  }

  myAutoHideTimer = new QTimer( this );
  myAutoHideTimer->setInterval( DefaultAutoHideDelay );
  myAutoHideTimer->setSingleShot( true );
  connect( myAutoHideTimer, SIGNAL( timeout() ), this, SLOT( hide() ) );

  updateShortcuts();

  hide();
}

/*!
  \brief Update shortcuts state.
  \internal
*/
void QtxSearchTool::updateShortcuts()
{
  int i;
  ShortcutList::Iterator it;
  for ( it = myShortcuts.begin(), i = 0; it != myShortcuts.end(); ++it, i++ )
  {
    (*it)->setEnabled( isEnabled() && ( ( i < 3 && myActivators & StandardKey ) ||
                                        ( i > 2 && myActivators & HotKey ) ) );
  }
}

/*!
  \brief Update controls state.
  \internal
*/
void QtxSearchTool::updateControls()
{
  myData->parentWidget()->setVisible( myControls & Search );
  myNext->parentWidget()->setVisible( myControls & Next );
  myPrev->parentWidget()->setVisible( myControls & Prev );
  myToFirst->parentWidget()->setVisible( myControls & First );
  myToLast->parentWidget()->setVisible( myControls & Last );
  myClose->parentWidget()->setVisible( myControls & Close );
  myIsCaseSens->parentWidget()->setVisible( myControls & Case );
  myIsRegExp->parentWidget()->setVisible( myControls & RegExp );
  myWrap->parentWidget()->setVisible( myControls & Wrap );

  myBtnWidget->setVisible( myControls & Standard );
  myModWidget->setVisible( myControls & Modifiers );
}

/*!
  \class QtxSearchTool::Searcher
  \brief Generic searcher class.

  Searcher is generic class which is used by the search tool to perform
  widget-dependant search.

  To implement a searcher for some widget, just inherit from QtxSearchTool::Searcher
  and override pure virtual methods find(), findNext(), findPrevious(),
  findFirst() and findLast()
*/

/*!
  \brief Constructor.
*/
QtxSearchTool::Searcher::Searcher()
{
}

/*!
  \brief Destructor.
*/
QtxSearchTool::Searcher::~Searcher()
{
}

/*!
  \fn QtxSearchTool::Searcher::find(const QString& text, QtxSearchTool* st)
  \brief Start new search.
  \param text text to be found
  \param st search tool widget
  \sa findNext(), findPrevious(), findFirst(), findLast()
*/

/*!
  \fn QtxSearchTool::Searcher::findNext(const QString& text, QtxSearchTool* st)
  \brief Search next appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findPrevious(), findFirst(), findLast()
*/

/*!
  \fn QtxSearchTool::Searcher::findPrevious(const QString& text, QtxSearchTool* st)
  \brief Search previous appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findNext(), findFirst(), findLast()
*/

/*!
  \fn QtxSearchTool::Searcher::findFirst(const QString& text, QtxSearchTool* st)
  \brief Search first appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findNext(), findPrevious(), findLast()
*/

/*!
  \fn QtxSearchTool::Searcher::findLast(const QString& text, QtxSearchTool* st)
  \brief Search last appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findNext(), findPrevious(), findFirst()
*/

/*!
  \class QtxTreeViewSearcher
  \brief A QTreeView class based searcher.

  The class QtxTreeViewSearcher can be used to find the items in the
  QTreeView widget.

  The column for which data should be searched can be get/set with the
  searchColumn(), setSearchColumn() methods.
  By default, column 0 is used.
*/

/*!
  \brief Constructor.
  \param view tree view widget
  \param col column for which search to be performed (0 by default)
  \sa setSearchColumn()
*/
QtxTreeViewSearcher::QtxTreeViewSearcher( QTreeView* view, int col )
  : myView( view ), myColumn( col )
{
}

/*!
  \brief Destructor.
*/
QtxTreeViewSearcher::~QtxTreeViewSearcher()
{
}

/*!
  \brief Get column for which search is performed.
  \return column number
  \sa setSearchColumn()
*/
int QtxTreeViewSearcher::searchColumn() const
{
  return myColumn;
}

/*!
  \brief Set column for which search should be performed.
  \param column column number
  \sa searchColumn()
*/
void QtxTreeViewSearcher::setSearchColumn( int column )
{
  myColumn = column;
}

/*!
  \brief Start new search.
  \param text text to be found
  \param st search tool widget
  \sa findNext(), findPrevious(), findFirst(), findLast()
*/
bool QtxTreeViewSearcher::find( const QString& text, QtxSearchTool* st )
{
  if ( !myView )
    return false;

  const QModelIndexList& l = myView->selectionModel() ?
    myView->selectionModel()->selectedIndexes() : QModelIndexList();

  QModelIndex current;
  if ( l.count() > 0 )
    current = l.first();

  bool wrapSearch = st->isSearchWrapped();

  QModelIndexList found = findItems( text, st );

  if ( found.count() > 0 )
  {
    if ( !current.isValid() )
    {
      showItem( found.first() );
      return true;
    }

    if ( found.contains( current ) )
    {
      showItem( current );
      return true;
    }

    QModelIndex next = findNearest( current, found, true );
    if ( next.isValid() )
    {
      showItem( next );
      return true;
    }

    if ( wrapSearch )
    {
      showItem( found.first() );
      return true;
    }
  }

  return false;
}

/*!
  \brief Search next appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findPrevious(), findFirst(), findLast()
*/
bool QtxTreeViewSearcher::findNext( const QString& text, QtxSearchTool* st )
{
  if ( !myView )
    return false;

  const QModelIndexList& l = myView->selectionModel() ?
    myView->selectionModel()->selectedIndexes() : QModelIndexList();

  QModelIndex current;
  if ( l.count() > 0 )
    current = l.first();
  else if ( myIndex.isValid() )
    current = myIndex;

  bool wrapSearch = st->isSearchWrapped();

  QModelIndexList found = findItems( text, st );

  if ( found.count() > 0 )
  {
    if ( !current.isValid() )
    {
      showItem( found.first() );
      return true;
    }

    QModelIndex next = findNearest( current, found, true );
    if ( next.isValid() )
    {
      showItem( next );
      return true;
    }

    if ( wrapSearch )
    {
      showItem( found.first() );
      return true;
    }
  }

  return false;
}

/*!
  \brief Search previous appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findNext(), findFirst(), findLast()
*/
bool QtxTreeViewSearcher::findPrevious( const QString& text, QtxSearchTool* st )
{
  if ( !myView )
    return false;

  const QModelIndexList& l = myView->selectionModel() ?
    myView->selectionModel()->selectedIndexes() : QModelIndexList();

  QModelIndex current;
  if ( l.count() > 0 )
    current = l.first();
  else if ( myIndex.isValid() )
    current = myIndex;

  bool wrapSearch = st->isSearchWrapped();

  QModelIndexList found = findItems( text, st );

  if ( found.count() > 0 )
  {
    if ( !current.isValid() )
    {
      showItem( found.first() );
      return true;
    }

    QModelIndex next = findNearest( current, found, false );
    if ( next.isValid() )
    {
      showItem( next );
      return true;
    }

    if ( wrapSearch )
    {
      showItem( found.last() );
      return true;
    }
  }

  return false;
}

/*!
  \brief Search first appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findNext(), findPrevious(), findLast()
*/
bool QtxTreeViewSearcher::findFirst( const QString& text, QtxSearchTool* st )
{
  QModelIndexList found = findItems( text, st );

  if ( found.count() > 0 )
  {
    showItem( found.first() );
    return true;
  }

  return false;
}

/*!
  \brief Search last appropriate item.
  \param text text to be found
  \param st search tool widget
  \sa find(), findNext(), findPrevious(), findFirst()
*/
bool QtxTreeViewSearcher::findLast( const QString& text, QtxSearchTool* st )
{
  QModelIndexList found = findItems( text, st );

  if ( found.count() > 0 )
  {
    showItem( found.last() );
    return true;
  }

  return false;
}

/*!
  \brief Get match flags to be used by the searcher.
  \param st search tool widget
*/
Qt::MatchFlags QtxTreeViewSearcher::matchFlags( QtxSearchTool* st ) const
{
  Qt::MatchFlags fl = Qt::MatchRecursive;

  if ( st->isCaseSensitive() )
    fl = fl | Qt::MatchCaseSensitive;
  if ( st->isRegExpSearch() )
    fl = fl | Qt::MatchRegExp;
  else
    fl = fl | Qt::MatchContains;

  return fl;
}

/*!
  \brief Find all appropriate items.
  \internal
  \param text text to be found
  \param st search tool widget
*/
QModelIndexList QtxTreeViewSearcher::findItems( const QString& text, QtxSearchTool* st )
{
  QString s = text;

  Qt::MatchFlags fl = matchFlags( st );
  if ( fl & Qt::MatchRegExp ) {
    if ( !s.startsWith( "^" ) && !s.startsWith( ".*" ) )
      s.prepend( ".*" );
    if ( !s.endsWith( "$" ) && !s.endsWith( ".*" ) )
      s.append( ".*" );
  }

  if ( myView->model() )
    return myView->model()->match( myView->model()->index( 0, myColumn ),
                                   Qt::DisplayRole,
                                   s, -1, fl );
  return QModelIndexList();
}

/*!
  \brief Find model index from the list nearest to the specified index.
  \internal
  \param index model index for which a nearest item is searched
  \param lst list of model indices
  \param direction if \c true find next appropriate item, otherwise find privious
  appropriate item
*/
QModelIndex QtxTreeViewSearcher::findNearest( const QModelIndex& index,
                                              const QModelIndexList& lst,
                                              bool direction )
{
  if ( direction )
  {
    QListIterator<QModelIndex> it( lst );
    while ( it.hasNext() )
    {
      QModelIndex found = it.next();
      if ( compareIndices( found, index ) > 0 )
        return found;
    }
  }
  else
  {
    QListIterator<QModelIndex> it( lst );
    it.toBack();
    while ( it.hasPrevious() )
    {
      QModelIndex found = it.previous();
      if ( compareIndices( found, index ) < 0 )
        return found;
    }
  }
  return QModelIndex();
}

/*!
  \brief Ensure the found item to become visible and selected.
  \internal
  \param index item to be shown
*/
void QtxTreeViewSearcher::showItem( const QModelIndex& index )
{
  if ( myView && index.isValid() && myView->selectionModel() )
  {
    QItemSelectionModel::SelectionFlags f =
      QItemSelectionModel::Select | QItemSelectionModel::Rows | QItemSelectionModel::Clear;
    myView->selectionModel()->select( index, f );
    myView->scrollTo( index );
    myIndex = index;
  }
}

/*!
  \brief Get unique item ID.
  \internal
  \param index model index
  \return item ID
*/
QString QtxTreeViewSearcher::getId( const QModelIndex& index )
{
  QStringList ids;
  QModelIndex p = index;
  while ( p.isValid() )
  {
    ids.prepend( QString::number( p.row() ) );
    p = p.parent();
  }
  ids.prepend( "0" );
  return ids.join( ":" );
}

/*!
  \brief Compare items.
  \internal
  \param left first model index to be compared
  \param right last model index to be compared
  \return 0 if items are equal, negative value if left item is less than right one
  and positive value otherwise
*/
int QtxTreeViewSearcher::compareIndices( const QModelIndex& left,
                                         const QModelIndex& right )
{
  QString leftId = getId( left );
  QString rightId = getId( right );

  QStringList idsLeft  = leftId.split( ":", QString::SkipEmptyParts );
  QStringList idsRight = rightId.split( ":", QString::SkipEmptyParts );

  for ( int i = 0; i < idsLeft.count() && i < idsRight.count(); i++ )
  {
    int lid = idsLeft[i].toInt();
    int rid = idsRight[i].toInt();
    if ( lid != rid )
      return lid - rid;
  }
  return idsLeft.count() < idsRight.count() ? -1 :
    ( idsLeft.count() == idsRight.count() ? 0 : 1 );
}
