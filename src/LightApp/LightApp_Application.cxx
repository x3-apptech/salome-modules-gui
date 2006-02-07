// File:      LightApp_Application.cxx
// Created:   6/20/2005 18:39:45 PM
// Author:    Natalia Donis
// Copyright (C) CEA 2005

#include "PythonConsole_PyInterp.h" // WARNING! This include must be the first!

#include "LightApp_Application.h"
#include "LightApp_WidgetContainer.h"
#include "LightApp_Module.h"
#include "LightApp_DataModel.h"
#include "LightApp_Study.h"
#include "LightApp_Preferences.h"
#include "LightApp_PreferencesDlg.h"
#include "LightApp_ModuleDlg.h"
#include "LightApp_AboutDlg.h"

#include "LightApp_OBFilter.h"

#include "LightApp_GLSelector.h"
#include "LightApp_OBSelector.h"
#include "LightApp_OCCSelector.h"
#include "LightApp_VTKSelector.h"
#include "LightApp_SelectionMgr.h"

#include <CAM_Module.h>
#include <CAM_DataModel.h>
#include <CAM_Study.h>
#include <STD_TabDesktop.h>

#include <SUIT_Session.h>
#include <SUIT_Study.h>
#include <SUIT_FileDlg.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Tools.h>
#include <SUIT_Accel.h>

#include <QtxMRUAction.h>
#include <QtxDockAction.h>
#include <QtxToolBar.h>

#include <LogWindow.h>
#include <OB_Browser.h>
#include <OB_ListView.h>
#include <PythonConsole_PyConsole.h>

#include <GLViewer_Viewer.h>
#include <GLViewer_ViewManager.h>

#include <Plot2d_ViewManager.h>
#include <Plot2d_ViewModel.h>
#include <SPlot2d_ViewModel.h>

#include <OCCViewer_ViewManager.h>
#include <SOCC_ViewModel.h>

#include <SVTK_ViewModel.h>
#include <SVTK_ViewManager.h>
#include <VTKViewer_ViewModel.h>

#include <SUPERVGraph_ViewModel.h>
#include <SUPERVGraph_ViewFrame.h>
#include <SUPERVGraph_ViewManager.h>

#include <QtxWorkstack.h>

#include <qdir.h>
#include <qimage.h>
#include <qstring.h>
#include <qwidget.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qapplication.h>
#include <qmap.h>
#include <qstatusbar.h>
#include <qthread.h>
#include <qobjectlist.h>
#include <qcombobox.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

#define FIRST_HELP_ID 1000000

#include "SALOME_InteractiveObject.hxx"
#include "SALOME_ListIO.hxx"

static const char* imageEmptyIcon[] = {
"20 20 1 1",
". 	c None",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"....................",
"...................."};

/*!Create new instance of LightApp_Application.*/
extern "C" LIGHTAPP_EXPORT SUIT_Application* createApplication()
{
  return new LightApp_Application();
}

LightApp_Preferences* LightApp_Application::_prefs_ = 0;

/*
  Class       : LightApp_Application
  Description : Application containing LightApp module
*/

/*!Constructor.*/
LightApp_Application::LightApp_Application()
: CAM_Application( false ),
myPrefs( 0 )
{
  STD_TabDesktop* desk = new STD_TabDesktop();

  setDesktop( desk );

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap aLogo = aResMgr->loadPixmap( "LightApp", tr( "APP_DEFAULT_ICO" ), false );

  desktop()->setIcon( aLogo );
  desktop()->setDockableMenuBar( true );
  desktop()->setDockableStatusBar( false );

  // base logo (salome itself)
  desktop()->addLogo( "_app_base",  aResMgr->loadPixmap( "LightApp", tr( "APP_BASE_LOGO" ), false ) );
  // extra logo (salome-based application)
  desktop()->addLogo( "_app_extra", aResMgr->loadPixmap( "LightApp", tr( "APP_EXTRA_LOGO" ), false ) );

  clearViewManagers();

  mySelMgr = new LightApp_SelectionMgr( this );

  myAccel = new SUIT_Accel( desktop() );
  myAccel->setActionKey( SUIT_Accel::PanLeft,     CTRL+Key_Left,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    CTRL+Key_Right,    OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       CTRL+Key_Up,       OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     CTRL+Key_Down,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      CTRL+Key_Plus,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     CTRL+Key_Minus,    OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     CTRL+Key_Asterisk, OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateLeft,  ALT+Key_Left,      OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateRight, ALT+Key_Right,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateUp,    ALT+Key_Up,        OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateDown,  ALT+Key_Down,      OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanLeft,     CTRL+Key_Left,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    CTRL+Key_Right,    VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       CTRL+Key_Up,       VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     CTRL+Key_Down,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      CTRL+Key_Plus,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     CTRL+Key_Minus,    VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     CTRL+Key_Asterisk, VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateLeft,  ALT+Key_Left,      VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateRight, ALT+Key_Right,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateUp,    ALT+Key_Up,        VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateDown,  ALT+Key_Down,      VTKViewer_Viewer::Type() );

  connect( mySelMgr, SIGNAL( selectionChanged() ), this, SLOT( onSelection() ) );
}

/*!Destructor.
 *\li Save window geometry.
 *\li Save desktop geometry.
 *\li Save resource maneger.
 *\li Delete selection manager.
 */
LightApp_Application::~LightApp_Application()
{
  saveWindowsGeometry();

  if ( resourceMgr() )
  {
    if ( desktop() )
      desktop()->saveGeometry( resourceMgr(), "desktop" );
    resourceMgr()->save();
  }
  delete mySelMgr;
}

/*!Start application.*/
void LightApp_Application::start()
{
  if ( desktop() )
    desktop()->loadGeometry( resourceMgr(), "desktop" );

  CAM_Application::start();

  QAction* a = action( ViewWindowsId );
  if ( a && a->inherits( "QtxDockAction" ) )
    ((QtxDockAction*)a)->setAutoPlace( true );

  updateWindows();
  updateViewManagers();

  putInfo( "" );
  desktop()->statusBar()->message( "" );
}

/*!Gets application name.*/
QString LightApp_Application::applicationName() const
{
  return tr( "APP_NAME" );
}

/*!Gets application version.*/
QString LightApp_Application::applicationVersion() const
{
  static QString _app_version;

  if ( _app_version.isEmpty() )
  {
    QString resVersion = tr( "APP_VERSION" );
    if ( resVersion != "APP_VERSION" )
    {
      _app_version = resVersion;
    }
    else
    {
      QString path( ::getenv( "GUI_ROOT_DIR" ) );
      if ( !path.isEmpty() )
        path += QDir::separator();
      path += QString( "bin/salome/VERSION" );

      QFile vf( path );
      if ( vf.open( IO_ReadOnly ) )
      {
        QString line;
	vf.readLine( line, 1024 );
	vf.close();

	if ( !line.isEmpty() )
        {
	  while ( !line.isEmpty() && line.at( line.length() - 1 ) == QChar( '\n' ) )
	    line.remove( line.length() - 1, 1 );

	  int idx = line.findRev( ":" );
	  if ( idx != -1 )
	    _app_version = line.mid( idx + 1 ).stripWhiteSpace();
        }
      }
    }
  }
  return _app_version;  
}

/*!Load module by \a name.*/
CAM_Module* LightApp_Application::loadModule( const QString& name )
{
  CAM_Module* mod = CAM_Application::loadModule( name );
  if ( mod )
  {
    connect( this, SIGNAL( studyOpened() ), mod, SLOT( onModelOpened() ) );
    connect( this, SIGNAL( studySaved() ),  mod, SLOT( onModelSaved() ) );
    connect( this, SIGNAL( studyClosed() ), mod, SLOT( onModelClosed() ) );
  }
  return mod;
}

/*!Activate module by \a modName*/
bool LightApp_Application::activateModule( const QString& modName )
{
  QString actName;
  CAM_Module* prevMod = activeModule();

  if ( prevMod )
    actName = prevMod->moduleName();

  if ( actName == modName )
    return true;

  putInfo( tr( "ACTIVATING_MODULE" ).arg( modName ) );  

  saveWindowsGeometry();

  bool status = CAM_Application::activateModule( modName );

  updateModuleActions();

  putInfo( "" );  

  if ( !status )
    return false;

  updateWindows();
  updateViewManagers();

  return true;
}

