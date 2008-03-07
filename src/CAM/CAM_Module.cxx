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
#include "CAM_Module.h"

#include "CAM_DataModel.h"
#include "CAM_Application.h"
#include "CAM_Study.h"

#include <QtxAction.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <SUIT_Session.h>
#include <SUIT_Application.h>

/*!Icon.*/
static const char* ModuleIcon[] = {
"20 20 2 1",
" 	c None",
".	c #000000",
"                    ",
"                    ",
"                    ",
" .................. ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .                . ",
" .................. ",
"    .     .     .   ",
"    .     .     .   ",
"   ...   ...   ...  ",
"  .. .. .. .. .. .. ",
"  .   . .   . .   . ",
"  .. .. .. .. .. .. ",
"   ...   ...   ...  "};

QPixmap MYPixmap( ModuleIcon );

/*!Constructor.*/
CAM_Module::CAM_Module()
: QObject(),
myApp( 0 ),
myIcon( MYPixmap ),
myDataModel( 0 )
{
}

/*!Constructor. initialize \a name.*/
CAM_Module::CAM_Module( const QString& name )
: QObject(),
myApp( 0 ),
myName( name ),
myIcon( MYPixmap ),
myDataModel( 0 )
{
}

/*!Destructor. Remove data model.*/
CAM_Module::~CAM_Module()
{
  delete myDataModel;
  myDataModel = 0;
}

/*!Initialize application.*/
void CAM_Module::initialize( CAM_Application* app )
{
  myApp = app;
  if ( myApp )
  {
    SUIT_Session* aSession = SUIT_Session::session();
    connect( aSession, SIGNAL( applicationClosed( SUIT_Application* ) ),
             this, SLOT( onApplicationClosed( SUIT_Application* ) ) );

    connect( myApp, SIGNAL( infoChanged( QString ) ), this, SLOT( onInfoChanged( QString ) ) );
  }
}

/*!\retval Module icon.*/
QPixmap CAM_Module::moduleIcon() const
{
  return myIcon;
}

/*!\retval Module icon name.*/
QString CAM_Module::iconName() const
{
  return "";
}

/*!\retval Module name.*/
QString CAM_Module::moduleName() const
{
  return myName;
}

/*! \brief Return data model.
 * Create data model, if it was't created before.
 */
CAM_DataModel* CAM_Module::dataModel() const
{
  if ( !myDataModel )
  {
    CAM_Module* that = (CAM_Module*)this;
    that->myDataModel = that->createDataModel();
    that->myDataModel->initialize();
  }
  return myDataModel;
}

/*!\retval CAM_Application pointer - application.*/
CAM_Application* CAM_Module::application() const
{
  return myApp;
}

/*!Public slot
 * \retval true.
 */
bool CAM_Module::activateModule( SUIT_Study* study )
{
  return true;
}

/*!Public slot
 * \retval true.
 */
bool CAM_Module::deactivateModule( SUIT_Study* )
{
  return true;
}

/*!Public slot, remove data model from \a study.*/
void CAM_Module::studyClosed( SUIT_Study* study )
{
  CAM_Study* camDoc = dynamic_cast<CAM_Study*>( study );
  if ( !camDoc ) 
    return;

  CAM_DataModel* dm = dataModel();
  if ( dm && camDoc->containsDataModel( dm ) ) {
    dm->close();
    camDoc->removeDataModel( dm );
  }
}

/*!Public slot, do nothing.*/
void CAM_Module::studyChanged( SUIT_Study* , SUIT_Study* )
{
}

/*!Return true if module is active.*/
bool CAM_Module::isActiveModule() const
{
  return application() ? application()->activeModule() == this : false;
}

/*!
  Put the message into the status bar of the desktop. Message will be displayed
  during specified \amscec milliseconds. If parameter \amsec is negative then
  message will be persistently displayed when module is active.
*/
void CAM_Module::putInfo( const QString& msg, const int msec )
{
  if ( application() )
    application()->putInfo( msg, msec );

  if ( msec < 0 )
    myInfo = msg;
}

/*!
  Restore persistently displayed info string when previos information status string erasing
  if module is active.
*/
void CAM_Module::onInfoChanged( QString txt )
{
  if ( txt.isEmpty() && isActiveModule() && !myInfo.isEmpty() && application() )
    application()->putInfo( myInfo );
}



