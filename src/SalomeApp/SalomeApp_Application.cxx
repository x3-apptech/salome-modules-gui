// File:      SalomeApp_Application.cxx
// Created:   10/22/2004 3:23:45 PM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_PyInterp.h" // WARNING! This include must be the first!

#include "SalomeApp_Application.h"

#include "SalomeApp_Study.h"
#include "SalomeApp_Module.h"
#include "SalomeApp_OBFilter.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_EventFilter.h"
#include "SalomeApp_WidgetContainer.h"

#include "SalomeApp_AboutDlg.h"
#include "SalomeApp_ModuleDlg.h"
#include "SalomeApp_Preferences.h"
#include "SalomeApp_PreferencesDlg.h"
#include "SalomeApp_StudyPropertiesDlg.h"
#include "SalomeApp_CheckFileDlg.h"

#include "SalomeApp_GLSelector.h"
#include "SalomeApp_OBSelector.h"
#include "SalomeApp_OCCSelector.h"
#include "SalomeApp_VTKSelector.h"
#include "SalomeApp_SelectionMgr.h"

#include <LogWindow.h>

#include <GLViewer_Viewer.h>
#include <GLViewer_ViewManager.h>

#include <Plot2d_ViewManager.h>
#include <SPlot2d_ViewModel.h>

#include <OCCViewer_ViewManager.h>
#include <SOCC_ViewModel.h>

#include <SVTK_ViewModel.h>
#include <SVTK_ViewManager.h>

#include <STD_TabDesktop.h>

#include "STD_LoadStudiesDlg.h"

#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_Accel.h>

#include <QtxToolBar.h>
#include <QtxMRUAction.h>
#include <QtxDockAction.h>
#include <QtxResourceEdit.h>

#include <OB_Browser.h>

#include <PythonConsole_PyConsole.h>

#include <SUIT_FileDlg.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ActionOperation.h>

#include <Utils_ORB_INIT.hxx>
#include <Utils_SINGLETON.hxx>
#include <SALOME_ModuleCatalog_impl.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

#include <qmap.h>
#include <qdir.h>
#include <qlabel.h>
#include <qimage.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qlistbox.h>
#include <qregexp.h>
#include <qthread.h>
#include <qstatusbar.h>

#include "SALOMEDS_StudyManager.hxx"

#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOME_ListIO.hxx"

#include "ToolsGUI_CatalogGeneratorDlg.h"
#include "ToolsGUI_RegWidget.h"

#define OBJECT_BROWSER_WIDTH 300
#define DEFAULT_BROWSER "mozilla"

/*!Image for empty icon.*/
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

/*!Create new instance of SalomeApp_Application.*/
extern "C" SALOMEAPP_EXPORT SUIT_Application* createApplication()
{
  return new SalomeApp_Application();
}

SalomeApp_Preferences* SalomeApp_Application::_prefs_ = 0;

/*
  Class       : SalomeApp_Application
  Description : Application containing SalomeApp module
*/

/*!Constructor.*/
SalomeApp_Application::SalomeApp_Application()
: CAM_Application( false ),
myPrefs( 0 )
{
  STD_TabDesktop* desk = new STD_TabDesktop();

  setDesktop( desk );

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap aLogo = aResMgr->loadPixmap( "SalomeApp", tr( "APP_DEFAULT_ICO" ), false );

  desktop()->setIcon( aLogo );
  desktop()->setDockableMenuBar( true );
  desktop()->setDockableStatusBar( false );

  // base logo (salome itself)
  desktop()->addLogo( "_app_base",  aResMgr->loadPixmap( "SalomeApp", tr( "APP_BASE_LOGO" ), false ) );
  // extra logo (salome-based application)
  desktop()->addLogo( "_app_extra", aResMgr->loadPixmap( "SalomeApp", tr( "APP_EXTRA_LOGO" ), false ) );

  clearViewManagers();

  mySelMgr = new SalomeApp_SelectionMgr( this );

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
  myAccel->setActionKey( SUIT_Accel::PanLeft,     CTRL+Key_Left,     SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    CTRL+Key_Right,    SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       CTRL+Key_Up,       SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     CTRL+Key_Down,     SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      CTRL+Key_Plus,     SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     CTRL+Key_Minus,    SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     CTRL+Key_Asterisk, SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateLeft,  ALT+Key_Left,      SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateRight, ALT+Key_Right,     SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateUp,    ALT+Key_Up,        SVTK_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateDown,  ALT+Key_Down,      SVTK_Viewer::Type() );

  connect( mySelMgr, SIGNAL( selectionChanged() ), this, SLOT( onSelection() ) );
}

/*!Destructor.
 *\li Save window geometry.
 *\li Save desktop geometry.
 *\li Save resource maneger.
 *\li Delete selection manager.
 *\li Destroy event filter.
 */
SalomeApp_Application::~SalomeApp_Application()
{
  saveWindowsGeometry();

  if ( resourceMgr() )
  {
    if ( desktop() )
      desktop()->saveGeometry( resourceMgr(), "desktop" );
    resourceMgr()->save();
  }

  delete mySelMgr;

  SalomeApp_EventFilter::Destroy();
}

/*!Start application.*/
void SalomeApp_Application::start()
{
  if ( desktop() )
    desktop()->loadGeometry( resourceMgr(), "desktop" );

  CAM_Application::start();

  QAction* a = action( ViewWindowsId );
  if ( a && a->inherits( "QtxDockAction" ) )
    ((QtxDockAction*)a)->setAutoPlace( true );

  SalomeApp_EventFilter::Init();

  updateWindows();
  updateViewManagers();

  putInfo( "" );
  desktop()->statusBar()->message( "" );
}

/*!Gets application name.*/
QString SalomeApp_Application::applicationName() const
{
  return tr( "APP_NAME" );
}

/*!Gets application version.*/
QString SalomeApp_Application::applicationVersion() const
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
CAM_Module* SalomeApp_Application::loadModule( const QString& name )
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
bool SalomeApp_Application::activateModule( const QString& modName )
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

/*!Gets selection manager.*/
SalomeApp_SelectionMgr* SalomeApp_Application::selectionMgr() const
{
  return mySelMgr;
}

