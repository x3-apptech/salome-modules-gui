// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:      LightApp_Application.cxx
// Created:   6/20/2005 18:39:45 PM
// Author:    Natalia Donis

#ifdef WNT
// E.A. : On windows with python 2.6, there is a conflict
// E.A. : between pymath.h and Standard_math.h which define
// E.A. : some same symbols : acosh, asinh, ...
  #include <Standard_math.hxx>
  #ifndef DISABLE_PYCONSOLE
    #include <pymath.h>
  #endif
#endif

#ifndef DISABLE_PYCONSOLE
  #include "LightApp_PyInterp.h" // WARNING! This include must be the first!
  #include <PyConsole_Console.h>
#endif

#include "LightApp_Application.h"
#include "LightApp_Module.h"
#include "LightApp_DataModel.h"
#include "LightApp_Study.h"
#include "LightApp_Preferences.h"
#include "LightApp_PreferencesDlg.h"
#include "LightApp_ModuleDlg.h"
#include "LightApp_AboutDlg.h"
#include "LightApp_ModuleAction.h"
// temporary commented
//#include "LightApp_OBFilter.h"
#include "LightApp_EventFilter.h"
#include "LightApp_OBSelector.h"
#include "LightApp_SelectionMgr.h"
#include "LightApp_DataObject.h"
#include "LightApp_WgViewModel.h"
#include "LightApp_FullScreenHelper.h"


#include <GUI_version.h>
#include <Basics_OCCTVersion.hxx>

#include <SALOME_Event.h>

#include <Style_Salome.h>
#include <Style_PrefDlg.h>

#include <CAM_Module.h>
#include <CAM_DataModel.h>
#include <CAM_Study.h>
#include <STD_TabDesktop.h>

#include <SUIT_DataBrowser.h>
#include <SUIT_Session.h>
#include <SUIT_Study.h>
#include <SUIT_FileDlg.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ShortcutMgr.h>
#include <SUIT_Tools.h>
#include <SUIT_Accel.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ViewWindow.h>

#include <Qtx.h>
#include <QtxToolBar.h>
#include <QtxTreeView.h>
#include <QtxMRUAction.h>
#include <QtxDockAction.h>
#include <QtxDockWidget.h>
#include <QtxActionToolMgr.h>
#include <QtxSearchTool.h>
#include <QtxWorkstack.h>
#include <QtxMap.h>
#include <QtxWebBrowser.h>

#include <LogWindow.h>

#ifndef DISABLE_GLVIEWER
  #include <GLViewer_Viewer.h>
  #include <GLViewer_ViewManager.h>
  #include "LightApp_GLSelector.h"
#endif

#ifndef DISABLE_PLOT2DVIEWER
  #include <Plot2d_ViewManager.h>
  #include <Plot2d_ViewModel.h>
  #include <Plot2d_ViewWindow.h>
  #include <Plot2d_ViewFrame.h>
  #include "LightApp_Plot2dSelector.h"
#ifndef DISABLE_SALOMEOBJECT
  #include <SPlot2d_ViewModel.h>
#else
  #include <Plot2d_ViewModel.h>
#endif
#endif

#ifndef DISABLE_OCCVIEWER
  #include <OCCViewer_ViewManager.h>
  #include <OCCViewer_ViewFrame.h>
#ifndef DISABLE_SALOMEOBJECT
  #include <SOCC_ViewModel.h>
#else
  #include <OCCViewer_ViewModel.h>
#endif
  #include "LightApp_OCCSelector.h"
#endif

#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
  #include <SVTK_ViewModel.h>
  #include <SVTK_ViewManager.h>
  #include "LightApp_VTKSelector.h"
#else
  #include <VTKViewer_ViewModel.h>
  #include <VTKViewer_ViewManager.h>
#endif
  #include <VTKViewer_ViewModel.h>
#endif

//#ifndef DISABLE_SUPERVGRAPHVIEWER
//  #include <SUPERVGraph_ViewModel.h>
//  #include <SUPERVGraph_ViewFrame.h>
//  #include <SUPERVGraph_ViewManager.h>
//#endif

#ifndef DISABLE_QXGRAPHVIEWER
//VSR: QxGraph has been replaced by QxScene
//  #include <QxGraph_ViewModel.h>
//  #include <QxGraph_ViewWindow.h>
//  #include <QxGraph_ViewManager.h>
  #include <QxScene_ViewManager.h>
  #include <QxScene_ViewModel.h>
  #include <QxScene_ViewWindow.h>
#endif


#define VISIBILITY_COLUMN_WIDTH 25

#include <QDir>
#include <QImage>
#include <QString>
#include <QWidget>
#include <QStringList>
#include <QFile>
#include <QApplication>
#include <QMap>
#include <QStatusBar>
#include <QThread>
#include <QObjectList>
#include <QComboBox>
#include <QInputDialog>
#include <QFontDatabase>
#include <QIcon>
#include <QByteArray>
#include <QMenu>
#include <QProcess>
#include <QTimer>
#include <QHeaderView>
#include <QTreeView>
#include <QMimeData>
#include <QShortcut>

#include <utilities.h>

#define FIRST_HELP_ID 1000000

#ifndef DISABLE_SALOMEOBJECT
  #include <SALOME_InteractiveObject.hxx>
  #include <SALOME_ListIO.hxx>
#endif

#include <Standard_Version.hxx>

#define ToolBarMarker    0
#define DockWidgetMarker 1

static const char* imageEmptyIcon[] = {
"20 20 1 1",
".      c None",
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

int LightApp_Application::lastStudyId = 0;

/*!
  \return last global id of study
*/
int LightApp_Application::studyId()
{
  return LightApp_Application::lastStudyId;
}

/*!Create new instance of LightApp_Application.*/
extern "C" LIGHTAPP_EXPORT SUIT_Application* createApplication()
{
  return new LightApp_Application();
}

/*! \var global preferences of LightApp */
LightApp_Preferences* LightApp_Application::_prefs_ = 0;


/*!
  \class LightApp_Application
  Application containing LightApp module
*/

/*!Constructor.*/
LightApp_Application::LightApp_Application()
: CAM_Application( false ),
  myPrefs( 0 ),
  myScreenHelper(new LightApp_FullScreenHelper())
{
  Q_INIT_RESOURCE( LightApp );

  STD_TabDesktop* desk = new STD_TabDesktop();
  desk->setFullScreenAllowed(false);

  setDesktop( desk );

  // initialize auto save timer
  myAutoSaveTimer = new QTimer( this );
  myAutoSaveTimer->setSingleShot( true );
  connect( myAutoSaveTimer, SIGNAL( timeout() ), this, SLOT( onSaveDoc() ) );

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap aLogo = aResMgr->loadPixmap( "LightApp", tr( "APP_DEFAULT_ICO" ), false );

  QtxWebBrowser::setData("browser:icon",          aResMgr->loadPixmap( "LightApp", tr( "BROWSER_ICON" ) ) );
  QtxWebBrowser::setData("browser:title",         tr( "BROWSER_TITLE" ) );
  QtxWebBrowser::setData("toolbar:title",         tr( "BROWSER_TOOLBAR_TITLE" ) );
  QtxWebBrowser::setData("menu:file:title",       tr( "BROWSER_FILEMENU" ) );
  QtxWebBrowser::setData("action:close:title",    tr( "BROWSER_CLOSE" ) );
  QtxWebBrowser::setData("action:close:icon",     aResMgr->loadPixmap( "LightApp", tr( "BROWSER_CLOSE_ICON" ) ) );
  QtxWebBrowser::setData("action:back:title",     tr( "BROWSER_BACK" ) );
  QtxWebBrowser::setData("action:forward:title",  tr( "BROWSER_FORWARD" ) );
  QtxWebBrowser::setData("action:find:title",     tr( "BROWSER_FIND" ) );
  QtxWebBrowser::setData("action:findnext:title", tr( "BROWSER_FINDNEXT" ) );
  QtxWebBrowser::setData("action:findprev:title", tr( "BROWSER_FINDPREV" ) );

  desktop()->setWindowIcon( aLogo );
  desktop()->setDockableMenuBar( false );
  desktop()->setDockableStatusBar( false );

  // base logo (salome itself)
  desktop()->logoInsert( "_app_base",  aResMgr->loadPixmap( "LightApp", tr( "APP_BASE_LOGO" ), false ) );
  // extra logo (salome-based application)
  desktop()->logoInsert( "_app_extra", aResMgr->loadPixmap( "LightApp", tr( "APP_EXTRA_LOGO" ), false ) );

  clearViewManagers();

  mySelMgr = new LightApp_SelectionMgr( this );

  myAccel = SUIT_Accel::getAccel();

#ifndef DISABLE_OCCVIEWER
  myAccel->setActionKey( SUIT_Accel::PanLeft,     Qt::CTRL+Qt::Key_Left,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    Qt::CTRL+Qt::Key_Right,    OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       Qt::CTRL+Qt::Key_Up,       OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     Qt::CTRL+Qt::Key_Down,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      Qt::CTRL+Qt::Key_Plus,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     Qt::CTRL+Qt::Key_Minus,    OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::CTRL+Qt::Key_Asterisk, OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateLeft,  Qt::ALT+Qt::Key_Left,      OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateRight, Qt::ALT+Qt::Key_Right,     OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateUp,    Qt::ALT+Qt::Key_Up,        OCCViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateDown,  Qt::ALT+Qt::Key_Down,      OCCViewer_Viewer::Type() );
#endif
#ifndef DISABLE_VTKVIEWER
  myAccel->setActionKey( SUIT_Accel::PanLeft,     Qt::CTRL+Qt::Key_Left,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    Qt::CTRL+Qt::Key_Right,    VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       Qt::CTRL+Qt::Key_Up,       VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     Qt::CTRL+Qt::Key_Down,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      Qt::CTRL+Qt::Key_Plus,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     Qt::CTRL+Qt::Key_Minus,    VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::CTRL+Qt::Key_Asterisk, VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateLeft,  Qt::ALT+Qt::Key_Left,      VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateRight, Qt::ALT+Qt::Key_Right,     VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateUp,    Qt::ALT+Qt::Key_Up,        VTKViewer_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::RotateDown,  Qt::ALT+Qt::Key_Down,      VTKViewer_Viewer::Type() );
#endif
#ifndef DISABLE_PLOT2DVIEWER
  myAccel->setActionKey( SUIT_Accel::PanLeft,     Qt::CTRL+Qt::Key_Left,     Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanRight,    Qt::CTRL+Qt::Key_Right,    Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanUp,       Qt::CTRL+Qt::Key_Up,       Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::PanDown,     Qt::CTRL+Qt::Key_Down,     Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomIn,      Qt::CTRL+Qt::Key_Plus,     Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomOut,     Qt::CTRL+Qt::Key_Minus,    Plot2d_Viewer::Type() );
  myAccel->setActionKey( SUIT_Accel::ZoomFit,     Qt::CTRL+Qt::Key_Asterisk, Plot2d_Viewer::Type() );
#endif

  connect( mySelMgr, SIGNAL( selectionChanged() ), this, SLOT( onSelection() ) );

  // Set existing font for the python console in resources
  if( !aResMgr->hasValue( "PyConsole", "font" ) )
    return;

  QFont f = aResMgr->fontValue( "PyConsole", "font" );
  QFontDatabase fdb;
  QStringList famdb = fdb.families();

  if ( famdb.contains(f.family()) || !aResMgr->hasValue( "PyConsole", "additional_families" ) )
    return;

  QStringList anAddFamilies = aResMgr->stringValue( "PyConsole", "additional_families" ).split( ";", QString::SkipEmptyParts );
  QString aFamily;
  for ( QStringList::Iterator it = anAddFamilies.begin(); it != anAddFamilies.end(); ++it )
  {
    aFamily = *it;
    if ( famdb.contains(aFamily) )
    {
      f.setFamily( aFamily );
      aResMgr->setValue( "PyConsole", "font", f );
      break;
    }
  }
}

/*!Destructor.
 *\li Save window geometry.
 *\li Save desktop geometry.
 *\li Save resource maneger.
 *\li Delete selection manager.
 */
LightApp_Application::~LightApp_Application()
{
  delete mySelMgr;
  delete myScreenHelper;
}

/*!Start application.*/
void LightApp_Application::start()
{
  CAM_Application::start();

  updateWindows();
  updateViewManagers();

  putInfo( "" );
  desktop()->statusBar()->showMessage( "" );

  LightApp_EventFilter::Init();
}

/*!Gets application name.*/
QString LightApp_Application::applicationName() const
{
  static QString _app_name;
  if ( _app_name.isEmpty() )
    _app_name = tr( "APP_NAME" );
  return _app_name;
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
      _app_version = GUI_VERSION_STR;
    }
  }
  return _app_version;
}