/*!Public slot, nullify application pointer if the application was closed.*/
void CAM_Module::onApplicationClosed( SUIT_Application* theApp )
{
  if (myApp == theApp)
    myApp = NULL;
}

/*!Create and return new instance of CAM_DataModel.*/
CAM_DataModel* CAM_Module::createDataModel()
{ 
  return new CAM_DataModel( this );
}

/*!Sets module name to \a name.
 * \param name - new name for module.
 */
void CAM_Module::setModuleName( const QString& name )
{
  myName = name;
}

/*!Sets module icon to \a icon.
 * \param icon - new icon for module.
 */
void CAM_Module::setModuleIcon( const QPixmap& icon )
{
  myIcon = icon;
}

/*! Return menu manager pointer.
 * \retval QtxActionMenuMgr pointer - menu manager.
 */
QtxActionMenuMgr* CAM_Module::menuMgr() const
{
  QtxActionMenuMgr* mgr = 0;
  if ( application() && application()->desktop() )
    mgr = application()->desktop()->menuMgr();
  return mgr;
}

/*! Return tool manager pointer.
 * \retval QtxActionToolMgr pointer - tool manager.
 */
QtxActionToolMgr* CAM_Module::toolMgr() const
{
  QtxActionToolMgr* mgr = 0;
  if ( application() && application()->desktop() )
    mgr = application()->desktop()->toolMgr();
  return mgr;
}

/*! Create tool bar with name \a name, if it was't created before.
 * \retval -1 - if tool manager was't be created.
 */
int CAM_Module::createTool( const QString& name )
{
  if ( !toolMgr() )
    return -1;

  return toolMgr()->createToolBar( name );
}

/*! Create tool. Register action \a a with id \a id.
 * Insert QAction to tool manager.
 *\param a - QAction
 *\param tBar - integer
 *\param id   - integer
 *\param idx  - integer
 *\retval integer id of new action in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = toolMgr()->insert( a, tBar, idx );
  return intId != -1 ? regId : -1;
}

/*! Create tool. Register action \a a with id \a id.
 * Insert QAction to tool manager.
 *\param a - QAction
 *\param tBar - QString&
 *\param id   - integer
 *\param idx  - integer
 *\retval integer id of new action in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = toolMgr()->insert( a, tBar, idx );
  return intId != -1 ? regId : -1;
}

/*! Create tool.
 * Insert QAction with id \a id from action map(myActionMap) to tool manager.
 *\param id   - integer
 *\param tBar - integer
 *\param idx  - integer
 *\retval integer id of new action in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createTool( const int id, const int tBar, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int intId = toolMgr()->insert( action( id ), tBar, idx );
  return intId != -1 ? id : -1;
}

/*! Create tool.
 * Insert QAction with id \a id from action map(myActionMap) to tool manager.
 *\param id   - integer
 *\param tBar - QString&
 *\param idx  - integer
 *\retval integer id of new action in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createTool( const int id, const QString& tBar, const int idx )
{
  if ( !toolMgr() )
    return -1;

  int intId = toolMgr()->insert( action( id ), tBar, idx );
  return intId != -1 ? id : -1;
}

/*! Create menu.
 * Insert submenu \a subMenu to menu manager.
 *\param subMenu - QString&
 *\param menu    - integer
 *\param id      - integer
 *\param group   - integer
 *\param index   - integer
 *\retval integer id of new menu in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createMenu( const QString& subMenu, const int menu,
                            const int id, const int group, const int index,
			    const bool enableEmpty )
{
  if ( !menuMgr() )
    return -1;

  return menuMgr()->insert( subMenu, menu, group, id, index, enableEmpty );
}

/*! Create menu.
 * Insert submenu \a subMenu to menu manager.
 *\param subMenu - QString&
 *\param menu    - QString&
 *\param id      - integer
 *\param group   - integer
 *\param index   - integer
 *\retval integer id of new menu in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createMenu( const QString& subMenu, const QString& menu,
                            const int id, const int group, const int index,
			    const bool enableEmpty )
{
  if ( !menuMgr() )
    return -1;

  return menuMgr()->insert( subMenu, menu, group, id, index, enableEmpty );
}


/*! Create menu. Register action \a a with id \a id.
 * Insert QAction to menu manager.
 *\param a       - Qaction
 *\param menu    - integer
 *\param id      - integer
 *\param group   - integer
 *\param index   - integer
 *\retval integer id of new menu in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createMenu( QAction* a, const int menu, const int id, const int group, const int index )
{
  if ( !a || !menuMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = menuMgr()->insert( a, menu, group, index );
  return intId != -1 ? regId : -1;
}

/*! Create menu. Register action \a a with id \a id.
 * Insert QAction to menu manager.
 *\param a       - Qaction
 *\param menu    - QString&
 *\param id      - integer
 *\param group   - integer
 *\param index   - integer
 *\retval integer id of new menu in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createMenu( QAction* a, const QString& menu, const int id, const int group, const int index )
{
  if ( !a || !menuMgr() )
    return -1;

  int regId = registerAction( id, a );
  int intId = menuMgr()->insert( a, menu, group, index );
  return intId != -1 ? regId : -1;
}

/*! Create menu.
 * Insert QAction with id \a id from action map(myActionMap) to menu manager.
 *\param menu    - integer
 *\param id      - integer
 *\param group   - integer
 *\param index   - integer
 *\retval integer id of new menu in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createMenu( const int id, const int menu, const int group, const int index )
{
  if ( !menuMgr() )
    return -1;

  int intId = menuMgr()->insert( action( id ), menu, group, index );
  return intId != -1 ? id : -1;
}

/*! Create menu.
 * Insert QAction with id \a id from action map(myActionMap) to menu manager.
 *\param menu    - QString&
 *\param id      - integer
 *\param group   - integer
 *\param index   - integer
 *\retval integer id of new menu in tool manager.
 *\retval Return -1 if something wrong.
 */