bool LightApp_Application::useStudy(const QString& theName)
{
  createEmptyStudy();
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(activeStudy());
  bool res = false;
  if (aStudy)
    res = aStudy->loadDocument( theName );
  updateDesktopTitle();
  updateCommandsStatus();
  return res;
}

/*!Gets selection manager.*/
LightApp_SelectionMgr* LightApp_Application::selectionMgr() const
{
  return mySelMgr;
}

/*!Create actions:*/
void LightApp_Application::createActions()
{
  STD_Application::createActions();

  SUIT_Desktop* desk = desktop();
  SUIT_ResourceMgr* resMgr = resourceMgr();

  //! Preferences
  createAction( PreferencesId, tr( "TOT_DESK_PREFERENCES" ), QIconSet(),
		tr( "MEN_DESK_PREFERENCES" ), tr( "PRP_DESK_PREFERENCES" ),
		CTRL+Key_F, desk, false, this, SLOT( onPreferences() ) );

  //! Help for modules
  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1, 1000 );
  int helpModuleMenu = createMenu( tr( "MEN_DESK_MODULE_HELP" ), helpMenu, -1, 0 );
  createMenu( separator(), helpMenu, -1, 1 );

  QStringList aModuleList;
  modules( aModuleList, false );

  int id = LightApp_Application::UserID + FIRST_HELP_ID;
  // help for KERNEL and GUI
  QCString dir;
  QString aFileName;
  QString root;
  QAction* a;
  if (dir = getenv("GUI_ROOT_DIR")) {
    aFileName = "GUI_index_v3.1.0.html";
    root = Qtx::addSlash( Qtx::addSlash(dir) +  Qtx::addSlash("doc") +  Qtx::addSlash("salome") );
    if ( QFileInfo( root + aFileName ).exists() ) {
      a = createAction( id, tr( QString("Kernel & GUI Help") ), QIconSet(),
			tr( QString("Kernel && GUI Help") ),
			tr( QString("Kernel & GUI Help") ),
			0, desk, false, this, SLOT( onHelpContentsModule() ) );
      a->setName( QString("GUI") );
      createMenu( a, helpModuleMenu, -1 );
      id++;
    }
  }
  // help for other existing modules
  QStringList::Iterator it;
  for ( it = aModuleList.begin(); it != aModuleList.end(); ++it )
  {
    if ( (*it).isEmpty() )
      continue;

    QString modName = moduleName( *it );
    aFileName = modName + "_index_v3.1.0.html";
    
    if (dir = getenv( modName + "_ROOT_DIR")) {
      root = Qtx::addSlash( Qtx::addSlash(dir) +  Qtx::addSlash("doc") +  Qtx::addSlash("salome") );
      if ( QFileInfo( root + aFileName ).exists() ) {

	QAction* a = createAction( id, tr( moduleTitle(modName) + QString(" Help") ), QIconSet(),
				   tr( moduleTitle(modName) + QString(" Help") ),
				   tr( moduleTitle(modName) + QString(" Help") ),
				   0, desk, false, this, SLOT( onHelpContentsModule() ) );
	a->setName( modName );
	createMenu( a, helpModuleMenu, -1 );
	id++;
      }
    }
  }

  //! MRU
  QtxMRUAction* mru = new QtxMRUAction( tr( "TOT_DESK_MRU" ), tr( "MEN_DESK_MRU" ), desk );
  connect( mru, SIGNAL( activated( QString ) ), this, SLOT( onMRUActivated( QString ) ) );
  registerAction( MRUId, mru );

  // default icon for neutral point ('SALOME' module)
  QPixmap defIcon = resMgr->loadPixmap( "LightApp", tr( "APP_DEFAULT_ICO" ), false );
  if ( defIcon.isNull() )
    defIcon = QPixmap( imageEmptyIcon );

  //! default icon for any module
  QPixmap modIcon = resMgr->loadPixmap( "LightApp", tr( "APP_MODULE_ICO" ), false );
  if ( modIcon.isNull() )
    modIcon = QPixmap( imageEmptyIcon );

  QToolBar* modTBar = new QtxToolBar( true, desk );
  modTBar->setLabel( tr( "INF_TOOLBAR_MODULES" ) );

  QActionGroup* modGroup = new QActionGroup( this );
  modGroup->setExclusive( true );
  modGroup->setUsesDropDown( true );

  a = createAction( -1, tr( "APP_NAME" ), defIcon, tr( "APP_NAME" ),
                    tr( "PRP_APP_MODULE" ), 0, desk, true );
  modGroup->add( a );
  myActions.insert( QString(), a );

  QMap<QString, QString> iconMap;
  moduleIconNames( iconMap );

  const int iconSize = 20;

  modGroup->addTo( modTBar );
  QObjectList *l = modTBar->queryList( "QComboBox" );
  QObjectListIt oit( *l );
  while ( QObject* obj = oit.current() ) {
    QComboBox* cb = (QComboBox*)obj;
    if ( cb ) cb->setFocusPolicy( QWidget::NoFocus );
    ++oit;
  }
  delete l;
  
   modTBar->addSeparator();

  QStringList modList;
  modules( modList, false );

  for ( it = modList.begin(); it != modList.end(); ++it )
  {
    if ( (*it).isEmpty() )
      continue;

    QString iconName;
    if ( iconMap.contains( *it ) )
      iconName = iconMap[*it];

    QString modName = moduleName( *it );

    QPixmap icon = resMgr->loadPixmap( modName, iconName, false );
    if ( icon.isNull() )
      icon = modIcon;

    icon.convertFromImage( icon.convertToImage().smoothScale( iconSize, iconSize, QImage::ScaleMin ) );

    QAction* a = createAction( -1, *it, icon, *it, tr( "PRP_MODULE" ).arg( *it ), 0, desk, true );
    a->addTo( modTBar );
    modGroup->add( a );

    myActions.insert( *it, a );
  }

  SUIT_Tools::simplifySeparators( modTBar );

  // New window
  int windowMenu = createMenu( tr( "MEN_DESK_WINDOW" ), -1, 100 );
  int newWinMenu = createMenu( tr( "MEN_DESK_NEWWINDOW" ), windowMenu, -1, 0 );
  createMenu( separator(), windowMenu, -1, 1 );

  QMap<int, int> accelMap;
  accelMap[NewGLViewId]  = ALT+Key_G;
  accelMap[NewPlot2dId]  = ALT+Key_P;
  accelMap[NewOCCViewId] = ALT+Key_O;
  accelMap[NewVTKViewId] = ALT+Key_K;

  for ( id = NewGLViewId; id <= NewVTKViewId; id++ )
  {
    QAction* a = createAction( id, tr( QString( "NEW_WINDOW_%1" ).arg( id - NewGLViewId ) ), QIconSet(),
			       tr( QString( "NEW_WINDOW_%1" ).arg( id - NewGLViewId ) ),
			       tr( QString( "NEW_WINDOW_%1" ).arg( id - NewGLViewId ) ),
			       accelMap.contains( id ) ? accelMap[id] : 0, desk, false, this, SLOT( onNewWindow() ) );
    createMenu( a, newWinMenu, -1 );
  }

  createAction( RenameId, tr( "TOT_RENAME" ), QIconSet(), tr( "MEN_DESK_RENAME" ), tr( "PRP_RENAME" ),
		SHIFT+Key_R, desk, false, this, SLOT( onRenameWindow() ) );
  createMenu( RenameId, windowMenu, -1 );

  connect( modGroup, SIGNAL( selected( QAction* ) ), this, SLOT( onModuleActivation( QAction* ) ) );

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );
  createMenu( PreferencesId, fileMenu, 15, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );

  /*
  createMenu( separator(), fileMenu, -1, 100, -1 );
  createMenu( MRUId, fileMenu, 100, -1 );
  createMenu( separator(), fileMenu, -1, 100, -1 );
  */
}

