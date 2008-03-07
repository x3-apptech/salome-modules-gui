// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "CAM_Application.h"

#include "CAM_Study.h"
#include "CAM_Module.h"

#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>

#include <qfile.h> 
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qlabel.h>
#include <qfont.h>
#include <qapplication.h>
#include <qregexp.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

/*!Create new instance of CAM_Application*/
extern "C" CAM_EXPORT SUIT_Application* createApplication()
{
  return new CAM_Application();
}

/*!Constructor. read module list.
 * \param autoLoad - auto load flag.
 */
CAM_Application::CAM_Application( const bool autoLoad )
: STD_Application(),
myModule( 0 ),
myAutoLoad( autoLoad )
{
  readModuleList();
}

/*!Destructor. Do nothing.*/
CAM_Application::~CAM_Application()
{
}

/*! Load modules, if \a myAutoLoad flag is true.\n
 * Start application - call start() method from parent class.
 */
void CAM_Application::start()
{
  if ( myAutoLoad )
    loadModules();

  STD_Application::start();
}

/*!Get active module.
 * \retval CAM_Module - active module.
 */
CAM_Module* CAM_Application::activeModule() const
{
  return myModule;
}

/*!Get module with name \a modName from modules list.
 * \retval CAM_Module pointer - module.
 */
CAM_Module* CAM_Application::module(  const QString& modName ) const
{
  CAM_Module* mod = 0;
  for ( ModuleListIterator it( myModules ); it.current() && !mod; ++it )
    if ( it.current()->moduleName() == modName )
      mod = it.current();
  return mod;
}

/*!Gets modules iterator.*/
CAM_Application::ModuleListIterator CAM_Application::modules() const
{
  return ModuleListIterator( myModules );
}

/*!Gets modules list.
 * \param out - output list of modules.
 */
void CAM_Application::modules( CAM_Application::ModuleList& out ) const
{
  out.setAutoDelete( false );
  out.clear();

  for ( ModuleListIterator it( myModules ); it.current(); ++it )
    out.append( it.current() );
}

/*!Gets list of names for modules.\n
 * Get loaded modules names, if \a loaded is true, else \n
 * get names from information list.
 * \param lst - output list of names.
 * \param loaded - boolean flag.
 */
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

/*!Adding module \a mod to list.
 *\param mod - module.
 */
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

/*!Load modules from information list.
 * \warning If some of modules not loaded, error message appear on desktop.
 */
void CAM_Application::loadModules()
{
  for ( ModuleInfoList::const_iterator it = myInfoList.begin(); it != myInfoList.end(); ++it )
  {
    CAM_Module* mod = loadModule( (*it).title );
    if ( mod )
      addModule( mod );
    else {
      if ( desktop() && desktop()->isShown() )
	SUIT_MessageBox::error1( desktop(), tr( "Loading modules" ),
				 tr( "Can not load module %1" ).arg( (*it).title ), tr( "Ok" ) );
      else
	qWarning( tr( "Can not load module %1" ).arg( (*it).title ).latin1() ); 
    }
  }
}

/*!Load module with name \a modName.
 *\param modName - module name for loading.
 *\warning If information list is empty.
 *\warning If module library (for module with \a modName) is empty.
 *\warning If module library is not loaded.
 */
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

  if ( !err.isEmpty() ) {
    if ( desktop() && desktop()->isShown() )
      SUIT_MessageBox::warn1( desktop(), tr( "Error" ), err, tr( "Ok" ) );
    else
      qWarning( err.latin1() ); 
  }

  return module;
}

/*! @name Activate module group. */
//@{
/*!Activate module with name \a modName.
 *\param modName - module name.
 *\ratval true, if module loaded and activated successful, else false.
 */
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

/*!Activate module \a mod
 *\param mod - module for activation.
 *\retval true - if all sucessful.
 *\warning Error message if module not activated in active study.
 */
bool CAM_Application::activateModule( CAM_Module* mod )
{
  if ( mod && !activeStudy() )
    return false;

  if ( myModule == mod )
    return true;

  if ( myModule )
  {
    if ( !myModule->deactivateModule( activeStudy() ) )
    {
      // ....      
    }    
  }	
  myModule = mod;

  if ( myModule ){
    // Connect the module to the active study
    myModule->connectToStudy( dynamic_cast<CAM_Study*>( activeStudy() ) );
    if ( !myModule->activateModule( activeStudy() ) )
    {
      myModule->setMenuShown( false );
      myModule->setToolShown( false );
      if ( desktop() && desktop()->isShown() )
	SUIT_MessageBox::error1( desktop(), tr( "ERROR_TLT" ), tr( "ERROR_ACTIVATE_MODULE_MSG" ).arg( myModule->moduleName() ), tr( "BUT_OK" ) );
      else
	qWarning( tr( "ERROR_ACTIVATE_MODULE_MSG" ).arg( myModule->moduleName() ).latin1() ); 
      myModule = 0;
      return false;
    }
  }

  updateCommandsStatus();

  return true;
}
//@}