/*!Load module by \a name.*/
CAM_Module* LightApp_Application::loadModule( const QString& name, const bool showMsg )
{
  CAM_Module* mod = CAM_Application::loadModule( name, showMsg );
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

  saveDockWindowsState();

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
LightApp_SelectionMgr* LightApp_Application::selectionMgr() const
{
  return mySelMgr;
}

/*!Creat action "New window" for certain type of viewer:*/
void LightApp_Application::createActionForViewer( const int id,
                                                  const int parentId,
                                                  const QString& suffix,
                                                  const int accel )
{
  QString vtlt = tr( QString( "NEW_WINDOW_%1" ).arg( suffix ).toLatin1().constData() );
  QString tip = tr( "CREATING_NEW_WINDOW" ).arg( vtlt.remove( "&" ) );
  QAction* a = createAction( id,                      // menu action id
			     tip,                     // status tip
			     QIcon(),                 // icon
			     vtlt,                    // menu text
                             tip,                     // tooltip
                             accel,                   // shortcut
			     desktop(),               // parent
                             false,                   // toggle flag
			     this,                    // receiver
                             SLOT( onNewWindow() ) ); // slot
  createMenu( a, parentId, -1 );
}

/*!Create actions:*/

void LightApp_Application::createActions()
{
  STD_Application::createActions();

  SUIT_Desktop* desk = desktop();
  SUIT_ResourceMgr* resMgr = resourceMgr();

  //! Preferences
  createAction( PreferencesId, tr( "TOT_DESK_PREFERENCES" ), QIcon(),
                tr( "MEN_DESK_PREFERENCES" ), tr( "PRP_DESK_PREFERENCES" ),
                Qt::CTRL+Qt::Key_R, desk, false, this, SLOT( onPreferences() ) );

  //! Help for modules
  int helpMenu = createMenu( tr( "MEN_DESK_HELP" ), -1, -1, 1000 );
  createMenu( separator(), helpMenu, -1, 1 );
  QStringList aModuleList;
  modules( aModuleList, false );
  aModuleList.prepend( "GUI" );
  aModuleList.prepend( "KERNEL" );

  int id = LightApp_Application::UserID + FIRST_HELP_ID;

  // help for other existing modules

  QString aModule;
  foreach( aModule, aModuleList ) {
    if ( aModule.isEmpty() )                                         // module title (user name)
      continue;
    IMap <QString, QString> helpData;                                // list of help files for the module
    QString helpSubMenu;                                             // help submenu name (empty if not needed)
    QString modName = moduleName( aModule );                         // module name
    if ( modName.isEmpty() ) modName = aModule;                      // for KERNEL and GUI
    QString rootDir = QString( "%1_ROOT_DIR" ).arg( modName );       // module root dir variable
    QString modDir  = getenv( rootDir.toLatin1().constData() );      // module root dir
    QString docSection;
    if (resMgr->hasValue( modName, "documentation" ) )
      docSection = resMgr->stringValue(modName, "documentation");
    else if ( resMgr->hasSection( modName + "_documentation" ) )
      docSection = modName + "_documentation";
    if ( !docSection.isEmpty() ) {
      helpSubMenu = resMgr->stringValue( docSection, "sub_menu", "" ).arg( aModule );
      QStringList listOfParam = resMgr->parameters( docSection );
      foreach( QString paramName, listOfParam ) {
        QString valueStr = resMgr->stringValue( docSection, paramName );
        if ( !valueStr.isEmpty() ) {
          QFileInfo fi( valueStr );
          if ( fi.isRelative() && !modDir.isEmpty() )
            valueStr = Qtx::addSlash( modDir ) + valueStr;
          if ( QFile::exists( valueStr ) )
            helpData.insert( paramName.arg( aModule ), valueStr );
        }
      }
    }

    if ( helpData.isEmpty() && !modDir.isEmpty() ) {
      QStringList idxLst = QStringList() << modDir << "share" << "doc" << "salome" << "gui" << modName << "index.html";
      QString indexFile = idxLst.join( QDir::separator() );          // index file
      if ( QFile::exists( indexFile ) )
        helpData.insert( tr( "%1 module Users's Guide" ).arg( aModule ), indexFile );
    }

    IMapConstIterator<QString, QString > fileIt;
    for ( fileIt = helpData.begin(); fileIt != helpData.end(); fileIt++ ) {
      QString helpFileName = fileIt.key();
      // remove all '//' occurances 
      while ( helpFileName.contains( "//" ) )
	helpFileName.replace( "//", "" );
      // obtain submenus hierarchy if given
      QStringList smenus = helpFileName.split( "/" );
      helpFileName = smenus.last();
      smenus.removeLast();
      QAction* a = createAction( id, helpFileName,
                                 resMgr->loadPixmap( "STD", tr( "ICON_HELP" ), false ),
                                 helpFileName, helpFileName,
                                 0, desk, false, this, SLOT( onHelpContentsModule() ) );
      a->setData( fileIt.value() );
      if ( !helpSubMenu.isEmpty() ) {
	smenus.prepend( helpSubMenu );
      }
      // create sub-menus hierarchy
      int menuId = helpMenu;
      foreach ( QString subMenu, smenus ) {
        menuId = createMenu( subMenu, menuId, -1, 0 );
      }
      createMenu( a, menuId, -1, 0 );
      id++;
    }
  }

  //! MRU
  static QtxMRUAction* mru = new QtxMRUAction( tr( "TOT_DESK_MRU" ), tr( "MEN_DESK_MRU" ), 0 );
  connect( mru, SIGNAL( activated( const QString& ) ), this, SLOT( onMRUActivated( const QString& ) ) );
  registerAction( MRUId, mru );

  // default icon for neutral point ('SALOME' module)
  QPixmap defIcon = resMgr->loadPixmap( "LightApp", tr( "APP_DEFAULT_ICO" ), false );
  if ( defIcon.isNull() )
    defIcon = QPixmap( imageEmptyIcon );

  //! default icon for any module
  QPixmap modIcon = resMgr->loadPixmap( "LightApp", tr( "APP_MODULE_ICO" ), false );
  if ( modIcon.isNull() )
    modIcon = QPixmap( imageEmptyIcon );

  QStringList modList;
  modules( modList, false );

  if ( modList.count() > 1 )
  {
    LightApp_ModuleAction* moduleAction =
      new LightApp_ModuleAction( tr( "APP_NAME" ), defIcon, desk );

    QMap<QString, QString> iconMap;
    moduleIconNames( iconMap );

    const int iconSize = 20;

    QStringList::Iterator it;
    for ( it = modList.begin(); it != modList.end(); ++it )
    {
      if ( !isLibExists( *it ) )
        continue;

      QString modName = moduleName( *it );

      if ( !isModuleAccessible( *it ) )
        continue;

      QString iconName;
      if ( iconMap.contains( *it ) )
        iconName = iconMap[*it];

      QPixmap icon = resMgr->loadPixmap( modName, iconName, false );
      if ( icon.isNull() )
      {
        icon = modIcon;
        INFOS ( "****************************************************************" << std::endl
                <<  "*    Icon for " << (*it).toLatin1().constData()
                << " not found. Using the default one." << std::endl
                << "****************************************************************" << std::endl );
      }

      icon = Qtx::scaleIcon( icon, iconSize );

      moduleAction->insertModule( *it, icon );
    }

    connect( moduleAction, SIGNAL( moduleActivated( const QString& ) ),
             this, SLOT( onModuleActivation( const QString& ) ) );
    registerAction( ModulesListId, moduleAction );
  }

  // New window
  int windowMenu = createMenu( tr( "MEN_DESK_WINDOW" ), -1, MenuWindowId, 100 );
  int newWinMenu = createMenu( tr( "MEN_DESK_NEWWINDOW" ), windowMenu, -1, 0 );

  createAction( CloseId, tr( "TOT_CLOSE" ), QIcon(), tr( "MEN_DESK_CLOSE" ), tr( "PRP_CLOSE" ),
                Qt::CTRL+Qt::Key_F4, desk, false, this, SLOT( onCloseWindow() ) );
  createAction( CloseAllId, tr( "TOT_CLOSE_ALL" ), QIcon(), tr( "MEN_DESK_CLOSE_ALL" ), tr( "PRP_CLOSE_ALL" ),
                0, desk, false, this, SLOT( onCloseAllWindow() ) );
  createAction( GroupAllId, tr( "TOT_GROUP_ALL" ), QIcon(), tr( "MEN_DESK_GROUP_ALL" ), tr( "PRP_GROUP_ALL" ),
                0, desk, false, this, SLOT( onGroupAllWindow() ) );

  createMenu( CloseId,     windowMenu, 0, -1 );
  createMenu( CloseAllId,  windowMenu, 0, -1 );
  createMenu( GroupAllId,  windowMenu, 0, -1 );
  createMenu( separator(), windowMenu, -1, 0 );

#ifndef DISABLE_GLVIEWER
  createActionForViewer( NewGLViewId, newWinMenu, QString::number( 0 ), Qt::ALT+Qt::Key_G );
#endif
#ifndef DISABLE_PLOT2DVIEWER
  createActionForViewer( NewPlot2dId, newWinMenu, QString::number( 1 ), Qt::ALT+Qt::Key_P );
#endif
#ifndef DISABLE_OCCVIEWER
  createActionForViewer( NewOCCViewId, newWinMenu, QString::number( 2 ), Qt::ALT+Qt::Key_O );
#endif
#ifndef DISABLE_VTKVIEWER
  createActionForViewer( NewVTKViewId, newWinMenu, QString::number( 3 ), Qt::ALT+Qt::Key_K );
#endif
#ifndef DISABLE_QXGRAPHVIEWER
//VSR: QxGraph has been replaced by QxScene
//  createActionForViewer( NewQxGraphViewId, newWinMenu, QString::number( 4 ), Qt::ALT+Qt::Key_C );
  createActionForViewer( NewQxSceneViewId, newWinMenu, QString::number( 4 ), Qt::ALT+Qt::Key_S );
#endif

  createAction( RenameId, tr( "TOT_RENAME" ), QIcon(), tr( "MEN_DESK_RENAME" ), tr( "PRP_RENAME" ),
                Qt::ALT+Qt::SHIFT+Qt::Key_R, desk, false, this, SLOT( onRenameWindow() ) );
  createMenu( RenameId, windowMenu, -1 );

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );
  createMenu( PreferencesId, fileMenu, 50, -1 );
  createMenu( separator(), fileMenu, -1, 50, -1 );

  createMenu( separator(), fileMenu, -1, 100, -1 );
  createMenu( MRUId, fileMenu, 100, -1 );
  createMenu( separator(), fileMenu, -1, 100, -1 );

  createAction( StyleId, tr( "TOT_THEME" ), QIcon(), tr( "MEN_DESK_THEME" ), tr( "PRP_THEME" ),
                0, desk, false, this, SLOT( onStylePreferences() ) );

  createAction( FullScreenId, tr( "TOT_FULLSCREEN" ), QIcon(), tr( "MEN_DESK_FULLSCREEN" ), tr( "PRP_FULLSCREEN" ),
                Qt::Key_F11, desk, false, this, SLOT( onFullScreen() ) );


  int viewMenu = createMenu( tr( "MEN_DESK_VIEW" ), -1 );
  createMenu( separator(), viewMenu, -1, 20, -1 );
  createMenu( StyleId, viewMenu, 20, -1 );
  createMenu( FullScreenId, viewMenu, 20, -1 );

  int modTBar = createTool( tr( "INF_TOOLBAR_MODULES" ) );
  createTool( ModulesListId, modTBar );
}

/*!On module activation action.*/
void LightApp_Application::onModuleActivation( const QString& modName )
{
  // Force user to create/open a study before module activation
  QMap<QString, QString> iconMap;
  moduleIconNames( iconMap );
  QPixmap icon = resourceMgr()->loadPixmap( moduleName( modName ), iconMap[ modName ], false );
  if ( icon.isNull() )
    icon = resourceMgr()->loadPixmap( "LightApp", tr( "APP_MODULE_BIG_ICO" ), false ); // default icon for any module

  bool cancelled = false;

  while ( !modName.isEmpty() && !activeStudy() && !cancelled ){
    LightApp_ModuleDlg aDlg( desktop(), modName, icon );
    QMap<int, QString> opmap = activateModuleActions();
    for ( QMap<int, QString>::ConstIterator it = opmap.begin(); it != opmap.end(); ++it )
      aDlg.addButton( it.value(), it.key() );

    int res = aDlg.exec();
    if ( res ) {
      // some operation is selected
      moduleActionSelected( res );
    }
    else {
      // cancelled
      putInfo( tr("INF_CANCELLED") );

      LightApp_ModuleAction* moduleAction =
        qobject_cast<LightApp_ModuleAction*>( action( ModulesListId ) );
      if ( moduleAction )
        moduleAction->setActiveModule( QString() );
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
#ifndef DISABLE_GLVIEWER
  case NewGLViewId:
    type = GLViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_PLOT2DVIEWER
  case NewPlot2dId:
    type = Plot2d_Viewer::Type();
    break;
#endif
#ifndef DISABLE_OCCVIEWER
  case NewOCCViewId:
    type = OCCViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_VTKVIEWER
  case NewVTKViewId:
    type = VTKViewer_Viewer::Type();
    break;
#endif
#ifndef DISABLE_QXGRAPHVIEWER
//VSR: QxGraph has been replaced by QxScene
//  case NewQxGraphViewId:
//    type = QxGraph_Viewer::Type();
//    break;
  case NewQxSceneViewId:
    type = QxScene_Viewer::Type();
    break;
#endif
  }

  if ( !type.isEmpty() )
    createViewManager( type );
}

/*!
  SLOT: Creates new document
*/
void LightApp_Application::onNewDoc()
{
  //asl: fix for 0020515
  if ( activeStudy() )
    saveDockWindowsState();

  CAM_Application::onNewDoc();
}

/*!
  SLOT: Opens new document
*/
void LightApp_Application::onOpenDoc()
{
  SUIT_Study* study = activeStudy();
  saveDockWindowsState();

  CAM_Application::onOpenDoc();

  if ( !study ) // new study will be create in THIS application
  {
    updateWindows();
    updateViewManagers();
  }
}

/*!
  SLOT: Opens new document.
  \param aName - name of file
*/
bool LightApp_Application::onOpenDoc( const QString& aName )
{
  // We should take mru action first because this application instance can be deleted later.
  QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );

  bool res = CAM_Application::onOpenDoc( aName );

  if ( mru )
  {
    if ( res )
      mru->insert( aName );
    else
      mru->remove( aName );
  }
  return res;
}

/*!
  SLOT: Displays "About" message box
*/
void LightApp_Application::onHelpAbout()
{
  LightApp_AboutDlg dlg( applicationName(), applicationVersion(), desktop() );
  dlg.exec();
}

/*!
  Private SLOT: Called on selection is changed
  Dispatchs active module that selection is changed
*/
void LightApp_Application::onSelection()
{
  onSelectionChanged();

  if ( activeModule() && activeModule()->inherits( "LightApp_Module" ) )
    ((LightApp_Module*)activeModule())->selectionChanged();
}

/*!
  Sets active study.
 \param study - SUIT_Study.
*/
void LightApp_Application::setActiveStudy( SUIT_Study* study )
{
  CAM_Application::setActiveStudy( study );
}