/*!On module activation action.*/
void LightApp_Application::onModuleActivation( QAction* a )
{
  if ( !a )
    return;

  QString modName = a->menuText();
  if ( modName == tr( "APP_NAME" ) )
    modName = QString::null;

  // Force user to create/open a study before module activation
  QMap<QString, QString> iconMap;
  moduleIconNames( iconMap );
  QPixmap icon = resourceMgr()->loadPixmap( moduleName( modName ), iconMap[ modName ], false );
  if ( icon.isNull() )
    icon = resourceMgr()->loadPixmap( "LightApp", tr( "APP_MODULE_BIG_ICO" ), false ); // default icon for any module

  bool cancelled = false;
  while ( !modName.isEmpty() && !activeStudy() && !cancelled ){
    LightApp_ModuleDlg aDlg( desktop(), modName, icon );
    int res = aDlg.exec();

    switch ( res ){
    case 1:
      onNewDoc();
      break;
    case 2:
      onOpenDoc();
      break;
    case 3:
      //onLoadStudy();
      //break;
    case 0:
    default:
      putInfo( tr("INF_CANCELLED") );
      myActions[QString()]->setOn( true );
      cancelled = true;
    }
  }

  if ( !cancelled )
    activateModule( modName );
}

/*!Default module activation.*/
QString LightApp_Application::defaultModule() const
{
  QStringList aModuleNames;
  modules( aModuleNames, false ); // obtain a complete list of module names for the current configuration
  //! If there's the one and only module --> activate it automatically
  //! TODO: Possible improvement - default module can be taken from preferences
  return aModuleNames.count() > 1 ? "" : ( aModuleNames.count() ? aModuleNames.first() : "" );
}

/*!On new window slot.*/
void LightApp_Application::onNewWindow()
{
  const QObject* obj = sender();
  if ( !obj || !obj->inherits( "QAction" ) )
    return;

  QString type;
  int id = actionId( (QAction*)obj );
  switch ( id )
  {
  case NewGLViewId:
    type = GLViewer_Viewer::Type();
    break;
  case NewPlot2dId:
    type = Plot2d_Viewer::Type();
    break;
  case NewOCCViewId:
    type = OCCViewer_Viewer::Type();
    break;
  case NewVTKViewId:
    type = VTKViewer_Viewer::Type();
    break;
  }

  if ( !type.isEmpty() )
    createViewManager( type );
}

//=======================================================================
//  name    : onNewDoc
/*! Purpose : SLOT. Create new document*/
//=======================================================================
void LightApp_Application::onNewDoc()
{
  SUIT_Study* study = activeStudy();

  saveWindowsGeometry();

  CAM_Application::onNewDoc();

  if ( !study ) // new study will be create in THIS application
  {
    updateWindows();
    updateViewManagers();
  }
}

//=======================================================================
// name    : onOpenDoc
/*! Purpose : SLOT. Open new document*/
//=======================================================================
void LightApp_Application::onOpenDoc()
{
  SUIT_Study* study = activeStudy();
  saveWindowsGeometry();

  CAM_Application::onOpenDoc();

  if ( !study ) // new study will be create in THIS application
  {
    updateWindows();
    updateViewManagers();
  }
}

#include "SUIT_MessageBox.h"
/*! Purpose : SLOT. Open new document with \a aName.*/
bool LightApp_Application::onOpenDoc( const QString& aName )
{
  bool isAlreadyOpen = false;

  // Look among opened studies
  if (activeStudy()) { // at least one study is opened
    SUIT_Session* aSession = SUIT_Session::session();
    QPtrList<SUIT_Application> aAppList = aSession->applications();
    QPtrListIterator<SUIT_Application> it (aAppList);
    SUIT_Application* aApp = 0;
    // iterate on all applications
    for (; (aApp = it.current()) && !isAlreadyOpen; ++it) {
      if (aApp->activeStudy()->studyName() == aName) {
        isAlreadyOpen = true; // Already opened, ask user what to do

        // The document ... is already open.
        // Do you want to reload it?
        int aAnswer = SUIT_MessageBox::warn2(desktop(), tr("WRN_WARNING"),
                                             tr("QUE_DOC_ALREADYOPEN").arg(aName),
                                             tr("BUT_YES"), tr("BUT_NO"), 1, 2, 2);
        if (aAnswer == 1) { // reload
          if (activeStudy()->studyName() == aName && aAppList.count() > 1) {
            // Opened in THIS (active) application.
            STD_Application* app1 = (STD_Application*)aAppList.at(0);
            STD_Application* app2 = (STD_Application*)aAppList.at(1);
            if (!app1 || !app2) {
              // Error
              return false;
            }
            if (app1->activeStudy()->studyName() == aName) {
              // app1 is this application, we need another one
              app1 = app2;
            }
            // Close document of this application. This application will be destroyed.
            onCloseDoc(/*ask = */false);
            // Open the file with another application, as this one will be destroyed.
            return app1->onOpenDoc(aName);
          } else {
            // Opened in another application.
            STD_Application* app = (STD_Application*)aApp;
            if (app)
              app->onCloseDoc(/*ask = */false);
          }
        } else { // do not reload
          // OK, the study will not be reloaded, but we call
          // CAM_Application::onOpenDoc( aName ) all the same.
          // It will activate a desktop of the study <aName>.
        }
      }
    }
  }

  bool res = CAM_Application::onOpenDoc( aName );

  QAction* a = action( MRUId );
  if ( a && a->inherits( "QtxMRUAction" ) )
  {
    QtxMRUAction* mru = (QtxMRUAction*)a;
    if ( res )
      mru->insert( aName );
    else
      mru->remove( aName );
  }
  return res;
}

//=======================================================================
// name    : onHelpAbout
/*! Purpose : SLOT. Display "About" message box*/
//=======================================================================
void LightApp_Application::onHelpAbout()
{
  LightApp_AboutDlg* dlg = new LightApp_AboutDlg( applicationName(), applicationVersion(), desktop() );
  dlg->exec();
  delete dlg;
}

/*!SLOT. Load document with \a aName.*/
bool LightApp_Application::onLoadDoc( const QString& aName )
{
  bool res = CAM_Application::onLoadDoc( aName );

  /*jfa tmp:QAction* a = action( MRUId );
  if ( a && a->inherits( "QtxMRUAction" ) )
  {
    QtxMRUAction* mru = (QtxMRUAction*)a;
    if ( res )
      mru->insert( aName );
    else
      mru->remove( aName );
  }*/
  return res;
}

/*!Private SLOT. Selection.*/
void LightApp_Application::onSelection()
{
  onSelectionChanged();

  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->selectionChanged();
}

/*!Set active study.
 *\param study - SUIT_Study.
 */
void LightApp_Application::setActiveStudy( SUIT_Study* study )
{
  CAM_Application::setActiveStudy( study );

  activateWindows();
}

//=======================================================================
// name    : updateCommandsStatus
/*! Purpose : Enable/Disable menu items and toolbar buttons. Rebuild menu*/
//=======================================================================
void LightApp_Application::updateCommandsStatus()
{
  CAM_Application::updateCommandsStatus();

  for ( int id = NewGLViewId; id <= NewVTKViewId; id++ )
  {
    QAction* a = action( id );
    if ( a )
      a->setEnabled( activeStudy() );
  }
}

// Helps to execute command
class RunBrowser: public QThread {
public:

  RunBrowser(QString theApp, QString theParams, QString theHelpFile, QString theContext=NULL):
    myApp(theApp), myParams(theParams), 
#ifdef WIN32
      myHelpFile("file://" + theHelpFile + theContext), 
#else
      myHelpFile("file:" + theHelpFile + theContext),
#endif
      myStatus(0) {};

