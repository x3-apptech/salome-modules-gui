//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME RegistryDisplay : GUI for Registry server implementation
//  File   : ToolsGUI_RegWidget.cxx
//  Author : Pascale NOYRET, EDF
//
# include "ToolsGUI_RegWidget.h"

# include <QAction>
# include <QDialog>
# include <QSpinBox>
# include <QPushButton>
# include <QTreeWidget>
# include <QTabWidget> 
# include <QStatusBar>
# include <QTextEdit>
# include <QTextStream>
# include <QTimer>
# include <QToolBar>
# include <QEvent>
# include <QKeyEvent>
# include <QCloseEvent>
# include <QFileInfo>
# include <QGridLayout>
# include <QLabel>
# include <QGroupBox>

# include <SALOME_NamingService.hxx>
# include <ServiceUnreachable.hxx>
# include <Utils_SINGLETON.hxx>
# include <Utils_CommException.hxx>

# include <utilities.h>
# include <OpUtil.hxx>

# include <Qtx.h>

typedef int PIXELS;

#define MARGIN_SIZE      11
#define SPACING_SIZE      6
#define MIN_SPIN_WIDTH  100 

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
  \brief Create components list.
  \internal
  \param orb CORBA ORB reference
  \return list of registered components
*/
static Registry::Components_var MakeRegistry( CORBA::ORB_var& orb )
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
  \class ToolsGUI_RegWidget::InfoWindow
  \brief Information window.
  \internal
*/

class ToolsGUI_RegWidget::InfoWindow : public QMainWindow
{
public:
  InfoWindow( QWidget* parent );

  void setText( const QString& text );

protected:
  void keyPressEvent( QKeyEvent * e );

private:
  QTextEdit* myTextView;
};

/*!
  \brief Constructor.
  \internal
  \param parent parent widget
*/
ToolsGUI_RegWidget::InfoWindow::InfoWindow( QWidget* parent )
: QMainWindow( parent )
{
  setAttribute( Qt::WA_DeleteOnClose );

  myTextView = new QTextEdit( this );
  myTextView->setReadOnly( true );
  setCentralWidget( myTextView );
  setMinimumSize( 450, 250 );
}

/*!
  \brief Set text to the information window.
  \internal
  \param text ionfo text
*/
void ToolsGUI_RegWidget::InfoWindow::setText( const QString& text )
{
  myTextView->setText( text );
}

/*!
  \brief Key press event handler. Closeswindow on \c Escape key pressing.
  \internal
  \param e key press event
*/
void ToolsGUI_RegWidget::InfoWindow::keyPressEvent( QKeyEvent * e )
{
  QMainWindow::keyPressEvent( e );
  if ( e->key() == Qt::Key_Escape )
    close();
}

/*!
  \class ToolsGUI_RegWidget::HelpWindow
  \brief Help window.
  \internal
*/

class ToolsGUI_RegWidget::HelpWindow : public QMainWindow
{
public:
  HelpWindow( QWidget* parent );
  ~HelpWindow();

  void setText( const QString& text );

private:
  QTextEdit* myTextView;
};