/*!Create actions:*/
void SalomeApp_Application::createActions()
{
  STD_Application::createActions();

  SUIT_Desktop* desk = desktop();
  SUIT_ResourceMgr* resMgr = resourceMgr();

  //! Dump study
  createAction( DumpStudyId, tr( "TOT_DESK_FILE_DUMP_STUDY" ), QIconSet(),
		tr( "MEN_DESK_FILE_DUMP_STUDY" ), tr( "PRP_DESK_FILE_DUMP_STUDY" ),
		0, desk, false, this, SLOT( onDumpStudy() ) );

  //! Load script
  createAction( LoadScriptId, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), QIconSet(),
		tr( "MEN_DESK_FILE_LOAD_SCRIPT" ), tr( "PRP_DESK_FILE_LOAD_SCRIPT" ),
		0, desk, false, this, SLOT( onLoadScript() ) );

  //! Properties
  createAction( PropertiesId, tr( "TOT_DESK_PROPERTIES" ), QIconSet(),
	        tr( "MEN_DESK_PROPERTIES" ), tr( "PRP_DESK_PROPERTIES" ),
	        0, desk, false, this, SLOT( onProperties() ) );

  //! Preferences
  createAction( PreferencesId, tr( "TOT_DESK_PREFERENCES" ), QIconSet(),
		tr( "MEN_DESK_PREFERENCES" ), tr( "PRP_DESK_PREFERENCES" ),
		CTRL+Key_P, desk, false, this, SLOT( onPreferences() ) );

  //! Catalog Generator
  createAction( CatalogGenId, tr( "TOT_DESK_CATALOG_GENERATOR" ),  QIconSet(),
		tr( "MEN_DESK_CATALOG_GENERATOR" ), tr( "PRP_DESK_CATALOG_GENERATOR" ),
		0, desk, false, this, SLOT( onCatalogGen() ) );

  //! Registry Display
  createAction( RegDisplayId, tr( "TOT_DESK_REGISTRY_DISPLAY" ),  QIconSet(),
		tr( "MEN_DESK_REGISTRY_DISPLAY" ), tr( "PRP_DESK_REGISTRY_DISPLAY" ),
		0, desk, false, this, SLOT( onRegDisplay() ) );

  //! MRU
  QtxMRUAction* mru = new QtxMRUAction( tr( "TOT_DESK_MRU" ), tr( "MEN_DESK_MRU" ), desk );
  connect( mru, SIGNAL( activated( QString ) ), this, SLOT( onMRUActivated( QString ) ) );
  registerAction( MRUId, mru );

  //! default icon for neutral point ('SALOME' module)
  QPixmap defIcon = resMgr->loadPixmap( "SalomeApp", tr( "APP_DEFAULT_ICO" ), false );
  if ( defIcon.isNull() )
    defIcon = QPixmap( imageEmptyIcon );

  //! default icon for any module
  QPixmap modIcon = resMgr->loadPixmap( "SalomeApp", tr( "APP_MODULE_ICO" ), false );
  if ( modIcon.isNull() )
    modIcon = QPixmap( imageEmptyIcon );

  QToolBar* modTBar = new QtxToolBar( true, desk );
  modTBar->setLabel( tr( "INF_TOOLBAR_MODULES" ) );

  QActionGroup* modGroup = new QActionGroup( this );
  modGroup->setExclusive( true );
  modGroup->setUsesDropDown( true );

  QAction* a = createAction( -1, tr( "APP_NAME" ), defIcon, tr( "APP_NAME" ),
                             tr( "PRP_APP_MODULE" ), 0, desk, true );
  modGroup->add( a );
  myActions.insert( QString(), a );

  QMap<QString, QString> iconMap;
  moduleIconNames( iconMap );

  const int iconSize = 20;

  modGroup->addTo( modTBar );
  modTBar->addSeparator();

  QStringList modList;
  modules( modList, false );

  for ( QStringList::Iterator it = modList.begin(); it != modList.end(); ++it )
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

  //! New window

  int windowMenu = createMenu( tr( "MEN_DESK_WINDOW" ), -1, 100 );
  int newWinMenu = createMenu( tr( "MEN_DESK_NEWWINDOW" ), windowMenu, -1, 0 );
  createMenu( separator(), windowMenu, -1, 1 );

  QMap<int, int> accelMap;
  accelMap[NewGLViewId]  = ALT+Key_G;
  accelMap[NewPlot2dId]  = ALT+Key_P;
  accelMap[NewOCCViewId] = ALT+Key_O;
  accelMap[NewVTKViewId] = ALT+Key_K;

  for ( int id = NewGLViewId; id <= NewVTKViewId; id++ )
  {
    QAction* a = createAction( id, tr( QString( "NEW_WINDOW_%1" ).arg( id - NewGLViewId ) ), QIconSet(),
			       tr( QString( "NEW_WINDOW_%1" ).arg( id - NewGLViewId ) ),
			       tr( QString( "NEW_WINDOW_%1" ).arg( id - NewGLViewId ) ),
			       accelMap.contains( id ) ? accelMap[id] : 0, desk, false, this, SLOT( onNewWindow() ) );
    createMenu( a, newWinMenu, -1 );
  }
  connect( modGroup, SIGNAL( selected( QAction* ) ), this, SLOT( onModuleActivation( QAction* ) ) );

  //! Help for modules
  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1, 1000 );
  int helpModuleMenu = createMenu( tr( "MEN_DESK_MODULE_HELP" ), helpMenu, -1, 0 );
  createMenu( separator(), helpMenu, -1, 1 );

  QStringList aModuleList;
  modules( aModuleList, false );
  
  int id = SalomeApp_Application::UserID;
  // help for KERNEL and GUI
  QCString dir;
  if (dir = getenv("GUI_ROOT_DIR")) {
    a = createAction( id, tr( QString("Kernel & GUI Help") ), QIconSet(),
		      tr( QString("Kernel && GUI Help") ),
		      tr( QString("Kernel & GUI Help") ),
		      0, desk, false, this, SLOT( onHelpContentsModule() ) );
    a->setName( QString("GUI") );
    createMenu( a, helpModuleMenu, -1 );
    id++;
  }
  // help for other existing modules
  for ( QStringList::Iterator it = aModuleList.begin(); it != aModuleList.end(); ++it )
  {
    if ( (*it).isEmpty() )
      continue;

    QString modName = moduleName( *it );
    if ( modName.compare("GEOM") == 0 ) { // to be removed when documentation for other modules will be done
      QAction* a = createAction( id, tr( moduleTitle(modName) + QString(" Help") ), QIconSet(),
				 tr( moduleTitle(modName) + QString(" Help") ),
				 tr( moduleTitle(modName) + QString(" Help") ),
				 0, desk, false, this, SLOT( onHelpContentsModule() ) );
      a->setName( modName );
      createMenu( a, helpModuleMenu, -1 );
      id++;
    }
  }

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );

  createMenu( DumpStudyId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );
  createMenu( LoadScriptId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );
  createMenu( PropertiesId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );
  createMenu( PreferencesId, fileMenu, 15, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );

  int toolsMenu = createMenu( tr( "MEN_DESK_TOOLS" ), -1, -1, 50 );
  createMenu( CatalogGenId, toolsMenu, 10, -1 );
  createMenu( RegDisplayId, toolsMenu, 10, -1 );
  createMenu( separator(), toolsMenu, -1, 15, -1 );

  
  /*
  createMenu( separator(), fileMenu, -1, 100, -1 );
  createMenu( MRUId, fileMenu, 100, -1 );
  createMenu( separator(), fileMenu, -1, 100, -1 );
  */
}

/*!On module activation action.*/
void SalomeApp_Application::onModuleActivation( QAction* a )
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
    icon = resourceMgr()->loadPixmap( "SalomeApp", tr( "APP_MODULE_BIG_ICO" ), false ); // default icon for any module

  bool cancelled = false;
  while ( !modName.isEmpty() && !activeStudy() && !cancelled ){
    SalomeApp_ModuleDlg aDlg( desktop(), modName, icon );
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
QString SalomeApp_Application::defaultModule() const
{
  QStringList aModuleNames;
  modules( aModuleNames, false ); // obtain a complete list of module names for the current configuration
  //! If there's the one and only module --> activate it automatically
  //! TODO: Possible improvement - default module can be taken from preferences
  return aModuleNames.count() > 1 ? "" : ( aModuleNames.count() ? aModuleNames.first() : "" );
}

/*!On new window slot.*/
void SalomeApp_Application::onNewWindow()
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
    type = SVTK_Viewer::Type();
    break;
  }

  if ( !type.isEmpty() )
    createViewManager( type );
}

