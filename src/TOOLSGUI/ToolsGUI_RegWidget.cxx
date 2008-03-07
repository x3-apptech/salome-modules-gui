//  SALOME RegistryDisplay : GUI for Registry server implementation
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : ToolsGUI_RegWidget.cxx
//  Author : Pascale NOYRET, EDF
//  Module : SALOME
//  $Header$

# include <qpushbutton.h>
# include <qlistview.h>
# include <qtabwidget.h> 
# include <qstatusbar.h>
# include <qtextview.h>
# include <qtimer.h>
# include <qinputdialog.h>
# include <qtoolbar.h>
# include <qdir.h>
# include <qtooltip.h>

# include "SALOME_NamingService.hxx"
# include "ServiceUnreachable.hxx"
# include "Utils_SINGLETON.hxx"
# include "Utils_CommException.hxx"

# include "utilities.h"
# include "OpUtil.hxx"

# include "ToolsGUI_RegWidget.h"
# include "ToolsGUI_HelpWindow.h"
# include "ToolsGUI_IntervalWindow.h"

# include "Qtx.h"

using namespace std;

typedef int PIXELS;
ToolsGUI_RegWidget* ToolsGUI_RegWidget::myRegWidgetPtr = 0;

QString addSlash( const QString& );
QString findFile( QString filename );

#define BOLD( text ) ( QString( "<b>" ) + QString( text ) + QString( "</b>" ) )

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
  Creates components list
*/

Registry::Components_var MakeRegistry( CORBA::ORB_var &orb )
{

  const char *registryName="Registry" ;
  
  SALOME_NamingService &naming = *SINGLETON_<SALOME_NamingService>::Instance() ;
  naming.init_orb( orb ) ;
  
  // Recuperation de la reference de l'objet
  CORBA::Object_var object = 0 ;
  try
  {
    SCRUTE(registryName) ;
    object = naming.Resolve( registryName ) ;
    if(CORBA::is_nil(object)) throw CommException( "unable to find the RegistryService" ) ;
  }
  catch( const ServiceUnreachable& ex )
  {
    MESSAGE( ex.what() )
    exit( EXIT_FAILURE ) ;
  }
  catch( const CORBA::Exception& )
  {
    exit( EXIT_FAILURE ) ;
  }

  // Specialisation de l'objet generique

  return Registry::Components::_narrow( object ) ;
}

/*!
  Only one global registry window should exist at the moment
  This method creates registry window if necessary and returns it
  [ static ] 
*/
ToolsGUI_RegWidget* ToolsGUI_RegWidget::GetRegWidget( CORBA::ORB_var &orb , QWidget *parent, const char *name )
{
  if ( !myRegWidgetPtr ) 
    myRegWidgetPtr = new ToolsGUI_RegWidget( orb, parent, name );
  return myRegWidgetPtr;
}