/*!
  \brief Constructor.
  \internal
  \param parent parent widget
*/
ToolsGUI_RegWidget::HelpWindow::HelpWindow( QWidget* parent ) 
: QMainWindow( parent )
{
  setAttribute( Qt::WA_DeleteOnClose );
  setWindowTitle( tr( "Help" ) );

  myTextView = new QTextEdit( this );
  myTextView->setReadOnly( true );
  QPalette pal = myTextView->palette();

  pal.setBrush( QPalette::Active, QPalette::Highlight,       QBrush( QColor( 0, 0, 128 ) ) );
  pal.setBrush( QPalette::Active, QPalette::HighlightedText, QBrush( Qt::white ) );
  pal.setBrush( QPalette::Active, QPalette::Base,            QBrush( QColor( 255,255,220 ) ) );
  pal.setBrush( QPalette::Active, QPalette::Text,            QBrush( Qt::black ) );

  pal.setBrush( QPalette::Inactive, QPalette::Highlight,       QBrush( QColor( 0, 0, 128 ) ) );
  pal.setBrush( QPalette::Inactive, QPalette::HighlightedText, QBrush( Qt::white ) );
  pal.setBrush( QPalette::Inactive, QPalette::Base,            QBrush( QColor( 255,255,220 ) ) );
  pal.setBrush( QPalette::Inactive, QPalette::Text,            QBrush( Qt::black ) );

  pal.setBrush( QPalette::Disabled, QPalette::Highlight,       QBrush( QColor( 0, 0, 128 ) ) );
  pal.setBrush( QPalette::Disabled, QPalette::HighlightedText, QBrush( Qt::white ) );
  pal.setBrush( QPalette::Disabled, QPalette::Base,            QBrush( QColor( 255,255,220 ) ) );
  pal.setBrush( QPalette::Disabled, QPalette::Text,            QBrush( Qt::black ) );

  myTextView->setPalette( pal );
  
  setCentralWidget( myTextView );
  setMinimumSize( 450, 250 );

  QFile f ( "tmp.txt" );
  if ( f.open( QIODevice::ReadOnly ) ) {
    QTextStream t( &f ); 
    while ( !t.atEnd() ) {
      myTextView->append( t.readLine() );
    }
  }
  f.close();
}

/*!
  \brief Destructor.
  \internal
*/
ToolsGUI_RegWidget::HelpWindow::~HelpWindow()
{
};

/*!
  \brief Set text to the help window.
  \internal
  \param text help text
*/
void ToolsGUI_RegWidget::HelpWindow::setText( const QString& text )
{
  myTextView->setText( text );
}

/*!
  \class ToolsGUI_RegWidget::IntervalWindow
  \brief Dialog box to enter time delay between registry window updates
  \internal
*/

class ToolsGUI_RegWidget::IntervalWindow : public QDialog
{
public:
  IntervalWindow( QWidget* parent );
  ~IntervalWindow();
  
  QPushButton* Ok();
  QPushButton* Cancel();

  int getValue();
  void setValue( int );

private:
  QSpinBox* mySpinBox;
  QPushButton* myButtonOk;
  QPushButton* myButtonCancel;
};

/*!
  \brief Constructor.
  \internal
  \param parent parent widget
*/
ToolsGUI_RegWidget::IntervalWindow::IntervalWindow ( QWidget* parent )
: QDialog( parent )
{
  setModal( true );
  setAttribute( Qt::WA_DeleteOnClose );

  setWindowTitle( tr( "Refresh Interval"  ) );
  setSizeGripEnabled( true );

  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setSpacing( SPACING_SIZE );
  topLayout->setMargin( MARGIN_SIZE );

  QGroupBox* intervalGrp = new QGroupBox( this );
  intervalGrp->setObjectName( "intervalGrp" );
  QGridLayout* intervalGrpLayout = new QGridLayout( intervalGrp );
  intervalGrpLayout->setAlignment( Qt::AlignTop );
  intervalGrpLayout->setSpacing( SPACING_SIZE );
  intervalGrpLayout->setMargin( MARGIN_SIZE  );  

  QHBoxLayout* aBtnLayout = new QHBoxLayout;
  aBtnLayout->setSpacing( SPACING_SIZE );
  aBtnLayout->setMargin( 0 );

  myButtonOk = new QPushButton( this );
  myButtonOk->setObjectName( "buttonOk" );
  myButtonOk->setText( tr( "BUT_OK"  ) );
  myButtonOk->setAutoDefault( TRUE );
  myButtonOk->setDefault( TRUE );
  
  myButtonCancel = new QPushButton( this );
  myButtonCancel->setObjectName( "buttonCancel" );
  myButtonCancel->setText( tr( "BUT_CANCEL"  ) );
  myButtonCancel->setAutoDefault( TRUE );

  QLabel* TextLabel = new QLabel( intervalGrp );
  TextLabel->setObjectName( "TextLabel" );
  TextLabel->setText( tr( "Please, enter a number of seconds:"  ) );

  mySpinBox = new QSpinBox( intervalGrp );
  mySpinBox->setMinimum( 1 );
  mySpinBox->setMaximum( 999999999 );
  mySpinBox->setSingleStep( 1 );
  mySpinBox->setObjectName( "SpinBox" );
  mySpinBox->setValue( 100 );
  mySpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  mySpinBox->setMinimumWidth(MIN_SPIN_WIDTH);

  intervalGrpLayout->addWidget(TextLabel, 0, 0);
  intervalGrpLayout->addWidget(mySpinBox, 0, 1);

  aBtnLayout->addWidget( myButtonOk );
  aBtnLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aBtnLayout->addWidget( myButtonCancel );

  topLayout->addWidget( intervalGrp, 0, 0 );
  topLayout->addLayout( aBtnLayout, 1, 0 ); 
}