  virtual void run()
  {
    QString aCommand;

    if ( !myApp.isEmpty())
      {
	aCommand.sprintf("%s %s %s",myApp.latin1(),myParams.latin1(),myHelpFile.latin1());
	myStatus = system(aCommand);
	if(myStatus != 0)
	  {
	    QCustomEvent* ce2000 = new QCustomEvent (2000);
	    postEvent (qApp, ce2000);
	  }
      }
      if( myStatus != 0)
      {
        qApp->lock();
        SUIT_MessageBox::warn1(0, QObject::tr("WRN_WARNING"),
                               QObject::tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").arg(myApp).arg(myHelpFile),
                               QObject::tr("BUT_OK"));
        qApp->unlock();
      }
  }

private:
  QString myApp;
  QString myParams;
  QString myHelpFile;
  int myStatus;

};

//=======================================================================
// name    : onHelpContentsModule
/*! Purpose : SLOT. Display help contents for choosen module*/
//=======================================================================
void LightApp_Application::onHelpContentsModule()
{
  const QAction* obj = (QAction*) sender();

  QString aComponentName = obj->name();
  QString aFileName = aComponentName + "_index_v3.1.0.html";

  QCString dir = getenv( aComponentName + "_ROOT_DIR");
  QString homeDir = Qtx::addSlash( Qtx::addSlash(dir) +  Qtx::addSlash("doc") +  Qtx::addSlash("salome") );

  QString helpFile = QFileInfo( homeDir + aFileName ).absFilePath();
  SUIT_ResourceMgr* resMgr = resourceMgr();
  QString anApp = resMgr->stringValue("ExternalBrowser", "application");
  QString aParams = resMgr->stringValue("ExternalBrowser", "parameters");

  if (!anApp.isEmpty()) {
    RunBrowser* rs = new RunBrowser(anApp, aParams, helpFile);
    rs->start();
  }
  else {
    SUIT_MessageBox::warn1(desktop(), tr("WRN_WARNING"),
                           tr("DEFINE_EXTERNAL_BROWSER"),
                           tr("BUT_OK"));
  }
}

/*!Sets enable or disable some actions on selection changed.*/
void LightApp_Application::onSelectionChanged()
{
}

/*!Return window.
 *\param flag - key for window
 *\param studyId - study id
 * Flag used how identificator of window in windows list.
 */
QWidget* LightApp_Application::window( const int flag, const int studyId ) const
{
  QWidget* wid = 0;

 int sId = studyId;
  if ( sId < 0 )
  {
    if ( !activeStudy() )
      return 0;
    else
      sId = activeStudy()->id();
  }

  if ( myWindows.contains( flag ) )
    wid = myWindows[flag]->widget( sId );

  return wid;
}

/*!Adds window to application.
 *\param wid - QWidget
 *\param flag - key wor window
 *\param studyId - study id
 * Flag used how identificator of window in windows list.
 */
void LightApp_Application::addWindow( QWidget* wid, const int flag, const int studyId )
{
  if ( !wid )
    return;

  int sId = studyId;
  if ( sId < 0 )
  {
    if ( !activeStudy() )
      return;
    else
      sId = activeStudy()->id();
  }

  if ( !myWindows.contains( flag ) )
  {
    QMap<int, int> winMap;
    currentWindows( winMap );

    LightApp_WidgetContainer* newWC = new LightApp_WidgetContainer( flag, desktop() );
    connect( newWC, SIGNAL(  destroyed ( QObject* ) ), this, SLOT( onWCDestroyed( QObject* ) ) );
    myWindows.insert( flag, newWC );
    if ( winMap.contains( flag ) )
      desktop()->moveDockWindow( myWindows[flag], (Dock)winMap[flag] );

    myWindows[flag]->setResizeEnabled( true );
    myWindows[flag]->setCloseMode( QDockWindow::Always );
    myWindows[flag]->setName( QString( "dock_window_%1" ).arg( flag ) );
    myWindows[flag]->setFixedExtentWidth( wid->width() );
    myWindows[flag]->setFixedExtentHeight( wid->height() );
  }

  QFont f;
  if( wid->inherits( "PythonConsole" ) )
  {
    if( resourceMgr()->hasValue( "PyConsole", "font" ) )
      f = resourceMgr()->fontValue( "PyConsole", "font" );
    else
    {
      f = ( ( PythonConsole* )wid )->font();
      resourceMgr()->setValue( "PyConsole", "font", f );
    }
  }
  else
    f = wid->font();

  myWindows[flag]->insert( sId, wid );
  wid->setFont(f);

  setWindowShown( flag, !myWindows[flag]->isEmpty() );
}

/*!Remove window from application.
 *\param flag - key wor window
 *\param studyId - study id
 * Flag used how identificator of window in windows list.
 */
void LightApp_Application::removeWindow( const int flag, const int studyId )
{
  if ( !myWindows.contains( flag ) )
    return;

  int sId = studyId;
  if ( sId < 0 )
  {
    if ( !activeStudy() )
      return;
    else
      sId = activeStudy()->id();
  }

  QWidget* wid = myWindows[flag]->widget( sId );
  myWindows[flag]->remove( sId );
  delete wid;

  setWindowShown( flag, !myWindows[flag]->isEmpty() );
}

/*!Gets window.
 *\param flag - key wor window
 *\param studyId - study id
 * Flag used how identificator of window in windows list.
 */
QWidget* LightApp_Application::getWindow( const int flag, const int studyId )
{
  QWidget* wid = window( flag, studyId );
  if ( !wid )
    addWindow( wid = createWindow( flag ), flag, studyId );

  return wid;
}

/*!Check is window visible?(with identificator \a type)*/
bool LightApp_Application::isWindowVisible( const int type ) const
{
  bool res = false;
  if ( myWindows.contains( type ) )
  {
    SUIT_Desktop* desk = ((LightApp_Application*)this)->desktop();
    res = desk && desk->appropriate( myWindows[type] );
  }
  return res;
}

/*!Sets window show or hide.
 *\param type - window identificator.
 *\param on   - true/false (window show/hide)
 */
void LightApp_Application::setWindowShown( const int type, const bool on )
{
  if ( !desktop() || !myWindows.contains( type ) )
    return;

  QDockWindow* dw = myWindows[type];
  desktop()->setAppropriate( dw, on );
  on ? dw->show() : dw->hide();
}

/*!Gets "ObjectBrowser".*/
OB_Browser* LightApp_Application::objectBrowser()
{
  OB_Browser* ob = 0;
  QWidget* wid = window( WT_ObjectBrowser );
  if ( wid && wid->inherits( "OB_Browser" ) )
    ob = (OB_Browser*)wid;
  return ob;
}

/*!Gets "LogWindow".*/
LogWindow* LightApp_Application::logWindow()
{
  LogWindow* lw = 0;
  QWidget* wid = getWindow( WT_LogWindow );
  if ( wid->inherits( "LogWindow" ) )
    lw = (LogWindow*)wid;
  return lw;
}

/*!Get "PythonConsole"*/
PythonConsole* LightApp_Application::pythonConsole()
{
  PythonConsole* console = 0;
  QWidget* wid = getWindow( WT_PyConsole );
  if ( wid->inherits( "PythonConsole" ) )
    console = (PythonConsole*)wid;
  return console;
}

/*!Update obect browser*/
void LightApp_Application::updateObjectBrowser( const bool updateModels )
{
  // update existing data models
  if ( updateModels ) 
  {
    LightApp_Study* study = dynamic_cast<LightApp_Study*>(activeStudy());
    if ( study ) {
      CAM_Study::ModelList dm_list;
      study->dataModels( dm_list );
      for ( CAM_Study::ModelListIterator it( dm_list ); it.current(); ++it ) {
        CAM_DataModel* camDM = it.current();
        if ( camDM && camDM->inherits( "LightApp_DataModel" ) )
          ((LightApp_DataModel*)camDM)->update();
      }
    }
  }
  if ( objectBrowser() )
  {
    objectBrowser()->updateGeometry();
    objectBrowser()->updateTree( 0, false );
  }
}

/*!Gets preferences.*/
LightApp_Preferences* LightApp_Application::preferences() const
{
  return preferences( false );
}