/*!
  Constructor  
*/
ToolsGUI_RegWidget::ToolsGUI_RegWidget(CORBA::ORB_var &orb, QWidget *parent, const char *name ) 
     : QMainWindow( parent, name, WType_TopLevel | WDestructiveClose ),
       _VarComponents( MakeRegistry(orb) ),
       _clients(0), _history(0), _parent( parent ),
       _tabWidget(0), _refresh(0), _interval(0),
       myInfoWindow(0), myHelpWindow(0), myIntervalWindow(0)
{
   QString aFile = findFile("default.png");
 /* char* dir = getenv( "CSF_ResourcesDefaults" );
  QString path( "" );
  if ( dir ) {
    QDir qDir( dir );
    path = qDir.filePath( "default.png" );
  }*/
  QPixmap pm ( aFile );
  if ( !pm.isNull() )
    setIcon( pm );

  // pixmap for buttons
  QPixmap image_refresh ( ( const char** ) refresh_data );
  QPixmap image_interval( ( const char** ) time_data );
  QPixmap image_close   ( ( const char** ) close_data );

  // Buttons definition
  QToolBar* topbar = new QToolBar( tr("Toolbar"), this );
  setDockEnabled( topbar, DockTornOff, false );
  setDockMenuEnabled( false );

  _refresh = new QPushButton( tr( "Refresh" ), topbar );
  _refresh->setIconSet( image_refresh );
  _refresh->setFocusPolicy( NoFocus );
  connect( _refresh, SIGNAL( clicked() ), this, SLOT( slotListeSelect() ) );
  QToolTip::add( _refresh, "", toolTipGroup(), tr("Immediately updates list of components") );
  
  /* PAL5540 - this button is needless
  QPushButton* help = new QPushButton( tr( "Help" ), topbar );
  connect( help, SIGNAL( clicked() ), this, SLOT( slotHelp() ) );
  QToolTip::add( help, "", toolTipGroup(), tr("Opens Help window") );
  */
  
  _interval = new QPushButton( tr( "Interval" ), topbar );
  _interval->setIconSet( image_interval );
  _interval->setFocusPolicy( NoFocus );
  connect( _interval, SIGNAL( clicked() ), this, SLOT( slotSelectRefresh() ) );
  QToolTip::add( _interval, "", toolTipGroup(), tr("Changes refresh interval") );
  
  topbar->addSeparator();
  _close = new QPushButton( tr("Close"), topbar );
  _close->setIconSet( image_close );
  _close->setFocusPolicy( NoFocus );
  connect( _close, SIGNAL( clicked() ), this, SLOT( close() ) );
  QToolTip::add( _close, "", toolTipGroup(), tr("Closes Registry window") );
  
  // Display area and associated slots definition
  _tabWidget = new QTabWidget( this );
  _clients   = new QListView( _tabWidget );
  SetListe();
  _history   = new QListView( _tabWidget );
  SetListeHistory();
  
  _tabWidget->addTab( _clients, tr( "Running" ) );
  _tabWidget->addTab( _history, tr( "History" ) );
  connect( _tabWidget, SIGNAL( currentChanged( QWidget* )), this, SLOT( slotListeSelect() ) );
  connect( _clients,   SIGNAL( clicked( QListViewItem* ) ),        this, SLOT( slotClientChanged( QListViewItem* ) ) );
  connect( _history,   SIGNAL( clicked( QListViewItem* ) ),        this, SLOT( slotHistoryChanged( QListViewItem* ) ) );
  setCentralWidget( _tabWidget );
  
  // Timer definition (used to automaticaly refresh the display area)
  _counter = new QTimer( this );
  connect( _counter, SIGNAL( timeout() ), this, SLOT( slotListeSelect() ) );
  myRefreshInterval = 10;
  _counter->start( myRefreshInterval * 1000 );
  
  PIXELS xpos    = 160 ;
  PIXELS ypos    = 100 ;
  PIXELS largeur = 800 ;
  PIXELS hauteur = 350 ;
  setGeometry( xpos, ypos, largeur, hauteur ) ;
  setCaption( name ) ;
  statusBar()->message("    ");
}

/*!
  Destructor
*/
ToolsGUI_RegWidget::~ToolsGUI_RegWidget()
{
  MESSAGE("Debut du Destructeur");
  _counter->stop();
  myRegWidgetPtr = 0;
};

/*!
  Event filter
*/
bool ToolsGUI_RegWidget::eventFilter( QObject* object, QEvent* event )
{
  if ( object ) {
    if ( object == myHelpWindow && event->type() == QEvent::Close ) {
      myHelpWindow = 0;
    }
    else if ( object == myInfoWindow && event->type() == QEvent::Close ) {
      myInfoWindow = 0;
    }
    else if ( object == myIntervalWindow && event->type() == QEvent::Close ) {
      myIntervalWindow = 0;
    }
    else if ( object == _clients && event->type() == QEvent::KeyPress ) {
      QKeyEvent* ke = (QKeyEvent*)event;
      if ( ke->key() == Key_Enter || ke->key() == Key_Return ) {
	slotClientChanged( _clients->currentItem() );
      }
    }
    else if ( object == _history && event->type() == QEvent::KeyPress ) {
      QKeyEvent* ke = (QKeyEvent*)event;
      if ( ke->key() == Key_Enter || ke->key() == Key_Return ) {
	slotHistoryChanged( _history->currentItem() );
      }
    }
  }
  return QMainWindow::eventFilter( object, event );
}

/*!
  Searches item in the list and returns it's index or -1 if not found
*/
int ToolsGUI_RegWidget::numitem(const QString &name, const QString &pid, const QString &machine,const Registry::AllInfos *listclient)
{
  BEGIN_OF("numitem");
  for (CORBA::ULong i=0; i<listclient->length(); i++)
    {       
      const Registry::Infos & c_info=(*listclient)[i];
      ASSERT( c_info.name!=NULL);
      QString b;
      b.setNum(int(c_info.pid));
      if ( (name.compare(QString(c_info.name)) == 0) && 
	  (machine.compare(QString(c_info.machine)) == 0) && 
	  (pid.compare(b) == 0) )
	{
	  END_OF("numitem");
	  return i;
	}
    }
  END_OF("numitem");
  return -1;
}