/*!
  \brief Destructor
  \internal
*/
ToolsGUI_RegWidget::IntervalWindow::~IntervalWindow()
{
}

/*!
  \brief Set time interval value
  \internal
  \param size interval value
*/
void ToolsGUI_RegWidget::IntervalWindow::setValue( const int size )
{
  mySpinBox->setValue(size);
}

/*!
  \brief Get time interval value
  \internal
  \return interval value
*/
int ToolsGUI_RegWidget::IntervalWindow::getValue()
{
  return mySpinBox->value();
}

/*!
  \brief Get \c OK button
  \internal
  \return a pointer to \c OK button
*/
QPushButton* ToolsGUI_RegWidget::IntervalWindow::Ok()
{
  return myButtonOk;
}

/*!
  \brief Get \c Cancel button
  \internal
  \return a pointer to \c Cancel button
*/
QPushButton* ToolsGUI_RegWidget::IntervalWindow::Cancel()
{
  return myButtonCancel;
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
ToolsGUI_RegWidget* ToolsGUI_RegWidget::GetRegWidget( CORBA::ORB_var& orb,
						      QWidget* parent )
{
  if ( !myRegWidgetPtr ) 
    myRegWidgetPtr = new ToolsGUI_RegWidget( orb, parent );
  return myRegWidgetPtr;
}

/*!
  \brief This virtual function is reimplenented to disable popup menu on dock areas
  (instead of QMainWindow::setDockMenuEnabled( false ) method used in Qt3).
  \return always 0 to disable menu
*/
QMenu* ToolsGUI_RegWidget::createPopupMenu()
{
  QMenu* aPopup = 0;
  return aPopup;
}

/*!
  \brief Constructor
  \param orb CORBA ORB reference
  \param parent parent widget
*/
ToolsGUI_RegWidget::ToolsGUI_RegWidget( CORBA::ORB_var& orb, QWidget* parent ) 
: QMainWindow( parent, Qt::Window ),
  _VarComponents( MakeRegistry(orb) ),
  _clients( 0 ), 
  _history( 0 ), 
  _parent( parent ),
  _tabWidget( 0 ), 
  _refresh( 0 ), 
  _interval( 0 ),
  myInfoWindow( 0 ), 
  myHelpWindow( 0 ), 
  myIntervalWindow( 0 )
{
  setAttribute( Qt::WA_DeleteOnClose );

  if ( parent )
    setWindowIcon( parent->windowIcon() );

  // pixmap for buttons
  QPixmap image_refresh ( ( const char** ) refresh_data );
  QPixmap image_interval( ( const char** ) time_data );
  QPixmap image_close   ( ( const char** ) close_data );

  // Buttons definition
  QToolBar* topbar = new QToolBar( tr("Toolbar"), this );
  topbar->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
  addToolBar( Qt::TopToolBarArea, topbar );

  _refresh = topbar->addAction(image_refresh, tr( "Refresh" ), this, SLOT( slotListeSelect() ));
  _refresh->setToolTip( "" );
  _refresh->setStatusTip( tr("Immediately updates list of components") );
  
  /* PAL5540 - this button is needless
  QPushButton* help = new QPushButton( tr( "Help" ), topbar );
  connect( help, SIGNAL( clicked() ), this, SLOT( slotHelp() ) );
  QToolTip::add( help, "", toolTipGroup(), tr("Opens Help window") );
  */
  
  _interval = topbar->addAction(image_interval, tr( "Interval" ), this, SLOT( slotSelectRefresh() ));
  _interval->setToolTip( "" );
  _interval->setStatusTip( tr("Changes refresh interval") );
  
  topbar->addSeparator();

  _close = topbar->addAction( image_close, tr("Close"), this, SLOT( close() ));
  _close->setToolTip( "" );
  _close->setStatusTip( tr("Closes Registry window") );
  
  // Display area and associated slots definition
  _tabWidget = new QTabWidget( this );
  _clients   = new QTreeWidget( _tabWidget );
  SetListe();
  _history   = new QTreeWidget( _tabWidget );
  SetListeHistory();
  
  _tabWidget->addTab( _clients, tr( "Running" ) );
  _tabWidget->addTab( _history, tr( "History" ) );
  connect( _tabWidget, SIGNAL( currentChanged( QWidget* )), this, SLOT( slotListeSelect() ) );
  connect( _clients,   SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),        this, SLOT( slotClientChanged( QTreeWidgetItem* , int ) ) );
  connect( _history,   SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),        this, SLOT( slotHistoryChanged( QTreeWidgetItem* , int ) ) );
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
  setWindowTitle( tr( "Registry" ) ) ;
  statusBar()->showMessage("    ");

  slotListeSelect();
}