//=======================================================================
//  name    : onNewDoc
/*! Purpose : SLOT. Create new document*/
//=======================================================================
void SalomeApp_Application::onNewDoc()
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
void SalomeApp_Application::onOpenDoc()
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

/*! Purpose : SLOT. Open new document with \a aName.*/
bool SalomeApp_Application::onOpenDoc( const QString& aName )
{
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

/*!SLOT. Load document.*/
void SalomeApp_Application::onLoadDoc()
{
  QString name, studyname, ext;

  STD_LoadStudiesDlg aDlg( desktop(), TRUE);

  std::vector<std::string> List = studyMgr()->GetOpenStudies();

  SUIT_Session* aSession = SUIT_Session::session();
  QPtrList<SUIT_Application> aAppList = aSession->applications();
  SUIT_Application* aApp = 0;

  for (unsigned int ind = 0; ind < List.size(); ind++) {
     studyname = List[ind].c_str();
     //Add to list only unloaded studies
     bool isAlreadyOpen = false;
     for ( QPtrListIterator<SUIT_Application> it( aAppList ); it.current() && !isAlreadyOpen; ++it )
       {
	 aApp = it.current();
	 if(!aApp || !aApp->activeStudy()) continue;
	 if ( aApp->activeStudy()->studyName() == studyname ) isAlreadyOpen = true;
       }

     if ( !isAlreadyOpen ) aDlg.ListComponent->insertItem( studyname );
  }

  int retVal = aDlg.exec();
  studyname = aDlg.ListComponent->currentText();

  if (retVal == QDialog::Rejected)
    return;

  if ( studyname.isNull() || studyname.isEmpty() )
    return;

  name = studyname;
  name.replace( QRegExp(":"), "/" );

  if(onLoadDoc(name)) {
     updateWindows();
     updateViewManagers();
     updateObjectBrowser(true);
  }
}


/*!SLOT. Load document with \a aName.*/
bool SalomeApp_Application::onLoadDoc( const QString& aName )
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
void SalomeApp_Application::onSelection()
{
  onSelectionChanged();

  if ( activeModule() && activeModule()->inherits( "SalomeApp_Module" ) )
    ((SalomeApp_Module*)activeModule())->selectionChanged();
}

/*!SLOT. Copy objects to study maneger from selection maneger..*/
void SalomeApp_Application::onCopy()
{
  SALOME_ListIO list;
  SalomeApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(list);

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
  if(study == NULL) return;

  _PTR(Study) stdDS = study->studyDS();
  if(!stdDS) return;

  SALOME_ListIteratorOfListIO it( list );
  if(it.More())
    {
      _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());
      try {
	studyMgr()->Copy(so);
	onSelectionChanged();
      }
      catch(...) {
      }
    }
}

/*!SLOT. Paste objects to study maneger from selection manager.*/
void SalomeApp_Application::onPaste()
{
  SALOME_ListIO list;
  SalomeApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(list);

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
  if(study == NULL) return;

  _PTR(Study) stdDS = study->studyDS();
  if(!stdDS) return;

  SALOME_ListIteratorOfListIO it( list );
  if(it.More())
    {
      _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());
      try {
	studyMgr()->Paste(so);
	updateObjectBrowser( true );
	updateActions(); //SRN: BugID IPAL9377, case 3
      }
      catch(...) {
      }
    }
}

/*!Sets enable or disable some actions on selection changed.*/
void SalomeApp_Application::onSelectionChanged()
{
   SALOME_ListIO list;
   SalomeApp_SelectionMgr* mgr = selectionMgr();
   mgr->selectedObjects(list);

   SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
   if(study == NULL) return;

   _PTR(Study) stdDS = study->studyDS();
   if(!stdDS) return;

   QAction* qaction;

   SALOME_ListIteratorOfListIO it( list );
   if(it.More() && list.Extent() == 1)
   {
      _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());

      qaction = action(EditCopyId);
      if( so && studyMgr()->CanCopy(so) ) qaction->setEnabled(true);
      else qaction->setEnabled(false);

      qaction = action(EditPasteId);
      if( so && studyMgr()->CanPaste(so) ) qaction->setEnabled(true);
      else qaction->setEnabled(false);
   }
   else {
     qaction = action(EditCopyId);
     qaction->setEnabled(false);
     qaction = action(EditPasteId);
     qaction->setEnabled(false);
   }
}

/*!Update object browser.*/
void SalomeApp_Application::onRefresh()
{
  updateObjectBrowser( true );
}

/*!Delete references.*/
void SalomeApp_Application::onDeleteReferences()
{
  SALOME_ListIO aList;
  SalomeApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(aList);

  if (aList.Extent() < 1) return;

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(activeStudy());
  _PTR(Study) aStudyDS = aStudy->studyDS();
  _PTR(StudyBuilder) aStudyBuilder = aStudyDS->NewBuilder();
  _PTR(SObject) anObj;

  for ( SALOME_ListIteratorOfListIO it( aList ); it.More(); it.Next() )
    {
      if ( it.Value()->hasEntry() )
	{
	  _PTR(SObject) aSObject = aStudyDS->FindObjectID( it.Value()->getEntry() );
	  if ( aSObject->ReferencedObject(anObj) )
	    aStudyBuilder->RemoveReference(aSObject);
	}
    }

  updateObjectBrowser();
}

/*!Private SLOT. */
void SalomeApp_Application::onOpenWith()
{
  QApplication::setOverrideCursor( Qt::waitCursor );
  SALOME_ListIO aList;
  SalomeApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(aList);
  if (aList.Extent() != 1)
    {
      QApplication::restoreOverrideCursor();
      return;
    }
  Handle(SALOME_InteractiveObject) aIObj = aList.First();
  QString aModuleName(aIObj->getComponentDataType());
  QString aModuleTitle = moduleTitle(aModuleName);
  activateModule(aModuleTitle);
  QApplication::restoreOverrideCursor();
}

bool SalomeApp_Application::useStudy(const QString& theName)
{
  createEmptyStudy();
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(activeStudy());
  bool res = false;
  if (aStudy)
    res = aStudy->loadDocument( theName );
  updateDesktopTitle();
  updateCommandsStatus();
  return res;
}

/*!Set active study.
 *\param study - SUIT_Study.
 */
void SalomeApp_Application::setActiveStudy( SUIT_Study* study )
{
  CAM_Application::setActiveStudy( study );

  activateWindows();
}