/*!
  Returns text, containing information about client [ static ]
*/
QString ToolsGUI_RegWidget::setlongText( const Registry::Infos &c_info)
{
  BEGIN_OF("setlongText");
  ASSERT( c_info.name != NULL );
  QString a = QString( "<hr><h2>" ) + tr( "Code" ) + QString( " : " );
  a.append( QString( c_info.name ) );
  a.append( "</h1><hr><br>" );
  a.append( "<code>" ); // ASV: 28.07.06 : added <code> tags to make the text font be 
                        // fixed width (looks much better on Windows)
  a.append( tr( "Process Id" ) + QString( " : " ) );
  a.append( BOLD( QString::number( int( c_info.pid ) ) ) );
  a.append( QString( " " ) + tr( "on machine" ) + QString( " " ) );
  a.append( BOLD( c_info.machine ) ); 
  a.append( QString( " " ) + tr( "ip address" ) + QString( " : " ) );
  a.append( BOLD( c_info.adip ) );
  a.append( "<br>" );
  
  a.append( tr( "launched by user" ) + QString( " " ) );
  a.append( BOLD( c_info.pwname ) );
  a.append( QString( " ( " ) + tr( "pid" ) + QString( " : " ) );
  a.append( BOLD( QString::number( int( c_info.uid ) ) ) );
  a.append( QString( " )<br> " ) + tr( "in directory" ) + QString( " " ));
  a.append( BOLD( c_info.cdir ) );

  time_t aTime;
  a.append( QString( "<br>" ) + tr( "begins" ) + QString( " " ) );
  aTime = time_t(c_info.tc_start);
  char * t1 = (char * )duplicate(ctime(&aTime));
  t1 [strlen(t1) -1 ] = ' ';
  a.append( BOLD( t1 ) ); 
  delete [] t1;
  a.append( "<br>" );
  
  if (c_info.tc_hello != 0 )
    {
      aTime = time_t(c_info.tc_hello);
      char * t2 = (char * )duplicate(ctime(&aTime));
      t2 [strlen(t2) -1 ] = ' ';
      a.append( tr( "last signal" ) + QString(" : ") ); 
      a.append( BOLD( t2 ) ); 
      a.append( "<br>" );
      delete [] t2;
    }
  if ((c_info.tc_end - c_info.difftime) != 0)
    {
      aTime = time_t(c_info.tc_end);
      char * t3 = (char * )duplicate(ctime(&aTime));
      t3 [strlen(t3) -1 ] = ' ';
      a.append( tr( "ends" ) + QString( " " ) ); 
      a.append( BOLD( t3 ) ); 
      a.append( "<br>" );
      delete [] t3;
    }
  else
    {
      a.append( tr( "still running" ) + QString( "<br>" ) );
    }
  
  SCRUTE(c_info.difftime);
  if (c_info.difftime!= 0)
    {
      a.append( QString( "(" ) + tr( "Time on" ) + QString( " " ) ); 
      a.append( BOLD( c_info.machine ) ); 
      a.append( QString( " " ) + tr( "differs from server's time. The difference is" ) + QString( " " )); 
      a.append( BOLD( QString::number( int( c_info.difftime ) ) ) );
      a.append( QString( " " ) + tr( "seconds" ) + QString( ")<br>" ) );
    }
  a.append( "</code>" ); // ASV: 28.07.06 : added <code> tags to make the text font be 
                         // fixed width (looks much better on Windows)
  END_OF("setlongText");  
  return a;
  
}

/*!
  Close event
*/
void ToolsGUI_RegWidget::closeEvent( QCloseEvent *e)
{
  BEGIN_OF("closeEvent");
  if ( myInfoWindow )
    myInfoWindow->close();
  if ( myHelpWindow )
    myHelpWindow->close();
  if (myIntervalWindow)
    myIntervalWindow->close();
  e->accept();
  END_OF("closeEvent");
};

/*!
  Setups Clients list
*/
void ToolsGUI_RegWidget::SetListe()
{
  BEGIN_OF("SetListe");
  _clients->installEventFilter( this );
  _clients->setAllColumnsShowFocus( true );
  _clients->addColumn( tr( "Component" ), -1);
  _clients->addColumn( tr( "PID" ), -1 );
  _clients->addColumn( tr( "User Name" ), -1 );
  _clients->addColumn( tr( "Machine" ), -1 );
  _clients->addColumn( tr( "begins" ), -1 );
  _clients->addColumn( tr( "hello" ) , -1 );
  _clients->setColumnAlignment( 1, Qt::AlignRight );
  END_OF("SetListe");
}