/*!
  \brief Destructor
*/
ToolsGUI_RegWidget::~ToolsGUI_RegWidget()
{
  _counter->stop();
  myRegWidgetPtr = 0;
};

/*!
  \brief Event filter
  \param object event receiver
  \param event event being processed
  \return \c true if event processing should be stopped
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
      if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return ) {
	slotClientChanged( _clients->currentItem(), 0 );
      }
    }
    else if ( object == _history && event->type() == QEvent::KeyPress ) {
      QKeyEvent* ke = (QKeyEvent*)event;
      if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return ) {
	slotHistoryChanged( _history->currentItem(), 0 );
      }
    }
  }
  return QMainWindow::eventFilter( object, event );
}

/*!
  \brief Search item in the list.
  \param name component name
  \param pid PID
  \param machine machine name
  \param listclient list of registry data
  \return item index or -1 if it is not found.
*/
int ToolsGUI_RegWidget::numitem( const QString& name, 
				 const QString& pid, 
				 const QString& machine,
				 const Registry::AllInfos* listclient )
{
  for (CORBA::ULong i=0; i<listclient->length(); i++) {       
    const Registry::Infos & c_info=(*listclient)[i];
    ASSERT( c_info.name!=NULL);
    QString b;
    b.setNum(int(c_info.pid));
    if ( (name.compare(QString(c_info.name)) == 0) && 
	 (machine.compare(QString(c_info.machine)) == 0) && 
	 (pid.compare(b) == 0) ) {
      return i;
    }
  }
  return -1;
}

/*!
  \brief Get description text, containing information about client.
  \param c_info client info data
  \return formatted client description
*/
QString ToolsGUI_RegWidget::setlongText( const Registry::Infos& c_info )
{
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
  
  if (c_info.tc_hello != 0 ) {
    aTime = time_t(c_info.tc_hello);
    char * t2 = (char * )duplicate(ctime(&aTime));
    t2 [strlen(t2) -1 ] = ' ';
    a.append( tr( "last signal" ) + QString(" : ") ); 
    a.append( BOLD( t2 ) ); 
    a.append( "<br>" );
    delete [] t2;
  }
  if ((c_info.tc_end - c_info.difftime) != 0) {
    aTime = time_t(c_info.tc_end);
    char * t3 = (char * )duplicate(ctime(&aTime));
    t3 [strlen(t3) -1 ] = ' ';
    a.append( tr( "ends" ) + QString( " " ) ); 
    a.append( BOLD( t3 ) ); 
    a.append( "<br>" );
    delete [] t3;
  }
  else {
    a.append( tr( "still running" ) + QString( "<br>" ) );
  }
  
  SCRUTE(c_info.difftime);
  if (c_info.difftime!= 0) {
    a.append( QString( "(" ) + tr( "Time on" ) + QString( " " ) ); 
    a.append( BOLD( c_info.machine ) ); 
    a.append( QString( " " ) + tr( "differs from server's time. The difference is" ) + QString( " " )); 
    a.append( BOLD( QString::number( int( c_info.difftime ) ) ) );
    a.append( QString( " " ) + tr( "seconds" ) + QString( ")<br>" ) );
  }
  a.append( "</code>" ); // ASV: 28.07.06 : added <code> tags to make the text font be 
                         // fixed width (looks much better on Windows)
  return a;
  
}

