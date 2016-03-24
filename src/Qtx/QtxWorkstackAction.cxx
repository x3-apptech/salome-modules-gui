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

// File:      QtxWorkstackAction.cxx
// Author:    Sergey TELKOV
//
#include "QtxWorkstackAction.h"

#include "QtxWorkstack.h"

#include <QMenu>
#include <QWidgetList>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QListWidget>
#include <QStackedLayout>
#include <QToolButton>
#include <QIcon>
#include <QPixmap>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>

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

/*!
  \class QtxWorkstackAction
  \brief Implements actions group for menu Windows with standard operations, like
         "Split vertical", "Split horizontal", etc.
*/

/*!
  \brief Constructor.
  \param ws workstack
  \param parent parent object (owner of the action)
*/
QtxWorkstackAction::QtxWorkstackAction( QtxWorkstack* ws, QObject* parent )
: QtxActionSet( parent ),
  myWorkstack( ws ),
  myWindowsFlag( true )
{
  if ( myWorkstack )
    insertAction( myWorkstack->action( QtxWorkstack::SplitVertical ), SplitVertical );
  else
    insertAction( new QtxAction( tr( "Split the active window on two vertical parts" ),
                                 tr( "Split vertically" ), 0, this ), SplitVertical );

  if ( myWorkstack )
    insertAction( myWorkstack->action( QtxWorkstack::SplitHorizontal ), SplitHorizontal );
  else
    insertAction( new QtxAction( tr( "Split the active window on two horizontal parts" ),
                                 tr( "Split horizontally" ), 0, this ), SplitHorizontal );

  connect( this, SIGNAL( triggered( int ) ), this, SLOT( onTriggered( int ) ) );

  setMenuActions( Standard );

  myArrangeViewsAction = new QAction( tr( "Arrange Views" ), this );
}

/*!
  \brief Destructor.
*/
QtxWorkstackAction::~QtxWorkstackAction()
{
}

/*!
  \brief Get workstack.
  \return parent workstack
*/
QtxWorkstack* QtxWorkstackAction::workstack() const
{
  return myWorkstack;
}

/*!
  \brief Get arrange views action.
*/
QAction* QtxWorkstackAction::getArrangeViewsAction()
{
  return myArrangeViewsAction;
}

/*!
  \brief Set actions to be visible in the menu.

  Actions, which IDs are set in \a flags parameter, will be shown in the
  menu bar. Other actions will not be shown.

  \param flags ORed together actions flags
*/
void QtxWorkstackAction::setMenuActions( const int flags )
{
  action( SplitVertical )->setVisible( flags & SplitVertical );
  action( SplitHorizontal )->setVisible( flags & SplitHorizontal );
  myWindowsFlag = flags & Windows;
}

/*!
  \brief Get menu actions which are currently visible in the menu bar.
  \return ORed together actions flags
  \sa setMenuActions()
*/
int QtxWorkstackAction::menuActions() const
{
  int ret = 0;
  ret = ret | ( action( SplitVertical )->isVisible() ? SplitVertical : 0 );
  ret = ret | ( action( SplitHorizontal )->isVisible() ? SplitHorizontal : 0 );
  ret = ret | ( myWindowsFlag ? Windows : 0 );
  return ret;
}

/*!
  \brief Get keyboard accelerator for the specified action.
  \param id menu action ID
  \return keyboard accelerator of menu item or 0 if there is no such action
*/
int QtxWorkstackAction::accel( const int id ) const
{
  int a = 0;
  if ( action( id ) )
    a = action( id )->shortcut()[0];
  return a;
}

/*!
  \brief Get icon for the specified action.

  If \a id is invalid, null icon is returned.

  \param id menu action ID
  \return menu item icon
*/
QIcon QtxWorkstackAction::icon( const int id ) const
{
  QIcon ico;
  if ( action( id ) )
    ico = action( id )->icon();
  return ico;
}

/*!
  \brief Get menu item text for the specified action.
  \param id menu action ID
  \return menu item text or null QString if there is no such action
*/
QString QtxWorkstackAction::text( const int id ) const
{
  QString txt;
  if ( action( id ) )
    txt = action( id )->text();
  return txt;
}

/*!
  \brief Get status bar tip for the specified action.
  \param id menu action ID
  \return status bar tip menu item or null QString if there is no such action
*/
QString QtxWorkstackAction::statusTip( const int id ) const
{
  QString txt;
  if ( action( id ) )
    txt = action( id )->statusTip();
  return txt;
}