//=======================================================================
// name    : createNewStudy
/*! Purpose : Create new study*/
//=======================================================================
SUIT_Study* SalomeApp_Application::createNewStudy()
{
  SalomeApp_Study* aStudy = new SalomeApp_Study( this );

  // Set up processing of major study-related events
  connect( aStudy, SIGNAL( created( SUIT_Study* ) ), this, SLOT( onStudyCreated( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( opened ( SUIT_Study* ) ), this, SLOT( onStudyOpened ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( saved  ( SUIT_Study* ) ), this, SLOT( onStudySaved  ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( closed ( SUIT_Study* ) ), this, SLOT( onStudyClosed ( SUIT_Study* ) ) );

  return aStudy;
}

//=======================================================================
// name    : createNewStudy
/*! Purpose : Enable/Disable menu items and toolbar buttons. Rebuild menu*/
//=======================================================================
void SalomeApp_Application::updateCommandsStatus()
{
  CAM_Application::updateCommandsStatus();

  for ( int id = NewGLViewId; id <= NewVTKViewId; id++ )
  {
    QAction* a = action( id );
    if ( a )
      a->setEnabled( activeStudy() );
  }

  // Dump study menu
  QAction* a = action( DumpStudyId );
  if ( a )
    a->setEnabled( activeStudy() );

  // Load script menu
  a = action( LoadScriptId );
  if ( a )
    a->setEnabled( activeStudy() );

  a = action( PropertiesId );
  if( a )
    a->setEnabled( activeStudy() );

  a = action(EditCopyId);
  a->setEnabled(false);
  a = action(EditPasteId);
  a->setEnabled(false);
}

//=======================================================================
// name    : onHelpAbout
/*! Purpose : SLOT. Display "About" message box*/
//=======================================================================
void SalomeApp_Application::onHelpAbout()
{
  SalomeApp_AboutDlg* dlg = new SalomeApp_AboutDlg( applicationName(), applicationVersion(), desktop() );
  dlg->exec();
  delete dlg;
}

// Helps to execute command
class RunBrowser: public QThread {
public:
  
  RunBrowser(QString theApp, QString theParams, QString theHelpFile, QString theContext=NULL): 
    myApp(theApp), myParams(theParams), myHelpFile("file:" + theHelpFile + theContext), myStatus(0) {};
  
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
    
    if( myStatus != 0 || myApp.isEmpty())
      {
	myParams = "";
	aCommand.sprintf("%s %s %s", QString(DEFAULT_BROWSER).latin1(),myParams.latin1(), myHelpFile.latin1());	
	myStatus = system(aCommand);
	if(myStatus != 0)
	  {
	    QCustomEvent* ce2001 = new QCustomEvent (2001);
	    postEvent (qApp, ce2001);
	  }
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
void SalomeApp_Application::onHelpContentsModule()
{
  const QAction* obj = (QAction*) sender();
  
  QString aComponentName = obj->name();
  QString aFileName = aComponentName.lower() + ".htm";

  QCString dir;
  QString root;
  QString homeDir;
  if (dir = getenv( aComponentName + "_ROOT_DIR")) {
    root = Qtx::addSlash( Qtx::addSlash(dir) +  Qtx::addSlash("doc") +  Qtx::addSlash("salome")  +  Qtx::addSlash(aComponentName));
    if ( QFileInfo( root + aFileName ).exists() ) {
      homeDir = root;
    } else {
      SUIT_MessageBox::warn1( desktop(), tr("WRN_WARNING"), 
			      QString( "%1"+ aFileName + " doesn't exist." ).arg(root), tr ("BUT_OK") );
      return;
    }
  }

  QString helpFile = QFileInfo( homeDir + aFileName ).absFilePath();   
  SUIT_ResourceMgr* resMgr = resourceMgr();
  QString anApp = resMgr->stringValue("ExternalBrowser", "application");
  QString aParams = resMgr->stringValue("ExternalBrowser", "parameters");
   
  RunBrowser* rs = new RunBrowser(anApp, aParams, helpFile);
  rs->start();
}

QWidget* SalomeApp_Application::window( const int flag, const int studyId ) const
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
void SalomeApp_Application::addWindow( QWidget* wid, const int flag, const int studyId )
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

    myWindows.insert( flag, new SalomeApp_WidgetContainer( flag, desktop() ) );
    if ( winMap.contains( flag ) )
      desktop()->moveDockWindow( myWindows[flag], (Dock)winMap[flag] );

    myWindows[flag]->setResizeEnabled( true );
    myWindows[flag]->setCloseMode( QDockWindow::Always );
    myWindows[flag]->setName( QString( "dock_window_%1" ).arg( flag ) );
  }

  QFont f;
  if( wid->inherits( "PythonConsole" ) )
    f = ( ( PythonConsole* )wid )->font();
  else
    f = wid->font();

  myWindows[flag]->insert( sId, wid );
  wid->setFont( f );

  setWindowShown( flag, !myWindows[flag]->isEmpty() );
}

/*!Remove window from application.
 *\param flag - key wor window
 *\param studyId - study id
 * Flag used how identificator of window in windows list.
 */
void SalomeApp_Application::removeWindow( const int flag, const int studyId )
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
QWidget* SalomeApp_Application::getWindow( const int flag, const int studyId )
{
  QWidget* wid = window( flag, studyId );
  if ( !wid )
    addWindow( wid = createWindow( flag ), flag, studyId );

  return wid;
}

/*!Check is window visible?(with identificator \a type)*/
bool SalomeApp_Application::isWindowVisible( const int type ) const
{
  bool res = false;
  if ( myWindows.contains( type ) )
  {
    SUIT_Desktop* desk = ((SalomeApp_Application*)this)->desktop();
    res = desk && desk->appropriate( myWindows[type] );
  }
  return res;
}

/*!Sets window show or hide.
 *\param type - window identificator.
 *\param on   - true/false (window show/hide)
 */
void SalomeApp_Application::setWindowShown( const int type, const bool on )
{
  if ( !desktop() || !myWindows.contains( type ) )
    return;

  QDockWindow* dw = myWindows[type];
  desktop()->setAppropriate( dw, on );
  on ? dw->show() : dw->hide();
}

OB_Browser* SalomeApp_Application::objectBrowser()
{
  OB_Browser* ob = 0;
  QWidget* wid = getWindow( WT_ObjectBrowser );
  if ( wid->inherits( "OB_Browser" ) )
    ob = (OB_Browser*)wid;
  return ob;
}

/*!Gets "LogWindow".*/
LogWindow* SalomeApp_Application::logWindow()
{
  LogWindow* lw = 0;
  QWidget* wid = getWindow( WT_LogWindow );
  if ( wid->inherits( "LogWindow" ) )
    lw = (LogWindow*)wid;
  return lw;
}

/*!Get "PythonConsole"*/
PythonConsole* SalomeApp_Application::pythonConsole()
{
  PythonConsole* console = 0;
  QWidget* wid = getWindow( WT_PyConsole );
  if ( wid->inherits( "PythonConsole" ) )
    console = (PythonConsole*)wid;
  return console;
}

/*!Gets preferences.*/
SalomeApp_Preferences* SalomeApp_Application::preferences() const
{
  return preferences( false );
}

/*!Gets view manager*/
SUIT_ViewManager* SalomeApp_Application::getViewManager( const QString& vmType, const bool create )
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
SUIT_ViewManager* SalomeApp_Application::createViewManager( const QString& vmType )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

  SUIT_ViewManager* viewMgr = 0;
  if ( vmType == GLViewer_Viewer::Type() )
  {
    viewMgr = new GLViewer_ViewManager( activeStudy(), desktop() );
    new SalomeApp_GLSelector( (GLViewer_Viewer2d*)viewMgr->getViewModel(), mySelMgr );
  }
  else if ( vmType == Plot2d_Viewer::Type() )
  {
    viewMgr = new Plot2d_ViewManager( activeStudy(), desktop() );
    viewMgr->setViewModel( new SPlot2d_Viewer() );// custom view model, which extends SALOME_View interface
  }
  else if ( vmType == OCCViewer_Viewer::Type() )
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
    new SalomeApp_OCCSelector( (OCCViewer_Viewer*)viewMgr->getViewModel(), mySelMgr );
  }
  else if ( vmType == SVTK_Viewer::Type() )
  {
    viewMgr = new SVTK_ViewManager( activeStudy(), desktop() );
    SVTK_Viewer* vm = dynamic_cast<SVTK_Viewer*>( viewMgr->getViewModel() );
    if( vm )
    {
      vm->setBackgroundColor( resMgr->colorValue( "VTKViewer", "background", vm->backgroundColor() ) );
      vm->setTrihedronSize( resMgr->integerValue( "VTKViewer", "trihedron_size", vm->trihedronSize() ) );
      new SalomeApp_VTKSelector( vm, mySelMgr );
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

void SalomeApp_Application::onCloseView( SUIT_ViewManager* theVM )
{
  removeViewManager( theVM );
}

/*!Private SLOT. On study created.*/
void SalomeApp_Application::onStudyCreated( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->setName( tr( "DATA_MODELS" ) );
  }
  if ( objectBrowser() != 0 )
    objectBrowser()->setRootObject( aRoot );

  activateModule( defaultModule() );

  activateWindows();
}

/*!Private SLOT. On study opened.*/
void SalomeApp_Application::onStudyOpened( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->dump();
  }
  if ( objectBrowser() != 0 ) {
    objectBrowser()->setRootObject( aRoot );
  }

  activateModule( defaultModule() );

  activateWindows();

  emit studyOpened();
}

void SalomeApp_Application::onStudySaved( SUIT_Study* )
{
  emit studySaved();
}

/*!Private SLOT. On study closed.*/
void SalomeApp_Application::onStudyClosed( SUIT_Study* )
{
  emit studyClosed();

  activateModule( "" );

  saveWindowsGeometry();
}

/*!Private SLOT. On dump study.*/
void SalomeApp_Application::onDumpStudy( )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  QStringList aFilters;
  aFilters.append( tr( "PYTHON_FILES_FILTER" ) );

  SalomeApp_CheckFileDlg* fd = new SalomeApp_CheckFileDlg( desktop(), false, tr("PUBLISH_IN_STUDY"), true, true);
  fd->setCaption( tr( "TOT_DESK_FILE_DUMP_STUDY" ) );
  fd->setFilters( aFilters );
  fd->SetChecked(true);
  fd->exec();
  QString aFileName = fd->selectedFile();
  bool toPublish = fd->IsChecked();
  delete fd;

  if(!aFileName.isEmpty()) {
    QFileInfo aFileInfo(aFileName);
    aStudy->DumpStudy( aFileInfo.dirPath( true ).latin1(), aFileInfo.baseName().latin1(), toPublish );
  }
}

/*!Private SLOT. On load script.*/
void SalomeApp_Application::onLoadScript( )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  if ( aStudy->GetProperties()->IsLocked() ) {
    SUIT_MessageBox::warn1 ( desktop(),
			     QObject::tr("WRN_WARNING"),
			     QObject::tr("WRN_STUDY_LOCKED"),
			     QObject::tr("BUT_OK") );
    return;
  }

  QStringList filtersList;
  filtersList.append(tr("PYTHON_FILES_FILTER"));
  filtersList.append(tr("ALL_FILES_FILTER"));

  QString aFile = SUIT_FileDlg::getFileName( desktop(), "", filtersList, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), true, true );

  if ( !aFile.isEmpty() )
  {
    QString command = QString("execfile(\"%1\")").arg(aFile);

    PythonConsole* pyConsole = pythonConsole();

    if ( pyConsole )
      pyConsole->exec( command );
  }
}