/*!
  Enables/Disables menu items and toolbar buttons. Rebuild menu
*/
void LightApp_Application::updateCommandsStatus()
{
  CAM_Application::updateCommandsStatus();
  QAction* a = 0;

#ifndef DISABLE_GLVIEWER
  a = action( NewGLViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_PLOT2DVIEWER
  a = action( NewPlot2dId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_OCCVIEWER
  a = action( NewOCCViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_VTKVIEWER
  a = action( NewVTKViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif

#ifndef DISABLE_QXGRAPHVIEWER
//VSR: QxGraph has been replaced by QxScene
//  a = action( NewQxGraphViewId );
  a = action( NewQxSceneViewId );
  if( a )
    a->setEnabled( activeStudy() );
#endif
}

/*!
  \class RunBrowser
  Runs system command in separate thread
*/
class RunBrowser: public QThread
{
public:
  RunBrowser( LightApp_Application* app,
              const QString&        theApp,
              const QString&        theParams,
              const QString&        theHelpFile,
              const QString&        theContext = QString() )
    : myApp( theApp ),
      myParams( theParams ),
      myContext( theContext ),
      myStatus(0),
      myLApp( app )
  {
    //For the external browser always specify 'file://' protocol,
    //because some WEB browsers (for example Mozilla Firefox) can't open local file without protocol.
    myHelpFile = QString("file://%1").arg( QFileInfo( theHelpFile ).canonicalFilePath() );
  }

  virtual void run()
  {
    if ( !myApp.isEmpty() && !myHelpFile.isEmpty()) {
      QString aCommand = QString( "%1 %2 \"%3%4\"" ).arg( myApp, myParams, myHelpFile, myContext.isEmpty() ? QString("") : QString( "#%1" ).arg( myContext ) );

      QProcess* proc = new QProcess();

      proc->start( aCommand );
      if ( !proc->waitForStarted() ) {
        SALOME_CustomEvent* ce2000 = new SALOME_CustomEvent( 2000 );
        QString* msg = new QString( QObject::tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).arg( myApp, myHelpFile ) );
        ce2000->setData( msg );
        QApplication::postEvent( myLApp, ce2000 );
      }
    }
  }

private:
  QString               myApp;
  QString               myParams;
  QString               myHelpFile;
  QString               myContext;
  int                   myStatus;
  LightApp_Application* myLApp;
};

/*!
  SLOT: Displays help contents for choosen module
*/
void LightApp_Application::onHelpContentsModule()
{
  const QAction* a = (QAction*) sender();
  QString helpFile = a->data().toString();
  if ( helpFile.isEmpty() ) return;

  SUIT_ResourceMgr* resMgr = resourceMgr();
  QString platform;
#ifdef WIN32
  platform = "winapplication";
#else
  platform = "application";
#endif
  QString anApp = resMgr->stringValue("ExternalBrowser", platform);
#ifdef WIN32
  QString quote("\"");
  anApp.prepend( quote );
  anApp.append( quote );
#endif
  QString aParams = resMgr->stringValue("ExternalBrowser", "parameters");
  bool useExtBrowser = resMgr->booleanValue("ExternalBrowser", "use_external_browser", false );

  if( useExtBrowser ) {
    if ( !anApp.isEmpty() ) {
      RunBrowser* rs = new RunBrowser( this, anApp, aParams, helpFile );
      rs->start();
    }
    else {
      if ( SUIT_MessageBox::question( desktop(), tr( "WRN_WARNING" ), tr( "DEFINE_EXTERNAL_BROWSER" ),
                                      SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                      SUIT_MessageBox::Yes ) == SUIT_MessageBox::Yes )

        showPreferences( tr( "PREF_APP" ) );
    }
  }
  else {
#ifdef WIN32
    // On Win32 platform QWebKit of the Qt 4.6.3 hang up in case 'file://' protocol 
    // is defined. On Linux platform QWebKit doesn't work correctly without 'file://' protocol.
    QtxWebBrowser::loadUrl(helpFile);
#else
	QtxWebBrowser::loadUrl(QString("file://%1").arg(helpFile));
#endif
  }
}

/*!
  SLOT: Displays help contents for choosen dialog
*/
void LightApp_Application::onHelpContextModule( const QString& theComponentName,
                                                const QString& theFileName,
                                                const QString& theContext )
{
  QString fileName = theFileName;
  QString context  = theContext;
  if ( !QFile::exists( fileName ) && theContext.isEmpty() ) {
    // context might be passed within theFileName argument
    QStringList comps = fileName.split("#");
    if ( comps.count() > 1 ) {
      context = comps.last();
      comps.removeLast();
      fileName = comps.join("#");
    }
  }

  QString homeDir = "";
  if ( !theComponentName.isEmpty() ) {
    QString dir = getenv( ( theComponentName + "_ROOT_DIR" ).toLatin1().constData() );
    if ( !dir.isEmpty() )
      homeDir = Qtx::addSlash( Qtx::addSlash( dir )      +
                               Qtx::addSlash( "share" )  +
                               Qtx::addSlash( "doc" )    +
                               Qtx::addSlash( "salome" ) +
                               Qtx::addSlash( "gui" )    +
                               Qtx::addSlash( theComponentName ) );
  }

  QString helpFile = QFileInfo( homeDir + fileName ).absoluteFilePath();
  SUIT_ResourceMgr* resMgr = resourceMgr();
        QString platform;
#ifdef WIN32
        platform = "winapplication";
#else
        platform = "application";
#endif
        QString anApp = resMgr->stringValue("ExternalBrowser", platform);
#ifdef WIN32
        QString quote("\"");
        anApp.prepend( quote );
        anApp.append( quote );
#endif

  bool useExtBrowser = resMgr->booleanValue("ExternalBrowser", "use_external_browser", false );

  if(useExtBrowser) {
    QString aParams = resMgr->stringValue("ExternalBrowser", "parameters");

    if ( !anApp.isEmpty() ) {
      RunBrowser* rs = new RunBrowser( this, anApp, aParams, helpFile, context );
      rs->start();
    }
    else {
      if ( SUIT_MessageBox::question( desktop(), tr( "WRN_WARNING" ), tr( "DEFINE_EXTERNAL_BROWSER" ),
                                      SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                      SUIT_MessageBox::Yes ) == SUIT_MessageBox::Yes )
        showPreferences( tr( "PREF_APP" ) );
    }
  }
  else {
#ifdef WIN32
    // On Win32 platform QWebKit of the Qt 4.6.3 hang up in case 'file://' protocol 
    // is defined. On Linux platform QWebKit doesn't work correctly without 'file://' protocol.
	QtxWebBrowser::loadUrl(helpFile, context);
#else
	QtxWebBrowser::loadUrl(QString("file://%1").arg(helpFile),context);
#endif
    
  }
}

/*!
  Sets enable or disable some actions on selection changed.
*/
void LightApp_Application::onSelectionChanged()
{
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( activeModule() );
  bool canCopy  = m ? m->canCopy() : false;
  bool canPaste = m ? m->canPaste() : false;

  action( EditCopyId )->setEnabled(canCopy);
  action( EditPasteId )->setEnabled(canPaste);
}

/*!
  Adds window to application.
  \param wid - QWidget
  \param flag - key for window
  \param studyId - study id
  Flag used how identificator of window in windows list.
*/
/*
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
    // asv: connecting a slot for storing visibility flag of a window
    connect( newWC, SIGNAL( visibilityChanged ( bool ) ), SLOT( onVisibilityChanged( bool ) ) );
    myWindows.insert( flag, newWC );
    if ( winMap.contains( flag ) ) {
      //desktop()->removeDockWidget( myWindows[flag] );
      desktop()->addDockWidget( (Qt::DockWidgetArea)winMap[flag], myWindows[flag] );
    }

    //myWindows[flag]->setResizeEnabled( true );
    myWindows[flag]->setFeatures( QDockWidget::AllDockWidgetFeatures );
    myWindows[flag]->setObjectName( QString( "dock_window_%1" ).arg( flag ) );
    //myWindows[flag]->setFixedExtentWidth( wid->width() );
    //myWindows[flag]->setFixedExtentHeight( wid->height() );
    myWindows[flag]->resize( wid->width(), wid->height() );
  }

  QFont f;
#ifndef DISABLE_PYCONSOLE
  if( wid->inherits( "PyConsole_Console" ) )
  {
    if( resourceMgr()->hasValue( "PyConsole", "font" ) )
      f = resourceMgr()->fontValue( "PyConsole", "font" );
    else
      {
        f = ( ( PyConsole_Console* )wid )->font();
        resourceMgr()->setValue( "PyConsole", "font", f );
      }
  }
  else
#endif
    f = wid->font();

  myWindows[flag]->insert( sId, wid );
  wid->setFont( f );

  setWindowShown( flag, !myWindows[flag]->isEmpty() );
}
*/

QWidget* LightApp_Application::dockWindow( const int id ) const
{
  QWidget* wid = 0;
  if ( myWin.contains( id ) )
    wid = myWin[id];
  return wid;
}

QDockWidget* LightApp_Application::windowDock( QWidget* wid ) const
{
  if ( !wid )
    return 0;

  QDockWidget* dock = 0;
  QWidget* w = wid->parentWidget();
  while ( w && !dock )
  {
    dock = ::qobject_cast<QDockWidget*>( w );
    w = w->parentWidget();
  }
  return dock;
}

void LightApp_Application::insertDockWindow( const int id, QWidget* wid )
{
  if ( !wid )
    return;

  if ( wid != dockWindow( id ) )
    removeDockWindow( id );

  myWin.insert( id, wid );

  QtxDockWidget* dock = new QtxDockWidget( true, desktop() );
  connect( dock, SIGNAL(  destroyed( QObject* ) ), this, SLOT( onWCDestroyed( QObject* ) ) );

  dock->setFeatures( QDockWidget::AllDockWidgetFeatures );
  dock->setObjectName( QString( "window_%1" ).arg( id ) );
  dock->setWidget( wid );

  QKeySequence accel = wid->property( "shortcut" ).value<QKeySequence>();
  if ( !accel.isEmpty() )
    dock->toggleViewAction()->setShortcut( accel );

  dock->show();
}

void LightApp_Application::removeDockWindow( const int id )
{
  QWidget* wid = dockWindow( id );
  if ( !wid )
    return;

  myWin.remove( id );

  QDockWidget* dock = windowDock( wid );
  if ( !dock )
    return;

  dock->setWidget( 0 );
  wid->setParent( 0 );
  wid->setVisible( false );
  delete dock;
}

void LightApp_Application::placeDockWindow( const int id, Qt::DockWidgetArea place )
{
  QDockWidget* dock = windowDock( dockWindow( id ) );
  if ( dock && desktop() ) {
    desktop()->addDockWidget( place, dock );
    QtxDockAction* a = qobject_cast<QtxDockAction*>( action( ViewWindowsId ) );
    if ( a ) a->update();
  }
}

/*!
  Gets window.
  \param flag - key for window
  \param studyId - study id
  Flag used how identificator of window in windows list.
*/
QWidget* LightApp_Application::getWindow( const int flag, const int )
{
  QWidget* wid = dockWindow( flag );
  if ( !wid )
    insertDockWindow( flag, wid = createWindow( flag ) );

  QMap<int, int> winMap;
  currentWindows( winMap );
  if ( winMap.contains( flag ) )
    placeDockWindow( flag, (Qt::DockWidgetArea)winMap[flag] );

  return wid;
}

/*!
  \return Object Browser
*/
SUIT_DataBrowser* LightApp_Application::objectBrowser()
{
  return qobject_cast<SUIT_DataBrowser*>( dockWindow( WT_ObjectBrowser ) );
}

/*!
  \return Log Window
*/
LogWindow* LightApp_Application::logWindow()
{
  return qobject_cast<LogWindow*>( dockWindow( WT_LogWindow ) );
}

#ifndef DISABLE_PYCONSOLE
/*!
  \return Python Console
*/
PyConsole_Console* LightApp_Application::pythonConsole()
{
  return qobject_cast<PyConsole_Console*>( dockWindow( WT_PyConsole ) );
}
#endif

/*!
  Updates object browser and maybe data models
  \param updateModels - if it is true, then data models are updated
*/
void LightApp_Application::updateObjectBrowser( const bool updateModels )
{
  // update existing data models
  if ( updateModels )
  {
    const bool isAutoUpdate = objectBrowser() ? objectBrowser()->autoUpdate() : true;
    if ( objectBrowser() )
      objectBrowser()->setAutoUpdate( false );

    LightApp_Study* study = dynamic_cast<LightApp_Study*>(activeStudy());
    if ( study ) {
      CAM_Study::ModelList dm_list;
      study->dataModels( dm_list );
      QListIterator<CAM_DataModel*> it( dm_list );
      while ( it.hasNext() ) {
        CAM_DataModel* camDM = it.next();
        if ( camDM && camDM->inherits( "LightApp_DataModel" ) )
          ((LightApp_DataModel*)camDM)->update();
      }
    }

    if( objectBrowser() )
      objectBrowser()->setAutoUpdate( isAutoUpdate );
  }

  if ( objectBrowser() ) {
    objectBrowser()->updateGeometry();
    objectBrowser()->updateTree( 0, false );
  }
}

/*!
  \return preferences
*/
LightApp_Preferences* LightApp_Application::preferences() const
{
  return preferences( false );
}

/*!
  \return first view manager of some type
  \param vmType - type of view manager
  \param create - is it necessary to create view manager in case, when there is no manager of such type
*/
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

/*!
  Creates view manager of some type
  \param vmType - type of view manager
*/
SUIT_ViewManager* LightApp_Application::createViewManager( const QString& vmType )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

  SUIT_ViewManager* viewMgr = 0;
#ifndef DISABLE_GLVIEWER
  if( vmType == GLViewer_Viewer::Type() )
  {
    viewMgr = new GLViewer_ViewManager( activeStudy(), desktop() );
    new LightApp_GLSelector( (GLViewer_Viewer2d*)viewMgr->getViewModel(), mySelMgr );
  }
#endif
#ifndef DISABLE_PLOT2DVIEWER
  if( vmType == Plot2d_Viewer::Type() )
  {
    viewMgr = new Plot2d_ViewManager( activeStudy(), desktop() );
    Plot2d_Viewer* vm;
#ifndef DISABLE_SALOMEOBJECT
    SPlot2d_Viewer* v = new SPlot2d_Viewer();
    vm = v;
    new LightApp_Plot2dSelector( v, mySelMgr );
#else
    vm = new Plot2d_Viewer();
#endif
    viewMgr->setViewModel( vm  );// custom view model, which extends SALOME_View interface
    Plot2d_ViewWindow* wnd = dynamic_cast<Plot2d_ViewWindow*>( viewMgr->getActiveView() );
    if( wnd )
    {
      Plot2d_ViewFrame* frame = wnd->getViewFrame();
      frame->setBackgroundColor( resMgr->colorValue( "Plot2d", "Background", frame->backgroundColor() ) );
    }
  }
#endif
  //#ifndef DISABLE_SUPERVGRAPHVIEWER
  //  if( vmType == SUPERVGraph_Viewer::Type() )
  //  {
  //    viewMgr = new SUPERVGraph_ViewManager( activeStudy(), desktop(), new SUPERVGraph_Viewer() );
  //  }
  //#endif
#ifndef DISABLE_QXGRAPHVIEWER
//VSR: QxGraph has been replaced by QxScene
//  if( vmType == QxGraph_Viewer::Type() )
//    {
//      viewMgr = new QxGraph_ViewManager( activeStudy(), desktop(), new QxGraph_Viewer() );
//    }
  if( vmType == QxScene_Viewer::Type() )
  {
    viewMgr = new QxScene_ViewManager( activeStudy(), desktop() );
    QxScene_Viewer* vm = new QxScene_Viewer();
    viewMgr->setViewModel( vm  );
    //QxScene_ViewWindow* wnd = dynamic_cast<QxScene_ViewWindow*>( viewMgr->getActiveView() );
  }
#endif
#ifndef DISABLE_OCCVIEWER
  if( vmType == OCCViewer_Viewer::Type() )
  {
    viewMgr = new OCCViewer_ViewManager( activeStudy(), desktop() );
    OCCViewer_Viewer* vm;
#ifndef DISABLE_SALOMEOBJECT
    vm = new SOCC_Viewer();
#else
    vm = new OCCViewer_Viewer( true );
#endif
    vm->setBackground( OCCViewer_ViewFrame::TOP_LEFT,
		       resMgr->backgroundValue( "OCCViewer", "xz_background", vm->background(OCCViewer_ViewFrame::TOP_LEFT) ) );
    vm->setBackground( OCCViewer_ViewFrame::TOP_RIGHT,
		       resMgr->backgroundValue( "OCCViewer", "yz_background", vm->background(OCCViewer_ViewFrame::TOP_RIGHT) ) );
    vm->setBackground( OCCViewer_ViewFrame::BOTTOM_LEFT,
		       resMgr->backgroundValue( "OCCViewer", "xy_background", vm->background(OCCViewer_ViewFrame::BOTTOM_LEFT) ) );
    vm->setBackground( OCCViewer_ViewFrame::BOTTOM_RIGHT,
		       resMgr->backgroundValue( "OCCViewer", "background", vm->background(OCCViewer_ViewFrame::MAIN_VIEW) ) );

    vm->setTrihedronSize(  resMgr->doubleValue( "OCCViewer", "trihedron_size", vm->trihedronSize() ),
                           resMgr->booleanValue( "OCCViewer", "relative_size", vm->trihedronRelative() ));
    int u( 1 ), v( 1 );
    vm->isos( u, v );
    u = resMgr->integerValue( "OCCViewer", "iso_number_u", u );
    v = resMgr->integerValue( "OCCViewer", "iso_number_v", v );
    vm->setIsos( u, v );
    vm->setInteractionStyle( resMgr->integerValue( "OCCViewer", "navigation_mode", vm->interactionStyle() ) );
    vm->setZoomingStyle( resMgr->integerValue( "OCCViewer", "zooming_mode", vm->zoomingStyle() ) );
    viewMgr->setViewModel( vm );// custom view model, which extends SALOME_View interface
    new LightApp_OCCSelector( (OCCViewer_Viewer*)viewMgr->getViewModel(), mySelMgr );
  }
#endif
#ifndef DISABLE_VTKVIEWER
#ifndef DISABLE_SALOMEOBJECT
  if ( vmType == SVTK_Viewer::Type() )
#else
  if ( vmType == VTKViewer_Viewer::Type() )
#endif
  {
#ifndef DISABLE_SALOMEOBJECT
    viewMgr = new SVTK_ViewManager( activeStudy(), desktop() );
    SVTK_Viewer* vm = dynamic_cast<SVTK_Viewer*>( viewMgr->getViewModel() );
    if( vm )
    {
      vm->setProjectionMode( resMgr->integerValue( "VTKViewer", "projection_mode", vm->projectionMode() ) );
      vm->setBackground( resMgr->backgroundValue( "VTKViewer", "background", vm->background() ) );
      vm->setTrihedronSize( resMgr->doubleValue( "VTKViewer", "trihedron_size", vm->trihedronSize() ),
                            resMgr->booleanValue( "VTKViewer", "relative_size", vm->trihedronRelative() ) );
      vm->setStaticTrihedronVisible( resMgr->booleanValue( "VTKViewer", "show_static_trihedron", vm->isStaticTrihedronVisible() ) );
      vm->setInteractionStyle( resMgr->integerValue( "VTKViewer", "navigation_mode", vm->interactionStyle() ) );
      vm->setZoomingStyle( resMgr->integerValue( "VTKViewer", "zooming_mode", vm->zoomingStyle() ) );
      vm->setDynamicPreSelection( resMgr->booleanValue( "VTKViewer", "dynamic_preselection", vm->dynamicPreSelection() ) );
      vm->setIncrementalSpeed( resMgr->integerValue( "VTKViewer", "speed_value", vm->incrementalSpeed() ),
                               resMgr->integerValue( "VTKViewer", "speed_mode", vm->incrementalSpeedMode() ) );
      vm->setSpacemouseButtons( resMgr->integerValue( "VTKViewer", "spacemouse_func1_btn", vm->spacemouseBtn(1) ),
                                resMgr->integerValue( "VTKViewer", "spacemouse_func2_btn", vm->spacemouseBtn(2) ),
                                resMgr->integerValue( "VTKViewer", "spacemouse_func5_btn", vm->spacemouseBtn(3) ) );
      new LightApp_VTKSelector( vm, mySelMgr );
    }
#else
    viewMgr = new VTKViewer_ViewManager( activeStudy(), desktop() );
    VTKViewer_Viewer* vm = dynamic_cast<VTKViewer_Viewer*>( viewMgr->getViewModel() );
    if ( vm )
      vm->setBackground( resMgr->backgroundValue( "VTKViewer", "background", vm->background() ) );
#endif
  }
#endif

  if ( !viewMgr )
    return 0;

  addViewManager( viewMgr );
  SUIT_ViewWindow* viewWin = viewMgr->createViewWindow();

  if ( viewWin && desktop() ) {
    viewWin->resize( (int)( desktop()->width() * 0.6 ), (int)( desktop()->height() * 0.6 ) );
    viewWin->setDropDownButtons( resMgr->booleanValue( "viewers", "drop_down_buttons", true ) );
  }

  return viewMgr;
}

SUIT_ViewManager* LightApp_Application::createViewManager( const QString& vmType, QWidget* w )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();

  SUIT_ViewManager* vm = new SUIT_ViewManager( activeStudy(),
                                               desktop(),
                                               new LightApp_WgViewModel( vmType, w ) );
  vm->setTitle( QString( "%1: %M - viewer %V" ).arg( vmType ) );

  addViewManager( vm );
  SUIT_ViewWindow* vw = vm->createViewWindow();
  if ( vw && desktop() ) {
    vw->resize( (int)( desktop()->width() * 0.6 ), (int)( desktop()->height() * 0.6 ) );
    vw->setDropDownButtons( resMgr->booleanValue( "viewers", "drop_down_buttons", true ) );
  }

  if ( !vmType.isEmpty() && !myUserWmTypes.contains( vmType ) )
    myUserWmTypes << vmType;

  return vm;
}

/*!
  SLOT: Removes view manager from application
*/
void LightApp_Application::onCloseView( SUIT_ViewManager* theVM )
{
  removeViewManager( theVM );
}

/*!
  Protected SLOT: On study created.
  \param theStudy - just created study
*/
void LightApp_Application::onStudyCreated( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->setName( tr( "DATA_MODELS" ) );
  }

  getWindow( WT_ObjectBrowser );

  loadDockWindowsState();

  if ( objectBrowser() )
    objectBrowser()->setRoot( aRoot );

  activateModule( defaultModule() );

  if ( objectBrowser() )
    objectBrowser()->openLevels();
}