/*!
  Setups History list
*/
void ToolsGUI_RegWidget::SetListeHistory()
{
   BEGIN_OF("SetListeHistory")
  _history->installEventFilter( this );
  _history->setAllColumnsShowFocus( true );
  _history->addColumn( tr( "Component" ), -1);
  _history->addColumn( tr( "PID" ), -1 );
  _history->addColumn( tr( "User Name" ), -1 );
  _history->addColumn( tr( "Machine" ), -1 );
  _history->addColumn( tr( "begins" ), -1 );
  _history->addColumn( tr( "ends" ), -1 );
  _history->setColumnAlignment( 1, Qt::AlignRight );
   END_OF("SetListeHistory")
}

/*!
  Updates History list
*/
void ToolsGUI_RegWidget::InfoHistory()
{

  BEGIN_OF("InfoHistory")
    _history->clear();
  try
    {
      time_t aTime;
      _serverhistory = _VarComponents->history();
      for (CORBA::ULong i=0; i<_serverhistory->length(); i++)
	{       
	  const Registry::Infos & c_info=(*_serverhistory)[i];
	  ASSERT( c_info.name!=NULL);
	  QString a;
	  a.setNum(int(c_info.pid));
	  aTime = time_t(c_info.tc_start);
	  char * t1 = (char * )duplicate(ctime(&aTime));
	  t1 [strlen(t1) -1 ] = ' ';
	  aTime = time_t(c_info.tc_end);
	  char * t2 = (char * )duplicate(ctime(&aTime));
	  t2 [strlen(t2) -1 ] = ' ';
	  QListViewItem * item = new QListViewItem(_history, QString(c_info.name),\
						   a, QString(c_info.pwname), QString(c_info.machine), \
						   QString(t1), QString(t2));
	  item=0 ;
	  delete [] t1;
	  delete [] t2;
	  
	}
    }
  catch( ... )
    {
      _interval->setDisabled( TRUE ) ;
      _refresh->setDisabled( TRUE ) ;
      _counter->stop();
      MESSAGE("Sorry, No more Registry Server") ;
      statusBar()->message( tr( "Sorry, No more Registry Server" ) ) ;
    }
  END_OF("InfoHistory")
}

/*!
  Updates clients list
*/
void ToolsGUI_RegWidget::InfoReg()
{
  BEGIN_OF("InfoReg")
  _clients->clear();
  try
    {
      time_t aTime;
      _serverclients = _VarComponents->getall();
      for (CORBA::ULong i=0; i<_serverclients->length(); i++)
	{       
	  const Registry::Infos & c_info=(*_serverclients)[i];
	  ASSERT( c_info.name!=NULL);
	  QString a;
	  a.setNum(int(c_info.pid));
	  aTime = time_t(c_info.tc_start);
	  char * t1 = (char * )duplicate(ctime(&aTime));
	  t1 [strlen(t1) -1 ] = ' ';
	  aTime = time_t(c_info.tc_hello);
	  char * t2 = (char * )duplicate(ctime(&aTime));
	  t2 [strlen(t2) -1 ] = ' ';
	  QListViewItem * item = new QListViewItem(_clients, QString(c_info.name),\
						   a, QString(c_info.pwname), QString(c_info.machine), \
						   QString(t1), QString(t2));
	  item=0 ;
	  delete [] t1;
	  delete [] t2;
	  
	}
    }
  catch( ... )
    {
      _interval->setDisabled( TRUE ) ;
      _refresh->setDisabled( TRUE ) ;
      _counter->stop();
      MESSAGE("Sorry, No more Registry Server") ;
      statusBar()->message( tr( "Sorry, No more Registry Server" ) ) ;
    }
  END_OF("InfoReg")
}

/*!
  Called when <Refresh> button is clicked
*/
void ToolsGUI_RegWidget::slotListeSelect()
{
  try
    {
      ASSERT(_tabWidget->currentPage() != NULL);
      if (_tabWidget->currentPage () == _clients) InfoReg();
      else if (_tabWidget->currentPage () == _history) InfoHistory();
    }
  catch( ... )
    {
      MESSAGE("Sorry, No more Registry Server") ;
      statusBar()->message( tr( "Sorry, No more Registry Server" ) ) ;
    }
}