/*!Private SLOT. On preferences.*/
void SalomeApp_Application::onPreferences()
{
  QApplication::setOverrideCursor( Qt::waitCursor );

  SalomeApp_PreferencesDlg* prefDlg = new SalomeApp_PreferencesDlg( preferences( true ), desktop());

  QApplication::restoreOverrideCursor();

  if ( !prefDlg )
    return;

  prefDlg->exec();

  delete prefDlg;
}

/*!Private SLOT. On open document with name \a aName.*/
void SalomeApp_Application::onMRUActivated( QString aName )
{
  onOpenDoc( aName );
}

/*!Private SLOT. On preferences changed.*/
void SalomeApp_Application::onPreferenceChanged( QString& modName, QString& section, QString& param )
{
  SalomeApp_Module* sMod = 0;
  CAM_Module* mod = module( modName );
  if ( mod && mod->inherits( "SalomeApp_Module" ) )
    sMod = (SalomeApp_Module*)mod;

  if ( sMod )
    sMod->preferencesChanged( section, param );
  else
    preferencesChanged( section, param );
}

/*!Gets file filter.
 *\retval QString "(*.hdf)"
 */
QString SalomeApp_Application::getFileFilter() const
{
  return "(*.hdf)";
}

/*!Remove all windows from study.*/
void SalomeApp_Application::beforeCloseDoc( SUIT_Study* s )
{
  CAM_Application::beforeCloseDoc( s );

  for ( WindowMap::ConstIterator itr = myWindows.begin(); s && itr != myWindows.end(); ++itr )
    removeWindow( itr.key(), s->id() );
}

/*!Update actions.*/
void SalomeApp_Application::updateActions()
{
  updateCommandsStatus();
}