/*!
  \brief Set keyboard accelerator for the specified action.
  \param id menu action ID
  \param a new keyboard accelerator
*/
void QtxWorkstackAction::setAccel( const int id, const int a )
{
  if ( action( id ) )
    action( id )->setShortcut( a );
}

/*!
  \brief Set menu item icon for the specified action.
  \param id menu action ID
  \param ico new menu item icon
*/
void QtxWorkstackAction::setIcon( const int id, const QIcon& icon )
{
  if ( action( id ) )
    action( id )->setIcon( icon );
}

/*!
  \brief Set menu item text for the specified action.
  \param id menu action ID
  \param txt new menu item text
*/
void QtxWorkstackAction::setText( const int id, const QString& txt )
{
  if ( action( id ) )
    action( id )->setText( txt );
}

/*!
  \brief Set menu item status bar tip for the specified action.
  \param id menu action ID
  \param txt new menu item status bar tip
*/
void QtxWorkstackAction::setStatusTip( const int id, const QString& txt )
{
  if ( action( id ) )
    action( id )->setStatusTip( txt );
}

/*!
  \brief Process action activated by the user.
  \param type action ID
*/
void QtxWorkstackAction::perform( const int type )
{
  /*
  switch ( type )
  {
  case SplitVertical:
    splitVertical();
    break;
  case SplitHorizontal:
    splitHorizontal();
    break;
  }
  */
}

/*!
  \brief Split the window area in the workstack in the vertical direction.
*/
void QtxWorkstackAction::splitVertical()
{
  QtxWorkstack* ws = workstack();
  if ( ws )
    ws->splitVertical();
}

/*!
  \brief Split the window area in the workstack in the horizontal direction.
*/
void QtxWorkstackAction::splitHorizontal()
{
  QtxWorkstack* ws = workstack();
  if ( ws )
    ws->splitHorizontal();
}