/*!Gets view manager*/
SUIT_ViewManager* LightApp_Application::getViewManager( const QString& vmType, const bool create )
{
  SUIT_ViewManager* aVM = viewManager( vmType );
  SUIT_ViewManager* anActiveVM = CAM_Application::activeViewManager();

  if ( anActiveVM && anActiveVM->getType() == vmType )
    aVM = anActiveVM;

  if ( aVM && create )
  {
    if ( !aVM->getActiveView() )
      aVM->createView();
    else
      aVM->getActiveView()->setFocus();
  }
  else if ( create )
    aVM = createViewManager( vmType );

  return aVM;
}

/*!Create view manager.*/
SUIT_ViewManager* LightApp_Application::createViewManager( const QString& vmType )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

  SUIT_ViewManager* viewMgr = 0;
  if( vmType == GLViewer_Viewer::Type() )
  {
    viewMgr = new GLViewer_ViewManager( activeStudy(), desktop() );
    new LightApp_GLSelector( (GLViewer_Viewer2d*)viewMgr->getViewModel(), mySelMgr );
  }
  else if( vmType == Plot2d_Viewer::Type() )
  {
    viewMgr = new Plot2d_ViewManager( activeStudy(), desktop() );
    SPlot2d_Viewer* vm = new SPlot2d_Viewer();
    viewMgr->setViewModel( vm  );// custom view model, which extends SALOME_View interface 
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( viewMgr->getActiveView() );
    if( wnd )
    {
      Plot2d_ViewFrame* frame = wnd->getViewFrame();
      frame->setBackgroundColor( resMgr->colorValue( "Plot2d", "Background", frame->backgroundColor() ) );
    }
  }
  else if( vmType == SUPERVGraph_Viewer::Type() )
  {
    viewMgr = new SUPERVGraph_ViewManager( activeStudy(), desktop() );
    SUPERVGraph_Viewer* vm = new SUPERVGraph_Viewer();
    SUPERVGraph_ViewFrame* view = dynamic_cast<SUPERVGraph_ViewFrame*>( vm->getViewManager()->getActiveView() );
    if( view )
      view->setBackgroundColor( resMgr->colorValue( "SUPERVGraph", "Background", view->backgroundColor() ) );
  }
  else if( vmType == OCCViewer_Viewer::Type() )
  {
    viewMgr = new OCCViewer_ViewManager( activeStudy(), desktop() );
    SOCC_Viewer* vm = new SOCC_Viewer();
    vm->setBackgroundColor( resMgr->colorValue( "OCCViewer", "background", vm->backgroundColor() ) );
    vm->setTrihedronSize( resMgr->integerValue( "OCCViewer", "trihedron_size", vm->trihedronSize() ) );
    int u( 1 ), v( 1 );
    vm->isos( u, v );
    u = resMgr->integerValue( "OCCViewer", "iso_number_u", u );
    v = resMgr->integerValue( "OCCViewer", "iso_number_v", v );
    vm->setIsos( u, v );
    viewMgr->setViewModel( vm );// custom view model, which extends SALOME_View interface
    new LightApp_OCCSelector( (OCCViewer_Viewer*)viewMgr->getViewModel(), mySelMgr );
  }
  else if ( vmType == SVTK_Viewer::Type() )
  {
    viewMgr = new SVTK_ViewManager( activeStudy(), desktop() );
    SVTK_Viewer* vm = dynamic_cast<SVTK_Viewer*>( viewMgr->getViewModel() );
    if( vm )
    {
      vm->setBackgroundColor( resMgr->colorValue( "VTKViewer", "background", vm->backgroundColor() ) );
      vm->setTrihedronSize( resMgr->integerValue( "VTKViewer", "trihedron_size", vm->trihedronSize() ) );
      new LightApp_VTKSelector( vm, mySelMgr );
    }
  }

  if ( !viewMgr )
    return 0;

  addViewManager( viewMgr );
  SUIT_ViewWindow* viewWin = viewMgr->createViewWindow();

  if ( viewWin && desktop() )
    viewWin->resize( (int)( desktop()->width() * 0.6 ), (int)( desktop()->height() * 0.6 ) );

  connect( viewMgr, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onCloseView( SUIT_ViewManager* ) ) );

  return viewMgr;
}

//=======================================================================
// name    : onCloseView
/*! Purpose : SLOT. Remove view manager from application*/
//=======================================================================
void LightApp_Application::onCloseView( SUIT_ViewManager* theVM )
{
  removeViewManager( theVM );
}

/*!Protected SLOT. On study created.*/
void LightApp_Application::onStudyCreated( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->setName( tr( "DATA_MODELS" ) );
  }
  getWindow( WT_ObjectBrowser );
  if ( objectBrowser() != 0 )
    objectBrowser()->setRootObject( aRoot );

  activateModule( defaultModule() );

  activateWindows();
}

/*!Protected SLOT. On study opened.*/
void LightApp_Application::onStudyOpened( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->dump();
  }
  getWindow( WT_ObjectBrowser );
  if ( objectBrowser() != 0 ) {
    objectBrowser()->setRootObject( aRoot );
  }

  activateModule( defaultModule() );

  activateWindows();

  emit studyOpened();
}

/*!Protected SLOT. On study saved.*/
void LightApp_Application::onStudySaved( SUIT_Study* )
{
  emit studySaved();
}

/*!Protected SLOT. On study closed.*/
void LightApp_Application::onStudyClosed( SUIT_Study* )
{
  emit studyClosed();

  // Bug 10396: clear selection
  mySelMgr->clearSelected();

  activateModule( "" );

  saveWindowsGeometry();
}

/*!Protected SLOT.On desktop activated.*/
void LightApp_Application::onDesktopActivated()
{
  CAM_Application::onDesktopActivated();
  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>(activeModule());
  if(aModule)
    aModule->studyActivated();
}

/*!Gets file filter.
 *\retval QString "(*.bin)"
 */
QString LightApp_Application::getFileFilter() const
{
  return "(*.bin)";
  // HDF persistence not yet completed - to be uncommented later
  //return "(*.hdf)";
}

/*! Gets file name*/
QString LightApp_Application::getFileName( bool open, const QString& initial, const QString& filters, 
                                           const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  QStringList fls = QStringList::split( ";;", filters, false );
  return SUIT_FileDlg::getFileName( parent, initial, fls, caption, open, true );
}

/*! Gets directory*/
QString LightApp_Application::getDirectory( const QString& initial, const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  return SUIT_FileDlg::getExistingDirectory( parent, initial, caption, true );
}

/*! Get open file names*/
QStringList LightApp_Application::getOpenFileNames( const QString& initial, const QString& filters, 
                                                    const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  QStringList fls = QStringList::split( ";;", filters, false );
  return SUIT_FileDlg::getOpenFileNames( parent, initial, fls, caption, true );
}

/*!Private SLOT. Update object browser.*/
void LightApp_Application::onRefresh()
{
  updateObjectBrowser( true );
}

/*!Private SLOT. On preferences.*/
void LightApp_Application::onPreferences()
{
  QApplication::setOverrideCursor( Qt::waitCursor );

  LightApp_PreferencesDlg* prefDlg = new LightApp_PreferencesDlg( preferences( true ), desktop());

  QApplication::restoreOverrideCursor();

  if ( !prefDlg )
    return;

  if ( ( prefDlg->exec() == QDialog::Accepted || prefDlg->isSaved() ) &&  resourceMgr() ) {
    if ( desktop() )
      desktop()->saveGeometry( resourceMgr(), "desktop" );
    resourceMgr()->save();
  }

  delete prefDlg;
}

/*!Protected SLOT. On preferences changed.*/
void LightApp_Application::onPreferenceChanged( QString& modName, QString& section, QString& param )
{
  LightApp_Module* sMod = 0;
  CAM_Module* mod = module( modName );
  if ( mod && mod->inherits( "LightApp_Module" ) )
    sMod = (LightApp_Module*)mod;

  if ( sMod )
    sMod->preferencesChanged( section, param );
  else
    preferencesChanged( section, param );
}

/*!Private SLOT. On open document with name \a aName.*/
void LightApp_Application::onMRUActivated( QString aName )
{
  onOpenDoc( aName );
}

/*!Remove all windows from study.*/
void LightApp_Application::beforeCloseDoc( SUIT_Study* s )
{
  CAM_Application::beforeCloseDoc( s );

  for ( WindowMap::ConstIterator itr = myWindows.begin(); s && itr != myWindows.end(); ++itr )
    removeWindow( itr.key(), s->id() );
}

