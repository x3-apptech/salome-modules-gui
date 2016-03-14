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

# include "ToolsGUI_RegWidget.h"

#include <QApplication>
#include <QInputDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <Qtx.h>

#include <SALOME_NamingService.hxx>
#include <Utils_SINGLETON.hxx>
#include <Utils_CommException.hxx>

#define MARGIN_SIZE      11
#define SPACING_SIZE      6
#define MIN_SPIN_WIDTH  100 

#define BOLD( text ) QString( "<b>%1</b>" ).arg( QString( text ) )

static const char* const time_data[] = { 
"16 16 6 1",
". c None",
"b c None",
"# c #000000",
"a c #4c4c4c",
"d c #878787",
"c c #ffffff",
".....#####ab....",
"...##cc#dc##ab..",
"..#ccc###dcc#ab.",
".#ccccc#dcccc#a.",
".#ccccc#dcccc#ab",
"#cccccc#dccccc#a",
"#cccccc#dccc#c#a",
"#c##ccc########a",
"#ccddcccdddd#d#a",
"#cccccccccccdc#a",
".#ccccccccccc#aa",
".#ccccc#ccccc#a.",
"..#cccc#dccc#aa.",
"...##cccdc##aa..",
".....#####aaa...",
"......aaaaa....."};

static const char* const close_data[] = { 
"16 16 6 1",
"d c None",
". c None",
"# c #000000",
"a c #4c4c4c",
"c c #5b5b5b",
"b c #ffffff",
".....#####a.....",
"...##bbbbb##a...",
"..#bbbbbbbbb#a..",
".#b#cbbbbbb#d#a.",
".#bc#cbbbb#cb#a.",
"#bbbc#cbb#cbbb#a",
"#bbbbc#c#cbbbb#a",
"#bbbbbc#cbbbbb#a",
"#bbbbc#c#cbbbb#a",
"#bbb#ccbb##bbb#a",
".#b#c#bbbbc#b#aa",
".#bc#bbbbbb#c#a.",
"..#bbbbbbbbb#aa.",
"...##bbbbb##aa..",
".....#####aaa...",
"......aaaaa....."};

static const char* const refresh_data[] = { 
"16 16 5 1",
". c None",
"# c #000000",
"a c #4c4c4c",
"c c #878787",
"b c #ffffff",
".....#####a.....",
"...##bb#bb##a...",
"..#bbbbc#bbb#a..",
".#bbbb####cbb#a.",
".#bbb#cc#cbbb#a.",
"#bbb#cb#cbbbbb#a",
"#bb#cbbbbbb#bb#a",
"#bb#cbbbbbb#cb#a",
"#bb#cbbbbbb#cb#a",
"#bbbbbb#bb#cbb#a",
".#bbbb#cb#cbb#aa",
".#bbb####cbbb#a.",
"..#bbb#cbbbb#aa.",
"...##bb#cb##aa..",
".....#####aaa...",
"......aaaaa....."};

/*!
  \brief Get access to the Registry service
  \internal
  \param orb CORBA ORB reference
  \return reference to the Registry service
*/
static Registry::Components_ptr GetRegistry( CORBA::ORB_ptr orb )
{
  static const char* registryName = "Registry";
  Registry::Components_var aRegistry;
  
  SALOME_NamingService& naming = *SINGLETON_<SALOME_NamingService>::Instance();
  naming.init_orb( orb );
  
  CORBA::Object_var object = 0;
  try
  {
    object = naming.Resolve( registryName );
    if ( CORBA::is_nil( object ) ) throw CommException( "Unable to find the Registry service" );
    aRegistry = Registry::Components::_narrow( object );
  }
  catch( const CORBA::Exception& ex )
  {
    MESSAGE( "Error: can't access Registry server" );
  }

  return aRegistry._retn();
}

/*!
  \class ToolsGUI_RegWidget
  \brief SALOME Registry tool window.
*/

//! The only instance of Registry window
ToolsGUI_RegWidget* ToolsGUI_RegWidget::myRegWidgetPtr = 0;

/*!
  \brief Create/get the only instance of the Registry window.
  \param orb CORBA ORB reference
  \param parent parent widget
*/
ToolsGUI_RegWidget* ToolsGUI_RegWidget::GetRegWidget( CORBA::ORB_ptr orb,
                                                      QWidget* parent )
{
  if ( !myRegWidgetPtr ) 
    myRegWidgetPtr = new ToolsGUI_RegWidget( orb, parent );
  return myRegWidgetPtr;
}