int CAM_Module::createMenu( const int id, const QString& menu, const int group, const int index )
{
  if ( !menuMgr() )
    return -1;

  int intId = menuMgr()->insert( action( id ), menu, group, index );
  return intId != -1 ? id : -1;
}

/*!Sets menus shown to \a on floag.
 *\param on - flag.
 */
void CAM_Module::setMenuShown( const bool on )
{
  QtxActionMenuMgr* mMgr = menuMgr();
  if ( !mMgr )
    return;

  bool upd = mMgr->isUpdatesEnabled();
  mMgr->setUpdatesEnabled( false );

  QAction* sep = separator();
  for ( QMap<int, QAction*>::Iterator it = myActionMap.begin(); it != myActionMap.end(); ++it )
  {
    if ( it.data() != sep )
      mMgr->setShown( mMgr->actionId( it.data() ), on );
  }

  mMgr->setUpdatesEnabled( upd );
  if ( upd )
    mMgr->update();
}

/*!Sets menu shown for QAction \a a to \a on flag.
 * \param a - QAction
 * \param on - flag
 */
void CAM_Module::setMenuShown( QAction* a, const bool on )
{
  if ( menuMgr() )
    menuMgr()->setShown( menuMgr()->actionId( a ), on );
}

/*!Sets menu shown for action with id=\a id to \a on flag.
 * \param id - id of action
 * \param on - flag
 */
void CAM_Module::setMenuShown( const int id, const bool on )
{
  setMenuShown( action( id ), on );
}

/*!Set tools shown to \a on flag.
 *\param on - boolean flag.
 */
void CAM_Module::setToolShown( const bool on )
{
  QtxActionToolMgr* tMgr = toolMgr();
  if ( !tMgr )
    return;

  bool upd = tMgr->isUpdatesEnabled();
  tMgr->setUpdatesEnabled( false );

  QAction* sep = separator();
  for ( QMap<int, QAction*>::Iterator it = myActionMap.begin(); it != myActionMap.end(); ++it )
  {
    if ( it.data() != sep )
      tMgr->setShown( tMgr->actionId( it.data() ), on );
  }

  tMgr->setUpdatesEnabled( upd );
  if ( upd )
    tMgr->update();
}

/*!Set tools shown for QAction \a a to \a on flag.
 * \param a - QAction
 * \param on - boolean flag
 */
void CAM_Module::setToolShown( QAction* a, const bool on )
{
  if ( toolMgr() )
    toolMgr()->setShown( toolMgr()->actionId( a ), on );
}

/*!Set tools shown for action with id=\a id to \a on flag.
 * \param id - integer action id
 * \param on - boolean flag
 */