/*!
  Protected SLOT: On study opened.
  \param theStudy - just opened  study
*/
void LightApp_Application::onStudyOpened( SUIT_Study* theStudy )
{
  SUIT_DataObject* aRoot = 0;
  if ( theStudy && theStudy->root() )
  {
    aRoot = theStudy->root();
    //aRoot->dump();
  }

  getWindow( WT_ObjectBrowser );

  loadDockWindowsState();

  if ( objectBrowser() )
    objectBrowser()->setRoot( aRoot );

  activateModule( defaultModule() );

  if ( objectBrowser() )
    objectBrowser()->openLevels();

  emit studyOpened();
}

/*!Protected SLOT. On study saved.*/
void LightApp_Application::onStudySaved( SUIT_Study* s )
{
  QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
  if ( mru && s )
      mru->insert( s->studyName() );

  emit studySaved();
}

/*!Protected SLOT. On study closed.*/
void LightApp_Application::onStudyClosed( SUIT_Study* s )
{
  // stop auto-save timer
  myAutoSaveTimer->stop();

  // Bug 10396: clear selection
  mySelMgr->clearSelected();

  // Bug 12944: emit signal only after clear selection
  emit studyClosed();

  activateModule( "" );
}

/*!Protected SLOT.On desktop activated.*/
void LightApp_Application::onDesktopActivated()
{
  CAM_Application::onDesktopActivated();
  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>(activeModule());
  if(aModule)
    aModule->studyActivated();
}

void LightApp_Application::studyOpened( SUIT_Study* s )
{
  CAM_Application::studyOpened( s );

  updateWindows();
  updateViewManagers();
}

void LightApp_Application::studySaved( SUIT_Study* s )
{
  CAM_Application::studyOpened( s );
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( aResMgr && activeStudy() ) {
    int autoSaveInterval = aResMgr->integerValue( "Study", "auto_save_interval", 0 );
    if ( autoSaveInterval > 0 ) myAutoSaveTimer->start( autoSaveInterval*60000 );
  }
}

void LightApp_Application::studyCreated( SUIT_Study* s )
{
  CAM_Application::studyCreated( s );

  updateWindows();
  updateViewManagers();
}

/*!Gets file filter.
 *\retval QString "(*.bin)"
 */
QString LightApp_Application::getFileFilter() const
{
  //return "(*.bin)";
  // HDF persistence
  return "(*.hdf)";
}

/*!
  Shows file dialog and return user selected file name
*/
QString LightApp_Application::getFileName( bool open, const QString& initial, const QString& filters,
                                           const QString& caption, QWidget* parent )
{
  if ( !parent )
    parent = desktop();
  QStringList fls = filters.split( ";;", QString::SkipEmptyParts );
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
  QStringList fls = filters.split( ";;", QString::SkipEmptyParts );
  return SUIT_FileDlg::getOpenFileNames( parent, initial, fls, caption, true );
}

/*!Private SLOT. Update object browser.*/
void LightApp_Application::onRefresh()
{
  updateObjectBrowser( true );
}

/*!Private SLOT. Support drag-and-drop operation.*/
void LightApp_Application::onDropped( const QList<SUIT_DataObject*>& objects, SUIT_DataObject* parent, int row, Qt::DropAction action )
{
  LightApp_DataObject* parentObj = dynamic_cast<LightApp_DataObject*>( parent );
  if ( !parentObj )
    return;

  LightApp_Module* aModule = dynamic_cast<LightApp_Module*>( parentObj->module() );
  if ( aModule )
    aModule->dropObjects( objects, parentObj, row, action );
}

/*!Private SLOT. On preferences.*/
void LightApp_Application::onPreferences()
{
  showPreferences( activeModule() ? activeModule()->moduleName() : tr( "PREF_CATEGORY_SALOME" ) );
}