/*!
  \brief Called when action is added to the menu bar.
  \param w menu bar widget this action is being added to
*/
void QtxWorkstackAction::addedTo( QWidget* w )
{
  QtxActionSet::addedTo( w );

  QMenu* pm = ::qobject_cast<QMenu*>( w );
  if ( pm )
    connect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Called when action is removed from the menu bar.
  \param w menu bar widget this action is being removed from
*/
void QtxWorkstackAction::removedFrom( QWidget* w )
{
  QtxActionSet::removedFrom( w );

  QMenu* pm = ::qobject_cast<QMenu*>( w );
  if ( pm )
    disconnect( pm, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

/*!
  \brief Update all menu action state.
*/
void QtxWorkstackAction::updateContent()
{
  bool count = workstack() ? workstack()->splitWindowList().count() > 1 : 0;
  action( SplitVertical )->setEnabled( count );
  action( SplitHorizontal )->setEnabled( count );
  count = workstack() ? workstack()->windowList().count() > 1 : 0;
  myArrangeViewsAction->setEnabled( count );

  updateWindows();
}

/*!
  \brief Update actions which refer to the opened child windows.
*/
void QtxWorkstackAction::updateWindows()
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return;

  QList<QAction*> lst = actions();
  for ( QList<QAction*>::iterator it = lst.begin(); it != lst.end(); ++it )
  {
    int id = actionId( *it );
    if ( id >= Windows )
      removeAction( *it );
  }

  bool base = action( SplitVertical )->isVisible() || action( SplitHorizontal )->isVisible();

  QList<QAction*> items;
  QMap<QAction*, int> map;
  if ( menuActions() & Windows )
  {
    int index = 1;
    QWidgetList wList = ws->windowList();
    for ( QWidgetList::iterator it = wList.begin(); it != wList.end(); ++it, index++ )
    {
      QWidget* wid = *it;
      QAction* a = new QtxAction( wid->windowTitle(), wid->windowTitle(), 0, this, true );
      a->setChecked( wid == ws->activeWindow() );
      items.append( a );
      map.insert( a, Windows + index );
    }

    if ( base && !items.isEmpty() )
    {
      QAction* sep = new QtxAction( this );
      sep->setSeparator( true );
      items.prepend( sep );
      map.insert( sep, Windows );
    }
  }

  if ( !items.isEmpty() )
    insertActions( items );

  for ( QMap<QAction*, int>::const_iterator itr = map.begin(); itr != map.end(); ++itr )
    setActionId( itr.key(), itr.value() );
}

/*!
  \brief Called when parent menu is about to show.

  Updates all menu items.
*/
void QtxWorkstackAction::onAboutToShow()
{
  QMenu* pm = ::qobject_cast<QMenu*>( sender() );
  if ( pm )
    updateContent();
}

/*!
  \brief Called when menu item corresponding to some child window is activated.

  Activates correposponding child window.

  \param idx menu item index
*/
void QtxWorkstackAction::activateItem( const int idx )
{
  QtxWorkstack* ws = workstack();
  if ( !ws )
    return;

  QWidgetList wList = ws->windowList();
  if ( idx >= 0 && idx < (int)wList.count() )
    wList.at( idx )->setFocus();
}

/*!
  \brief Called when menu item is activated by the user.

  Perform the corresponding action.

  \param id menu item identifier
*/
void QtxWorkstackAction::onTriggered( int id )
{
  if ( id < Windows )
    perform( id );
  else
    activateItem( id - Windows - 1 );
}

/*!
  \class QtxSplitDlg
  \brief Used for arranging views(menu item "Window->Arrange Views")
         and for creating sub-views of current view(button "Create sub-views")
*/

/*!
  \brief Constructor.
  \param parent - parent object
  \param workstack - Work Stack widget
  \param mode - mode of current dialog
*/
QtxSplitDlg::QtxSplitDlg( QWidget* parent, QtxWorkstack* workstack, QtxSplitDlgMode mode )
: QDialog( parent ),
  myWorkstack( workstack ),
  myDlgMode( mode ),
  myViewsNB(2),
  mySplitMode(0),
  myNBSelectedViews(0),
  myIsCloseViews( false )
{
  Q_INIT_RESOURCE(Qtx);

  setModal( true );
  setObjectName( "Qtx_Split" );

  if( mode == ArrangeViews )
    setWindowTitle( tr( "Arrange views" ) );
  else if( mode == CreateSubViews )
    setWindowTitle( tr( "Create sub-views") );

  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  /////////////////////////////////////////////////////////////////////
  QGroupBox* aGroupViewsLayout = new QGroupBox();
  if( mode == ArrangeViews )
    aGroupViewsLayout->setTitle( tr("Views Layout") );
  else if( mode == CreateSubViews )
    aGroupViewsLayout->setTitle( tr("Sub-views Layout") );

  QGridLayout* OptionsViewsLayout = new QGridLayout( aGroupViewsLayout );

  // Radio Buttons for selecting the number of views
  QVBoxLayout* RadioButtonsLayout = new QVBoxLayout();
  myButton2Views = new QRadioButton( "2", this );
  myButton3Views = new QRadioButton( "3", this );
  myButton4Views = new QRadioButton( "4", this );

  myButton2Views->setChecked( true );

  RadioButtonsLayout->addWidget( myButton2Views );
  RadioButtonsLayout->addWidget( myButton3Views );
  RadioButtonsLayout->addWidget( myButton4Views );

  // Buttons for possibility of switching the variants of split
  myButtonPrevious = new QPushButton( this );
  myButtonPrevious->setIcon( QPixmap( prev_xpm ) );
  myButtonPrevious->setAutoDefault(true);
  myButtonPrevious->setEnabled( false );

  myButtonNext = new QPushButton( this );
  myButtonNext->setIcon( QPixmap( next_xpm ) );
  myButtonNext->setAutoDefault(true);
  myButtonNext->setEnabled( false );

  // Split options
  myStackedSplitLayout = new QStackedLayout();

  QButtonGroup* SplitOptions = new QButtonGroup( this );
  SplitOptions->setExclusive( true );

  // Arrange icons for 2 views according to next scheme
  //     x
  //     x
  QGridLayout* aSplit2Layout = new QGridLayout();
  for( int i=1; i<=2; i++ ) {
    QToolButton* aSplitBtn = createSplitButton( 2, i );
    aSplit2Layout->addWidget( aSplitBtn, i-1, 0 );
    SplitOptions->addButton( aSplitBtn, i-1 );
  }
  QWidget* aSplit2Widget = new QWidget( this );
  aSplit2Widget->setLayout( aSplit2Layout );
  myStackedSplitLayout->addWidget( aSplit2Widget );

  // Arrange icons for 3 views according to next scheme
  //     x x x
  //     x x x
  QGridLayout* aSplit3Layout = new QGridLayout();
  bool anIconPosition = 0;
  for( int i=1; i<=6; i++ ) {
    QToolButton* aSplitBtn = createSplitButton( 3, i );
    aSplit3Layout->addWidget( aSplitBtn, anIconPosition, int((i-1)/2) );
    SplitOptions->addButton( aSplitBtn, i+1 );
    anIconPosition = !anIconPosition;
  }
  QWidget* aSplit3Widget = new QWidget( this );
  aSplit3Widget->setLayout( aSplit3Layout );
  myStackedSplitLayout->addWidget( aSplit3Widget );

  // Arrange icons for 4 views according to next scheme
  //     x x x      x x x x      x x
  //     x x x      x x x x      x x
  //     x x x
  QGridLayout* aSplit4Layout = new QGridLayout();
  int aPosition = 0;
  for( int i=1; i<=21; i++ ) {
    QToolButton* aSplitBtn = createSplitButton( 4, i );
    SplitOptions->addButton( aSplitBtn, i+7 );
    if( i <= 9 ) {
      aSplit4Layout->addWidget( aSplitBtn, int((i-1)/3), aPosition );
      aPosition = ( aPosition == 2 ) ? 0: aPosition + 1;
    }
    else if( i > 9 && i <= 17 ) {
      aSplit4Layout->addWidget( aSplitBtn, int( (i-10)/4), aPosition );
      aPosition = ( aPosition == 3 ) ? 0: aPosition + 1;
    }
    else if( i>17 ) {
      aSplit4Layout->addWidget( aSplitBtn, int( (i-18)/2 ), aPosition );
      aPosition = ( aPosition == 1 ) ? 0: aPosition + 1;
    }
    if( i == 9 || i == 17 || i == 21 ) { //finish set icon in current stack widget
      QWidget* aSplit4Widget = new QWidget( this );
      aSplit4Widget->setLayout( aSplit4Layout );
      myStackedSplitLayout->addWidget( aSplit4Widget );
      aSplit4Layout = new QGridLayout();
    }
  }

  QHBoxLayout* ArrowLayout = new QHBoxLayout();
  ArrowLayout->addSpacing(130);
  ArrowLayout->addWidget( myButtonPrevious );
  ArrowLayout->addWidget( myButtonNext );
  ArrowLayout->addSpacing(130);

  OptionsViewsLayout->addLayout( ArrowLayout, 0, 1, 1, 2);
  OptionsViewsLayout->addLayout( RadioButtonsLayout, 1, 0 );
  OptionsViewsLayout->addLayout( myStackedSplitLayout, 1, 1, 1, 2 );

  /////////////////////////////////////////////////////////////////////

  QGroupBox* GroupProperties = new QGroupBox();

  if( mode == ArrangeViews ) {
    GroupProperties->setTitle( tr("Views List") );

    QVBoxLayout* ViewsListLayout = new QVBoxLayout( GroupProperties );

    QWidgetList aWidgetList = myWorkstack->windowList();
    myViewsList = new QListWidget( GroupProperties );
    myViewsList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect( myViewsList, SIGNAL( itemClicked(QListWidgetItem*) ), this, SLOT( onSynchronize() ) );

    for( int i=0; i< aWidgetList.count(); i++ ) {
      QWidget* aWidget = aWidgetList.at(i);
      QListWidgetItem* anItem = new QListWidgetItem( aWidget->windowTitle(), myViewsList );
      anItem->setCheckState( Qt::Unchecked );
      anItem->setFlags( anItem->flags() & ~Qt::ItemIsSelectable );
    }

    QRadioButton* CloseViews = new QRadioButton( GroupProperties );
    CloseViews->setText( tr( "Close remaining views" ) );
    connect( CloseViews, SIGNAL( pressed() ), this, SLOT( onCloseViews() ) );

    QRadioButton* StackViews = new QRadioButton( GroupProperties );
    StackViews->setText( tr( "Stack remaining views in the last area" ) );
    StackViews->setChecked( true );
    connect( StackViews, SIGNAL( pressed() ), this, SLOT( onStackViews() ) );

    ViewsListLayout->addWidget( myViewsList );
    ViewsListLayout->addWidget( CloseViews );
    ViewsListLayout->addWidget( StackViews );
  }
  else {
    GroupProperties->setTitle( tr( "Sub-views Properties" ) );
    for( int i = 0; i < 4; i++ ) {
      QLabel* SubView = new QLabel( tr( "Sub-view" ) + " " + QString::number(i+1),
                                    GroupProperties);
      myLabels << SubView;

      QComboBox* ComboBox = new QComboBox( GroupProperties );
      ComboBox->addItem("XYZ");
      ComboBox->addItem("XY");
      ComboBox->addItem("XZ");
      ComboBox->addItem("YZ");
      myComboBox.append( ComboBox );
      myMapComboBoxMode.insert( ComboBox, (ViewMode)i );
      ComboBox->setCurrentIndex(i);
      connect( ComboBox, SIGNAL( currentIndexChanged (int) ), this, SLOT( onComboBoxChanged(int) ) );
    }

    QGridLayout* SubViewsPropLayout = new QGridLayout( GroupProperties );
    SubViewsPropLayout->addWidget( myLabels[0],   0, 0 );
    SubViewsPropLayout->addWidget( myComboBox[0], 0, 1 );
    SubViewsPropLayout->addWidget( myLabels[1],   0, 2 );
    SubViewsPropLayout->addWidget( myComboBox[1], 0, 3 );
    SubViewsPropLayout->addWidget( myLabels[2],   1, 0 );
    SubViewsPropLayout->addWidget( myComboBox[2], 1, 1 );
    SubViewsPropLayout->addWidget( myLabels[3],   1, 2 );
    SubViewsPropLayout->addWidget( myComboBox[3], 1, 3 );
  }

  /////////////////////////////////////////////////////////////////////

  QGroupBox* GroupButtons = new QGroupBox();
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout(GroupButtons);

  myButtonApply = new QPushButton(tr( "&Apply" ), GroupButtons);
  myButtonApply->setAutoDefault(true);
  myButtonApply->setDefault(true);
  myButtonApply->setEnabled( false );
  QPushButton* buttonClose = new QPushButton(tr( "&Close" ), GroupButtons);
  buttonClose->setAutoDefault(true);

  GroupButtonsLayout->addWidget( myButtonApply );
  GroupButtonsLayout->addSpacing(100);
  GroupButtonsLayout->addWidget( buttonClose );

  /////////////////////////////////////////////////////////////////////

  topLayout->addWidget( aGroupViewsLayout );
  topLayout->insertStretch(1);
  topLayout->addWidget( GroupProperties );
  topLayout->addWidget( GroupButtons );


  connect( myButton2Views,   SIGNAL( pressed() ), this,    SLOT( onChangeIcons() ) );
  connect( myButton3Views,   SIGNAL( pressed() ), this,    SLOT( onChangeIcons() ) );
  connect( myButton4Views,   SIGNAL( pressed() ), this,    SLOT( onChangeIcons() ) );
  connect( myButtonPrevious, SIGNAL( pressed() ), this,    SLOT( onPreviousViews() ) );
  connect( myButtonNext,     SIGNAL( pressed() ), this,    SLOT( onNextViews() ) );
  connect( SplitOptions,     SIGNAL( buttonClicked(int) ), SLOT( onSplitChanged(int)));
  connect( buttonClose,      SIGNAL( pressed() ), this,    SLOT( reject() ) );
  connect( myButtonApply,    SIGNAL( pressed() ), this,    SLOT( onApply() ) );

  initialize();
}

/*!
  \brief Destructor
*/
QtxSplitDlg::~QtxSplitDlg()
{
}

/*!
  \brief Return current split mode
*/
int QtxSplitDlg::getSplitMode()
{
  return mySplitMode;
}

/*!
  \brief Return selected view modes
*/
QList<int> QtxSplitDlg::getViewsMode()
{
  QList<int> aList;
  for ( int i = 0; i < myViewsNB; i++ ) {
    int aMode = myMapComboBoxMode[myComboBox[i]];
    aList<<aMode;
  }
  return aList;
}

/*!
  \brief Create new QToolButton using icon of split mode
*/
QToolButton* QtxSplitDlg::createSplitButton( int theViewsNB, int theSplitMode )
{
  QString anImageName = QString( ":/images/qtx_split%1_%2.png" )
  		                  .arg(theViewsNB).arg(theSplitMode).toLatin1().constData();
  QPixmap aSplitIcon( anImageName );
  QToolButton* aSplitBtn = new QToolButton( this );
  aSplitBtn->setCheckable( true );
  aSplitBtn->setIcon( aSplitIcon );
  aSplitBtn->setIconSize( QSize( aSplitIcon.width(), aSplitIcon.height() ) );
  return aSplitBtn;
}

/*!
  \brief Initialisation of widgets
*/
void QtxSplitDlg::initialize()
{
  if( myDlgMode == ArrangeViews ) {
    int aViewsNumber = myWorkstack->windowList().count();
    if( aViewsNumber == 2 ) {
      myButton3Views->setEnabled( false );
      myButton4Views->setEnabled( false );
    }
    else if( aViewsNumber == 3 )
      myButton4Views->setEnabled( false );
  }
  else if( myDlgMode == CreateSubViews ) {
    for( int i = 0; i < 4; i++) {
      if( i < myViewsNB )
        myMapModeIsBusy[(ViewMode)i] = true;
      else
        myMapModeIsBusy[ (ViewMode)i] = false;
    }
    onSynchronize();
  }
}

/*!
  \brief Verification if all items necessary for dialog were selected
*/
void QtxSplitDlg::valid()
{
  bool isValid = false;
  if( myViewsNB == 2 )
    isValid = ( mySplitMode >= 0 && mySplitMode < 2 )? true:false;
  else if( myViewsNB == 3 )
    isValid = ( mySplitMode >= 2 && mySplitMode < 8 )? true:false;
  else if( myViewsNB == 4)
    isValid = ( mySplitMode >=8 && mySplitMode < 29 )?true:false;

  if( myDlgMode == ArrangeViews ) {
    if( myNBSelectedViews != myViewsNB)
      isValid = false;
  }
  myButtonApply->setEnabled( isValid );
}

/*!
  \brief Called when number of views was changed
         and it's necessary to set others split icons
*/
void QtxSplitDlg::onChangeIcons()
{
  if( myButton2Views->isDown() ) {
    myViewsNB = 2;
    mySplitMode = 0;
    myStackedSplitLayout->setCurrentIndex(0);
    myButtonPrevious->setEnabled( false );
    myButtonNext->setEnabled( false );
  }
  else if( myButton3Views->isDown() ) {
    myViewsNB = 3;
    mySplitMode = 2;
    myStackedSplitLayout->setCurrentIndex(1);
    myButtonPrevious->setEnabled( false );
    myButtonNext->setEnabled( false );
  }
  else if( myButton4Views->isDown() ) {
    myViewsNB = 4;
    mySplitMode = 8;
    myStackedSplitLayout->setCurrentIndex(2);
    myButtonPrevious->setEnabled( false );
    myButtonNext->setEnabled( true );
  }
  onSynchronize();
}

/*!
  \brief Called when user selects an icon for split
*/
void QtxSplitDlg::onSplitChanged( int theMode )
{
  mySplitMode = theMode;
  valid();
}

/*!
  \brief Called when user selects item of Combo Box
*/
void QtxSplitDlg::onComboBoxChanged( int theItem )
{
  QMap<ViewMode,bool > aModeEnabledMap;
  for( int i = 0; i < 4; i++ )
    aModeEnabledMap[(ViewMode)i] = false;
  QComboBox* aSender = qobject_cast<QComboBox*>(sender());
  for ( int i=0;i<4;i++ )
    if( myComboBox[i] == aSender )
      myMapComboBoxMode[myComboBox[i]]=(ViewMode)theItem;

  for( int i = 0; i < 4; i++ ) {
    if( myComboBox[i]->isVisible() ) {
      ViewMode aViewMode = myMapComboBoxMode[myComboBox[i]];
      aModeEnabledMap[aViewMode] = true;
    }
  }
  for( int i = 0; i < 4; i++ ) {
    if( myComboBox[i] != aSender ) {
      ViewMode aNewMode;
      if( myMapComboBoxMode[myComboBox[i]] == (ViewMode)(theItem) ) {
        for( int j = 0; j < 4; j++ )
          if( !aModeEnabledMap[(ViewMode)j] )
            aNewMode = (ViewMode)j;
        myComboBox[i]->setCurrentIndex( (int)aNewMode );
      }
    }
  }
}

/*!
  \brief Called when user taps previous button
*/
void QtxSplitDlg::onPreviousViews()
{
  int aCurrentIndex = myStackedSplitLayout->currentIndex();
  myStackedSplitLayout->setCurrentIndex( aCurrentIndex - 1 );
  if( myStackedSplitLayout->currentIndex() == 2 )
    myButtonPrevious->setEnabled( false );
  if( myStackedSplitLayout->currentIndex() < 4 )
    myButtonNext->setEnabled( true );
}

/*!
  \brief Called when user taps next button
*/
void QtxSplitDlg::onNextViews()
{
  int aCurrentIndex = myStackedSplitLayout->currentIndex();
  myStackedSplitLayout->setCurrentIndex( aCurrentIndex + 1 );
  if( myStackedSplitLayout->currentIndex() == 4 )
    myButtonNext->setEnabled( false );
  if( myStackedSplitLayout->currentIndex() > 2 )
    myButtonPrevious->setEnabled( true );
}

/*!
  \brief Synchronize data and widgets
*/
void QtxSplitDlg::onSynchronize( )
{
  if( myDlgMode == ArrangeViews) {
    int aCheckedNumber = 0;
    for( int i=0; i < myViewsList->count(); i++ ) {
      if( myViewsList->item(i)->checkState() == Qt::Checked )
        aCheckedNumber++;
      if( aCheckedNumber == myViewsNB )
        for( int p = i+1; p < myViewsList->count(); p++ )
          myViewsList->item(p)->setCheckState( Qt::Unchecked );
    }
    if( aCheckedNumber == myViewsNB ) {
      for( int i=0; i < myViewsList->count(); i++ )
        if( myViewsList->item(i)->checkState() == Qt::Unchecked )
          myViewsList->item(i)->setFlags( myViewsList->item(i)->flags() & ~Qt::ItemIsEnabled );
    }
    else if( aCheckedNumber < myViewsNB ) {
      for( int i=0; i < myViewsList->count(); i++ )
        if( myViewsList->item(i)->checkState() == Qt::Unchecked )
          myViewsList->item(i)->setFlags( myViewsList->item(i)->flags() | Qt::ItemIsEnabled );
    }
    myNBSelectedViews = aCheckedNumber;
  }
  else if( myDlgMode == CreateSubViews ) {
    foreach( QComboBox* aBox, myComboBox )
      aBox->setVisible( true );
    foreach( QLabel* aLabel, myLabels )
      aLabel->setVisible( true );

    for( int i = myViewsNB; i < 4; i++ ) {
      myComboBox[i]->setVisible( false );
      myLabels[i]->setVisible( false );
    }
    for( int i = 0; i < 4; i++ )
      myComboBox[i]->setCurrentIndex(i);
  }
  valid();
}

/*!
  \brief Called when check box "Close remaining views" is active
*/
void QtxSplitDlg::onCloseViews()
{
  myIsCloseViews = true;
}

/*!
  \brief Called when check box "Stack remaining views
         in the last area" is active
*/
void QtxSplitDlg::onStackViews()
{
  myIsCloseViews = false;
}

/*!
  \brief Apply dialog
*/
void QtxSplitDlg::onApply()
{
  if( myDlgMode == ArrangeViews ) {
    myWorkstack->stack();
    int Split3Map[6][6] = {
    //  View, Dir, View,Move, Dir
      {  1,    1,   2,   -1,   1 },
      {  1,    2,   2,   -1,   2 },
      {  2,    1,   1,   -1,   2 },
      {  1,    1,   2,    1,   2 },
      {  1,    2,   2,    1,   1 },
      {  2,    2,   1,   -1,   1 },
    };

    int Split4Map[21][9] = {
    //  View, Dir, View,Move, Dir, View,Move, Dir
        { 1,   1,   2,   -1,   1,   3,   -1,   1 },
        { 1,   2,   2,   -1,   2,   3,   -1,   2 },
        { 1,   1,   2,   -1,   2,   3,    1,   2 },
        { 2,   1,   3,   -1,   1,   1,   -1,   2 },
        { 1,   1,   3,   -1,   1,   2,    1,   2 },
        { 1,   1,   2,   -1,   1,   3,    2,   2 },
        { 2,   2,   3,   -1,   2,   1,   -1,   1 },
        { 1,   2,   3,   -1,   2,   2,    1,   1 },
        { 1,   2,   2,   -1,   2,   3,    2,   1 },
        { 3,   2,   2,   -1,   1,   1,   -1,   2 },
        { 3,   2,   1,   -1,   1,   2,    1,   2 },
        { 1,   2,   3,    1,   1,   2,    1,   2 },
        { 1,   2,   2,    1,   1,   3,    2,   2 },
        { 1,   1,   3,    1,   2,   2,    1,   1 },
        { 1,   1,   2,    1,   2,   3,    2,   1 },
        { 3,   1,   2,   -1,   2,   1,   -1,   1 },
        { 3,   1,   1,   -1,   2,   2,    1,   1 },
        { 1,   2,   2,    1,   1,   3,    1,   1 },
        { 3,   2,   1,   -1,   1,   2,   -1,   1 },
        { 3,   1,   1,   -1,   2,   2,   -1,   2 },
        { 1,   1,   2,    1,   2,   3,    1,   2 }
    };

    QWidgetList aWidgetList = myWorkstack->windowList();
    QWidgetList aListChecked;
    QWidgetList aListUnchecked;
    QtxWorkstack::SplitType aSplitType = QtxWorkstack::SplitMove;

    for( int i = 0; i < myViewsList->count(); i++ ) {
      QString aName = myViewsList->item(i)->text();
      for( int j = 0; j< aWidgetList.count(); j++ ) {
        QWidget* aWidget = aWidgetList.at(j);
        if( aWidget->windowTitle() == aName ) {
          if( myViewsList->item(i)->checkState() == Qt::Checked )
            aListChecked.append( aWidget );
          else if( myViewsList->item(i)->checkState() == Qt::Unchecked )
            aListUnchecked.append( aWidget );
        }
      }
    }

    if( myViewsNB == 2 )
    {
      if( aListChecked.size() != 2 )
        return;
      if( mySplitMode == 0)
        myWorkstack->Split( aListChecked.at(1),Qt::Horizontal, aSplitType );
      else if( mySplitMode == 1 )
        myWorkstack->Split( aListChecked.at(1),Qt::Vertical, aSplitType );
    }
    else if( myViewsNB == 3 ) {
      if( aListChecked.size() != 3 )
        return;
      mySplitMode = mySplitMode - 2;
      myWorkstack->Split( aListChecked.at( Split3Map[mySplitMode][0] ),
                          Qt::Orientation( Split3Map[mySplitMode][1] ),
                          aSplitType );
      if( Split3Map[mySplitMode][3] >= 0 )
        myWorkstack->move( aListChecked.at( Split3Map[mySplitMode][2] ),
                           aListChecked.at( Split3Map[mySplitMode][3] ),
                           false );

      myWorkstack->Split( aListChecked.at( Split3Map[mySplitMode][2] ),
                          Qt::Orientation( Split3Map[mySplitMode][4] ),
                          aSplitType );
    }
    else if( myViewsNB == 4 ) {
      if( aListChecked.size() != 4 )
        return;
      mySplitMode = mySplitMode - 8;
      myWorkstack->Split( aListChecked.at( Split4Map[mySplitMode][0] ),
                          Qt::Orientation( Split4Map[mySplitMode][1] ),
                          aSplitType );
      if( Split4Map[mySplitMode][3] >= 0 )
        myWorkstack->move( aListChecked.at( Split4Map[mySplitMode][2] ),
	                         aListChecked.at( Split4Map[mySplitMode][3] ),
                           false );

      myWorkstack->Split( aListChecked.at( Split4Map[mySplitMode][2] ),
                          Qt::Orientation( Split4Map[mySplitMode][4] ),
                          aSplitType );
      if( Split4Map[mySplitMode][6] >= 0 )
        myWorkstack->move( aListChecked.at( Split4Map[mySplitMode][5] ),
                           aListChecked.at( Split4Map[mySplitMode][6] ),
                           false );

      myWorkstack->Split( aListChecked.at( Split4Map[mySplitMode][5] ),
                          Qt::Orientation( Split4Map[mySplitMode][7] ),
                          aSplitType );
    }
    for( int i = 0; i < aListUnchecked.count(); i++ ) {
      if( myIsCloseViews )
        aListUnchecked.at(i)->close();
      else
        myWorkstack->move( aListUnchecked.at(i),
                           aListChecked.at( myViewsNB - 1 ),
                           false );
    }
  }
  accept();
}