/*!
  \brief Close event handler.
  \param e close event
*/
void ToolsGUI_RegWidget::closeEvent( QCloseEvent* e )
{
  if ( myInfoWindow )
    myInfoWindow->close();
  if ( myHelpWindow )
    myHelpWindow->close();
  if (myIntervalWindow)
    myIntervalWindow->close();
  e->accept();
};

/*!
  \brief Setup clients list.
*/
void ToolsGUI_RegWidget::SetListe()
{
  _clients->installEventFilter( this );
  _clients->setColumnCount(6);
  _clients->setAllColumnsShowFocus( true );
  QStringList aLabels;
  aLabels << tr("Component") << tr("PID") << tr("User Name") << tr("Machine") << tr("begins") << tr("hello");
  _clients->setHeaderLabels( aLabels );
  //_clients->setColumnAlignment( 1, Qt::AlignRight );
}

/*!
  \brief Setup history list.
*/
void ToolsGUI_RegWidget::SetListeHistory()
{
  _history->installEventFilter( this );
   _history->setColumnCount(6);
  _history->setAllColumnsShowFocus( true );
  QStringList aLabels;
  aLabels << tr("Component") << tr("PID") << tr("User Name") << tr("Machine") << tr("begins") << tr("ends");
  _history->setHeaderLabels( aLabels );
  //_history->setColumnAlignment( 1, Qt::AlignRight );
}

/*!
  \brief Update history list
*/
void ToolsGUI_RegWidget::InfoHistory()
{

  _history->clear();
  try {
    time_t aTime;
    _serverhistory = _VarComponents->history();
    for (CORBA::ULong i=0; i<_serverhistory->length(); i++) {       
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
      QStringList anItem;
      anItem << QString(c_info.name) << a << QString(c_info.pwname) << QString(c_info.machine) << QString(t1) << QString(t2);
      QTreeWidgetItem * item = new QTreeWidgetItem(_history, anItem);
      item=0 ;
      delete [] t1;
      delete [] t2;
      
    }
  }
  catch( ... ) {
    _interval->setDisabled( TRUE ) ;
    _refresh->setDisabled( TRUE ) ;
    _counter->stop();
    MESSAGE("Sorry, No more Registry Server") ;
    statusBar()->showMessage( tr( "Sorry, No more Registry Server" ) ) ;
  }
}

/*!
  \brief Update clients list
*/
void ToolsGUI_RegWidget::InfoReg()
{
  _clients->clear();
  try {
    time_t aTime;
    _serverclients = _VarComponents->getall();
    for (CORBA::ULong i=0; i<_serverclients->length(); i++) {       
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
      QStringList anItem;
      anItem << QString(c_info.name) << a << QString(c_info.pwname) << QString(c_info.machine) << QString(t1) << QString(t2);
      QTreeWidgetItem * item = new QTreeWidgetItem(_clients, anItem);
      item=0 ;
      delete [] t1;
      delete [] t2;
      
    }
  }
  catch( ... ) {
    _interval->setDisabled( TRUE ) ;
    _refresh->setDisabled( TRUE ) ;
    _counter->stop();
    MESSAGE("Sorry, No more Registry Server") ;
    statusBar()->showMessage( tr( "Sorry, No more Registry Server" ) ) ;
  }
}

/*!
  \brief Called when \c Refresh button is clicked
*/
void ToolsGUI_RegWidget::slotListeSelect()
{
  try {
    ASSERT(_tabWidget->currentWidget() != NULL);
    if (_tabWidget->currentWidget () == _clients) InfoReg();
    else if (_tabWidget->currentWidget () == _history) InfoHistory();
  }
  catch( ... ) {
    MESSAGE("Sorry, No more Registry Server") ;
    statusBar()->showMessage( tr( "Sorry, No more Registry Server" ) ) ;
  }
}