/*!Update actions.*/
void LightApp_Application::updateActions()
{
  updateCommandsStatus();
}

//=======================================================================
// name    : createNewStudy
/*! Purpose : Create new study*/
//=======================================================================
SUIT_Study* LightApp_Application::createNewStudy()
{
  LightApp_Study* aStudy = new LightApp_Study( this );

  // Set up processing of major study-related events
  connect( aStudy, SIGNAL( created( SUIT_Study* ) ), this, SLOT( onStudyCreated( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( opened ( SUIT_Study* ) ), this, SLOT( onStudyOpened ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( saved  ( SUIT_Study* ) ), this, SLOT( onStudySaved  ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( closed ( SUIT_Study* ) ), this, SLOT( onStudyClosed ( SUIT_Study* ) ) );

  return aStudy;
}

/*!Create window.*/
QWidget* LightApp_Application::createWindow( const int flag )
{
  QWidget* wid = 0;
  if ( flag == WT_ObjectBrowser )
  {
    OB_Browser* ob = new OB_Browser( desktop() );
    ob->setAutoUpdate( true );
    ob->setAutoOpenLevel( 1 );
    ob->setCaption( tr( "OBJECT_BROWSER" ) );

    OB_ListView* ob_list = dynamic_cast<OB_ListView*>( const_cast<QListView*>( ob->listView() ) );
    if( ob_list )
      ob_list->setColumnMaxWidth( 0, desktop()->width()/4 );

    ob->setFilter( new LightApp_OBFilter( selectionMgr() ) );
    ob->setNameTitle( tr( "OBJ_BROWSER_NAME" ) );

    // Create OBSelector
    new LightApp_OBSelector( ob, mySelMgr );

    wid = ob;

    ob->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
  else  if ( flag == WT_PyConsole )
  {
    PythonConsole* pyCons = new PythonConsole( desktop() );
    pyCons->setCaption( tr( "PYTHON_CONSOLE" ) );
    wid = pyCons;
    //    pyCons->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
  else if ( flag == WT_LogWindow )
  {
    LogWindow* logWin = new LogWindow( desktop() );
    logWin->setCaption( tr( "LOG_WINDOW" ) );
    wid = logWin;
    logWin->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
  return wid;
}

/*!Default windows(Object Browser, Python Console).
 * Adds to map \a aMap.
 */
void LightApp_Application::defaultWindows( QMap<int, int>& aMap ) const
{  
  aMap.insert( WT_ObjectBrowser, Qt::DockLeft );
  aMap.insert( WT_PyConsole, Qt::DockBottom );
  //  aMap.insert( WT_LogWindow, Qt::DockBottom );
}

/*!Default view manager.*/
void LightApp_Application::defaultViewManagers( QStringList& ) const
{
  /*!Do nothing.*/
}

/*!Gets preferences.
 * Create preferences, if \a crt = true.
 */
LightApp_Preferences* LightApp_Application::preferences( const bool crt ) const
{
  if ( myPrefs )
    return myPrefs;

  LightApp_Application* that = (LightApp_Application*)this;

  bool toCreate = !_prefs_ && crt;
  if( toCreate )
  {
    _prefs_ = new LightApp_Preferences( resourceMgr() );
    that->createPreferences( _prefs_ );
  }

  that->myPrefs = _prefs_;

  QPtrList<SUIT_Application> appList = SUIT_Session::session()->applications();
  for ( QPtrListIterator<SUIT_Application> appIt ( appList ); appIt.current(); ++appIt )
  {
    if ( !appIt.current()->inherits( "LightApp_Application" ) )
      continue;

    LightApp_Application* app = (LightApp_Application*)appIt.current();

    QStringList modNameList;
    app->modules( modNameList, false );
    for ( QStringList::const_iterator it = modNameList.begin(); it != modNameList.end(); ++it )
    {
      int id = _prefs_->addPreference( *it );
      _prefs_->setItemProperty( id, "info", tr( "PREFERENCES_NOT_LOADED" ).arg( *it ) );
    }

    ModuleList modList;
    app->modules( modList );
    for ( ModuleListIterator itr( modList ); itr.current(); ++itr )
    {
      LightApp_Module* mod = 0;
      if ( itr.current()->inherits( "LightApp_Module" ) )
	mod = (LightApp_Module*)itr.current();

      if ( mod && !_prefs_->hasModule( mod->moduleName() ) )
      {
	int modCat = _prefs_->addPreference( mod->moduleName() );
	_prefs_->setItemProperty( modCat, "info", QString::null );
	if( toCreate )
	  mod->createPreferences();
      }
    }
  }

  connect( myPrefs, SIGNAL( preferenceChanged( QString&, QString&, QString& ) ),
           this, SLOT( onPreferenceChanged( QString&, QString&, QString& ) ) );

  return myPrefs;
}

/*!Add new module to application.*/
void LightApp_Application::moduleAdded( CAM_Module* mod )
{
  CAM_Application::moduleAdded( mod );

  LightApp_Module* lightMod = 0;
  if ( mod && mod->inherits( "LightApp_Module" ) )
    lightMod = (LightApp_Module*)mod;

  if ( myPrefs && lightMod && !myPrefs->hasModule( lightMod->moduleName() ))
  {
    int modCat = myPrefs->addPreference( mod->moduleName() );
    myPrefs->setItemProperty( modCat, "info", QString::null );
    lightMod->createPreferences();
  }
}

/*!Create preferences.*/
void LightApp_Application::createPreferences( LightApp_Preferences* pref )
{
  if ( !pref )
    return;

  int salomeCat = pref->addPreference( tr( "PREF_CATEGORY_SALOME" ) );

  int genTab = pref->addPreference( tr( "PREF_TAB_GENERAL" ), salomeCat );
  int studyGroup = pref->addPreference( tr( "PREF_GROUP_STUDY" ), genTab );
  pref->setItemProperty( studyGroup, "columns", 1 );

  pref->addPreference( tr( "PREF_MULTI_FILE" ), studyGroup, LightApp_Preferences::Bool, "Study", "multi_file" );
  pref->addPreference( tr( "PREF_ASCII_FILE" ), studyGroup, LightApp_Preferences::Bool, "Study", "ascii_file" );
  int undoPref = pref->addPreference( tr( "PREF_UNDO_LEVEL" ), studyGroup, LightApp_Preferences::IntSpin, "Study", "undo_level" );
  pref->setItemProperty( undoPref, "min", 1 );
  pref->setItemProperty( undoPref, "max", 100 );
  pref->addPreference( tr( "PREF_STORE_POS" ), studyGroup, LightApp_Preferences::Bool, "Study", "store_positions" );

  int extgroup = pref->addPreference( tr( "PREF_GROUP_EXT_BROWSER" ), genTab );
  pref->setItemProperty( extgroup, "columns", 1 );
  int apppref = pref->addPreference( tr( "PREF_APP" ), extgroup, LightApp_Preferences::File, "ExternalBrowser", "application" );
  pref->setItemProperty( apppref, "existing", true );
  pref->setItemProperty( apppref, "flags", QFileInfo::ExeUser );
  pref->setItemProperty( apppref, "readOnly", false );

  pref->addPreference( tr( "PREF_PARAM" ), extgroup, LightApp_Preferences::String, "ExternalBrowser", "parameters" );

  int pythonConsoleGroup = pref->addPreference( tr( "PREF_GROUP_PY_CONSOLE" ), genTab );
  pref->setItemProperty( pythonConsoleGroup, "columns", 1 );
  pref->addPreference( tr( "PREF_FONT" ), pythonConsoleGroup, LightApp_Preferences::Font, "PyConsole", "font" );

  int viewTab = pref->addPreference( tr( "PREF_TAB_VIEWERS" ), salomeCat );

  int occGroup = pref->addPreference( tr( "PREF_GROUP_OCCVIEWER" ), viewTab );

  int vtkGroup = pref->addPreference( tr( "PREF_GROUP_VTKVIEWER" ), viewTab );

  int plot2dGroup = pref->addPreference( tr( "PREF_GROUP_PLOT2DVIEWER" ), viewTab );

  int supervGroup = pref->addPreference( tr( "PREF_GROUP_SUPERV" ), viewTab );

  pref->setItemProperty( occGroup, "columns", 1 );
  pref->setItemProperty( vtkGroup, "columns", 1 );
  pref->setItemProperty( plot2dGroup, "columns", 1 );

  int occTS = pref->addPreference( tr( "PREF_TRIHEDRON_SIZE" ), occGroup,
				   LightApp_Preferences::IntSpin, "OCCViewer", "trihedron_size" );
  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), occGroup,
		       LightApp_Preferences::Color, "OCCViewer", "background" );

  pref->setItemProperty( occTS, "min", 1 );
  pref->setItemProperty( occTS, "max", 150 );

  int isoU = pref->addPreference( tr( "PREF_ISOS_U" ), occGroup,
				  LightApp_Preferences::IntSpin, "OCCViewer", "iso_number_u" );
  int isoV = pref->addPreference( tr( "PREF_ISOS_V" ), occGroup,
				  LightApp_Preferences::IntSpin, "OCCViewer", "iso_number_v" );

  pref->setItemProperty( isoU, "min", 0 );
  pref->setItemProperty( isoU, "max", 100000 );

  pref->setItemProperty( isoV, "min", 0 );
  pref->setItemProperty( isoV, "max", 100000 );

  int vtkTS = pref->addPreference( tr( "PREF_TRIHEDRON_SIZE" ), vtkGroup,
				   LightApp_Preferences::IntSpin, "VTKViewer", "trihedron_size" );
  pref->addPreference( tr( "PREF_RELATIVE_SIZE" ), vtkGroup, LightApp_Preferences::Bool, "VTKViewer", "relative_size" );
  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), vtkGroup,
		       LightApp_Preferences::Color, "VTKViewer", "background" );

  pref->setItemProperty( vtkTS, "min", 1 );
  pref->setItemProperty( vtkTS, "max", 150 );

  pref->addPreference( tr( "PREF_SHOW_LEGEND" ), plot2dGroup,
		       LightApp_Preferences::Bool, "Plot2d", "ShowLegend" );

  int legendPosition = pref->addPreference( tr( "PREF_LEGEND_POSITION" ), plot2dGroup,
					    LightApp_Preferences::Selector, "Plot2d", "LegendPos" );
  QStringList aLegendPosList;
  aLegendPosList.append( tr("PREF_LEFT") );
  aLegendPosList.append( tr("PREF_RIGHT") );
  aLegendPosList.append( tr("PREF_TOP") );
  aLegendPosList.append( tr("PREF_BOTTOM") );

  QValueList<QVariant> anIndexesList;
  anIndexesList.append(0);
  anIndexesList.append(1);
  anIndexesList.append(2);
  anIndexesList.append(3);

  pref->setItemProperty( legendPosition, "strings", aLegendPosList );
  pref->setItemProperty( legendPosition, "indexes", anIndexesList );

  int curveType = pref->addPreference( tr( "PREF_CURVE_TYPE" ), plot2dGroup,
				       LightApp_Preferences::Selector, "Plot2d", "CurveType" );
  QStringList aCurveTypesList;
  aCurveTypesList.append( tr("PREF_POINTS") );
  aCurveTypesList.append( tr("PREF_LINES") );
  aCurveTypesList.append( tr("PREF_SPLINE") );

  anIndexesList.clear();
  anIndexesList.append(0);
  anIndexesList.append(1);
  anIndexesList.append(2);
  
  pref->setItemProperty( curveType, "strings", aCurveTypesList );
  pref->setItemProperty( curveType, "indexes", anIndexesList );

  int markerSize = pref->addPreference( tr( "PREF_MARKER_SIZE" ), plot2dGroup,
					LightApp_Preferences::IntSpin, "Plot2d", "MarkerSize" );

  pref->setItemProperty( markerSize, "min", 0 );
  pref->setItemProperty( markerSize, "max", 100 );
  
  QStringList aScaleModesList;
  aScaleModesList.append( tr("PREF_LINEAR") );
  aScaleModesList.append( tr("PREF_LOGARITHMIC") );
  
  anIndexesList.clear();
  anIndexesList.append(0);
  anIndexesList.append(1);

  int horScale = pref->addPreference( tr( "PREF_HOR_AXIS_SCALE" ), plot2dGroup,
				      LightApp_Preferences::Selector, "Plot2d", "HorScaleMode" );

  pref->setItemProperty( horScale, "strings", aScaleModesList );
  pref->setItemProperty( horScale, "indexes", anIndexesList );

  int verScale = pref->addPreference( tr( "PREF_VERT_AXIS_SCALE" ), plot2dGroup,
				      LightApp_Preferences::Selector, "Plot2d", "VerScaleMode" );

  pref->setItemProperty( verScale, "strings", aScaleModesList );
  pref->setItemProperty( verScale, "indexes", anIndexesList );

  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), plot2dGroup,
		       LightApp_Preferences::Color, "Plot2d", "Background" );

  int dirTab = pref->addPreference( tr( "PREF_TAB_DIRECTORIES" ), salomeCat );
  int dirGroup = pref->addPreference( tr( "PREF_GROUP_DIRECTORIES" ), dirTab );
  pref->setItemProperty( dirGroup, "columns", 1 );
  pref->addPreference( tr( "" ), dirGroup,
		       LightApp_Preferences::DirList, "FileDlg", "QuickDirList" );

  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), supervGroup,
		       LightApp_Preferences::Color, "SUPERVGraph", "Background" );
  pref->addPreference( tr( "PREF_SUPERV_TITLE_COLOR" ), supervGroup,
		       LightApp_Preferences::Color, "SUPERVGraph", "Title" );