/*!
  Called when <Interval> button is clicked (changing refresh interval)
*/
void ToolsGUI_RegWidget::slotSelectRefresh()
{
  BEGIN_OF("slotSelectRefresh");
  myIntervalWindow = new ToolsGUI_IntervalWindow(this);
  myIntervalWindow->installEventFilter( this );
  myIntervalWindow->setValue(myRefreshInterval);
  myIntervalWindow->show();
  connect( myIntervalWindow->Cancel(), SIGNAL( clicked() ), myIntervalWindow, SLOT( close() ) );
  connect( myIntervalWindow->Ok(), SIGNAL( clicked() ), this, SLOT( slotIntervalOk() ) );
  END_OF("slotSelectRefresh");
}

/*!
  SLOT: called when IntervalWindow's OK button is clicked
*/
void ToolsGUI_RegWidget::slotIntervalOk()
{
  BEGIN_OF("slotIntervalOk");
  myRefreshInterval = myIntervalWindow->getValue();
  _counter->changeInterval( myRefreshInterval * 1000 );
  SCRUTE(myRefreshInterval);
  myIntervalWindow->close();
  END_OF("slotIntervalOk");
}
/*!
  Called when <Help> button is clicked
*/
void ToolsGUI_RegWidget::slotHelp()
{
  BEGIN_OF("slotHelp()");

  if ( !myHelpWindow ) {
    myHelpWindow  = new ToolsGUI_HelpWindow( this );
    myHelpWindow->installEventFilter( this );
  }
  myHelpWindow->show();
  myHelpWindow->raise();
  myHelpWindow->setActiveWindow();
  
  END_OF("slotHelp()") ;
}

/*!
  Called when user clicks on item in <Running> list
*/
void ToolsGUI_RegWidget::slotClientChanged( QListViewItem* item )
{
  BEGIN_OF("slotClientChanged()") ;

  if ( item <= 0)
    return;

  blockSignals( true ); // for sure that item will not be deleted when refreshing

  int numeroItem = numitem(item->text(0), item->text(1), item->text(3), _serverclients);
  SCRUTE(numeroItem) ;
  SCRUTE(item->text(1)) ;
  
  ASSERT(numeroItem>=0) ;
  ASSERT((size_t)numeroItem<_serverclients->length()) ;
  const Registry::Infos & c_info=(*_serverclients)[numeroItem];
  ASSERT( c_info.name!=NULL);
  
  if ( !myInfoWindow ) {
    myInfoWindow  = new ToolsGUI_InfoWindow( this );
    myInfoWindow->installEventFilter( this );
  }
  QString a = tr( "More about" ) + QString( " " ) + QString( c_info.name );
  myInfoWindow->setCaption(a);
  myInfoWindow->setText( ToolsGUI_RegWidget::setlongText( c_info) );
  myInfoWindow->show();
  myInfoWindow->raise();
  myInfoWindow->setActiveWindow();

  blockSignals( false ); // enabling signals again

  END_OF("slotClientChanged()") ;
  return ;
}

/*!
  Called when user clicks on item in <History> list
*/
void ToolsGUI_RegWidget::slotHistoryChanged( QListViewItem* item )
{

  BEGIN_OF("slotHistoryChanged()") ;
  
  if ( item <= 0)
    return;

  blockSignals( true ); // for sure that item will not be deleted when refreshing

  int numeroItem = numitem(item->text(0), item->text(1), item->text(3), _serverhistory);
  
  SCRUTE(numeroItem) ;
  SCRUTE(item->text(1)) ;
  ASSERT(numeroItem>=0) ;
  ASSERT((size_t)numeroItem<_serverhistory->length()) ;
  const Registry::Infos & c_info=(*_serverhistory)[numeroItem];
  ASSERT( c_info.name!=NULL);
  
  if ( !myInfoWindow ) {
    myInfoWindow  = new ToolsGUI_InfoWindow( this );
    myInfoWindow->installEventFilter( this );
  }
  QString a = tr( "More about" ) + QString( " " ) + QString( c_info.name );
  myInfoWindow->setCaption(a);
  myInfoWindow->setText( ToolsGUI_RegWidget::setlongText( c_info ) );
  myInfoWindow->show();
  myInfoWindow->raise();
  myInfoWindow->setActiveWindow();

  blockSignals( false ); // enabling signals again

  END_OF("slotHistoryChanged()") ;
  return ;
}