/*!
  \brief Called when \c Interval button is clicked (open dialog box to 
  change refresh interval).
*/
void ToolsGUI_RegWidget::slotSelectRefresh()
{
  myIntervalWindow = new ToolsGUI_RegWidget::IntervalWindow(this);
  myIntervalWindow->installEventFilter( this );
  myIntervalWindow->setValue(myRefreshInterval);
  myIntervalWindow->show();
  connect( myIntervalWindow->Cancel(), SIGNAL( clicked() ), myIntervalWindow, SLOT( close() ) );
  connect( myIntervalWindow->Ok(), SIGNAL( clicked() ), this, SLOT( slotIntervalOk() ) );
}

/*!
  \brief Called when IntervalWindow's \c OK button is clicked
*/
void ToolsGUI_RegWidget::slotIntervalOk()
{
  myRefreshInterval = myIntervalWindow->getValue();
  _counter->start( myRefreshInterval * 1000 );
  SCRUTE(myRefreshInterval);
  myIntervalWindow->close();
}
/*!
  \brief Called when \c Help button is clicked
*/
void ToolsGUI_RegWidget::slotHelp()
{
  if ( !myHelpWindow ) {
    myHelpWindow  = new ToolsGUI_RegWidget::HelpWindow( this );
    myHelpWindow->installEventFilter( this );
  }
  myHelpWindow->show();
  myHelpWindow->raise();
  myHelpWindow->activateWindow();
}

/*!
  \brief Called when user clicks on item in \c Running list
  \param item item clicked by the user
*/
void ToolsGUI_RegWidget::slotClientChanged( QTreeWidgetItem* item, int col )
{
  if ( !item || col < 0 )
    return;

  blockSignals( true ); // for sure that item will not be deleted when refreshing

  int numeroItem = numitem(item->text(0), item->text(1), item->text(3), _serverclients);
  SCRUTE(numeroItem) ;
  SCRUTE(item->text(1).toLatin1().constData()) ;
  
  ASSERT(numeroItem>=0) ;
  ASSERT((size_t)numeroItem<_serverclients->length()) ;
  const Registry::Infos & c_info=(*_serverclients)[numeroItem];
  ASSERT( c_info.name!=NULL);
  
  if ( !myInfoWindow ) {
    myInfoWindow  = new ToolsGUI_RegWidget::InfoWindow( this );
    myInfoWindow->installEventFilter( this );
  }
  QString a = tr( "More about" ) + QString( " " ) + QString( c_info.name );
  myInfoWindow->setWindowTitle(a);
  myInfoWindow->setText( ToolsGUI_RegWidget::setlongText( c_info) );
  myInfoWindow->show();
  myInfoWindow->raise();
  myInfoWindow->activateWindow();

  blockSignals( false ); // enabling signals again
}

/*!
  \brief Called when user clicks on item in \c History list
  \param item item clicked by the user
*/
void ToolsGUI_RegWidget::slotHistoryChanged( QTreeWidgetItem* item, int col )
{
  if ( !item || col < 0 )
    return;

  blockSignals( true ); // for sure that item will not be deleted when refreshing

  int numeroItem = numitem(item->text(0), item->text(1), item->text(3), _serverhistory);
  
  SCRUTE(numeroItem) ;
  SCRUTE(item->text(1).toLatin1().constData()) ;
  ASSERT(numeroItem>=0) ;
  ASSERT((size_t)numeroItem<_serverhistory->length()) ;
  const Registry::Infos & c_info=(*_serverhistory)[numeroItem];
  ASSERT( c_info.name!=NULL);
  
  if ( !myInfoWindow ) {
    myInfoWindow  = new ToolsGUI_RegWidget::InfoWindow( this );
    myInfoWindow->installEventFilter( this );
  }
  QString a = tr( "More about" ) + QString( " " ) + QString( c_info.name );
  myInfoWindow->setWindowTitle(a);
  myInfoWindow->setText( ToolsGUI_RegWidget::setlongText( c_info ) );
  myInfoWindow->show();
  myInfoWindow->raise();
  myInfoWindow->activateWindow();

  blockSignals( false ); // enabling signals again
}