/*!Create new study for current application.
 *\retval study pointer.
 */
SUIT_Study* CAM_Application::createNewStudy() 
{ 
  return new CAM_Study( this );
}

/*!Update commands status for parent class and for current class(if module is active)*/
void CAM_Application::updateCommandsStatus()
{
  STD_Application::updateCommandsStatus();

  if ( activeModule() )
    activeModule()->updateCommandsStatus();
}

/*!Close all modules in study \a theDoc.
 *\param theDoc - study
 */
void CAM_Application::beforeCloseDoc( SUIT_Study* theDoc )
{
  for ( ModuleListIterator it( myModules ); it.current(); ++it )
    it.current()->studyClosed( theDoc );
}

/*!Sets active study for parent class.
 *\param study - study.
 */
void CAM_Application::setActiveStudy( SUIT_Study* study )
{
  STD_Application::setActiveStudy( study );
}

/*!Do nothing.*/
void CAM_Application::moduleAdded( CAM_Module* mod )
{
//  CAM_Study* study = dynamic_cast<CAM_Study*>( activeStudy() );
//  if ( !study )
//    return;

//  study->insertDataModel( mod->dataModel() );
}

/*!Gets module name by title \a title
 *\param title - title name
 *\retval QString module name.
 */
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

/*!Gets module title by module name \a name
 *\param name - module name
 *\retval QString module title.
 */
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

/*!Get library name for module with title \a title.
 *\param title - module title name.
 *\param full  - boolean flag (if true - return full library name, else internal name)
 *\retval QString - library name.
 */
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

/*!Read modules list*/
void CAM_Application::readModuleList()
{
  if ( !myInfoList.isEmpty() )
    return;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QStringList modList;

  QStringList args;
  for (int i = 1; i < qApp->argc(); i++)
    args.append( qApp->argv()[i] );

  QRegExp rx("--modules\\s+\\(\\s*(.*)\\s*\\)");
  rx.setMinimal( true );
  if ( rx.search( args.join(" ") ) >= 0 && rx.capturedTexts().count() > 0 ) {
    QString modules = rx.capturedTexts()[1];
    QStringList mods = QStringList::split(":",modules,false);
    for ( uint i = 0; i < mods.count(); i++ ) {
      if ( !mods[i].stripWhiteSpace().isEmpty() )
	modList.append( mods[i].stripWhiteSpace() );
    }
  }
  if ( modList.isEmpty() ) {
    QString mods = resMgr->stringValue( "launch", "modules", QString::null );
    modList = QStringList::split( ",", mods );
  }

  for ( QStringList::const_iterator it = modList.begin(); it != modList.end(); ++it )
  {
    QString modName = (*it).stripWhiteSpace();
    if ( modName.isEmpty() )
      continue;

    QString modTitle = resMgr->stringValue( *it, QString( "name" ), QString::null );
    if ( modTitle.isEmpty() )
      {
	printf( "****************************************************************\n" );
	printf( "*    Warning: %s not found in resources.\n", (*it).latin1() );
	printf( "*    Module will not be available\n" );
	printf( "****************************************************************\n" );
	continue;
      }

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

  if ( myInfoList.isEmpty() ) {
    if ( desktop() && desktop()->isShown() )
      SUIT_MessageBox::warn1( desktop(), tr( "Warning" ), tr( "Modules list is empty" ), tr( "&OK" ) );
    else
      {
	printf( "****************************************************************\n" );
	printf( "*    Warning: modules list is empty.\n" );
	printf( "****************************************************************\n" );
      }
  }
}

/*!Add common items for popup menu ( if they are exist )
 *\param type - type of popup menu
 *\param thePopup - popup menu
 *\param title - title of popup menu
 */
void CAM_Application::contextMenuPopup( const QString& type, QPopupMenu* thePopup, QString& title )
{
  // to do : add common items for popup menu ( if they are exist )
  if ( activeModule() ) 
    activeModule()->contextMenuPopup( type, thePopup, title );
}

/*!Create empty study.*/
void CAM_Application::createEmptyStudy()
{
  /*SUIT_Study* study = */activeStudy();
  STD_Application::createEmptyStudy();
}
