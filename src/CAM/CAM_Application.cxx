#include "CAM_Application.h"

#include "CAM_Study.h"
#include "CAM_Module.h"

#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>

#include <qfile.h> 
#include <qfileinfo.h>
#include <qtextstream.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

extern "C" CAM_EXPORT SUIT_Application* createApplication()
{
  return new CAM_Application();
}

CAM_Application::CAM_Application( const bool autoLoad )
: STD_Application(),
myModule( 0 ),
myAutoLoad( autoLoad )
{
  readModuleList();
}

CAM_Application::~CAM_Application()
{
}

void CAM_Application::start()
{
  if ( myAutoLoad )
    loadModules();

  STD_Application::start();
}

CAM_Module* CAM_Application::activeModule() const
{
  return myModule;
}

CAM_Module* CAM_Application::module(  const QString& modName ) const
{
  CAM_Module* mod = 0;
  for ( ModuleListIterator it( myModules ); it.current() && !mod; ++it )
    if ( it.current()->moduleName() == modName )
      mod = it.current();
  return mod;
}

CAM_Application::ModuleListIterator CAM_Application::modules() const
{
  return ModuleListIterator( myModules );
}

void CAM_Application::modules( CAM_Application::ModuleList& out ) const
{
  out.setAutoDelete( false );
  out.clear();

  for ( ModuleListIterator it( myModules ); it.current(); ++it )
    out.append( it.current() );
}

void CAM_Application::modules( QStringList& lst, const bool loaded ) const
{
  lst.clear();

  if ( loaded )
    for ( ModuleListIterator it( myModules ); it.current(); ++it )
      lst.append( it.current()->moduleName() );
  else
    for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end(); ++it )
      lst.append( (*it).title );
}

void CAM_Application::addModule( CAM_Module* mod )
{
  if ( !mod || myModules.contains( mod ) )
    return;

  mod->initialize( this );

  QMap<CAM_Module*, int> map;

  ModuleList newList;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end(); ++it )
  {
    if ( (*it).title == mod->moduleName() )
      newList.append( mod );
    else
    {
      CAM_Module* curMod = module( (*it).title );
      if ( curMod )
        newList.append( curMod );
    }
    if ( !newList.isEmpty() )
      map.insert( newList.getLast(), 0 );
  }

  for ( ModuleListIterator itr( myModules ); itr.current(); ++itr )
  {
    if ( !map.contains( itr.current() ) )
      newList.append( itr.current() );
  }

  if ( !map.contains( mod ) )
      newList.append( mod );

  myModules = newList;

  moduleAdded( mod );
}

void CAM_Application::loadModules()
{
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end(); ++it )
  {
    CAM_Module* mod = loadModule( (*it).title );
    if ( mod )
      addModule( mod );
    else
      SUIT_MessageBox::error1( desktop(), tr( "Loading modules" ),
                               tr( "Can not load module %1" ).arg( (*it).title ), tr( "Ok" ) );
  }
}

CAM_Module* CAM_Application::loadModule( const QString& modName )
{
  if ( myInfoList.isEmpty() )
  {
    qWarning( tr( "Modules configuration is not defined." ) );
    return 0;
  }

  QString libName = moduleLibrary( modName );
  if ( libName.isEmpty() )
  {
    qWarning( tr( "Information about module \"%1\" doesn't exist." ).arg( modName ) );
    return 0;
  }

  QString err;
  GET_MODULE_FUNC crtInst = 0;

#ifdef WIN32
  HINSTANCE modLib = ::LoadLibrary( libName ); 
  if ( !modLib )
  {
    LPVOID lpMsgBuf;
    ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                     FORMAT_MESSAGE_IGNORE_INSERTS, 0, ::GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, 0 );
    err = QString( "Failed to load  %1. %2" ).arg( libName ).arg( (LPTSTR)lpMsgBuf );
    ::LocalFree( lpMsgBuf );
  }
  else
  {
    crtInst = (GET_MODULE_FUNC)::GetProcAddress( modLib, GET_MODULE_NAME );
    if ( !crtInst )
    {
      LPVOID lpMsgBuf;
      ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS, 0, ::GetLastError(), 0, (LPTSTR)&lpMsgBuf, 0, 0 );
    err = QString( "Failed to find  %1 function. %2" ).arg( GET_MODULE_NAME ).arg( (LPTSTR)lpMsgBuf );
    ::LocalFree( lpMsgBuf );
    }
  }
#else
  void* modLib = dlopen( (char*)libName.latin1(), RTLD_LAZY );
  if ( !modLib )
    err = QString( "Can not load library %1. %2" ).arg( libName ).arg( dlerror() );
  else
  {
    crtInst = (GET_MODULE_FUNC)dlsym( modLib, GET_MODULE_NAME );
    if ( !crtInst )
      err = QString( "Failed to find function %1. %2" ).arg( GET_MODULE_NAME ).arg( dlerror() );
  }
#endif

  CAM_Module* module = crtInst ? crtInst() : 0;
  if ( module )
  {
    module->setModuleName( modName );
    module->setName( moduleName( modName ) );
  }

  if ( !err.isEmpty() )
    SUIT_MessageBox::warn1( desktop(), tr( "Error" ), err, tr( "Ok" ) );

  return module;
}