/*!Create window.*/
QWidget* SalomeApp_Application::createWindow( const int flag )
{
  QWidget* wid = 0;

  SUIT_ResourceMgr* resMgr = resourceMgr();

  if ( flag == WT_ObjectBrowser )
  {
    OB_Browser* ob = new OB_Browser( desktop() );
    ob->setAutoUpdate( true );
    ob->setAutoOpenLevel( 1 );
    ob->setCaption( tr( "OBJECT_BROWSER" ) );
    ob->resize( OBJECT_BROWSER_WIDTH, ob->height() );
    ob->setFilter( new SalomeApp_OBFilter( selectionMgr() ) );

    ob->setNameTitle( tr( "OBJ_BROWSER_NAME" ) );

    bool autoSize = resMgr->booleanValue( "ObjectBrowser", "auto_size", false );
    for ( int i = SalomeApp_DataObject::CT_Value; i <= SalomeApp_DataObject::CT_RefEntry; i++ )
    {
      ob->addColumn( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i ) ), i );
      ob->setColumnShown( i, resMgr->booleanValue( "ObjectBrowser",
                                                   QString().sprintf( "visibility_column_%d", i ), true ) );
    }
    ob->setWidthMode( autoSize ? QListView::Maximum : QListView::Manual );

    // Create OBSelector
    new SalomeApp_OBSelector( ob, mySelMgr );

    wid = ob;

    ob->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
  else if ( flag == WT_PyConsole )
  {
    PythonConsole* pyCons = new PythonConsole( desktop(), new SalomeApp_PyInterp() );
    pyCons->setCaption( tr( "PYTHON_CONSOLE" ) );
    pyCons->setFont( resMgr->fontValue( "PyConsole", "font" ) );
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
void SalomeApp_Application::defaultWindows( QMap<int, int>& aMap ) const
{
  aMap.insert( WT_ObjectBrowser, Qt::DockLeft );
  aMap.insert( WT_PyConsole, Qt::DockBottom );
  //  aMap.insert( WT_LogWindow, Qt::DockBottom );
}

/*!Default view manager.*/
void SalomeApp_Application::defaultViewManagers( QStringList& ) const
{
  /*!Do nothing.*/
}

/*!Gets preferences.
 * Create preferences, if \a crt = true.
 */
SalomeApp_Preferences* SalomeApp_Application::preferences( const bool crt ) const
{
  if ( myPrefs )
    return myPrefs;

  SalomeApp_Application* that = (SalomeApp_Application*)this;

  if ( !_prefs_ && crt )
  {
    _prefs_ = new SalomeApp_Preferences( resourceMgr() );
    that->createPreferences( _prefs_ );
  }

  that->myPrefs = _prefs_;

  QPtrList<SUIT_Application> appList = SUIT_Session::session()->applications();
  for ( QPtrListIterator<SUIT_Application> appIt ( appList ); appIt.current(); ++appIt )
  {
    if ( !appIt.current()->inherits( "SalomeApp_Application" ) )
      continue;

    SalomeApp_Application* app = (SalomeApp_Application*)appIt.current();

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
      SalomeApp_Module* mod = 0;
      if ( itr.current()->inherits( "SalomeApp_Module" ) )
	mod = (SalomeApp_Module*)itr.current();

      if ( mod && !_prefs_->hasModule( mod->moduleName() ) )
      {
	int modCat = _prefs_->addPreference( mod->moduleName() );
	_prefs_->setItemProperty( modCat, "info", QString::null );
	mod->createPreferences();
      }
    }
  }

  connect( myPrefs, SIGNAL( preferenceChanged( QString&, QString&, QString& ) ),
           this, SLOT( onPreferenceChanged( QString&, QString&, QString& ) ) );

  return myPrefs;
}

/*!Add new module to application.*/
void SalomeApp_Application::moduleAdded( CAM_Module* mod )
{
  CAM_Application::moduleAdded( mod );

  SalomeApp_Module* salomeMod = 0;
  if ( mod && mod->inherits( "SalomeApp_Module" ) )
    salomeMod = (SalomeApp_Module*)mod;

  if ( myPrefs && salomeMod && !myPrefs->hasModule( salomeMod->moduleName() ) )
  {
    int modCat = myPrefs->addPreference( mod->moduleName() );
    myPrefs->setItemProperty( modCat, "info", QString::null );
    salomeMod->createPreferences();
  }
}

/*!Create preferences.*/
void SalomeApp_Application::createPreferences( SalomeApp_Preferences* pref )
{
  if ( !pref )
    return;

  int salomeCat = pref->addPreference( tr( "PREF_CATEGORY_SALOME" ) );

  int genTab = pref->addPreference( tr( "PREF_TAB_GENERAL" ), salomeCat );
  int studyGroup = pref->addPreference( tr( "PREF_GROUP_STUDY" ), genTab );
  pref->setItemProperty( studyGroup, "columns", 1 );

  pref->addPreference( tr( "PREF_MULTI_FILE" ), studyGroup, SalomeApp_Preferences::Bool, "Study", "multi_file" );
  pref->addPreference( tr( "PREF_ASCII_FILE" ), studyGroup, SalomeApp_Preferences::Bool, "Study", "ascii_file" );
  int undoPref = pref->addPreference( tr( "PREF_UNDO_LEVEL" ), studyGroup, SalomeApp_Preferences::IntSpin, "Study", "undo_level" );
  pref->setItemProperty( undoPref, "min", 1 );
  pref->setItemProperty( undoPref, "max", 100 );

  int extgroup = pref->addPreference( tr( "PREF_GROUP_EXT_BROWSER" ), genTab );
  pref->setItemProperty( extgroup, "columns", 1 );
  int apppref = pref->addPreference( tr( "PREF_APP" ), extgroup, SalomeApp_Preferences::File, "ExternalBrowser", "application" );
  pref->setItemProperty( apppref, "existing", true );
  pref->setItemProperty( apppref, "flags", QFileInfo::ExeUser );

  pref->addPreference( tr( "PREF_PARAM" ), extgroup, SalomeApp_Preferences::String, "ExternalBrowser", "parameters" );

  int pythonConsoleGroup = pref->addPreference( tr( "PREF_GROUP_PY_CONSOLE" ), genTab );
  pref->setItemProperty( pythonConsoleGroup, "columns", 1 );
  pref->addPreference( tr( "PREF_FONT" ), pythonConsoleGroup, SalomeApp_Preferences::Font, "PyConsole", "font" );



  int obTab = pref->addPreference( tr( "PREF_TAB_OBJBROWSER" ), salomeCat );
  int defCols = pref->addPreference( tr( "PREF_GROUP_DEF_COLUMNS" ), obTab );
  for ( int i = SalomeApp_DataObject::CT_Value; i <= SalomeApp_DataObject::CT_RefEntry; i++ )
  {
    pref->addPreference( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i ) ), defCols,
                         SalomeApp_Preferences::Bool, "ObjectBrowser", QString().sprintf( "visibility_column_%d", i ) );
  }
  pref->setItemProperty( defCols, "columns", 1 );

  int objSetGroup = pref->addPreference( tr( "PREF_OBJ_BROWSER_SETTINGS" ), obTab );
  pref->addPreference( tr( "PREF_AUTO_SIZE" ), objSetGroup, SalomeApp_Preferences::Bool, "ObjectBrowser", "auto_size" );

  int viewTab = pref->addPreference( tr( "PREF_TAB_VIEWERS" ), salomeCat );

  int occGroup = pref->addPreference( tr( "PREF_GROUP_OCCVIEWER" ), viewTab );

  int vtkGroup = pref->addPreference( tr( "PREF_GROUP_VTKVIEWER" ), viewTab );

  int plot2dGroup = pref->addPreference( tr( "PREF_GROUP_PLOT2DVIEWER" ), viewTab );

  pref->setItemProperty( occGroup, "columns", 1 );
  pref->setItemProperty( vtkGroup, "columns", 1 );
  pref->setItemProperty( plot2dGroup, "columns", 1 );

  int occTS = pref->addPreference( tr( "PREF_TRIHEDRON_SIZE" ), occGroup,
				   SalomeApp_Preferences::IntSpin, "OCCViewer", "trihedron_size" );
  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), occGroup,
		       SalomeApp_Preferences::Color, "OCCViewer", "background" );

  pref->setItemProperty( occTS, "min", 1 );
  pref->setItemProperty( occTS, "max", 150 );

  int isoU = pref->addPreference( tr( "PREF_ISOS_U" ), occGroup,
				  SalomeApp_Preferences::IntSpin, "OCCViewer", "iso_number_u" );
  int isoV = pref->addPreference( tr( "PREF_ISOS_V" ), occGroup,
				  SalomeApp_Preferences::IntSpin, "OCCViewer", "iso_number_v" );

  pref->setItemProperty( isoU, "min", 0 );
  pref->setItemProperty( isoU, "max", 100000 );

  pref->setItemProperty( isoV, "min", 0 );
  pref->setItemProperty( isoV, "max", 100000 );

  int vtkTS = pref->addPreference( tr( "PREF_TRIHEDRON_SIZE" ), vtkGroup,
				   SalomeApp_Preferences::IntSpin, "VTKViewer", "trihedron_size" );
  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), vtkGroup,
		       SalomeApp_Preferences::Color, "VTKViewer", "background" );

  pref->setItemProperty( vtkTS, "min", 1 );
  pref->setItemProperty( vtkTS, "max", 150 );

  pref->addPreference( tr( "PREF_SHOW_LEGEND" ), plot2dGroup,
		       SalomeApp_Preferences::Bool, "Plot2d", "ShowLegend" );

  int legendPosition = pref->addPreference( tr( "PREF_LEGEND_POSITION" ), plot2dGroup,
					    SalomeApp_Preferences::Selector, "Plot2d", "LegendPos" );
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
				       SalomeApp_Preferences::Selector, "Plot2d", "CurveType" );
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
					SalomeApp_Preferences::IntSpin, "Plot2d", "MarkerSize" );

  pref->setItemProperty( markerSize, "min", 0 );
  pref->setItemProperty( markerSize, "max", 100 );

  QStringList aScaleModesList;
  aScaleModesList.append( tr("PREF_LINEAR") );
  aScaleModesList.append( tr("PREF_LOGARITHMIC") );

  anIndexesList.clear();
  anIndexesList.append(0);
  anIndexesList.append(1);

  int horScale = pref->addPreference( tr( "PREF_HOR_AXIS_SCALE" ), plot2dGroup,
				      SalomeApp_Preferences::Selector, "Plot2d", "HorScaleMode" );

  pref->setItemProperty( horScale, "strings", aScaleModesList );
  pref->setItemProperty( horScale, "indexes", anIndexesList );

  int verScale = pref->addPreference( tr( "PREF_VERT_AXIS_SCALE" ), plot2dGroup,
				      SalomeApp_Preferences::Selector, "Plot2d", "VerScaleMode" );

  pref->setItemProperty( verScale, "strings", aScaleModesList );
  pref->setItemProperty( verScale, "indexes", anIndexesList );

  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), plot2dGroup,
		       SalomeApp_Preferences::Color, "Plot2d", "Background" );

  int dirTab = pref->addPreference( tr( "PREF_TAB_DIRECTORIES" ), salomeCat );
  int dirGroup = pref->addPreference( tr( "PREF_GROUP_DIRECTORIES" ), dirTab );
  pref->setItemProperty( dirGroup, "columns", 1 );
  pref->addPreference( tr( "" ), dirGroup,
		       SalomeApp_Preferences::DirList, "FileDlg", "QuickDirList" );
}