/*!Private SLOT. On preferences.*/
void LightApp_Application::showPreferences( const QString& itemText )
{
  QApplication::setOverrideCursor( Qt::WaitCursor );

  LightApp_PreferencesDlg* prefDlg = new LightApp_PreferencesDlg( preferences( true ), desktop());

  QApplication::restoreOverrideCursor();

  if ( !prefDlg )
    return;

  preferences()->activateItem( itemText );

  if ( ( prefDlg->exec() == QDialog::Accepted || prefDlg->isSaved() ) &&  resourceMgr() )
  {
    if ( desktop() )
      resourceMgr()->setValue( "desktop", "geometry", desktop()->storeGeometry() );
    resourceMgr()->save();

    // Update shortcuts
    shortcutMgr()->updateShortcuts();
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
  // emit signal to allow additional preferences changing processing
  emit preferenceChanged( modName, section, param );
}

/*!Remove all windows from study.*/
void LightApp_Application::beforeCloseDoc( SUIT_Study* s )
{
  CAM_Application::beforeCloseDoc( s );
}

/*!Update actions.*/
void LightApp_Application::updateActions()
{
  updateCommandsStatus();
}

/*!
  Creates new study
*/
SUIT_Study* LightApp_Application::createNewStudy()
{
  LightApp_Application::lastStudyId++;

  LightApp_Study* aStudy = new LightApp_Study( this );

  // Set up processing of major study-related events
  connect( aStudy, SIGNAL( created( SUIT_Study* ) ), this, SLOT( onStudyCreated( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( opened ( SUIT_Study* ) ), this, SLOT( onStudyOpened ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( saved  ( SUIT_Study* ) ), this, SLOT( onStudySaved  ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( closed ( SUIT_Study* ) ), this, SLOT( onStudyClosed ( SUIT_Study* ) ) );

  return aStudy;
}

/*!
  Creates window by flag.
  \param flag - identificator of window type
*/
QWidget* LightApp_Application::createWindow( const int flag )
{
  QWidget* wid = 0;

  SUIT_ResourceMgr* resMgr = resourceMgr();

  if ( flag == WT_ObjectBrowser )
  {
    SUIT_DataBrowser* ob = new SUIT_DataBrowser( new LightApp_DataObject(), desktop() );
    ob->setSortMenuEnabled( true );
    ob->setAutoUpdate( true );
    if ( resMgr->hasValue( "ObjectBrowser", "auto_hide_search_tool" ) )
      ob->searchTool()->enableAutoHide( resMgr->booleanValue( "ObjectBrowser", "auto_hide_search_tool" ) );

    //ob->setAutoOpenLevel( 1 ); // commented by ASV as a fix to bug IPAL10107
    ob->setWindowTitle( tr( "OBJECT_BROWSER" ) );
    connect( ob, SIGNAL( requestUpdate() ), this, SLOT( onRefresh() ) );

    QString EntryCol = QObject::tr( "ENTRY_COLUMN" );
    SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( ob->model() );
    treeModel->setSearcher( this );
    treeModel->registerColumn( 0, EntryCol, LightApp_DataObject::EntryId );
    treeModel->setAppropriate( EntryCol, Qtx::Toggled );

    // Mantis issue 0020136: Drag&Drop in OB
    SUIT_ProxyModel* proxyModel = dynamic_cast<SUIT_ProxyModel*>(treeModel);
    if ( proxyModel ) {
      connect( proxyModel, SIGNAL( dropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ),
	       this,       SLOT( onDropped( const QList<SUIT_DataObject*>&, SUIT_DataObject*, int, Qt::DropAction ) ) );
    }

    // temporary commented
    /*
    OB_ListView* ob_list = dynamic_cast<OB_ListView*>( const_cast<QListView*>( ob->listView() ) );
    if( ob_list )
      ob_list->setColumnMaxWidth( 0, desktop()->width()/4 );

    ob->setFilter( new LightApp_OBFilter( selectionMgr() ) );
    */

    // Create OBSelector
    new LightApp_OBSelector( ob, mySelMgr );

    ob->treeView()->header()->setResizeMode(SUIT_DataObject::VisibilityId, QHeaderView::Fixed);
    ob->treeView()->header()->moveSection(SUIT_DataObject::NameId,SUIT_DataObject::VisibilityId);
    ob->treeView()->setColumnWidth(SUIT_DataObject::VisibilityId, VISIBILITY_COLUMN_WIDTH);
    ob->setProperty( "shortcut", QKeySequence( "Alt+Shift+O" ) );
    wid = ob;
    ob->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
#ifndef DISABLE_PYCONSOLE
  else  if ( flag == WT_PyConsole )
  {
    PyConsole_Console* pyCons = new PyConsole_Console( desktop(),new LightApp_PyInterp());
    pyCons->setWindowTitle( tr( "PYTHON_CONSOLE" ) );
    pyCons->setFont(resourceMgr()->fontValue( "PyConsole", "font" ));
    pyCons->setIsShowBanner(resourceMgr()->booleanValue( "PyConsole", "show_banner", true ));
    pyCons->setProperty( "shortcut", QKeySequence( "Alt+Shift+P" ) );

    wid = pyCons;
    pyCons->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
#endif
  else if ( flag == WT_LogWindow )
  {
    LogWindow* logWin = new LogWindow( desktop() );
    logWin->setWindowTitle( tr( "LOG_WINDOW" ) );
    logWin->setProperty( "shortcut", QKeySequence( "Alt+Shift+L" ) );
    wid = logWin;
    logWin->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
  return wid;
}

/*!
  \return default windows( Object Browser, Python Console )
  Adds to map \a aMap.
 */
void LightApp_Application::defaultWindows( QMap<int, int>& aMap ) const
{
  aMap.insert( WT_ObjectBrowser, Qt::LeftDockWidgetArea );
#ifndef DISABLE_PYCONSOLE
  aMap.insert( WT_PyConsole, Qt::BottomDockWidgetArea );
#endif
  //  aMap.insert( WT_LogWindow, Qt::DockBottom );
}

/*!Default view managers*/
void LightApp_Application::defaultViewManagers( QStringList& ) const
{
  /*!Do nothing.*/
}

/*!
  \return preferences.
  Create preferences, if \a crt = true.
*/
LightApp_Preferences* LightApp_Application::preferences( const bool crt ) const
{
  if ( myPrefs )
    return myPrefs;

  LightApp_Application* that = (LightApp_Application*)this;

  bool toCreate = !_prefs_ && crt;
  if ( toCreate )
  {
    _prefs_ = new LightApp_Preferences( resourceMgr() );
    that->createPreferences( _prefs_ );
  }

  that->myPrefs = _prefs_;

  connect( myPrefs, SIGNAL( preferenceChanged( QString&, QString&, QString& ) ),
           this, SLOT( onPreferenceChanged( QString&, QString&, QString& ) ) );

  if ( !crt )
    return myPrefs;

  SUIT_ResourceMgr* resMgr = resourceMgr();

  QList<SUIT_Application*> appList = SUIT_Session::session()->applications();
  for ( QList<SUIT_Application*>::iterator appIt = appList.begin(); appIt != appList.end(); ++appIt )
  {
    LightApp_Application* app = ::qobject_cast<LightApp_Application*>( *appIt );
    if ( !app )
      continue;

    QStringList modNameList;
    app->modules( modNameList, false );

    QMap<QString, QString> iconMap;
    app->moduleIconNames( iconMap );

    for ( QStringList::const_iterator it = modNameList.begin(); it != modNameList.end(); ++it )
    {
      if ( !app->isLibExists( *it ) || _prefs_->hasModule( *it ) )
        continue;

      int modId = _prefs_->addPreference( *it );
      if ( iconMap.contains( *it ) )
        _prefs_->setItemIcon( modId, Qtx::scaleIcon( resMgr->loadPixmap( moduleName( *it ), iconMap[*it], false ), 20 ) );
    }

    ModuleList modList;
    app->modules( modList );
    QListIterator<CAM_Module*> itr( modList );
    while ( itr.hasNext() )
    {
      LightApp_Module* mod = 0;

      CAM_Module* anItem = itr.next();
      if ( anItem->inherits( "LightApp_Module" ) )
        mod = (LightApp_Module*)anItem;

      if ( mod && !_prefs_->hasModule( mod->moduleName() ) )
      {
        _prefs_->addPreference( mod->moduleName() );
        mod->createPreferences();
        that->emptyPreferences( mod->moduleName() );
      }
    }
  }
  _prefs_->setItemProperty( "info", tr( "PREFERENCES_NOT_LOADED" ) );

  return myPrefs;
}

/*!
  Adds new module to application
*/
void LightApp_Application::moduleAdded( CAM_Module* mod )
{
  CAM_Application::moduleAdded( mod );

  LightApp_Module* lightMod = 0;
  if ( mod && mod->inherits( "LightApp_Module" ) )
    lightMod = (LightApp_Module*)mod;

  if ( myPrefs && lightMod && !myPrefs->hasModule( lightMod->moduleName() ))
  {
    myPrefs->addPreference( mod->moduleName() );
    lightMod->createPreferences();
    emptyPreferences( mod->moduleName() );
  }
}

void LightApp_Application::emptyPreferences( const QString& modName )
{
  QtxPreferenceItem* item = myPrefs->findItem( modName, true );
  if ( !item || !item->isEmpty() )
    return;

  //  printf( "---------------------> Modify for empty module.\n" );

  QtxPagePrefFrameItem* frm = new QtxPagePrefFrameItem( item->title(), item->parentItem() );
  frm->setIcon( item->icon() );
  frm->setStretch( false );
  item->parentItem()->insertItem( frm, item );
  new QtxPagePrefLabelItem( Qt::AlignCenter, tr( "PREFERENCES_NOT_SUPPORTED" ).arg( modName ), frm );
  delete item;
}

/*!
  Create preferences
*/
void LightApp_Application::createPreferences( LightApp_Preferences* pref )
{
  if ( !pref )
    return;

  QStringList     aValuesList;
  QList<QVariant> anIndicesList;
  QIntList        idList;
  QIntList        txtList;

  // . Top-level "SALOME" preferences group <<start>>
  int salomeCat = pref->addPreference( tr( "PREF_CATEGORY_SALOME" ) );
  pref->setItemIcon( salomeCat, Qtx::scaleIcon( resourceMgr()->loadPixmap( "LightApp", tr( "APP_DEFAULT_ICO" ), false ), 20 ) );

  // .. "General" preferences tab <<start>>
  int genTab = pref->addPreference( tr( "PREF_TAB_GENERAL" ), salomeCat );

  // ... "Language" group <<start>>
  int langGroup = pref->addPreference( tr( "PREF_GROUP_LANGUAGE" ), genTab );
  pref->setItemProperty( "columns", 2, langGroup );
  // .... -> application language
  int curLang = pref->addPreference( tr( "PREF_CURRENT_LANGUAGE" ), langGroup,
                                          LightApp_Preferences::Selector, "language", "language" );
  QStringList aLangs = SUIT_Session::session()->resourceMgr()->stringValue( "language", "languages", "en" ).split( "," );
  QList<QVariant> aIcons;
  foreach ( QString aLang, aLangs ) {
    aIcons << QPixmap( QString( ":/images/%1" ).arg( aLang ) );
  }
  pref->setItemProperty( "strings", aLangs, curLang );
  pref->setItemProperty( "icons",   aIcons, curLang );
  // ... "Language" group <<end>>

  // ... "Look and feel" group <<start>>
  int lookGroup = pref->addPreference( tr( "PREF_GROUP_LOOK_AND_FEEL" ), genTab );
  // .... -> opaque resize
  pref->addPreference( tr( "PREF_OPAQUE_RESIZE" ), lookGroup, LightApp_Preferences::Bool, "desktop", "opaque_resize" );
  // .... -> drop-down buttons 
  pref->addPreference( tr( "PREF_DROP_DOWN_BUTTONS" ), lookGroup, LightApp_Preferences::Bool, "viewers", "drop_down_buttons" );
  // ... "Look and feel" group <<end>>

  // ... "Study properties" group <<start>>
  int studyGroup = pref->addPreference( tr( "PREF_GROUP_STUDY" ), genTab );
  pref->setItemProperty( "columns", 2, studyGroup );
  // .... -> multi-file save
  pref->addPreference( tr( "PREF_MULTI_FILE" ), studyGroup, LightApp_Preferences::Bool, "Study", "multi_file" );
  // .... -> ascii save mode
  pref->addPreference( tr( "PREF_ASCII_FILE" ), studyGroup, LightApp_Preferences::Bool, "Study", "ascii_file" );
  // .... -> store windows geometry
  pref->addPreference( tr( "PREF_STORE_POS" ),  studyGroup, LightApp_Preferences::Bool, "Study", "store_positions" );
  // .... -> auto-save
  int autoSaveInterval = pref->addPreference( tr( "PREF_AUTO_SAVE" ),  studyGroup,
                                              LightApp_Preferences::IntSpin, "Study", "auto_save_interval" );
  pref->setItemProperty( "min",        0, autoSaveInterval );
  pref->setItemProperty( "max",     1440, autoSaveInterval );
  pref->setItemProperty( "special", tr( "PREF_AUTO_SAVE_DISABLED" ), autoSaveInterval );
  // ... "Study properties" group <<end>>

  // ... "Help browser" group <<start>>
  int extgroup = pref->addPreference( tr( "PREF_GROUP_EXT_BROWSER" ), genTab, LightApp_Preferences::Auto, "ExternalBrowser", "use_external_browser");
#ifdef WIN32
  QString platform = "winapplication";
#else
  QString platform = "application";
#endif
  // .... -> browser application
  int apppref = pref->addPreference( tr( "PREF_APP" ), extgroup, LightApp_Preferences::File, "ExternalBrowser", platform );
  pref->setItemProperty( "mode", Qtx::PT_OpenFile, apppref );
  // .... -> browser parameters
  pref->addPreference( tr( "PREF_PARAM" ), extgroup, LightApp_Preferences::String, "ExternalBrowser", "parameters" );
  // ... "Help browser" group <<end>>

  // ... "Python console properties" group <<start>>
  int pythonConsoleGroup = pref->addPreference( tr( "PREF_GROUP_PY_CONSOLE" ), genTab );
  // .... -> font
  pref->addPreference( tr( "PREF_FONT" ), pythonConsoleGroup, LightApp_Preferences::Font, "PyConsole", "font" );
  // .... -> show banner
  pref->addPreference( tr( "PREF_SHOW_BANNER" ), pythonConsoleGroup, LightApp_Preferences::Bool, "PyConsole", "show_banner" );
  // ... "Python console properties" group <<end>>

  // ... "MRU" preferences group <<start>>
  int mruGroup = pref->addPreference( tr( "PREF_GROUP_MRU" ), genTab, LightApp_Preferences::Auto, "MRU", "show_mru" );
  pref->setItemProperty( "columns", 4, mruGroup );
  // number of MRU items
  int mruVisCount = pref->addPreference( tr( "PREF_MRU_VISIBLE_COUNT" ), mruGroup, LightApp_Preferences::IntSpin,
                                         "MRU", "visible_count" );
  pref->setItemProperty( "min", 0,   mruVisCount );
  pref->setItemProperty( "max", 100, mruVisCount );
  // MRU links type
  int mruLinkType = pref->addPreference( tr( "PREF_MRU_LINK_TYPE" ), mruGroup, LightApp_Preferences::Selector,
                                         "MRU", "link_type" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_MRU_LINK_AUTO") << tr("PREF_MRU_LINK_SHORT") << tr("PREF_MRU_LINK_FULL");
  anIndicesList << 0                        << 1                         << 2                       ;
  pref->setItemProperty( "strings", aValuesList,   mruLinkType );
  pref->setItemProperty( "indexes", anIndicesList, mruLinkType );
  // ... "MRU" preferences group <<end>>
  // .. "General" preferences tab <<end>>

  // .. "OCC viewer" group <<start>>
  int occGroup = pref->addPreference( tr( "PREF_GROUP_OCCVIEWER" ), salomeCat );

  // ... "Trihedron" group <<start>>
  int occTriGroup = pref->addPreference( tr( "PREF_TRIHEDRON" ), occGroup );
  pref->setItemProperty( "columns", 2, occTriGroup );
  // .... -> trihedron size
  int occTS = pref->addPreference( tr( "PREF_TRIHEDRON_SIZE" ), occTriGroup,
                                   LightApp_Preferences::DblSpin, "OCCViewer", "trihedron_size" );
  pref->setItemProperty( "min", 1.0E-06, occTS );
  pref->setItemProperty( "max", 1000, occTS );
  // .... -> relative size of trihedron
  pref->addPreference( tr( "PREF_RELATIVE_SIZE" ), occTriGroup, LightApp_Preferences::Bool, "OCCViewer", "relative_size" );
  // .... -> show static trihedron
  pref->addPreference( tr( "PREF_SHOW_STATIC_TRIHEDRON" ), occTriGroup, LightApp_Preferences::Bool, "OCCViewer", "show_static_trihedron" );
  // ... "Trihedron" group <<end>>

  // ... "Iso-lines" group <<start>>
  int isoGroup = pref->addPreference( tr( "PREF_ISOS" ), occGroup );
  pref->setItemProperty( "columns", 2, isoGroup );
  // .... -> nb isos U
  int isoU = pref->addPreference( tr( "PREF_ISOS_U" ), isoGroup,
                                  LightApp_Preferences::IntSpin, "OCCViewer", "iso_number_u" );
  pref->setItemProperty( "min", 0, isoU );
  pref->setItemProperty( "max", 100000, isoU );
  // .... -> nb isos V
  int isoV = pref->addPreference( tr( "PREF_ISOS_V" ), isoGroup,
                                  LightApp_Preferences::IntSpin, "OCCViewer", "iso_number_v" );
  pref->setItemProperty( "min", 0, isoV );
  pref->setItemProperty( "max", 100000, isoV );
  // ... "Iso-lines" group <<end>>

  // ... "Background" group <<start>>
  int bgGroup = pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), occGroup );
  //  pref->setItemProperty( "columns", 2, bgGroup );
  aValuesList.clear();
  anIndicesList.clear();
  txtList.clear();
  QString formats = OCCViewer_Viewer::backgroundData( aValuesList, idList, txtList );
  foreach( int gid, idList ) anIndicesList << gid;
  // .... -> 3D viewer background
  int bgId = pref->addPreference( tr( "PREF_3DVIEWER_BACKGROUND" ), bgGroup,
				  LightApp_Preferences::Background, "OCCViewer", "background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // .... -> XZ viewer background
  bgId = pref->addPreference( tr( "PREF_XZVIEWER_BACKGROUND" ), bgGroup,
			      LightApp_Preferences::Background, "OCCViewer", "xz_background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // .... -> YZ viewer background
  bgId = pref->addPreference( tr( "PREF_YZVIEWER_BACKGROUND" ), bgGroup,
			      LightApp_Preferences::Background, "OCCViewer", "yz_background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // .... -> XY viewer background
  bgId = pref->addPreference( tr( "PREF_XYVIEWER_BACKGROUND" ), bgGroup,
			      LightApp_Preferences::Background, "OCCViewer", "xy_background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // ... "Background" group <<end>>

  // ... -> empty frame (for layout) <<start>>
  int occGen = pref->addPreference( "", occGroup, LightApp_Preferences::Frame );
  pref->setItemProperty( "margin",  0, occGen );
  pref->setItemProperty( "columns", 2, occGen );
  // .... -> navigation mode
  int occStyleMode = pref->addPreference( tr( "PREF_NAVIGATION" ), occGen,
                                          LightApp_Preferences::Selector, "OCCViewer", "navigation_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_STANDARD_STYLE") << tr("PREF_KEYFREE_STYLE");
  anIndicesList << 0                         << 1;
  pref->setItemProperty( "strings", aValuesList,   occStyleMode );
  pref->setItemProperty( "indexes", anIndicesList, occStyleMode );
  // .... -> zooming mode
#if OCC_VERSION_LARGE > 0x0603000A // available only with OCC-6.3-sp11 and higher version
  int occZoomingStyleMode = pref->addPreference( tr( "PREF_ZOOMING" ), occGen,
                                                 LightApp_Preferences::Selector, "OCCViewer", "zooming_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ZOOMING_AT_CENTER") << tr("PREF_ZOOMING_AT_CURSOR");
  anIndicesList << 0                            << 1;
  pref->setItemProperty( "strings", aValuesList,   occZoomingStyleMode );
  pref->setItemProperty( "indexes", anIndicesList, occZoomingStyleMode );
#endif
  // ... -> empty frame (for layout) <<end>>
  // .. "OCC viewer" group <<end>>

  // .. "VTK viewer" group <<start>>
  int vtkGroup = pref->addPreference( tr( "PREF_GROUP_VTKVIEWER" ), salomeCat ); //viewTab

  // ... -> empty frame (for layout) <<start>>
  int vtkGen = pref->addPreference( "", vtkGroup, LightApp_Preferences::Frame );
  //pref->setItemProperty( "columns", 2, vtkGen );
  // .... -> projection mode
  int vtkProjMode = pref->addPreference( tr( "PREF_PROJECTION_MODE" ), vtkGen,
                                         LightApp_Preferences::Selector, "VTKViewer", "projection_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ORTHOGRAPHIC") << tr("PREF_PERSPECTIVE");
  anIndicesList << 0                       << 1;
  pref->setItemProperty( "strings", aValuesList,   vtkProjMode );
  pref->setItemProperty( "indexes", anIndicesList, vtkProjMode );
  // .... -> background
  aValuesList.clear();
  anIndicesList.clear();
  txtList.clear();
  formats = SVTK_Viewer::backgroundData( aValuesList, idList, txtList );
  foreach( int gid, idList ) anIndicesList << gid;
  bgId = pref->addPreference( tr( "PREF_VIEWER_BACKGROUND" ), vtkGen,
			      LightApp_Preferences::Background, "VTKViewer", "background" );
  pref->setItemProperty( "gradient_names", aValuesList, bgId );
  pref->setItemProperty( "gradient_ids", anIndicesList, bgId );
  pref->setItemProperty( "texture_enabled", !txtList.isEmpty(), bgId );
  pref->setItemProperty( "texture_center_enabled", (bool)txtList.contains( Qtx::CenterTexture ), bgId );
  pref->setItemProperty( "texture_tile_enabled", (bool)txtList.contains( Qtx::TileTexture ), bgId );
  pref->setItemProperty( "texture_stretch_enabled", (bool)txtList.contains( Qtx::StretchTexture ), bgId );
  pref->setItemProperty( "custom_enabled", false, bgId );
  pref->setItemProperty( "image_formats", formats, bgId );
  // .... -> navigation mode
  int vtkStyleMode = pref->addPreference( tr( "PREF_NAVIGATION" ), vtkGen,
                                          LightApp_Preferences::Selector, "VTKViewer", "navigation_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_STANDARD_STYLE") << tr("PREF_KEYFREE_STYLE");
  anIndicesList << 0                         << 1;
  pref->setItemProperty( "strings", aValuesList,   vtkStyleMode );
  pref->setItemProperty( "indexes", anIndicesList, vtkStyleMode );
  // .... -> zooming mode
  int vtkZoomingStyleMode = pref->addPreference( tr( "PREF_ZOOMING" ), vtkGen,
                                                 LightApp_Preferences::Selector, "VTKViewer", "zooming_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ZOOMING_AT_CENTER") << tr("PREF_ZOOMING_AT_CURSOR");
  anIndicesList << 0                            << 1;
  pref->setItemProperty( "strings", aValuesList,   vtkZoomingStyleMode );
  pref->setItemProperty( "indexes", anIndicesList, vtkZoomingStyleMode );
  // .... -> speed increment
  int vtkSpeed = pref->addPreference( tr( "PREF_INCREMENTAL_SPEED" ), vtkGen,
                                      LightApp_Preferences::IntSpin, "VTKViewer", "speed_value" );
  pref->setItemProperty( "min", 1, vtkSpeed );
  pref->setItemProperty( "max", 1000, vtkSpeed );
  // .... -> speed mode
  int vtkSpeedMode = pref->addPreference( tr( "PREF_INCREMENTAL_SPEED_MODE" ), vtkGen,
                                          LightApp_Preferences::Selector, "VTKViewer", "speed_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_ARITHMETIC") << tr("PREF_GEOMETRICAL");
  anIndicesList << 0                     << 1;
  pref->setItemProperty( "strings", aValuesList,   vtkSpeedMode );
  pref->setItemProperty( "indexes", anIndicesList, vtkSpeedMode );
  // .... -> dynamic pre-selection
  pref->addPreference( tr( "PREF_DYNAMIC_PRESELECTION" ),  vtkGen, LightApp_Preferences::Bool, "VTKViewer", "dynamic_preselection" );
  // ... -> empty frame (for layout) <<end>>

  // ... "Trihedron" group <<start>>
  int vtkTriGroup = pref->addPreference( tr( "PREF_TRIHEDRON" ), vtkGroup );
  pref->setItemProperty( "columns", 2, vtkTriGroup );
  // .... -> trihedron size
  int vtkTS = pref->addPreference( tr( "PREF_TRIHEDRON_SIZE" ), vtkTriGroup,
                                   LightApp_Preferences::DblSpin, "VTKViewer", "trihedron_size" );
  pref->setItemProperty( "min", 1.0E-06, vtkTS );
  pref->setItemProperty( "max", 150, vtkTS );
  // .... -> relative size of trihedron
  pref->addPreference( tr( "PREF_RELATIVE_SIZE" ), vtkTriGroup, LightApp_Preferences::Bool, "VTKViewer", "relative_size" );
  // .... -> static trihedron
  pref->addPreference( tr( "PREF_SHOW_STATIC_TRIHEDRON" ), vtkTriGroup, LightApp_Preferences::Bool, "VTKViewer", "show_static_trihedron" );
  // ... "Trihedron" group <<end>>

  // ... space mouse sub-group <<start>>
  int vtkSM = pref->addPreference( tr( "PREF_FRAME_SPACEMOUSE" ), vtkGroup, LightApp_Preferences::GroupBox );
  //pref->setItemProperty( "columns", 2, vtkSM );
  // .... -> decrease speed increment
  int spacemousePref1 = pref->addPreference( tr( "PREF_SPACEMOUSE_FUNC_1" ), vtkSM,
                                             LightApp_Preferences::Selector, "VTKViewer",
                                             "spacemouse_func1_btn" );
  // .... -> increase speed increment
  int spacemousePref2 = pref->addPreference( tr( "PREF_SPACEMOUSE_FUNC_2" ), vtkSM,
                                             LightApp_Preferences::Selector, "VTKViewer",
                                             "spacemouse_func2_btn" );
  // .... -> dominant / combined switch  
  int spacemousePref3 = pref->addPreference( tr( "PREF_SPACEMOUSE_FUNC_3" ), vtkSM,
                                             LightApp_Preferences::Selector, "VTKViewer",
                                             "spacemouse_func5_btn" ); //
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_1" )  << tr( "PREF_SPACEMOUSE_BTN_2" ) << tr( "PREF_SPACEMOUSE_BTN_3" );
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_4" )  << tr( "PREF_SPACEMOUSE_BTN_5" ) << tr( "PREF_SPACEMOUSE_BTN_6" );
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_7" )  << tr( "PREF_SPACEMOUSE_BTN_8" ) << tr( "PREF_SPACEMOUSE_BTN_*" );
  aValuesList << tr( "PREF_SPACEMOUSE_BTN_10" ) << tr( "PREF_SPACEMOUSE_BTN_11" );
  anIndicesList << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11;
  pref->setItemProperty( "strings", aValuesList,   spacemousePref1 );
  pref->setItemProperty( "indexes", anIndicesList, spacemousePref1 );
  pref->setItemProperty( "strings", aValuesList,   spacemousePref2 );
  pref->setItemProperty( "indexes", anIndicesList, spacemousePref2 );
  pref->setItemProperty( "strings", aValuesList,   spacemousePref3 );
  pref->setItemProperty( "indexes", anIndicesList, spacemousePref3 );
  // ... space mouse sub-group <<end>>

  // ... avi recording sub-group <<start>>
  int vtkRec = pref->addPreference( tr( "PREF_FRAME_RECORDING" ), vtkGroup, LightApp_Preferences::GroupBox );
  pref->setItemProperty( "columns", 2, vtkRec );
  // .... -> recording mode
  int modePref = pref->addPreference( tr( "PREF_RECORDING_MODE" ), vtkRec,
                                      LightApp_Preferences::Selector, "VTKViewer", "recorder_mode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr( "PREF_SKIPPED_FRAMES" ) << tr( "PREF_ALL_DISLPAYED_FRAMES" );
  anIndicesList << 0                           << 1;
  pref->setItemProperty( "strings", aValuesList,   modePref );
  pref->setItemProperty( "indexes", anIndicesList, modePref );
  // .... -> fps
  int fpsPref = pref->addPreference( tr( "PREF_FPS" ), vtkRec,
                                     LightApp_Preferences::DblSpin, "VTKViewer", "recorder_fps" );
  pref->setItemProperty( "min", 0.1, fpsPref );
  pref->setItemProperty( "max", 100, fpsPref );
  // .... -> quality
  int qualityPref = pref->addPreference( tr( "PREF_QUALITY" ), vtkRec,
                                         LightApp_Preferences::IntSpin, "VTKViewer", "recorder_quality" );
  pref->setItemProperty( "min", 1, qualityPref );
  pref->setItemProperty( "max", 100, qualityPref );
  // .... -> progressive mode
  pref->addPreference( tr( "PREF_PROGRESSIVE" ), vtkRec,
                       LightApp_Preferences::Bool, "VTKViewer", "recorder_progressive" );
  // ... avi recording sub-group <<end>>

  // ... group names sub-group <<start>>
  int vtkGN = pref->addPreference( tr( "PREF_FRAME_GROUP_NAMES" ), vtkGroup,
                                   LightApp_Preferences::GroupBox, "VTKViewer", "show_group_names" );
  pref->setItemProperty( "columns", 2, vtkGN );
  // .... -> text color
  pref->addPreference( tr(  "PREF_GROUP_NAMES_TEXT_COLOR" ), vtkGN,
                       LightApp_Preferences::Color, "VTKViewer", "group_names_text_color" );
  // .... -> transparency
  int transPref = pref->addPreference( tr( "PREF_GROUP_NAMES_TRANSPARENCY" ), vtkGN,
                                       LightApp_Preferences::DblSpin, "VTKViewer", "group_names_transparency" );
  pref->setItemProperty( "min", 0.0, transPref );
  pref->setItemProperty( "max", 1.0, transPref );
  pref->setItemProperty( "step", 0.1, transPref );
  // ... -> group names sub-group <<end>>
  // .. "VTK viewer" group <<end>>

  // .. "Plot2d viewer" group <<start>>
  int plot2dGroup = pref->addPreference( tr( "PREF_GROUP_PLOT2DVIEWER" ), salomeCat ); //viewTab
  //pref->setItemProperty( "columns", 2, plot2dGroup );

  // ... -> show legend
  pref->addPreference( tr( "PREF_SHOW_LEGEND" ), plot2dGroup,
                       LightApp_Preferences::Bool, "Plot2d", "ShowLegend" );
  // ... -> legend position
  int legendPosition = pref->addPreference( tr( "PREF_LEGEND_POSITION" ), plot2dGroup,
                                            LightApp_Preferences::Selector, "Plot2d", "LegendPos" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_LEFT") << tr("PREF_RIGHT") << tr("PREF_TOP") << tr("PREF_BOTTOM");
  anIndicesList << 0               << 1                << 2              << 3                ;
  pref->setItemProperty( "strings", aValuesList,   legendPosition );
  pref->setItemProperty( "indexes", anIndicesList, legendPosition );
  // ... -> legend font
  pref->addPreference( tr( "PREF_LEGEND_FONT" ), plot2dGroup, LightApp_Preferences::Font, "Plot2d", "LegendFont" );
  // ... -> curve type
  int curveType = pref->addPreference( tr( "PREF_CURVE_TYPE" ), plot2dGroup,
                                       LightApp_Preferences::Selector, "Plot2d", "CurveType" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_POINTS") << tr("PREF_LINES") << tr("PREF_SPLINE");
  anIndicesList << 0                 << 1                << 2                ;
  pref->setItemProperty( "strings", aValuesList,   curveType );
  pref->setItemProperty( "indexes", anIndicesList, curveType );
  // ... -> marker size
  int markerSize = pref->addPreference( tr( "PREF_MARKER_SIZE" ), plot2dGroup,
                                        LightApp_Preferences::IntSpin, "Plot2d", "MarkerSize" );
  pref->setItemProperty( "min", 0, markerSize );
  pref->setItemProperty( "max", 100, markerSize );
  // ... -> horizontal scaling mode
  int horScale = pref->addPreference( tr( "PREF_HOR_AXIS_SCALE" ), plot2dGroup,
                                      LightApp_Preferences::Selector, "Plot2d", "HorScaleMode" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList   << tr("PREF_LINEAR") << tr("PREF_LOGARITHMIC");
  anIndicesList << 0                 << 1                     ;
  pref->setItemProperty( "strings", aValuesList,   horScale );
  pref->setItemProperty( "indexes", anIndicesList, horScale );
  // ... -> vertical scaling mode
  int verScale = pref->addPreference( tr( "PREF_VERT_AXIS_SCALE" ), plot2dGroup,
                                      LightApp_Preferences::Selector, "Plot2d", "VerScaleMode" );
  pref->setItemProperty( "strings", aValuesList,   verScale );
  pref->setItemProperty( "indexes", anIndicesList, verScale );
  // ... -> background
  pref->addPreference( tr( "PREF_VIEWER_BACKGROUND_COLOR" ), plot2dGroup,
                       LightApp_Preferences::Color, "Plot2d", "Background" );
  // ... -> font color
  pref->addPreference( tr( "PREF_FONT_COLOR" ), plot2dGroup, LightApp_Preferences::Color, "Plot2d", "LegendFontColor" );
  // ... -> selection font color
  pref->addPreference( tr( "PREF_SELECTED_FONT_COLOR" ), plot2dGroup, LightApp_Preferences::Color, "Plot2d", "SelectedLegendFontColor" );
  // ... -> selection color
  pref->addPreference( tr( "PREF_VIEWER_SELECTION" ), plot2dGroup,
                       LightApp_Preferences::Color, "Plot2d", "SelectionColor" );
  // ... -> errors/deviation colot
  pref->addPreference( tr( "PREF_DEVIATION_COLOR" ), plot2dGroup,
                       LightApp_Preferences::Color, "Plot2d", "DeviationMarkerColor" );
  // ... -> deviation markers line size
  int deviationMarkerLw = pref->addPreference( tr( "PREF_DEVIATION_MARKER_LW" ), plot2dGroup,
                                        LightApp_Preferences::IntSpin, "Plot2d", "DeviationMarkerLineWidth" );
  pref->setItemProperty( "min", 1, deviationMarkerLw );
  pref->setItemProperty( "max", 5, deviationMarkerLw );
  // ... -> deviation markers tick mark size
  int deviationMarkerTs = pref->addPreference( tr( "PREF_DEVIATION_MARKER_TS" ), plot2dGroup,
                                        LightApp_Preferences::IntSpin, "Plot2d", "DeviationMarkerTickSize" );
  pref->setItemProperty( "min", 1, deviationMarkerTs );
  pref->setItemProperty( "max", 5, deviationMarkerTs );
  // .. "Plot2d viewer" group <<end>>

  // .. "Directories" preferences tab <<start>>
  int dirTab = pref->addPreference( tr( "PREF_TAB_DIRECTORIES" ), salomeCat );
  // ... --> quick directories list
  int dirGroup = pref->addPreference( tr( "PREF_GROUP_DIRECTORIES" ), dirTab );
  pref->addPreference( tr( "" ), dirGroup,
                       LightApp_Preferences::DirList, "FileDlg", "QuickDirList" );
  // .. "Directories" preferences tab <<end>>

  // .. "Object browser" preferences tab <<start>>
  int obTab = pref->addPreference( tr( "PREF_TAB_OBJBROWSER" ), salomeCat );

  // ... "Search tool" group <<start>>
  int stGroup = pref->addPreference( tr( "PREF_OBJ_BROWSER_SEARCH_TOOL" ), obTab );
  // .... --> auto-hide
  pref->addPreference( tr( "PREF_AUTO_HIDE_SEARCH_TOOL" ), stGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "auto_hide_search_tool" );
  // ... "Search tool" group <<end>>

  // ... "Object browser settings" group <<start>>
  int objSetGroup = pref->addPreference( tr( "PREF_GROUP_LOOK_AND_FEEL" ), obTab );
  pref->setItemProperty( "columns", 2, objSetGroup );
  // .... -> auto size first column
  pref->addPreference( tr( "PREF_AUTO_SIZE_FIRST" ), objSetGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "auto_size_first" );
  // .... -> auto size other columns
  pref->addPreference( tr( "PREF_AUTO_SIZE" ), objSetGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "auto_size" );
  // .... -> resize columns on expand item
  pref->addPreference( tr( "PREF_RESIZE_ON_EXPAND_ITEM" ), objSetGroup, LightApp_Preferences::Bool,
                       "ObjectBrowser", "resize_on_expand_item" );
  // .... -> browse to published object
  int browsePublished = pref->addPreference( tr( "PREF_BROWSE_TO_THE_PUBLISHED_OBJECT" ), objSetGroup, LightApp_Preferences::Selector,
                                             "ObjectBrowser", "browse_published_object" );
  aValuesList.clear();
  anIndicesList.clear();
  aValuesList << tr( "PREF_BROWSE_NEVER" ) << tr( "PREF_BROWSE_AFTER_APPLY_AND_CLOSE_ONLY" ) << tr( "PREF_BROWSE_ALWAYS" );
  anIndicesList << BP_Never << BP_ApplyAndClose << BP_Always;
  pref->setItemProperty( "strings", aValuesList,   browsePublished );
  pref->setItemProperty( "indexes", anIndicesList, browsePublished );
  // ... "Object browser settings" group <<end>>
  // .. "Object browser" preferences tab <<end>>

  // .. "Shortcuts" preferences tab <<start>>
  int shortcutTab = pref->addPreference( tr( "PREF_TAB_SHORTCUTS" ), salomeCat );
  // ... "Shortcuts settings" group <<start>>
  int shortcutGroup = pref->addPreference( tr( "PREF_GROUP_SHORTCUTS" ), shortcutTab );
  pref->addPreference( tr( "" ), shortcutGroup,
                       LightApp_Preferences::ShortcutTree, "shortcuts" );
  // ... "Shortcuts settings" group <<end>>
  // .. "Shortcuts" preferences tab <<end>>
  // . Top-level "SALOME" preferences group <<end>>

  pref->retrieve();
}

/*!
  Changes appearance of application according to changed preferences
  \param sec - section
  \param param - name of changed parameter
*/
void LightApp_Application::preferencesChanged( const QString& sec, const QString& param )
{
  SUIT_ResourceMgr* resMgr = resourceMgr();
  if ( !resMgr )
    return;

  if ( sec == "viewers" && param == "drop_down_buttons" )
  {
    ViewManagerList vmlist = viewManagers();
    foreach( SUIT_ViewManager* vm, vmlist )
    {
      QVector<SUIT_ViewWindow*> vwlist = vm->getViews();
      foreach( SUIT_ViewWindow* vw, vwlist )
        if ( vw ) vw->setDropDownButtons( resMgr->booleanValue( "viewers", "drop_down_buttons", true ) );
    }
  }

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && (param == QString( "trihedron_size" ) || param == QString( "relative_size" )))
  {
    double sz = resMgr->doubleValue( sec, "trihedron_size", -1 );
    bool relative = resMgr->booleanValue( sec, "relative_size", true );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() && sz >= 0 )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setTrihedronSize( sz, relative );
      occVM->getAISContext()->UpdateCurrentViewer();
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "show_static_trihedron" ) )
  {
    bool isVisible = resMgr->booleanValue( "OCCViewer", "show_static_trihedron", true );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = dynamic_cast<OCCViewer_Viewer*>( vm );
      if( occVM )
      {
        occVM->setStaticTrihedronDisplayed( isVisible );
      }
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "navigation_mode" ) )
  {
    int mode = resMgr->integerValue( "OCCViewer", "navigation_mode", 0 );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setInteractionStyle( mode );
    }
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && param == QString( "zooming_mode" ) )
  {
    int mode = resMgr->integerValue( "OCCViewer", "zooming_mode", 0 );
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "OCCViewer_Viewer" ) )
        continue;

      OCCViewer_Viewer* occVM = (OCCViewer_Viewer*)vm;
      occVM->setZoomingStyle( mode );
    }
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && (param == QString( "trihedron_size" ) || param == QString( "relative_size" )) )
  {
    double sz = resMgr->doubleValue( "VTKViewer", "trihedron_size", -1 );
    bool isRelative = resMgr->booleanValue( "VTKViewer", "relative_size", true );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() && sz >= 0 )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM )
      {
        vtkVM->setTrihedronSize( sz, isRelative );
        vtkVM->Repaint();
      }
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && (param == QString( "speed_value" ) || param == QString( "speed_mode" )) )
  {
    int speed = resMgr->integerValue( "VTKViewer", "speed_value", 10 );
    int mode = resMgr->integerValue( "VTKViewer", "speed_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setIncrementalSpeed( speed, mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "projection_mode" ) )
  {
    int mode = resMgr->integerValue( "VTKViewer", "projection_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setProjectionMode( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "navigation_mode" ) )
  {
    int mode = resMgr->integerValue( "VTKViewer", "navigation_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setInteractionStyle( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "zooming_mode" ) )
  {
    int mode = resMgr->integerValue( "VTKViewer", "zooming_mode", 0 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setZoomingStyle( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "dynamic_preselection" ) )
  {
    bool mode = resMgr->booleanValue( "VTKViewer", "dynamic_preselection", true );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setDynamicPreSelection( mode );
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && param == QString( "show_static_trihedron" ) )
  {
    bool isVisible = resMgr->booleanValue( "VTKViewer", "show_static_trihedron", true );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM )
      {
        vtkVM->setStaticTrihedronVisible( isVisible );
        vtkVM->Repaint();
      }
    }
#endif
  }
#endif

#ifndef DISABLE_VTKVIEWER
  if ( sec == QString( "VTKViewer" ) && (param == QString( "spacemouse_func1_btn" ) ||
                                         param == QString( "spacemouse_func2_btn" ) ||
                                         param == QString( "spacemouse_func5_btn" ) ) )
  {
    int btn1 = resMgr->integerValue( "VTKViewer", "spacemouse_func1_btn", 1 );
    int btn2 = resMgr->integerValue( "VTKViewer", "spacemouse_func2_btn", 2 );
    int btn3 = resMgr->integerValue( "VTKViewer", "spacemouse_func5_btn", 9 );
    QList<SUIT_ViewManager*> lst;
#ifndef DISABLE_SALOMEOBJECT
    viewManagers( SVTK_Viewer::Type(), lst );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      SUIT_ViewModel* vm = it.next()->getViewModel();
      if ( !vm || !vm->inherits( "SVTK_Viewer" ) )
        continue;

      SVTK_Viewer* vtkVM = dynamic_cast<SVTK_Viewer*>( vm );
      if( vtkVM ) vtkVM->setSpacemouseButtons( btn1, btn2, btn3 );
    }
#endif
  }
#endif

#ifndef DISABLE_OCCVIEWER
  if ( sec == QString( "OCCViewer" ) && ( param == QString( "iso_number_u" ) || param == QString( "iso_number_v" ) ) )
  {
    QList<SUIT_ViewManager*> lst;
    viewManagers( OCCViewer_Viewer::Type(), lst );
    int u = resMgr->integerValue( sec, "iso_number_u" );
    int v = resMgr->integerValue( sec, "iso_number_v" );
    QListIterator<SUIT_ViewManager*> it( lst );
    while ( it.hasNext() )
    {
      OCCViewer_ViewManager* mgr = dynamic_cast<OCCViewer_ViewManager*>( it.next() );
      if( mgr && mgr->getOCCViewer() )
        mgr->getOCCViewer()->setIsos( u, v );
    }
  }
#endif

  if( sec=="ObjectBrowser" )
  {
    SUIT_DataBrowser* ob = objectBrowser();
    if ( !ob )
      return;

    if ( param=="auto_size_first" )
    {
      bool autoSizeFirst = resMgr->booleanValue( "ObjectBrowser", "auto_size_first", true );
      ob->setAutoSizeFirstColumn( autoSizeFirst );
      if ( autoSizeFirst )
        ob->adjustFirstColumnWidth();
    }
    else if ( param=="auto_size" ) {
      bool autoSize = resMgr->booleanValue( "ObjectBrowser", "auto_size", false );
      ob->setAutoSizeColumns(autoSize);
      if ( autoSize )
        ob->adjustColumnsWidth();
    }
    else if ( param=="resize_on_expand_item" ) {
      bool resizeOnExpandItem = resMgr->booleanValue( "ObjectBrowser", "resize_on_expand_item", false );
      ob->setResizeOnExpandItem(resizeOnExpandItem);
    }
    else if ( param == "auto_hide_search_tool" ) {
      ob->searchTool()->enableAutoHide( resMgr->booleanValue( "ObjectBrowser", "auto_hide_search_tool" ) );
    }
  }

  if( sec=="Study" )
  {
    if( param=="store_positions" )
      updateWindows();
    if( param=="auto_save_interval" ) {
      myAutoSaveTimer->stop();
      int autoSaveInterval = resMgr->integerValue( "Study", "auto_save_interval", 0 );
      if ( activeStudy() && autoSaveInterval > 0 ) myAutoSaveTimer->start( autoSaveInterval*60000 );
    }
  }

#ifndef DISABLE_PYCONSOLE
  if( sec=="PyConsole" && pythonConsole() )
  {
    if ( param=="font" ) {
      pythonConsole()->setFont( resMgr->fontValue( "PyConsole", "font" ) );
    }
    else if ( param=="show_banner" ) {
      pythonConsole()->setIsShowBanner( resMgr->booleanValue( "PyConsole", "show_banner", true ) );
    }
  }
#endif

  if( sec=="MRU" )
  {
    QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
    if ( mru ) {
      if ( param == "visible_count" )
        mru->setVisibleCount( resMgr->integerValue( "MRU", "visible_count", 5 ) );    // 5 MRU items by default
      else if ( param == "max_count" )
        mru->setHistoryCount( resMgr->integerValue( "MRU", "max_count", -1 ) );       // unlimited history by default
      else if ( param == "insert_mode" )
        mru->setInsertMode( resMgr->integerValue( "MRU", "insert_mode", 0 ) );        // QtxMRUAction::MoveFirst by default
      else if ( param == "link_type" )
        mru->setLinkType( resMgr->integerValue( "MRU", "link_type", 0 ) );            // QtxMRUAction::LinkAuto by default
      else if ( param == "show_clear" )
        mru->setClearPossible( resMgr->booleanValue( "MRU", "show_clear", false ) );  // do not show "Clear" item by default
      else if ( param == "show_mru" )
        mru->setVisible( resMgr->booleanValue( "MRU", "show_mru", false ) );          // do not show MRU menu item by default
    }
  }
  if ( sec == "language" && param == "language" )
  {
    SUIT_MessageBox::information( desktop(), tr( "WRN_WARNING" ), tr( "LANG_CHANGED" ) );
  }
  if ( sec == "desktop" && param == "opaque_resize" ) {
    bool opaqueResize = resMgr->booleanValue( "desktop", "opaque_resize", false );
    QMainWindow::DockOptions dopts = desktop()->dockOptions();
    if ( opaqueResize ) dopts |= QMainWindow::AnimatedDocks;
    else                dopts &= ~QMainWindow::AnimatedDocks;
    desktop()->setDockOptions( dopts );
    desktop()->setOpaqueResize( opaqueResize );
    if ( dynamic_cast<STD_TabDesktop*>( desktop() ) )
      dynamic_cast<STD_TabDesktop*>( desktop() )->workstack()->setOpaqueResize( opaqueResize );
  }

  if ( sec == "ExternalBrowser" && param == "use_external_browser" ) {
    if ( resMgr->booleanValue("ExternalBrowser", "use_external_browser", false ) )
      {
        if(QtxWebBrowser::webBrowser())
          QtxWebBrowser::webBrowser()->close();
      }
  }

#ifndef DISABLE_PLOT2DVIEWER
  if ( sec == "Plot2d" ) {
    if( param == "SelectionColor" ) {
      QColor c = resMgr->colorValue( sec, param );
      Plot2d_Object::setSelectionColor(c);
    }
    else if (param == "SelectedLegendFontColor") {
      QColor c = resMgr->colorValue( sec, param );      
      Plot2d_Object::setHighlightedLegendTextColor(c);
    }
  }
#endif
}

/*!
  Loads preferences
*/
void LightApp_Application::loadPreferences()
{
  CAM_Application::loadPreferences();

  SUIT_ResourceMgr* aResMgr = resourceMgr();

  if ( !aResMgr )
    return;

  static bool mru_load = true;
  if ( mru_load )
  {
    QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
    if ( mru ) {
      mru->setVisible( aResMgr->booleanValue( "MRU", "show_mru", false ) );         // do not show MRU menu item by default
      mru->setVisibleCount( aResMgr->integerValue( "MRU", "visible_count", 5 ) );   // 5 MRU items by default
      mru->setHistoryCount( aResMgr->integerValue( "MRU", "max_count", -1 ) );      // unlimited history by default
      mru->setInsertMode( aResMgr->integerValue( "MRU", "insert_mode", 0 ) );       // QtxMRUAction::MoveFirst by default
      mru->setLinkType( aResMgr->integerValue( "MRU", "link_type", 0 ) );           // QtxMRUAction::LinkAuto by default
      mru->setClearPossible( aResMgr->booleanValue( "MRU", "show_clear", false ) ); // do not show "Clear" item by default
      mru->loadLinks( aResMgr, "MRU" );
    }
    mru_load = false;
  }

  myWinGeom.clear();
  QStringList mods = aResMgr->parameters( "windows_geometry" );
  for ( QStringList::const_iterator it = mods.begin(); it != mods.end(); ++it )
  {
    QByteArray arr;
    if ( aResMgr->value( "windows_geometry", *it, arr ) )
      myWinGeom.insert( *it, arr );
  }

  myWinVis.clear();
  mods = aResMgr->parameters( "windows_visibility" );
  for ( QStringList::const_iterator itr = mods.begin(); itr != mods.end(); ++itr )
  {
    QByteArray arr;
    if ( aResMgr->value( "windows_visibility", *itr, arr ) )
      myWinVis.insert( *itr, arr );
  }

  if ( desktop() ) {
    desktop()->retrieveGeometry( aResMgr->stringValue( "desktop", "geometry" ) );
    bool opaqueResize = aResMgr->booleanValue( "desktop", "opaque_resize", false );
    QMainWindow::DockOptions dopts = desktop()->dockOptions();
    if ( opaqueResize ) dopts |= QMainWindow::AnimatedDocks;
    else                dopts &= ~QMainWindow::AnimatedDocks;
    desktop()->setDockOptions( dopts );
    desktop()->setOpaqueResize( opaqueResize );
    if ( dynamic_cast<STD_TabDesktop*>( desktop() ) )
      dynamic_cast<STD_TabDesktop*>( desktop() )->workstack()->setOpaqueResize( opaqueResize );
  }
}

/*!
  Saves preferences
*/
void LightApp_Application::savePreferences()
{
  CAM_Application::savePreferences();

  saveDockWindowsState();

  SUIT_ResourceMgr* aResMgr = resourceMgr();

  if ( !aResMgr )
    return;

  QtxMRUAction* mru = ::qobject_cast<QtxMRUAction*>( action( MRUId ) );
  if ( mru )
    mru->saveLinks( aResMgr, "MRU" );

  for ( WinGeom::const_iterator it = myWinGeom.begin(); it != myWinGeom.end(); ++it )
    aResMgr->setValue( "windows_geometry", it.key(), it.value() );

  for ( WinVis::const_iterator itr = myWinVis.begin(); itr != myWinVis.end(); ++itr )
    aResMgr->setValue( "windows_visibility", itr.key(), itr.value() );

  if ( desktop() )
    aResMgr->setValue( "desktop", "geometry", desktop()->storeGeometry() );

  aResMgr->save();
}

/*!
  Updates desktop title
*/
void LightApp_Application::updateDesktopTitle()
{
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  if ( activeStudy() ) {
    QString sName = SUIT_Tools::file( activeStudy()->studyName().trimmed(), false );
    aTitle += QString( " - [%1]" ).arg( sName );
  }

  desktop()->setWindowTitle( aTitle );
}

/*!
  \brief Get map of the operations which can be performed
  on the module activation.

  The method should return the map of the kind \c {<id>:<name>}
  where \c <id> is an integer identifier of the operation and
  \c <name> is a title for the button to be added to the
  dialog box. After user selects the required operation by the
  clicking the corresponding button in the dialog box, its identifier
  is passed to the moduleActionSelected() method to process
  the made choice.

  \return map of the operations
  \sa moduleActionSelected()
*/
QMap<int, QString> LightApp_Application::activateModuleActions() const
{
  QMap<int, QString> opmap;
  opmap.insert( NewStudyId,  tr( "ACTIVATE_MODULE_OP_NEW" ) );
  opmap.insert( OpenStudyId, tr( "ACTIVATE_MODULE_OP_OPEN" ) );
  return opmap;
}

/*!
  \brief Called when the used selectes required operation chosen
  from "Activate module" dialog box.

  Performs the required operation according to the user choice.

  \param id operation identifier
  \sa activateModuleActions()
*/
void LightApp_Application::moduleActionSelected( const int id )
{
  switch ( id ) {
  case NewStudyId:
    onNewDoc();
    break;
  case OpenStudyId:
    onOpenDoc();
    break;
  default:
    break;
  }
}

/*!
  Updates windows after close document
*/
void LightApp_Application::afterCloseDoc()
{
  updateWindows();

  CAM_Application::afterCloseDoc();
}

/*!
  Updates actions of active module
*/
void LightApp_Application::updateModuleActions()
{
  QString modName;
  if ( activeModule() ) {
    modName = activeModule()->moduleName();
    if ( !isModuleAccessible( modName ) ) {
      QList<SUIT_Application*> apps = SUIT_Session::session()->applications();
      foreach( SUIT_Application* app, apps ) {
        LightApp_Application* lapp = dynamic_cast<LightApp_Application*>( app );
        if ( lapp && lapp != this )
          lapp->removeModuleAction( modName );
      }
    }
  }

  LightApp_ModuleAction* moduleAction =
    qobject_cast<LightApp_ModuleAction*>( action( ModulesListId ) );
  if ( moduleAction )
    moduleAction->setActiveModule( modName );
}

void LightApp_Application::removeModuleAction( const QString& modName )
{
  LightApp_ModuleAction* moduleAction =
    qobject_cast<LightApp_ModuleAction*>( action( ModulesListId ) );
  if ( moduleAction )
    moduleAction->removeModule( modName );
}

/*!
  Gets current windows.
  \param winMap - output current windows map.
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

/*!
  Gets current view managers.
  \param lst - output current view managers list.
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

/*!
  Updates windows
*/
void LightApp_Application::updateWindows()
{
  QMap<int, int> winMap;
  currentWindows( winMap );

  if ( activeStudy() )
  {
    for ( QMap<int, int>::ConstIterator it = winMap.begin(); it != winMap.end(); ++it )
    {
      if ( !dockWindow( it.key() ) )
        getWindow( it.key() );
    }
  }

  for ( WinMap::ConstIterator it = myWin.begin(); it != myWin.end(); ++it )
  {
    QWidget* wid = it.value();
    if ( activeStudy() )
      wid->setVisible( winMap.contains( it.key() ) );
    else
      delete wid;
  }

  if ( activeStudy() )
    loadDockWindowsState();
  else
    myWin.clear();
}

/*!
  Updates view managers
*/
void LightApp_Application::updateViewManagers()
{
  QStringList lst;
  currentViewManagers( lst );

  for ( QStringList::const_iterator it = lst.begin(); it != lst.end(); ++it )
    getViewManager( *it, true );
}

/*!
  Loads windows geometry
*/
void LightApp_Application::loadDockWindowsState()
{
  if ( !desktop() )
    return;

  bool store = resourceMgr()->booleanValue( "Study", "store_positions", true );
  if( !store )
    return;

  QString modName;
  if ( activeModule() )
    modName = activeModule()->name();

  if ( myWinGeom.contains( modName ) )
    desktop()->restoreState( myWinGeom[modName] );

  if ( !myWinVis.contains( modName ) )
    return;

  QMap<QString, bool> tbMap, dwMap;
  dockWindowsState( myWinVis[modName], tbMap, dwMap );

  QList<QToolBar*> tbList = qFindChildren<QToolBar*>( desktop() );
  for ( QList<QToolBar*>::iterator tit = tbList.begin(); tit != tbList.end(); ++tit )
  {
    QToolBar* tb = *tit;

    QObject* po = Qtx::findParent( tb, "QMainWindow" );
    if ( po != desktop() )
      continue;

    if ( tbMap.contains( tb->objectName() ) )
      tb->setVisible( tbMap[tb->objectName()] );
  }

  QList<QDockWidget*> dwList = qFindChildren<QDockWidget*>( desktop() );
  for ( QList<QDockWidget*>::iterator dit = dwList.begin(); dit != dwList.end(); ++dit )
  {
    QDockWidget* dw = *dit;

    QObject* po = Qtx::findParent( dw, "QMainWindow" );
    if ( po != desktop() )
      continue;

    if ( dwMap.contains( dw->objectName() ) )
      dw->setVisible( dwMap[dw->objectName()] );
  }
}

/*!
  Saves windows geometry
*/
void LightApp_Application::saveDockWindowsState()
{
  if ( !desktop() )
    return;

  bool store = resourceMgr()->booleanValue( "Study", "store_positions", true );
  if( !store )
    return;

  QString modName;
  if ( activeModule() )
    modName = activeModule()->name();

  myWinGeom.insert( modName, desktop()->saveState() );

  QByteArray visArr;
  if ( myWinVis.contains( modName ) )
    visArr = myWinVis[modName];

  QMap<QString, bool> tbMap, dwMap;
  dockWindowsState( visArr, tbMap, dwMap );

  QList<QToolBar*> tbList = qFindChildren<QToolBar*>( desktop() );
  for ( QList<QToolBar*>::iterator it = tbList.begin(); it != tbList.end(); ++it )
  {
    QToolBar* tb = *it;
    tbMap.insert( tb->objectName(), tb->toggleViewAction()->isChecked() );
  }

  QList<QDockWidget*> dwList = qFindChildren<QDockWidget*>( desktop() );
  for ( QList<QDockWidget*>::iterator it = dwList.begin(); it != dwList.end(); ++it )
  {
    QDockWidget* wid = *it;
    dwMap.insert( wid->objectName(), wid->toggleViewAction()->isChecked() );
  }

  visArr = dockWindowsState( tbMap, dwMap );

  myWinVis.insert( modName, visArr );
}

QByteArray LightApp_Application::dockWindowsState( const QMap<QString, bool>& tb, const QMap<QString, bool>& dw ) const
{
  QByteArray visArr;
  QDataStream stream( &visArr, QIODevice::WriteOnly );

  stream << (uchar)ToolBarMarker;
  stream << tb.size();
  for ( QMap<QString, bool>::const_iterator tit = tb.begin(); tit != tb.end(); ++tit )
  {
    stream << tit.key();
    stream << (uchar)( tit.value() ? 1 : 0 );
  }

  stream << (uchar)DockWidgetMarker;
  stream << dw.size();
  for ( QMap<QString, bool>::const_iterator wit = dw.begin(); wit != dw.end(); ++wit )
  {
    stream << wit.key();
    stream << (uchar)( wit.value() ? 1 : 0 );
  }

  return visArr;
}

void LightApp_Application::dockWindowsState( const QByteArray& arr, QMap<QString, bool>& tb, QMap<QString, bool>& dw ) const
{
  tb.clear();
  dw.clear();

  QByteArray visArr = arr;
  QDataStream stream( &visArr, QIODevice::ReadOnly );

  uchar marker;
  stream >> marker;
  if ( marker != ToolBarMarker )
    return;

  int lines;
  stream >> lines;
  for ( int i = 0; i < lines; ++i )
  {
    QString objectName;
    stream >> objectName;
    uchar shown;
    stream >> shown;
    tb.insert( objectName, shown );
  }

  stream >> marker;
  if ( marker != DockWidgetMarker )
    return;

  stream >> lines;
  for ( int j = 0; j < lines; ++j )
  {
    QString objectName;
    stream >> objectName;
    uchar shown;
    stream >> shown;
    dw.insert( objectName, shown );
  }
}

/*!
  Adds icon names for modules
*/
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
    QString modIcon = resMgr->stringValue( modIntr, "icon", QString() );

    if ( modIcon.isEmpty() )
      continue;

    if ( SUIT_Tools::extension( modIcon ).isEmpty() )
      modIcon += QString( ".png" );

    iconMap.insert( modName, modIcon );
  }
}

/*!
  Inserts items in popup, which necessary for current application
*/
void LightApp_Application::contextMenuPopup( const QString& type, QMenu* thePopup, QString& title )
{
  //Add "Rename" item
  LightApp_SelectionMgr* selMgr = LightApp_Application::selectionMgr();
  bool cacheIsOn = selMgr->isSelectionCacheEnabled();
  selMgr->setSelectionCacheEnabled( true );

  SUIT_DataBrowser* ob = objectBrowser();

  CAM_Application::contextMenuPopup( type, thePopup, title );

  if ( ob && type == ob->popupClientType() ) {
    thePopup->addSeparator();
    QAction* a = thePopup->addAction( tr( "MEN_REFRESH" ), this, SLOT( onRefresh() ) );
    if ( ob->shortcutKey(SUIT_DataBrowser::UpdateShortcut) )
      a->setShortcut( ob->shortcutKey(SUIT_DataBrowser::UpdateShortcut) );
  }

  if ( selMgr && ob ) {
    SALOME_ListIO selected;
    selMgr->selectedObjects( selected );
    if(selected.Extent() == 1){
      Handle(SALOME_InteractiveObject) anIObject = selected.First();
      SUIT_DataObject* obj = findObject(anIObject->getEntry());
      if(obj && obj->renameAllowed()) {
        QAction* a = new QAction(tr("MEN_RENAME_OBJ"), thePopup);
        connect( a, SIGNAL( triggered(bool) ), ob, SLOT( onStartEditing() ) );
        if ( ob->shortcutKey(SUIT_DataBrowser::RenameShortcut) )
          a->setShortcut( ob->shortcutKey(SUIT_DataBrowser::RenameShortcut) );

        QList<QAction*> acts = thePopup->actions();
        QAction* firstAction = acts.count() > 0 ? acts.first() : 0;
        thePopup->insertAction(firstAction,a);
      }
    }
  }

  selMgr->setSelectionCacheEnabled( cacheIsOn );
}

/*!
  Create empty study
*/
void LightApp_Application::createEmptyStudy()
{
  CAM_Application::createEmptyStudy();

  if ( objectBrowser() )
    objectBrowser()->updateTree();

  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  if ( aResMgr && activeStudy() ) {
    int autoSaveInterval = aResMgr->integerValue( "Study", "auto_save_interval", 0 );
    if ( autoSaveInterval > 0 ) myAutoSaveTimer->start( autoSaveInterval*60000 );
  }
}

/*!Set desktop:*/
void LightApp_Application::setDesktop( SUIT_Desktop* desk )
{
  CAM_Application::setDesktop( desk );

  if ( desk ) {
    connect( desk, SIGNAL( message( const QString& ) ),
             this, SLOT( onDesktopMessage( const QString& ) ), Qt::UniqueConnection );
  }
}

/*!
  Activates module
  \param mod - module to be activated
*/
bool LightApp_Application::activateModule( CAM_Module* mod )
{
  bool res = CAM_Application::activateModule( mod );

  if ( objectBrowser() )
    objectBrowser()->updateTree();

  return res;
}

/*!
  \return keyborad accelerators manager object
*/
SUIT_Accel* LightApp_Application::accel() const
{
  return myAccel;
}

/*!
  Removes dead widget container from map
*/
void LightApp_Application::onWCDestroyed( QObject* ob )
{
  // remove destroyed widget container from windows map
  for ( WinMap::ConstIterator itr = myWin.begin(); itr != myWin.end(); ++itr )
  {
    if ( itr.value() != ob )
      continue;

    int key = itr.key();
    myWin.remove( key );
    break;
  }
}

void LightApp_Application::onMRUActivated( const QString& name )
{
  SUIT_Session* s = SUIT_Session::session();
  if ( s && s->activeApplication() == this )
    onOpenDoc( name );
}

void LightApp_Application::onStylePreferences()
{
  Style_PrefDlg dlg( desktop() );
  dlg.exec();

  resourceMgr()->setValue( "Style", "use_salome_style", Style_Salome::isActive() );
}

void LightApp_Application::onFullScreen(){
  if(myScreenHelper) {
    if(desktop()->isFullScreen())
      myScreenHelper->switchToNormalScreen();
    else
      myScreenHelper->switchToFullScreen();
  }
}

/*!
  Connects just added view manager
*/
void LightApp_Application::addViewManager( SUIT_ViewManager* vm )
{
  connect( vm, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  STD_Application::addViewManager( vm );
}

/*!
  Remove view manager from memory
*/
void LightApp_Application::removeViewManager( SUIT_ViewManager* vm )
{
  disconnect( vm, SIGNAL( lastViewClosed( SUIT_ViewManager* ) ),
           this, SLOT( onCloseView( SUIT_ViewManager* ) ) );
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(activeStudy());
  if (aStudy )
    aStudy->removeViewMgr(vm->getGlobalId());

  STD_Application::removeViewManager( vm );

  // IPAL22894: Crash on closing OCC view
  //delete vm;
  vm->deleteLater();
}

/*!
  Renames active window of desktop
*/
void LightApp_Application::onRenameWindow()
{
  if( !desktop() )
    return;

  QWidget* w = desktop()->activeWindow();
  if( !w )
    return;

  bool ok;
  QString name = QInputDialog::getText( w, tr( "TOT_RENAME" ), tr( "PRP_RENAME" ), QLineEdit::Normal, w->windowTitle(), &ok );
  if( ok && !name.isEmpty() )
    w->setWindowTitle( name );
}

/*!
  Closes active window of desktop
*/
void LightApp_Application::onCloseWindow()
{
  if( !desktop() )
    return;

  QWidget* w = desktop()->activeWindow();
  if( !w )
    return;

  w->close();
}

/*!
  Closes all windows of desktop
*/
void LightApp_Application::onCloseAllWindow()
{
  STD_TabDesktop* desk = dynamic_cast<STD_TabDesktop*>( desktop() );
  if( !desk )
    return;

  QList<SUIT_ViewWindow*> wndList = desk->windows();
  SUIT_ViewWindow* wnd;
  foreach( wnd, wndList )
  {
    if ( wnd )
      wnd->close();
  }
}

/*!
  Groups all windows of desktop
*/
void LightApp_Application::onGroupAllWindow()
{
  STD_TabDesktop* desk = dynamic_cast<STD_TabDesktop*>( desktop() );
  if( !desk )
    return;

  QtxWorkstack* wgStack = desk->workstack();
  if ( wgStack )
    wgStack->stack();
}

/*!
  \return if the library of module exists
  \param moduleTitle - title of module
*/
bool LightApp_Application::isLibExists( const QString& moduleTitle ) const
{
  if( moduleTitle.isEmpty() )
    return false;

  QString lib = moduleLibrary( moduleTitle );

  //abd: changed libSalomePyQtGUI to SalomePyQtGUI for WIN32
  bool isPythonModule = lib.contains("SalomePyQtGUI");
  bool isPythonLightModule = lib.contains("libSalomePyQtGUILight");

  QStringList paths;
#ifdef WIN32
  paths = QString(::getenv( "PATH" )).split( ";", QString::SkipEmptyParts );
#else
  paths = QString(::getenv( "LD_LIBRARY_PATH" )).split( ":", QString::SkipEmptyParts );
#endif

  bool isLibFound = false;
  QStringList::const_iterator anIt = paths.begin(), aLast = paths.end();
  for( ; anIt!=aLast; anIt++ )
  {
    QFileInfo inf( Qtx::addSlash( *anIt ) + lib );

    if( inf.exists() )
      {
        isLibFound = true;
        break;
      }
  }

  if ( !isLibFound )
    {
      INFOS( "****************************************************************" << std::endl
          << "*    Warning: library " << lib.toLatin1().constData() << " cannot be found" << std::endl
          << "*    Module " << moduleTitle.toLatin1().constData() << " will not be available in GUI mode" << std::endl
          << "****************************************************************" << std::endl );
    }
  else if ( !isPythonModule && !isPythonLightModule)
    return true;

  if ( isPythonModule || isPythonLightModule)
    {
      QString pylib = moduleName( moduleTitle ) + QString(".py");
      QString pylibgui = moduleName( moduleTitle ) + QString("GUI.py");

      // Check the python library
#ifdef WIN32
      paths = QString(::getenv( "PATH" )).split( ";", QString::SkipEmptyParts );
#else
      paths = QString(::getenv( "PYTHONPATH" )).split( ":", QString::SkipEmptyParts );
#endif
      bool isPyLib = false, isPyGuiLib = false;
      QStringList::const_iterator anIt = paths.begin(), aLast = paths.end();
      for( ; anIt!=aLast; anIt++ )
        {
          QFileInfo inf( Qtx::addSlash( *anIt ) + pylib );
          QFileInfo infgui( Qtx::addSlash( *anIt ) + pylibgui );

          if(!isPythonLightModule)
            if( !isPyLib && inf.exists() )
              isPyLib = true;

          if( !isPyGuiLib && infgui.exists() )
            isPyGuiLib = true;

          if ((isPyLib || isPythonLightModule ) && isPyGuiLib && isLibFound)
            return true;
        }

      printf( "****************************************************************\n" );
      printf( "*    Warning: python library for %s cannot be found:\n", moduleTitle.toLatin1().constData() );
      if (!isPyLib)
        printf( "*    No module named %s\n", moduleName( moduleTitle ).toLatin1().constData() );
      if (!isPyGuiLib)
        printf( "*    No module named %s\n", (moduleName( moduleTitle ) + QString("GUI")).toLatin1().constData() );
      printf( "****************************************************************\n" );
      return true;
  }
  return false;
}

/*!
  \return default name for an active study
*/
void LightApp_Application::setDefaultStudyName( const QString& theName )
{
  QStringList anInfoList;
  modules( anInfoList, false );

  LightApp_Study* aStudy = (LightApp_Study*)activeStudy();
  if( anInfoList.count() == 1 && // to avoid a conflict between different modules
      !aStudy->isSaved() )
  {
    aStudy->setStudyName( theName );
    updateDesktopTitle();
  }
}

/*!
  Custom event handler
*/
bool LightApp_Application::event( QEvent* e )
{
  if( e && e->type()==2000 )
  {
    SALOME_CustomEvent* ce = ( SALOME_CustomEvent* )e;
    QString* d = ( QString* )ce->data();
    if( SUIT_MessageBox::question(0, tr("WRN_WARNING"),
                                  d ? *d : "",
                                  SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                  SUIT_MessageBox::Yes ) == SUIT_MessageBox::Yes )
      showPreferences( tr( "PREF_APP" ) );
    if( d )
      delete d;
    return true;
  }
  return CAM_Application::event( e );
}

/*! Check data object */
bool LightApp_Application::checkDataObject(LightApp_DataObject* theObj)
{
  if (theObj)
    {
      bool isSuitable = !theObj->entry().isEmpty() &&
                        !theObj->componentDataType().isEmpty() &&
                        !theObj->name().isEmpty();
      return isSuitable;
    }

  return false;
}

int LightApp_Application::openChoice( const QString& aName )
{
  int choice = CAM_Application::openChoice( aName );

  if ( choice == OpenExist ) // The document is already open.
  {
    // Do you want to reload it?
    if ( SUIT_MessageBox::question( desktop(), tr( "WRN_WARNING" ), tr( "QUE_DOC_ALREADYOPEN" ).arg( aName ),
                                    SUIT_MessageBox::Yes | SUIT_MessageBox::No, SUIT_MessageBox::No ) == SUIT_MessageBox::Yes )
      choice = OpenReload;
  }

  return choice;
}

bool LightApp_Application::openAction( const int choice, const QString& aName )
{
  bool res = false;
  switch ( choice )
  {
  case OpenReload:
    {
      STD_Application* app = 0;
      SUIT_Session* session = SUIT_Session::session();
      QList<SUIT_Application*> appList = session->applications();
      for ( QList<SUIT_Application*>::iterator it = appList.begin(); it != appList.end() && !app; ++it )
      {
        if ( (*it)->activeStudy() && (*it)->activeStudy()->studyName() == aName )
          app = ::qobject_cast<STD_Application*>( *it );
      }

      if ( app )
      {
        app->onCloseDoc( false );
        appList = session->applications();
        STD_Application* other = 0;
        for ( QList<SUIT_Application*>::iterator it = appList.begin(); it != appList.end() && !other; ++it )
          other = ::qobject_cast<STD_Application*>( *it );

        if ( other )
          res = other->onOpenDoc( aName );
      }
    }
    break;
  default:
    res = CAM_Application::openAction( choice, aName );
    break;
  }

  return res;
}

QStringList LightApp_Application::viewManagersTypes() const
{
  QStringList aTypesList;
  aTypesList += myUserWmTypes;
#ifndef DISABLE_GLVIEWER
  aTypesList<<GLViewer_Viewer::Type();
#endif
#ifndef DISABLE_PLOT2DVIEWER
  aTypesList<<Plot2d_Viewer::Type();
#endif
#ifndef DISABLE_QXGRAPHVIEWER
  aTypesList<<QxScene_Viewer::Type();
#endif
#ifndef DISABLE_OCCVIEWER
  aTypesList<<OCCViewer_Viewer::Type();
#endif
#ifndef DISABLE_VTKVIEWER
 #ifndef DISABLE_SALOMEOBJECT
  aTypesList<<SVTK_Viewer::Type();
 #else
  aTypesList<<VTKViewer_Viewer::Type();
 #endif
#endif
  return aTypesList;
}
/*!
 * Removes all view managers of known types
 * Other view managers are ignored
 */
void LightApp_Application::clearKnownViewManagers()
{
  QStringList aTypesList = viewManagersTypes();
  QList<SUIT_ViewManager*> aMgrList;
  viewManagers( aMgrList );
  foreach (SUIT_ViewManager* aMgr, aMgrList) {
    if (aTypesList.contains(aMgr->getType()))
      removeViewManager(aMgr);
  }
}

/*!
  Copy of current selection
 */
void LightApp_Application::onCopy()
{
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( activeModule() );
  if( m )
    m->copy();
}

/*!
  Paste of current data in clipboard
 */
void LightApp_Application::onPaste()
{
  LightApp_Module* m = dynamic_cast<LightApp_Module*>( activeModule() );
  if( m )
    m->paste();
}

/*!
  Browse (i.e. set focus on) the published objects
  \param theIsApplyAndClose - flag indicating that the dialog for creating objects
                              has been accepted by Ok (or Apply & Close) button
  \param theIsOptimizedBrowsing - flag switching to optimized browsing mode
                                  (to select the first published object only)
  \return entry of the selected object
 */
QString LightApp_Application::browseObjects( const QStringList& theEntryList,
                                             const bool theIsApplyAndClose,
                                             const bool theIsOptimizedBrowsing )
{
  QString aResult;
  if( SUIT_ResourceMgr* aResourceMgr = resourceMgr() )
  {
    int aBrowsePolicy = aResourceMgr->integerValue( "ObjectBrowser", "browse_published_object", (int)BP_Never );
    switch( aBrowsePolicy )
    {
      case BP_Never:
        return aResult;
      case BP_ApplyAndClose:
        if( !theIsApplyAndClose )
          return aResult;
      case BP_Always:
      default:
        break;
    }
  }

  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( activeStudy() );
  if( !aStudy )
    return aResult;

  SUIT_DataBrowser* anOB = objectBrowser();
  if( !anOB )
    return aResult;

  SUIT_AbstractModel* aModel = dynamic_cast<SUIT_AbstractModel*>( anOB->model() );
  if( !aModel )
    return aResult;

  QStringListIterator anIter( theEntryList );
  if( theIsOptimizedBrowsing )
  {
    // go to the last entry
    anIter.toBack();
    if( anIter.hasPrevious() )
      anIter.previous();
  }

  // scroll to each entry in the list
  // (in optimized mode - to the last entry only)
  QString anEntry;
  LightApp_DataObject* anObject;
  while( anIter.hasNext() )
  {
    anEntry = anIter.next();
    if( !anEntry.isEmpty() )
    {
      anObject = aStudy->findObjectByEntry( anEntry );
      if( anObject )
      {
        QModelIndex anIndex = aModel->index( anObject );
        anOB->treeView()->scrollTo( anIndex );
      }
    }
  }

  // always select the last object
  if( anObject && !anEntry.isEmpty() )
  {
    QList<SUIT_Selector*> aSelectorList;
    selectionMgr()->selectors( "ObjectBrowser", aSelectorList );
    if( !aSelectorList.isEmpty() )
    {
      if( LightApp_OBSelector* aSelector = dynamic_cast<LightApp_OBSelector*>( aSelectorList.first() ) )
      {
        bool anIsAutoBlock = aSelector->autoBlock();

        // temporarily disable auto block, to emit LightApp_SelectionMgr::currentSelectionChanged() signal
        aSelector->setAutoBlock( false );

        SUIT_DataOwnerPtrList aList;
#ifndef DISABLE_SALOMEOBJECT
        Handle(SALOME_InteractiveObject) aSObj = new SALOME_InteractiveObject
          ( anObject->entry().toLatin1().constData(),
            anObject->componentDataType().toLatin1().constData(),
            anObject->name().toLatin1().constData() );
        LightApp_DataOwner* owner = new LightApp_DataOwner( aSObj  );
#else
        LightApp_DataOwner* owner = new LightApp_DataOwner( anEntry );
#endif

        aList.append( owner );
        selectionMgr()->setSelected( aList );
        aResult = anEntry;

        // restore auto block flag
        aSelector->setAutoBlock( anIsAutoBlock );
      }
    }
  }

  return aResult;
}

SUIT_DataObject* LightApp_Application::findObject( const QString& id ) const
{
  LightApp_Study* study = dynamic_cast<LightApp_Study*>( activeStudy() );
  return study ? study->findObjectByEntry( id ) : 0;
}

/*!
  Checks that an object can be renamed.
  \param entry entry of the object
  \brief Return \c true if object can be renamed
*/
bool LightApp_Application::renameAllowed( const QString& /*entry*/) const {
  return false;
}

/*!
  Rename object by entry.
  \param entry entry of the object
  \param name new name of the object
  \brief Return \c true if rename operation finished successfully, \c false otherwise.
*/
bool LightApp_Application::renameObject( const QString& entry, const QString& ) {
  return false;
}

/*! Process standard messages from desktop */
void LightApp_Application::onDesktopMessage( const QString& message )
{
  const QString sectionSeparator = "/";

  if ( message.toLower() == "updateobjectbrowser" ||
       message.toLower() == "updateobjbrowser" ) {
    // update object browser
    updateObjectBrowser();
  }
  else {
    QStringList data = message.split( sectionSeparator );
    if ( data.count() > 1 ) {
      QString msgType = data[0].trimmed();
      LightApp_Module* sMod = 0;
      CAM_Module* mod = module( msgType );
      if ( !mod )
	mod = module( moduleTitle( msgType ) );
      if ( mod && mod->inherits( "LightApp_Module" ) )
	sMod = (LightApp_Module*)mod;

      if ( msgType.toLower() == "preferences" ) {
	// requested preferences change: should be given as "preferences/<section>/<name>/<value>"
	// for example "preferences/Study/multi_file_dump/true"
	if ( data.count() > 3 ) {
	  QString section = data[1].trimmed();
	  QString param   = data[2].trimmed();
	  QString value   = QStringList( data.mid(3) ).join( sectionSeparator );
	  resourceMgr()->setValue( section, param, value );
	}
      }
      else if ( sMod ) {
	// received message for the module
	QString msg = QStringList( data.mid(1) ).join( sectionSeparator );
	sMod->message( msg );
      }
    }
  }
}