void CAM_Module::setToolShown( const int id, const bool on )
{
  setToolShown( action( id ), on );
}

/*! Return action by id. 
 * \param id - id of action.
 * \retval QAction.
 */
QAction* CAM_Module::action( const int id ) const
{
  QAction* a = 0;
  if ( myActionMap.contains( id ) )
    a = myActionMap[id];
  return a;
}

/*! Return id by action. 
 * \param a - QAction.
 * \retval id of action.
 */
int CAM_Module::actionId( const QAction* a ) const
{
  int id = -1;
  for ( QMap<int, QAction*>::ConstIterator it = myActionMap.begin(); it != myActionMap.end() && id == -1; ++it )
  {
    if ( it.data() == a )
      id = it.key();
  }
  return id;
}

/*! Create new instance of QtxAction and register action with \a id.
 * \param id - id for new action.
 * \param text - parameter for creation QtxAction
 * \param icon - parameter for creation QtxAction
 * \param menu - parameter for creation QtxAction
 * \param tip  - tip status for QtxAction action.
 * \param key  - parameter for creation QtxAction
 * \param parent - parent for action
 * \param toggle - parameter for creation QtxAction
 * \param reciever - 
 * \param member   - 
 */
QAction* CAM_Module::createAction( const int id, const QString& text, const QIconSet& icon,
                                   const QString& menu, const QString& tip, const int key,
                                   QObject* parent, const bool toggle, QObject* reciever, const char* member )
{
  QtxAction* a = new QtxAction( text, icon, menu, key, parent, 0, toggle );
  a->setStatusTip( tip );

  if ( reciever && member )
    connect( a, SIGNAL( activated() ), reciever, member );

  registerAction( id, a );

  return a;
}

/*! Register action in action map.
 * \param id - id for action.
 * \param a  - action
 * \retval new id for action.
 */
int CAM_Module::registerAction( const int id, QAction* a )
{
  int ident = -1;
  for ( QMap<int, QAction*>::ConstIterator it = myActionMap.begin(); it != myActionMap.end() && ident == -1; ++it )
    if ( it.data() == a )
      ident = it.key();

  if ( ident != -1 )
    return ident;

  static int generatedId = -1;
  ident = id < 0 ? --generatedId : id;

  myActionMap.insert( ident, a );

  if ( menuMgr() )
    menuMgr()->registerAction( a );

  if ( toolMgr() )
    toolMgr()->registerAction( a );

  return ident;
}

/*! Unregister an action.
 * \param id - id for action.
 * \retval true if succeded, false if action is used
 */
bool CAM_Module::unregisterAction( const int id )
{
  return unregisterAction( action( id ) );
}

/*! Unregister an action.
 * \param a  - action
 * \retval true if succeded, false if action is used
 */
bool CAM_Module::unregisterAction( QAction* a )
{
  if ( !a )
    return false;
  if ( menuMgr() ) {
    int id = menuMgr()->actionId( a );
    if ( id != -1 && menuMgr()->containsMenu( id, -1 ) )
      return false;
  }
  if ( toolMgr() ) {
    int id = toolMgr()->actionId( a );
    if ( id != -1 && toolMgr()->containsAction( id ) )
      return false;
  }
  if ( menuMgr() )
    menuMgr()->unRegisterAction( menuMgr()->actionId( a ) );
  if ( toolMgr() )
    toolMgr()->unRegisterAction( toolMgr()->actionId( a ) );
  return true;
}

/*! Return qt action manager separator.*/
QAction* CAM_Module::separator()
{
  return QtxActionMgr::separator();
}

/*! Connect data model of module with active study */
void CAM_Module::connectToStudy( CAM_Study* camStudy )
{
  CAM_Application* app = camStudy ? dynamic_cast<CAM_Application*>( camStudy->application() ) : 0;
  if( !app )
    return;

  CAM_DataModel* prev = 0;
  for( CAM_Application::ModuleListIterator it = app->modules(); it.current(); ++it )
  {
    CAM_DataModel* dm = it.current()->dataModel();
    if( it.current() == this && !camStudy->containsDataModel( dm ) )
    {
      if ( prev )
	      camStudy->insertDataModel( it.current()->dataModel(), prev );
      else
	      camStudy->insertDataModel( it.current()->dataModel(), 0 );
    }
    prev = dm;
  }
}