void SalomeApp_Application::preferencesChanged( const QString& sec, const QString& param )
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

  if ( sec == QString( "VTKViewer" ) && param == QString( "trihedron_size" ) )
  {
    int sz = resMgr->integerValue( sec, param, -1 );
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
      ((OCCViewer_Viewer*)it.current())->setIsos( u, v );
  }

  if( sec=="ObjectBrowser" )
  {
    if( param=="auto_size" )
    {
      OB_Browser* ob = objectBrowser();
      if( !ob )
	return;

      bool autoSize = resMgr->booleanValue( "ObjectBrowser", "auto_size", false );
      ob->setWidthMode( autoSize ? QListView::Maximum : QListView::Manual );

      updateObjectBrowser( false );
    }
  }

  if( sec=="PyConsole" )
  {
    if( param=="font" )
      if( pythonConsole() )
	pythonConsole()->setFont( resMgr->fontValue( "PyConsole", "font" ) );
  }
}

/*!Update desktop title.*/
void SalomeApp_Application::updateDesktopTitle() {
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  if ( activeStudy() )
  {
    QString sName = SUIT_Tools::file( activeStudy()->studyName().stripWhiteSpace(), false );
    if ( !sName.isEmpty() ) {
      SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
      _PTR(Study) stdDS = study->studyDS();
      if(stdDS) {
	if ( stdDS->GetProperties()->IsLocked() ) {
	  aTitle += QString( " - [%1 (%2)]").arg( sName ).arg( tr( "STUDY_LOCKED" ) );
	} else {
	  aTitle += QString( " - [%1]" ).arg( sName );
	}
      }
    }
  }

  desktop()->setCaption( aTitle );
}

/*!Update windows after close document.*/
void SalomeApp_Application::afterCloseDoc()
{
  updateWindows();

  CAM_Application::afterCloseDoc();
}

/*!Gets CORBA::ORB_var*/
CORBA::ORB_var SalomeApp_Application::orb()
{
  ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
  static CORBA::ORB_var _orb = init( qApp->argc(), qApp->argv() );
  return _orb;
}

/*!Create and return SALOMEDS_StudyManager.*/
SALOMEDSClient_StudyManager* SalomeApp_Application::studyMgr()
{
  static SALOMEDSClient_StudyManager* _sm = new SALOMEDS_StudyManager();
  return _sm;
}

/*!Create and return SALOME_NamingService.*/
SALOME_NamingService* SalomeApp_Application::namingService()
{
  static SALOME_NamingService* _ns = new SALOME_NamingService( orb() );
  return _ns;
}

/*!Create and return SALOME_LifeCycleCORBA.*/
SALOME_LifeCycleCORBA* SalomeApp_Application::lcc()
{
  static SALOME_LifeCycleCORBA* _lcc = new SALOME_LifeCycleCORBA( namingService() );
  return _lcc;
}

QString SalomeApp_Application::defaultEngineIOR()
{
  /// Look for a default module engine (needed for CORBAless modules to use SALOMEDS persistence)
  QString anIOR( "" );
  CORBA::Object_ptr anEngine = namingService()->Resolve( "/SalomeAppEngine" );
  if ( !CORBA::is_nil( anEngine ) )
    anIOR = orb()->object_to_string( anEngine );
  return anIOR;
}

/*!Adds icon names for modules.*/
void SalomeApp_Application::moduleIconNames( QMap<QString, QString>& iconMap ) const
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

/*!Update module action.*/
void SalomeApp_Application::updateModuleActions()
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
void SalomeApp_Application::currentWindows( QMap<int, int>& winMap ) const
{
  winMap.clear();
  if ( !activeStudy() )
    return;

  if ( activeModule() && activeModule()->inherits( "SalomeApp_Module" ) )
    ((SalomeApp_Module*)activeModule())->windows( winMap );
  else
    defaultWindows( winMap );
}

/*!Gets current view managers.
 *\param lst - output current view managers list.
 */