/*!
  Constructor
*/
ToolsGUI_InfoWindow::ToolsGUI_InfoWindow( QWidget* parent, const char* name )
     : QMainWindow( parent, name, WType_TopLevel | WDestructiveClose )
{
  BEGIN_OF("InfoWindow");
  myTextView = new QTextView( this, "myTextView" );
  setCentralWidget( myTextView );
  setMinimumSize( 450, 250 );
  END_OF("InfoWindow");
}

/*!
  Sets text
*/
void ToolsGUI_InfoWindow::setText( const QString& text )
{
  myTextView->setText( text );
}

/*!
  Key press event
*/
void ToolsGUI_InfoWindow::keyPressEvent( QKeyEvent * e )
{
  QMainWindow::keyPressEvent( e );
  if ( e->key() == Key_Escape )
    close();
}

static const char* SEPARATOR    = ":";

QString findFile( QString filename )
{
  QString dir;
  char* cenv;
  
  // Try ${HOME}/.salome/resources directory
  cenv = getenv( "HOME" );
  if ( cenv ) {
    dir.sprintf( "%s", cenv );
    if ( !dir.isEmpty() ) {
      dir = addSlash(dir) ;
      dir = dir + ".salome" ;
      dir = addSlash(dir) ;
      dir = dir + "resources" ;
      dir = addSlash(dir) ;
      QFileInfo fileInfo( dir + filename );
      if ( fileInfo.isFile() && fileInfo.exists() )
	return fileInfo.filePath();
    }
  }
  // Try ${SALOME_SITE_DIR}/share/salome/resources directory
  cenv = getenv( "SALOME_SITE_DIR" );
  if ( cenv ) {
    dir.sprintf( "%s", cenv );
    if ( !dir.isEmpty() ) {
      dir = addSlash(dir) ;
      dir = dir + "share" ;
      dir = addSlash(dir) ;
      cenv = getenv("SALOME_SITE_NAME");
      if (cenv)  dir = dir + cenv;
      else       dir = dir + "salome" ;
      dir = addSlash(dir) ;
      dir = dir + "resources" ;
      dir = addSlash(dir) ;
      QFileInfo fileInfo( dir + filename );
      if ( fileInfo.isFile() && fileInfo.exists() )
	return fileInfo.filePath();
    }
  }
  // Try ${KERNEL_ROOT_DIR}/share/salome/resources directory
  cenv = getenv( "KERNEL_ROOT_DIR" );
  if ( cenv ) {
    dir.sprintf( "%s", cenv );
    if ( !dir.isEmpty() ) {
      dir = addSlash(dir) ;
      dir = dir + "share" ;
      dir = addSlash(dir) ;
      dir = dir + "salome" ;
      dir = addSlash(dir) ;
      dir = dir + "resources" ;
      dir = addSlash(dir) ;
      dir = dir + "kernel" ;
      dir = addSlash(dir) ;
      QFileInfo fileInfo( dir + filename );
      if ( fileInfo.isFile() && fileInfo.exists() )
	return fileInfo.filePath();
    }
  }

  //SRN Added support for SALOMEGUI
  cenv = getenv( "SALOMEGUI_ROOT_DIR" );
  if ( cenv ) {
    dir.sprintf( "%s", cenv );
    if ( !dir.isEmpty() ) {
      dir = addSlash(dir) ;
      dir = dir + "share" ;
      dir = addSlash(dir) ;
      dir = dir + "salome" ;
      dir = addSlash(dir) ;
      dir = dir + "resources" ;
      dir = addSlash(dir) ;
      QFileInfo fileInfo( dir + filename );
      if ( fileInfo.isFile() && fileInfo.exists() )
	return fileInfo.filePath();
    }
  }

  // Try CSF_SaloameResources env.var directory ( or directory list )
  cenv = getenv( "CSF_SalomeResources" );
  if ( cenv ) {
  dir.sprintf( "%s", cenv );
  if ( !dir.isEmpty() )
  {
    QStringList dirList = QStringList::split( SEPARATOR, dir, false ); // skip empty entries
    for ( int i = 0; i < (int)dirList.count(); i++ )
    {
	    QFileInfo fileInfo( addSlash( dirList[ i ] ) + filename );
	    if ( fileInfo.isFile() && fileInfo.exists() )
	      return fileInfo.filePath();
      }
    }
  }
  return filename;
}

QString addSlash( const QString& path )
{
  return Qtx::addSlash( path );
}