//  pref->addPreference( tr( "PREF_SUPERV_CTRL_COLOR" ), supervGroup,
//		       LightApp_Preferences::Color, "SUPERVGraph", "Ctrl" );

  int obTab = pref->addPreference( tr( "PREF_TAB_OBJBROWSER" ), salomeCat );
  int objSetGroup = pref->addPreference( tr( "PREF_OBJ_BROWSER_SETTINGS" ), obTab );
  pref->addPreference( tr( "PREF_AUTO_SIZE_FIRST" ), objSetGroup, LightApp_Preferences::Bool,
		       "ObjectBrowser", "auto_size_first" );
  pref->addPreference( tr( "PREF_AUTO_SIZE" ), objSetGroup, LightApp_Preferences::Bool,
		       "ObjectBrowser", "auto_size" );
}

/*!Changed preferences */
void LightApp_Application::preferencesChanged( const QString& sec, const QString& param )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();
  if ( !resMgr )
    return;

  if ( sec == QString( "OCCViewer" ) && param == QString( "trihedron_size" ) )
  {
    int sz = resMgr->integerValue( sec, param, -1 );
    QPtrList<SUIT_ViewManager> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    for ( QPtrListIterator<SUIT_ViewManager> it( lst ); it.current() && sz >= 0; ++it )
    {
      SUIT_ViewModel* vm = it.current()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
	continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setTrihedronSize( sz );
      occVM->getAISContext()->UpdateCurrentViewer();
    }
  }

  if ( sec == QString( "VTKViewer" ) && (param == QString( "trihedron_size" ) || param == QString( "relative_size" )) )
  {
    int sz = resMgr->integerValue( "VTKViewer", "trihedron_size", -1 );
    QPtrList<SUIT_ViewManager> lst;
    viewManagers( SVTK_Viewer::Type(), lst );
    for ( QPtrListIterator<SUIT_ViewManager> it( lst ); it.current() && sz >= 0; ++it )
    {
      SUIT_ViewModel* vm = it.current()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
	continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM )
      {
	vtkVM->setTrihedronSize( sz );
	vtkVM->Repaint();
      }
    }
  }

  if ( sec == QString( "OCCViewer" ) && ( param == QString( "iso_number_u" ) || param == QString( "iso_number_v" ) ) )
  {
    QPtrList<SUIT_ViewManager> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    int u = resMgr->integerValue( sec, "iso_number_u" );
    int v = resMgr->integerValue( sec, "iso_number_v" );
    for ( QPtrListIterator<SUIT_ViewManager> it( lst ); it.current(); ++it )
    {
      OCCViewer_ViewManager* mgr = dynamic_cast<OCCViewer_ViewManager*>( it.current() );
      if( mgr && mgr->getOCCViewer() )
	mgr->getOCCViewer()->setIsos( u, v );
    }
  }

  if( sec=="ObjectBrowser" )
  {
    if( param=="auto_size" || param=="auto_size_first" )
    {
      OB_Browser* ob = objectBrowser();
      if( !ob )
	return;

      bool autoSize = resMgr->booleanValue( "ObjectBrowser", "auto_size", false ),
           autoSizeFirst = resMgr->booleanValue( "ObjectBrowser", "auto_size_first", true );
      ob->setWidthMode( autoSize ? QListView::Maximum : QListView::Manual );
      ob->listView()->setColumnWidthMode( 0, autoSizeFirst ? QListView::Maximum : QListView::Manual );
      updateObjectBrowser( false );
    }
  }

  if( sec=="Study" )
  { 
    if( param=="store_positions" )
      updateWindows();
  }

  if( sec=="PyConsole" )
  {
    if( param=="font" )
      if( pythonConsole() )
	pythonConsole()->setFont( resMgr->fontValue( "PyConsole", "font" ) );
  }
}