void SalomeApp_Application::currentViewManagers( QStringList& lst ) const
{
  lst.clear();
  if ( !activeStudy() )
    return;

  if ( activeModule() && activeModule()->inherits( "SalomeApp_Module" ) )
    ((SalomeApp_Module*)activeModule())->viewManagers( lst );
  else
    defaultViewManagers( lst );
}

/*!Update windows.*/
void SalomeApp_Application::updateWindows()
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
void SalomeApp_Application::updateViewManagers()
{
  QStringList lst;
  currentViewManagers( lst );

  for ( QStringList::const_iterator it = lst.begin(); it != lst.end(); ++it )
    getViewManager( *it, true );
}

/*!Load windows geometry.*/
void SalomeApp_Application::loadWindowsGeometry()
{
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
void SalomeApp_Application::saveWindowsGeometry()
{
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
void SalomeApp_Application::activateWindows()
{
  if ( activeStudy() )
  {
    for ( WindowMap::Iterator itr = myWindows.begin(); itr != myWindows.end(); ++itr )
      itr.data()->activate( activeStudy()->id() );
  }
}

/*!Private SLOT. On preferences.*/
void SalomeApp_Application::onProperties()
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if( !study )
    return;

  _PTR(StudyBuilder) SB = study->studyDS()->NewBuilder();
  SB->NewCommand();

  SalomeApp_StudyPropertiesDlg aDlg( desktop() );
  int res = aDlg.exec();
  if( res==QDialog::Accepted && aDlg.isChanged() )
    SB->CommitCommand();
  else
    SB->AbortCommand();

  //study->updateCaptions();
  updateDesktopTitle();
  updateActions();
}

/*!*/
QString SalomeApp_Application::getFileName( bool open, const QString& initial, const QString& filters,
					    const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  QStringList fls = QStringList::split( ";;", filters, false );
  return SUIT_FileDlg::getFileName( parent, initial, fls, caption, open, true );
}

/*!*/
QString SalomeApp_Application::getDirectory( const QString& initial, const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  return SUIT_FileDlg::getExistingDirectory( parent, initial, caption, true );
}

/*!*/
QStringList SalomeApp_Application::getOpenFileNames( const QString& initial, const QString& filters,
						     const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  QStringList fls = QStringList::split( ";;", filters, false );
  return SUIT_FileDlg::getOpenFileNames( parent, initial, fls, caption, true );
}

/*!*/
void SalomeApp_Application::contextMenuPopup( const QString& type, QPopupMenu* thePopup, QString& title )
{
  CAM_Application::contextMenuPopup( type, thePopup, title );

  OB_Browser* ob = objectBrowser();
  if ( !ob || type != ob->popupClientType() )
    return;

  thePopup->insertSeparator();
  thePopup->insertItem( tr( "MEN_REFRESH" ), this, SLOT( onRefresh() ) );

  // Get selected objects
  SALOME_ListIO aList;
  SalomeApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(aList);

  // "Delete reference" item should appear only for invalid references

  // Check if selected objects is invalid references
  bool isInvalidRefs = true;

  if ( aList.Extent() < 1 )
    isInvalidRefs = false;

  if ( isInvalidRefs )
    {
      SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(activeStudy());
      _PTR(Study) aStudyDS = aStudy->studyDS();
      _PTR(SObject) anObj;

      for ( SALOME_ListIteratorOfListIO it( aList ); it.More() && isInvalidRefs; it.Next() )
	{
	  if ( it.Value()->hasEntry() )
	    {
	      _PTR(SObject) aSObject = aStudyDS->FindObjectID( it.Value()->getEntry() );
	      if ( aSObject->ReferencedObject(anObj) == false || !QString(anObj->GetName().c_str()).isEmpty() )
		isInvalidRefs = false;
	    }
	}
    }

  // Add "Delete refrence" item to popup
  if ( isInvalidRefs )
    {
      thePopup->insertSeparator();
      thePopup->insertItem( tr( "MEN_DELETE_REFERENCE" ), this, SLOT( onDeleteReferences() ) );
      return;
    }

  // "Activate module" item should appear only if it's necessary
  if (aList.Extent() != 1)
    return;
  Handle(SALOME_InteractiveObject) aIObj = aList.First();
  QString aModuleName(aIObj->getComponentDataType());
  QString aModuleTitle = moduleTitle(aModuleName);
  CAM_Module* currentModule = activeModule();
  if (currentModule && currentModule->moduleName() == aModuleTitle)
    return;
  thePopup->insertItem( tr( "MEN_OPENWITH" ), this, SLOT( onOpenWith() ) );

}

/*!Update obect browser*/
void SalomeApp_Application::updateObjectBrowser( const bool updateModels )
{
  // update existing data models (already loaded SComponents)
  if ( updateModels )
  {
    for ( ModuleListIterator it = modules(); it.current(); ++it )
    {
      CAM_DataModel* camDM = it.current()->dataModel();
      if ( camDM && camDM->inherits( "SalomeApp_DataModel" ) )
        ((SalomeApp_DataModel*)camDM)->update();
    }
  }
  // update "non-existing" (not loaded yet) data models
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
  if ( study )
  {
    _PTR(Study) stdDS = study->studyDS();
    if( stdDS )
    {
      for ( _PTR(SComponentIterator) it ( stdDS->NewComponentIterator() ); it->More(); it->Next() )
      {
	_PTR(SComponent) aComponent ( it->Value() );

	if ( aComponent->ComponentDataType() == "Interface Applicative" )
	  continue; // skip the magic "Interface Applicative" component

	SalomeApp_DataModel::BuildTree( aComponent, study->root(), study, /*skipExisitng=*/true );
      }
    }
  }

  if ( objectBrowser() )
  {
    objectBrowser()->updateGeometry();
    objectBrowser()->updateTree();
  }
}

/*!Protected SLOT.On desktop activated.*/
void SalomeApp_Application::onDesktopActivated()
{
  CAM_Application::onDesktopActivated();
  SalomeApp_Module* aModule = dynamic_cast<SalomeApp_Module*>(activeModule());
  if(aModule)
    aModule->studyActivated();
}

/*!Create empty study.*/
void SalomeApp_Application::createEmptyStudy()
{
  CAM_Application::createEmptyStudy();
  if ( objectBrowser() )
    objectBrowser()->updateTree();
}

/*!Activate module \a mod.*/
bool SalomeApp_Application::activateModule( CAM_Module* mod )
{
  bool res = CAM_Application::activateModule( mod );
  if ( objectBrowser() )
    objectBrowser()->updateTree();
  return res;
}

/*!Display Catalog Genenerator dialog */
void SalomeApp_Application::onCatalogGen()
{
  ToolsGUI_CatalogGeneratorDlg aDlg( desktop() );
  aDlg.exec();
}

/*!Display Registry Display dialog */
void SalomeApp_Application::onRegDisplay()
{
  CORBA::ORB_var anOrb = orb();
  ToolsGUI_RegWidget* regWnd = ToolsGUI_RegWidget::GetRegWidget( anOrb, desktop(), "Registry" );
  regWnd->show();
  regWnd->raise();
  regWnd->setActiveWindow();
}

/*!return keyborad accelerators manager object */
SUIT_Accel* SalomeApp_Application::accel() const
{
  return myAccel;
}