/*!
  \brief Constructor
  \param orb CORBA ORB reference
  \param parent parent widget
*/
ToolsGUI_RegWidget::ToolsGUI_RegWidget( CORBA::ORB_ptr orb, QWidget* parent ) 
: QMainWindow( parent )
{
  myOrb = CORBA::ORB::_duplicate( orb );

  // set window attributes
  setAttribute( Qt::WA_DeleteOnClose );
  setWindowTitle( tr( "TLT_REGISTRY" ) );

  // show status bar
  statusBar();

  // get main menu bar
  QMenuBar* mainMenu = menuBar();
  
  // add 'Actions' menu
  QMenu* m = mainMenu->addMenu( tr( "MNU_ACTIONS" ) );
  // add 'Actions' toolbar
  QToolBar* tb = addToolBar( tr( "TB_ACTIONS" ) );

  // create actions
  myActions[Refresh] = new QAction( QPixmap( refresh_data ), tr( "MEN_REFRESH" ), this );
  myActions[Refresh]->setStatusTip( tr( "STB_REFRESH" ) );
  connect( myActions[Refresh], SIGNAL( triggered() ), this, SLOT( refresh() ) );
  m->addAction( myActions[Refresh] );
  tb->addAction( myActions[Refresh] );
  
  myActions[Interval] = new QAction( QPixmap( time_data ), tr( "MEN_INTERVAL" ), this );
  myActions[Interval]->setStatusTip( tr( "STB_INTERVAL" ) );
  connect( myActions[Interval], SIGNAL( triggered() ), this, SLOT( refreshInterval() ) );
  m->addAction( myActions[Interval] );
  tb->addAction( myActions[Interval] );
  
  m->addSeparator();
  tb->addSeparator();

  myActions[Close] = new QAction( QPixmap( close_data ), tr( "MEN_CLOSE" ), this );
  myActions[Close]->setStatusTip( tr( "STB_CLOSE" ) );
  connect( myActions[Close], SIGNAL( triggered() ), this, SLOT( close() ) );
  m->addAction( myActions[Close] );
  tb->addAction( myActions[Close] );

  // central widget
  setCentralWidget( new QWidget( this ) );

  // Create tab widget: SALOME serives list (running and history)
  QTabWidget* tabWidget = new QTabWidget( centralWidget() );

  myViews[Clients]   = new QTreeWidget( tabWidget );
  myViews[Clients]->setColumnCount( 6 );
  myViews[Clients]->setAllColumnsShowFocus( true );
  myViews[Clients]->setRootIsDecorated( false );
  QStringList runningLabels;
  runningLabels << tr( "HDR_COMPONENT" ) << tr( "HDR_PID" )     << tr( "HDR_USERNAME" )
		<< tr( "HDR_HOSTNAME" )  << tr( "HDR_STARTED" ) << tr( "HDR_HELLO" );
  myViews[Clients]->setHeaderLabels( runningLabels );
  tabWidget->addTab( myViews[Clients], tr( "TAB_RUNNING" ) );

  myViews[History]   = new QTreeWidget( tabWidget );
  myViews[History]->setColumnCount(6);
  myViews[History]->setAllColumnsShowFocus( true );
  myViews[History]->setRootIsDecorated( false );
  QStringList historyLabels;
  historyLabels << tr( "HDR_COMPONENT" ) << tr( "HDR_PID" )     << tr( "HDR_USERNAME" )
		<< tr( "HDR_HOSTNAME" )  << tr( "HDR_STARTED" ) << tr( "HDR_FINISHED" );
  myViews[History]->setHeaderLabels( historyLabels );
  tabWidget->addTab( myViews[History], tr( "TAB_HISTORY" ) );

  connect( tabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( refresh() ) );
  connect( myViews[Clients], SIGNAL( itemActivated( QTreeWidgetItem*, int ) ), 
	   this,             SLOT( showDetails( QTreeWidgetItem*, int ) ) );
  connect( myViews[History], SIGNAL( itemActivated( QTreeWidgetItem*, int ) ),
	   this,             SLOT( showDetails( QTreeWidgetItem*, int ) ) );

  // create information window
  myDetails = new QTextEdit( centralWidget() );
  myDetails->setReadOnly( true );
  myDetails->setMinimumHeight( 100 );

  // layout widgets
  QVBoxLayout* topLayout = new QVBoxLayout( centralWidget() );
  topLayout->setMargin( 0 );
  topLayout->setSpacing( SPACING_SIZE );
  topLayout->addWidget( tabWidget );
  topLayout->addWidget( myDetails );
  
  // install auto-update timer
  myTimer = new QTimer( this );
  connect( myTimer, SIGNAL( timeout() ), this, SLOT( refresh() ) );
  myTimer->start( 10000 ); // 10 seconds by default
  
  // resize window and set its position
  resize( 800, 400 );
  Qtx::alignWidget( (QWidget*)this, (QWidget*)QApplication::desktop(), Qtx::AlignCenter );

  // refresh registry information
  refresh();
}

/*!
  \brief Destructor
*/
ToolsGUI_RegWidget::~ToolsGUI_RegWidget()
{
  myRegWidgetPtr = 0;
};