/*!Update desktop title.*/
void LightApp_Application::updateDesktopTitle() {
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  desktop()->setCaption( aTitle );
}

/*!Update windows after close document.*/
void LightApp_Application::afterCloseDoc()
{
  updateWindows();

  CAM_Application::afterCloseDoc();
}

/*!Update module action.*/
void LightApp_Application::updateModuleActions()
{
  QString modName;
  if ( activeModule() )
    modName = activeModule()->moduleName();

  if ( myActions.contains( modName ) )
    myActions[modName]->setOn( true );
}

/*!Gets current windows.
 *\param winMap - output current windows map.
 */
void LightApp_Application::currentWindows( QMap<int, int>& winMap ) const
{
  winMap.clear();
  if ( !activeStudy() )
    return;

  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->windows( winMap );
  else
    defaultWindows( winMap );
}

/*!Gets current view managers.
 *\param lst - output current view managers list.
 */
void LightApp_Application::currentViewManagers( QStringList& lst ) const
{
  lst.clear();
  if ( !activeStudy() )
    return;

  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->viewManagers( lst );
  else
    defaultViewManagers( lst );
}

/*!Update windows.*/
void LightApp_Application::updateWindows()
{
  QMap<int, int> winMap;
  currentWindows( winMap );

  for ( QMap<int, int>::ConstIterator it = winMap.begin(); it != winMap.end(); ++it )
    getWindow( it.key() );

  loadWindowsGeometry();

  for ( WindowMap::ConstIterator itr = myWindows.begin(); itr != myWindows.end(); ++itr )
    setWindowShown( itr.key(), !itr.data()->isEmpty() && winMap.contains( itr.key() ) );
}

/*!Update view managers.*/
void LightApp_Application::updateViewManagers()
{
  QStringList lst;
  currentViewManagers( lst );

  for ( QStringList::const_iterator it = lst.begin(); it != lst.end(); ++it )
    getViewManager( *it, true );
}

/*!Load windows geometry.*/
void LightApp_Application::loadWindowsGeometry()
{
  bool store = resourceMgr()->booleanValue( "Study", "store_positions", true );
  if( !store )
    return;

  QtxDockAction* dockMgr = 0;

  QAction* a = action( ViewWindowsId );
  if ( a && a->inherits( "QtxDockAction" ) )
    dockMgr = (QtxDockAction*)a;

  if ( !dockMgr )
    return;

  QString modName;
  if ( activeModule() )
    modName = activeModule()->name("");

  QString section = QString( "windows_geometry" );
  if ( !modName.isEmpty() )
    section += QString( "." ) + modName;

  dockMgr->loadGeometry( resourceMgr(), section, false );
  dockMgr->restoreGeometry();
}

/*!Save windows geometry.*/
void LightApp_Application::saveWindowsGeometry()
{
  bool store = resourceMgr()->booleanValue( "Study", "store_positions", true );
  if( !store )
    return;

  QtxDockAction* dockMgr = 0;

  QAction* a = action( ViewWindowsId );
  if ( a && a->inherits( "QtxDockAction" ) )
    dockMgr = (QtxDockAction*)a;

  if ( !dockMgr )
    return;

  QString modName;
  if ( activeModule() )
    modName = activeModule()->name("");

  QString section = QString( "windows_geometry" );
  if ( !modName.isEmpty() )
    section += QString( "." ) + modName;

  dockMgr->storeGeometry();
  dockMgr->saveGeometry( resourceMgr(), section, false );
}

/*!Activate windows.*/
void LightApp_Application::activateWindows()
{
  if ( activeStudy() )
  {
    for ( WindowMap::Iterator itr = myWindows.begin(); itr != myWindows.end(); ++itr )
      itr.data()->activate( activeStudy()->id() );
  }
}

/*!Adds icon names for modules.*/
void LightApp_Application::moduleIconNames( QMap<QString, QString>& iconMap ) const
{
  iconMap.clear();

  SUIT_ResourceMgr* resMgr = resourceMgr();
  if ( !resMgr )
    return;

  QStringList modList;
  modules( modList, false );

  for ( QStringList::const_iterator it = modList.begin(); it != modList.end(); ++it )
  {
    QString modName = *it;
    QString modIntr = moduleName( modName );
    QString modIcon = resMgr->stringValue( modIntr, "icon", QString::null );

    if ( modIcon.isEmpty() )
      continue;

    if ( SUIT_Tools::extension( modIcon ).isEmpty() )
      modIcon += QString( ".png" );

    iconMap.insert( modName, modIcon );
  }
}

/*!Insert items in popup, which necessary for current application*/
void LightApp_Application::contextMenuPopup( const QString& type, QPopupMenu* thePopup, QString& title )
{
  CAM_Application::contextMenuPopup( type, thePopup, title );

  OB_Browser* ob = objectBrowser();
  if ( !ob || type != ob->popupClientType() )
    return;

  thePopup->insertSeparator();
  thePopup->insertItem( tr( "MEN_REFRESH" ), this, SLOT( onRefresh() ) );
}

/*!Create empty study.*/
void LightApp_Application::createEmptyStudy()
{
  CAM_Application::createEmptyStudy();
  if ( objectBrowser() )
    objectBrowser()->updateTree();
}

/*!Activate module \a mod.*/
bool LightApp_Application::activateModule( CAM_Module* mod )
{
  bool res = CAM_Application::activateModule( mod );
  if ( objectBrowser() )
    objectBrowser()->updateTree();
  return res;
}

/*!return keyborad accelerators manager object */
SUIT_Accel* LightApp_Application::accel() const
{
  return myAccel;
}

/*! remove dead widget container from map */
void LightApp_Application::onWCDestroyed( QObject* ob )
{
  // remove destroyed widget container from windows map
  for ( WindowMap::ConstIterator itr = myWindows.begin(); itr != myWindows.end(); ++itr )
  {
    if ( itr.data() != ob )
      continue;

    int key = itr.key();
    myWindows.remove( key );
    break;
  }
}

/*! redefined to remove view manager from memory */
void LightApp_Application::removeViewManager( SUIT_ViewManager* vm )
{
  disconnect( vm, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  STD_Application::removeViewManager( vm );
  delete vm;
}

/*! rename active window of desktop */
void LightApp_Application::onRenameWindow()
{
  if( !desktop() )
    return;

  QWidget* w = desktop()->activeWindow();
  if( !w )
    return;

  bool ok;
  QString name = QInputDialog::getText( tr( "TOT_RENAME" ), tr( "PRP_RENAME" ), QLineEdit::Normal, w->caption(), &ok, w );
  if( ok && !name.isEmpty() )
    w->setCaption( name );
}