bool CAM_Application::activateModule( const QString& modName )
{
  if ( !modName.isEmpty() && !activeStudy() )
    return false;

  bool res = false;
  if ( !modName.isEmpty() )
  {
    CAM_Module* mod = module( modName );
    if ( !mod && !moduleLibrary( modName ).isEmpty() )
    {
      mod = loadModule( modName );
      addModule( mod );
    }

    if ( mod )
      res = activateModule( mod );
  }
  else
    res = activateModule( 0 );

  return res;
}

bool CAM_Application::activateModule( CAM_Module* mod )
{
  if ( mod && !activeStudy() )
    return false;

  if ( myModule == mod )
    return true;

  if ( myModule )
    myModule->deactivateModule( activeStudy() );

  myModule = mod;

  if ( myModule ){
    // Connect the module to the active study
    CAM_Study* camStudy = dynamic_cast<CAM_Study*>( activeStudy() );
    if ( camStudy ){
      CAM_DataModel* prev = 0;
      for ( ModuleListIterator it( myModules ); it.current(); ++it ) {
	CAM_DataModel* dm = it.current()->dataModel();
	if ( it.current() == myModule && !camStudy->containsDataModel( dm ) ){
	  if ( prev )
	    camStudy->insertDataModel( it.current()->dataModel(), prev );
	  else
	    camStudy->insertDataModel( it.current()->dataModel(), 0 );
	}
	prev = dm;
      }
    }
    myModule->activateModule( activeStudy() );
  }

  updateCommandsStatus();

  return true;
}

SUIT_Study* CAM_Application::createNewStudy() 
{ 
  return new CAM_Study( this );
}

void CAM_Application::updateCommandsStatus()
{
  STD_Application::updateCommandsStatus();

  if ( activeModule() )
    activeModule()->updateCommandsStatus();
}

void CAM_Application::beforeCloseDoc( SUIT_Study* theDoc )
{
  for ( ModuleListIterator it( myModules ); it.current(); ++it )
    it.current()->studyClosed( theDoc );
}

void CAM_Application::setActiveStudy( SUIT_Study* study )
{
  STD_Application::setActiveStudy( study );
}

void CAM_Application::moduleAdded( CAM_Module* mod )
{
//  CAM_Study* study = dynamic_cast<CAM_Study*>( activeStudy() );
//  if ( !study )
//    return;

//  study->insertDataModel( mod->dataModel() );
}

QString CAM_Application::moduleName( const QString& title ) const
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isEmpty(); ++it )
  {
    if ( (*it).title == title )
      res = (*it).name;
  }
  return res;
}

QString CAM_Application::moduleTitle( const QString& name ) const
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isEmpty(); ++it )
  {
    if ( (*it).name == name )
      res = (*it).title;
  }
  return res;
}

QString CAM_Application::moduleLibrary( const QString& title, const bool full ) const
{
  QString res;
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end() && res.isEmpty(); ++it )
  {
    if ( (*it).title == title )
      res = (*it).internal;
  }
  if ( !res.isEmpty() && full )
    res = SUIT_Tools::library( res );
  return res;
}

void CAM_Application::readModuleList()
{
  if ( !myInfoList.isEmpty() )
    return;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QString modStr = resMgr->stringValue( "launch", "modules", QString::null );
  QStringList modList = QStringList::split( ",", modStr );

  for ( QStringList::const_iterator it = modList.begin(); it != modList.end(); ++it )
  {
    QString modName = (*it).stripWhiteSpace();
    if ( modName.isEmpty() )
      continue;

    QString modTitle = resMgr->stringValue( *it, QString( "name" ), QString::null );
    if ( modTitle.isEmpty() )
      continue;

    QString modLibrary = resMgr->stringValue( *it, QString( "library" ), QString::null ).stripWhiteSpace();
    if ( !modLibrary.isEmpty() )
    {
      QString libExt;
      modLibrary = SUIT_Tools::file( modLibrary.stripWhiteSpace() );
      libExt = QString( "so" );
      if ( SUIT_Tools::extension( modLibrary ).lower() == libExt )
        modLibrary = modLibrary.mid( 0, modLibrary.length() - libExt.length() - 1 );
      libExt = QString( "dll" );
      if ( SUIT_Tools::extension( modLibrary ).lower() == libExt )
        modLibrary = modLibrary.mid( 0, modLibrary.length() - libExt.length() - 1 );
#ifndef WIN32
      if ( modLibrary.startsWith( "lib" ) )
        modLibrary = modLibrary.mid( 3 );
#endif
    }
    else
      modLibrary = modName;

    ModuleInfo inf;
    inf.name = modName;
    inf.title = modTitle;
    inf.internal = modLibrary;
    myInfoList.append( inf );
  }

  if ( myInfoList.isEmpty() )
    SUIT_MessageBox::error1( 0, tr( "Error" ), tr( "Can not load modules configuration file " ), tr( "Ok" ) );
}

void CAM_Application::contextMenuPopup( const QString& type, QPopupMenu* thePopup )
{
  // to do : add common items for popup menu ( if they are exist )
  if ( activeModule() ) 
    activeModule()->contextMenuPopup( type, thePopup );
}