/*!
  \brief Get description text, containing information about the service.
  \param c_info client info data
  \return formatted client description
*/
QString ToolsGUI_RegWidget::getDetails( const Registry::Infos& c_info )
{
  QString a;
  if ( c_info.name != NULL ) {
    time_t aStarted( c_info.tc_start );
    time_t aLastPing( c_info.tc_hello );
    time_t aFinished( c_info.tc_end );

    a.append( BOLD( tr( "INFO_SERVICE" ).arg( QString( c_info.name ) ) ) );
    a.append( "<br>" );
    a.append( "<br>" );
    a.append( tr( "INFO_PROCESS" ).arg( BOLD( QString::number( c_info.pid ) ),
					BOLD( c_info.machine ),
					BOLD( c_info.adip ),
					BOLD( c_info.pwname ),
					BOLD( QString::number( c_info.uid ) ),
					BOLD( c_info.cdir ) ) );
    a.append( "<br>" );
    a.append( "<br>" );
    a.append( tr( "INFO_STARTED" ).arg( BOLD( QString( ctime( &aStarted ) ).trimmed() ) ) );
    a.append( "<br>" );
    if ( c_info.tc_hello != 0 ) {
      a.append( tr( "INFO_LAST_PING" ).arg( BOLD( QString( ctime( &aLastPing ) ).trimmed() ) ) );
      a.append( "<br>" );
    }
    if ( c_info.tc_end - c_info.difftime != 0 ) {
      a.append( tr( "INFO_FINISHED" ).arg( BOLD( QString( ctime( &aFinished ) ).trimmed() ) ) );
      a.append( "<br>" );
    }
    else {
      a.append( tr( "INFO_RUNNING" ) );
      a.append( "<br>" );
    }
    if ( c_info.difftime != 0 ) {
      a.append( tr( "INFO_TIME_DIFF" ).arg( BOLD( c_info.machine ), BOLD( QString::number( c_info.difftime ) ) ) ); 
      a.append( "<br>" );
    }
    a.append( "</code>" ); // ASV: 28.07.06 : added <code> tags to make the text font be 
                           // fixed width (looks much better on Windows)
  }
  return a;
}

/*!
  \brief Refresh registry information
*/
void ToolsGUI_RegWidget::refresh()
{
  try {
    // get reference to the Registry service
    Registry::Components_var aRegistry = GetRegistry( myOrb );
    if ( aRegistry->_is_nil() ) {
      myActions[Interval]->setDisabled( true );
      myActions[Refresh]->setDisabled( true );
      myTimer->stop();
      MESSAGE( "Sorry, no more Registry Server" );
      statusBar()->showMessage( tr( "ERR_NO_REGISTRY" ) );
      return;
    }

    myData[Clients] = aRegistry->getall();
    myData[History] = aRegistry->history();

    // update current services list
    myViews[Clients]->clear();
    for ( int i = 0; i < myData[Clients]->length(); i++ ) {
      const Registry::Infos& c_info = (*myData[Clients])[i];
      time_t aStarted  = time_t( c_info.tc_start );
      time_t aLastPing = time_t( c_info.tc_hello );
      QStringList aData;
      aData << QString( c_info.name )
	    << QString::number( c_info.pid )
	    << QString( c_info.pwname )
	    << QString( c_info.machine )
	    << QString( ctime( &aStarted ) ).trimmed()
	    << QString( ctime( &aLastPing ) ).trimmed();
      myViews[Clients]->addTopLevelItem( new QTreeWidgetItem( aData ) );
    }

    // update history
    myViews[History]->clear();
    for ( int i = 0; i < myData[History]->length(); i++ ) {       
      const Registry::Infos& c_info = (*myData[History])[i];
      time_t aStarted  = time_t( c_info.tc_start );
      time_t aFinished = time_t( c_info.tc_end );
      QStringList aData;
      aData << QString( c_info.name )
	    << QString::number( c_info.pid )
	    << QString( c_info.pwname )
	    << QString( c_info.machine )
	    << QString( ctime( &aStarted ) ).trimmed()
	    << QString( ctime( &aFinished ) ).trimmed();
      myViews[History]->addTopLevelItem( new QTreeWidgetItem( aData ) );
    }
  }
  catch( ... ) {
    myActions[Interval]->setDisabled( true );
    myActions[Refresh]->setDisabled( true );
    myTimer->stop();
    MESSAGE( "Sorry, no more Registry Server" );
    statusBar()->showMessage( tr( "ERR_NO_REGISTRY" ) );
  }
}

/*!
  \brief Called when \c Interval button is clicked (open dialog box to 
  change refresh interval).
*/
void ToolsGUI_RegWidget::refreshInterval()
{
  bool ok;
  int sec = QInputDialog::getInt( this, tr( "TLT_REFRESH_INTERVAL" ), tr( "LAB_REFRESH_INTERVAL" ), 
				  myTimer->interval() / 1000, 1, 24 * 60 * 60, 1, &ok );
  if ( ok )
    myTimer->start( sec * 1000 );
}

/*!
  \brief Show details about the chosen service.
  \param item item activated by the user
  \param column column index
*/
void ToolsGUI_RegWidget::showDetails( QTreeWidgetItem* item, int column )
{
  if ( !item || column < 0 ) return;
  int i = sender() == myViews[Clients] ? Clients : History;
  int idx  = myViews[i]->indexOfTopLevelItem( item );
  int size = myData[i]->length();
  if ( idx < 0 || idx > size-1 ) return;
  const Registry::Infos& c_info = (*myData[i])[idx];
  myDetails->setText( getDetails( c_info ) );
}
