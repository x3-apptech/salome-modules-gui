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

#include "CAM_Module.h"

#include "CAM_DataModel.h"
#include "CAM_Application.h"
#include "CAM_Study.h"

#include <QtxAction.h>
#include <QtxActionGroup.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QMenu>

/*!
  \class CAM_Module
  \brief Base implementation of the module in the CAM application architecture.

  Provides support of menu/toolbars management.
*/

class ActionMgrLocker
{
public:
  ActionMgrLocker( QtxActionMgr* m, bool use ) : myMgr( m ), myUseLock( use )
  {
    if ( myUseLock ) {
      myUpdEnabled = myMgr->isUpdatesEnabled();
      myMgr->setUpdatesEnabled( false );
    }
  }
  ~ActionMgrLocker()
  {
    if ( myUseLock ) {
      myMgr->setUpdatesEnabled( myUpdEnabled );
      //myMgr->update();
    }
  }

  QtxActionMgr* myMgr;
  bool          myUseLock;
  bool          myUpdEnabled;
};

/*!
  \brief Default constructor.

  Creates unnamed module.
*/
CAM_Module::CAM_Module()
: QObject(),
  myApp( 0 ),
  myDataModel( 0 )
{
}

/*!
  \brief Constructor.

  Creates module with the specified \a name.

  \param name module name
*/
CAM_Module::CAM_Module( const QString& name )
: QObject(),
  myApp( 0 ),
  myName( name ),
  myDataModel( 0 ),
  myMenuShown( false ),
  myToolShown( false )
{
}

/*!
  \brief Destructor.

  Destroy data model.
*/
CAM_Module::~CAM_Module()
{
  delete myDataModel;
  myDataModel = 0;
}

/*!
  \brief Initialize module.

  This method is usually called when the module is created (for example,
  on the module library loading).
  Successor classes can use this method to create menu/toolbar actions
  and perform other module initialization.

  \param app parent application object
  \sa activateModule(), deactivateModule()
*/
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

/*!
  \brief Get module icon.
  \return module icon pixmap
  \sa iconName()
*/
QPixmap CAM_Module::moduleIcon() const
{
  if ( myIcon.isNull() ) {
    QString iname = iconName();
    if ( !iname.isEmpty() ) {
      CAM_Module* that = (CAM_Module*)this;
      that->myIcon = application()->resourceMgr()->loadPixmap( name(), iname, false );
    }
  }
  return myIcon;
}

/*!
  \brief Get module icon's name.

  This function is used to get module icon's file name.
  Default implementation returns empty string.

  \return module icon's name.
  \sa moduleIcon()
*/
QString CAM_Module::iconName() const
{
  return application()->moduleIcon( name() );
}

/*!
  \brief Get module (internal) name
  \return module name
  \sa setName(), moduleName(), setModuleName()
*/
QString CAM_Module::name() const
{
  return objectName();
}

/*!
  \brief Get module title (user name)
  \return module title
  \sa setModuleName(), name(), setName()
*/
QString CAM_Module::moduleName() const
{
  return myName;
}

/*!
  \brief Get data model.

  Creates data model, if it is not yet created.

  \return data model pointer
  \sa createDataModel()
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

/*!
  \brief Get application.
  \return application pointer
*/
CAM_Application* CAM_Module::application() const
{
  return myApp;
}

/*!
  \brief If return false, selection will be cleared at module activation
*/
bool CAM_Module::isSelectionCompatible()
{
  return false;
}

/*!
  \brief Activate module.

  This method is called when the user activates module.
  Successor classes can use this method to customize module activation process,
  for example, to show own menus, toolbars, etc.

  Default implementation always returns \c true.

  \return \c true if module is activated successfully.
  \sa initialize(), deactivateModule()
 */
bool CAM_Module::activateModule( SUIT_Study* /*study*/ )
{
  // IMN 05/03/2015: we copied myActionMap for reset/unset actions accelerator keys
  // after activate/deactivate modules
  for ( QMap<QAction*, QKeySequence>::Iterator it = myActionShortcutMap.begin(); it != myActionShortcutMap.end(); ++it )
  {
    // Reset actions accelerator keys
    it.key()->setShortcut( it.value() );
  }
  myActionShortcutMap.clear();
  return true;
}

/*!
  \brief Deactivate module.

  This method is called when the user deactivates module.
  Successor classes can use this method to customize module deactivation process,
  for example, to hide own menus, toolbars, etc.

  Default implementation always returns \c true.

  \return \c true if module is deactivated successfully.
  \sa initialize(), activateModule()
 */
bool CAM_Module::deactivateModule( SUIT_Study* )
{
  // IMN 05/03/2015: we copied myActionMap for reset/unset actions accelerator keys
  // after activate/deactivate modules
  myActionShortcutMap.clear();
  for ( QMap<int, QAction*>::Iterator it = myActionMap.begin(); it != myActionMap.end(); ++it )
  {
    myActionShortcutMap.insert( it.value(), it.value()->shortcut() );
    it.value()->setShortcut( QKeySequence() );
  }
  return true;
}

/*!
  \brief Called when study is closed.

  Removes data model from the \a study.

  \param study study being closed
*/
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

/*!
  \brief Called when study is changed (obsolete).

  Default implementation does nothing.

  \param oldStudy old study
  \param newStudy new study
*/
void CAM_Module::studyChanged( SUIT_Study* /*oldStudy*/, SUIT_Study* /*newStudy*/ )
{
}

/*!
  \brief Check if the module is active.
  \return \c true if module is active.
*/
bool CAM_Module::isActiveModule() const
{
  return application() ? application()->activeModule() == this : false;
}

/*!
  \brief Put the text message into the status bar of the application main window.

  If \a msec > 0, the message will be shown \a msec milliseconds.
  If \a msec < 0, the message will be constantly displayed until module is active.

  \param msg text message
  \param msec message displaying duration in milliseconds
*/
void CAM_Module::putInfo( const QString& msg, const int msec )
{
  if ( application() )
    application()->putInfo( msg, msec );

  if ( msec < 0 )
    myInfo = msg;
}

/*!
  \brief Restore message info.

  Restores constant text message when previous information status message is removed.

  \param txt previous message (being removed)
  \sa putInfo()
*/
void CAM_Module::onInfoChanged( QString txt )
{
  if ( txt.isEmpty() && isActiveModule() && !myInfo.isEmpty() && application() )
    application()->putInfo( myInfo );
}

/*!
  \brief Called when application is closed.

  Nullify application pointer if the application is being closed.

  \param theApp application
*/
void CAM_Module::onApplicationClosed( SUIT_Application* theApp )
{
  if (myApp == theApp)
    myApp = NULL;
}

/*!
  \brief Create data model.
  \return created data model object or 0 if it could not be created
*/
CAM_DataModel* CAM_Module::createDataModel()
{
  return new CAM_DataModel( this );
}

/*!
  \brief Set module (internal) name
  \param name new module name
  \sa name(), moduleName(), setModuleName()
 */
void CAM_Module::setName( const QString& name )
{
  setObjectName( name );
}

/*!
  \brief Set module title (user name)
  \param name new module title
  \sa moduleName(), name(), setName()
 */
void CAM_Module::setModuleName( const QString& name )
{
  myName = name;
}

/*!
  \brief Get menu manager.
  \return menu manager pointer
*/
QtxActionMenuMgr* CAM_Module::menuMgr() const
{
  QtxActionMenuMgr* mgr = 0;
  if ( application() && application()->desktop() )
    mgr = application()->desktop()->menuMgr();
  return mgr;
}

/*!
  \brief Get toolbar manager.
  \return toolbar manager pointer
*/
QtxActionToolMgr* CAM_Module::toolMgr() const
{
  QtxActionToolMgr* mgr = 0;
  if ( application() && application()->desktop() )
    mgr = application()->desktop()->toolMgr();
  return mgr;
}

/*!
  \brief Create toolbar with speicifed \a name.

  If the toolbar has been already created, its ID is just returned.

  \param title toolbar title
  \param name toolbar name (identifier)
  \return toolbar ID or -1 if toolbar could not be created
*/
int CAM_Module::createTool( const QString& title, const QString& name )
{
  if ( !toolMgr() )
    return -1;

  ActionMgrLocker lock( toolMgr(), !myToolShown );

  return toolMgr()->createToolBar( title, name );
}

/*!
  \brief Add toolbar item.

  Insert action \a to the toolbar manager and register it with specified \a id.
  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If action has been already added previously, its ID is just returned.

  If \a id < 0, the action ID is generated automatically.

  If \a idx < 0, the action is added to the end of the toolbar.

  \param a action
  \param tBar toolbar ID
  \param id requested action ID
  \param idx action index (desired position in the toolbar)
  \return action ID or -1 if toolbar item could not be added
*/
int CAM_Module::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  if ( !toolMgr() )
    return -1;

  ActionMgrLocker lock( toolMgr(), !myToolShown );

  int regId = registerAction( id, a );
  int intId = toolMgr()->insert( a, tBar, idx );

  if ( !myToolShown )
    setToolShown( a, false );

  return intId != -1 ? regId : -1;
}

/*!
  \brief Add toolbar item.

  Insert action \a to the toolbar manager and register it with specified \a id.
  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If action has been already added previously, its ID is just returned.

  If \a id < 0, the action ID is generated automatically.

  If \a idx < 0, the action is added to the end of the toolbar.

  \param a action
  \param tBar toolbar name
  \param id requested action ID
  \param idx action index (desired position in the toolbar)
  \return action ID or -1 if toolbar item could not be added
*/
int CAM_Module::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  if ( !toolMgr() )
    return -1;

  ActionMgrLocker lock( toolMgr(), !myToolShown );

  int regId = registerAction( id, a );
  int intId = toolMgr()->insert( a, tBar, idx );

  if ( !myToolShown )
    setToolShown( a, false );

  return intId != -1 ? regId : -1;
}

/*!
  \brief Add toolbar item.

  Insert action with \a id identifier to the toolbar manager.
  It is assumed that action has been already registered.

  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If action has been already added previously, its ID is just returned.

  If \a idx < 0, the action is added to the end of the toolbar.

  \param id action ID
  \param tBar toolbar ID
  \param idx action index (desired position in the toolbar)
  \return action ID or -1 if toolbar item could not be added
*/
int CAM_Module::createTool( const int id, const int tBar, const int idx )
{
  if ( !toolMgr() )
    return -1;

  ActionMgrLocker lock( toolMgr(), !myToolShown );

  int intId = toolMgr()->insert( action( id ), tBar, idx );

  if ( !myToolShown )
    setToolShown( action( id ), false );

  return intId != -1 ? id : -1;
}

/*!
  \brief Add toolbar item.

  Insert action with \a id identifier to the toolbar manager.
  It is assumed that action has been already registered.

  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If action has been already added previously, its ID is just returned.

  If \a idx < 0, the action is added to the end of the toolbar.

  \param id action ID
  \param tBar toolbar name
  \param idx action index (desired position in the toolbar)
  \return action ID or -1 if toolbar item could not be added
*/
int CAM_Module::createTool( const int id, const QString& tBar, const int idx )
{
  if ( !toolMgr() )
    return -1;

  ActionMgrLocker lock( toolMgr(), !myToolShown );

  int intId = toolMgr()->insert( action( id ), tBar, idx );

  if ( !myToolShown )
    setToolShown( action( id ), false );

  return intId != -1 ? id : -1;
}

/*!
  \brief Create menu or submenu.

  Create main menu or popup submenu and register it with specified \a id.
  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If \a id < 0, the menu ID is generated automatically.
  If menu has been already created previously, its ID is just returned.

  The \a menu parameter represents the menu name - it could be a sequence
  of strings, separated by '|' symbol. For example, "File|Edit" means
  File->Edit submenu. If menu doesn't exist, it is created automatically.

  Parameter \a idx defines the index of the menu item in the menu group which
  is defined by the \a group. If \a idx < 0, the menu/submenu is added to the
  end of the menu group.

  \param subMenu subMenu name
  \param menu parent menu ID
  \param id requested menu ID
  \param group menu group ID
  \param idx menu item index (desired position in the menu group)
  \return menu item ID or -1 if menu item could not be added
*/
int CAM_Module::createMenu( const QString& subMenu, const int menu,
                            const int id, const int group, const int idx, QMenu * menuObj )
{
  if ( !menuMgr() )
    return -1;
  
  return menuMgr()->insert( subMenu, menu, group, id, idx, menuObj );
}

/*!
  \brief Create menu or submenu.

  Create main menu or popup submenu and register it with specified \a id.
  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If \a id < 0, the menu ID is generated automatically.
  If menu has been already created previously, its ID is just returned.

  The \a menu parameter represents the menu name - it could be a sequence
  of strings, separated by '|' symbol. For example, "File|Edit" means
  File->Edit submenu. If menu doesn't exist, it is created automatically.

  Parameter \a idx defines the index of the menu item in the menu group which
  is defined by the \a group. If \a idx < 0, the menu/submenu is added to the
  end of the menu group.

  \param subMenu subMenu name
  \param menu parent menu name(s)
  \param id requested menu ID
  \param group menu group ID
  \param idx menu item index (desired position in the menu group)
  \return menu item ID or -1 if menu item could not be added
*/
int CAM_Module::createMenu( const QString& subMenu, const QString& menu,
                            const int id, const int group, const int idx )
{
  if ( !menuMgr() )
    return -1;

  return menuMgr()->insert( subMenu, menu, group, id, idx );
}

/*!
  \brief Add menu item.

  Insert action \a to the menu manager and register it with specified \a id.
  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If \a id < 0, the action ID is generated automatically.

  If action has been already added previously, its ID is just returned.

  Parameter \a idx defines the index of the menu item in the menu group which
  is defined by the \a group. If \a idx < 0, the action is added to the
  end of the menu group.

  \param a action
  \param menu menu ID
  \param id requested action ID
  \param group menu group ID
  \param idx action index (desired position in the menu group)
  \return action ID or -1 if menu item could not be added
*/
int CAM_Module::createMenu( QAction* a, const int menu, const int id, const int group, const int idx )
{
  if ( !a || !menuMgr() )
    return -1;
  
  ActionMgrLocker lock( menuMgr(), !myMenuShown );

  int regId = registerAction( id, a );
  int intId = menuMgr()->insert( a, menu, group, idx );

  if ( !myMenuShown )
    setMenuShown( a, false );

  return intId != -1 ? regId : -1;
}

/*!
  \brief Add menu item.

  Insert action \a to the menu manager and register it with specified \a id.
  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If \a id < 0, the action ID is generated automatically.

  If action has been already added previously, its ID is just returned.

  The \a menu parameter represents the menu name - it could be a sequence
  of strings, separated by '|' symbol. For example, "File|Edit" means
  File->Edit submenu. If menu doesn't exist, it is created automatically.

  Parameter \a idx defines the index of the menu item in the menu group which
  is defined by the \a group. If \a idx < 0, the action is added to the
  end of the menu group.

  \param a action
  \param menu menu name(s)
  \param id requested action ID
  \param group menu group ID
  \param idx action index (desired position in the menu group)
  \return action ID or -1 if menu item could not be added
*/
int CAM_Module::createMenu( QAction* a, const QString& menu, const int id, const int group, const int idx )
{
  if ( !a || !menuMgr() )
    return -1;

  ActionMgrLocker lock( menuMgr(), !myMenuShown );

  int regId = registerAction( id, a );
  int intId = menuMgr()->insert( a, menu, group, idx );

  if ( !myMenuShown )
    setMenuShown( a, false );
  
  return intId != -1 ? regId : -1;
}

/*!
  \brief Add menu item.

  Insert action with \a id identifier to the menu manager.
  It is assumed that action has been already registered.

  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If action has been already added previously, its ID is just returned.

  Parameter \a idx defines the index of the menu item in the menu group which
  is defined by the \a group. If \a idx < 0, the action is added to the
  end of the menu group.

  \param id action ID
  \param menu menu ID
  \param group menu group ID
  \param idx action index (desired position in the menu group)
  \return action ID or -1 if menu item could not be added
*/
int CAM_Module::createMenu( const int id, const int menu, const int group, const int idx )
{
  if ( !menuMgr() )
    return -1;

  ActionMgrLocker lock( menuMgr(), !myMenuShown );

  int intId = menuMgr()->insert( action( id ), menu, group, idx );

  if ( !myMenuShown )
    setMenuShown( action( id ), false );

  return intId != -1 ? id : -1;
}

/*!
  \brief Add menu item.

  Insert action with \a id identifier to the menu manager.
  It is assumed that action has been already registered.

  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If action has been already added previously, its ID is just returned.

  The \a menu parameter represents the menu name - it could be a sequence
  of strings, separated by '|' symbol. For example, "File|Edit" means
  File->Edit submenu. If menu doesn't exist, it is created automatically.

  Parameter \a idx defines the index of the menu item in the menu group which
  is defined by the \a group. If \a idx < 0, the action is added to the
  end of the menu group.

  \param id action ID
  \param menu menu name(s)
  \param group menu group ID
  \param idx action index (desired position in the menu group)
  \return action ID or -1 if menu item could not be added
*/
int CAM_Module::createMenu( const int id, const QString& menu, const int group, const int idx )
{
  if ( !menuMgr() )
    return -1;

  ActionMgrLocker lock( menuMgr(), !myMenuShown );

  int intId = menuMgr()->insert( action( id ), menu, group, idx );

  if ( !myMenuShown )
    setMenuShown( action( id ), false );

  return intId != -1 ? id : -1;
}

/*!
  \brief Show/hide all module's menus.
  \param on if \c true, show menus, otherwise, hide all menus
  \sa setToolShown()
*/
void CAM_Module::setMenuShown( const bool on )
{
  myMenuShown = on;

  QtxActionMenuMgr* mMgr = menuMgr();
  if ( !mMgr )
    return;

  bool upd = mMgr->isUpdatesEnabled();
  mMgr->setUpdatesEnabled( false );

  QAction* sep = separator();
  for ( QMap<int, QAction*>::Iterator it = myActionMap.begin(); it != myActionMap.end(); ++it )
  {
    if ( it.value() != sep )
      mMgr->setShown( mMgr->actionId( it.value() ), on );
  }

  mMgr->setUpdatesEnabled( upd );
  if ( upd )
    mMgr->update();
}

/*!
  \brief Show/hide specified menu item.
  \param a action
  \param on if \c true, show menu item, otherwise, hide it
*/
void CAM_Module::setMenuShown( QAction* a, const bool on )
{
  if ( menuMgr() )
    menuMgr()->setShown( menuMgr()->actionId( a ), on );
}

/*!
  \brief Show/hide specified menu item.
  \param id menu item ID
  \param on if \c true, show menu item, otherwise, hide it
*/
void CAM_Module::setMenuShown( const int id, const bool on )
{
  setMenuShown( action( id ), on );
}

/*!
  \brief Show/hide all module's toolbars.
  \param on if \c true, show toolbars, otherwise, hide all toolbars
  \sa setMenuShown()
*/
void CAM_Module::setToolShown( const bool on )
{
  myToolShown = on;

  QtxActionToolMgr* tMgr = toolMgr();
  if ( !tMgr )
    return;

  bool upd = tMgr->isUpdatesEnabled();
  tMgr->setUpdatesEnabled( false );

  QAction* sep = separator();
  for ( QMap<int, QAction*>::Iterator it = myActionMap.begin(); it != myActionMap.end(); ++it )
  {
    if ( it.value() != sep )
      tMgr->setShown( tMgr->actionId( it.value() ), on );
  }

  tMgr->setUpdatesEnabled( upd );
  if ( upd )
    tMgr->update();
}

/*!
  \brief Show/hide specified toolbar item.
  \param a action
  \param on if \c true, show toolbar item, otherwise, hide it
*/
void CAM_Module::setToolShown( QAction* a, const bool on )
{
  if ( toolMgr() )
    toolMgr()->setShown( toolMgr()->actionId( a ), on );
}

/*!
  \brief Show/hide specified toolbar item.
  \param id toolbar item ID
  \param on if \c true, show toolbar item, otherwise, hide it
*/
void CAM_Module::setToolShown( const int id, const bool on )
{
  setToolShown( action( id ), on );
}

/*!
  \brief Get action by specified \a id.
  \param id action ID
  \return action or 0 if not found
*/
QAction* CAM_Module::action( const int id ) const
{
  QAction* a = 0;
  if ( myActionMap.contains( id ) )
    a = myActionMap[id];
  else if ( menuMgr() ) {
    QMenu* m = menuMgr()->findMenu( id );
    if ( m ) a = m->menuAction();
  }
  return a;
}

/*!
  \brief Get action ID.
  \param a action
  \return action ID or -1 if not found
*/
int CAM_Module::actionId( const QAction* a ) const
{
  int id = -1;
  for ( QMap<int, QAction*>::ConstIterator it = myActionMap.begin(); it != myActionMap.end() && id == -1; ++it )
  {
    if ( it.value() == a )
      id = it.key();
  }
  return id;
}

/*!
  \brief Create new instance of QtxAction and register action with specified \a id.

  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If \a id < 0, the action ID is generated automatically.

  \param id required action ID
  \param text tooltip text
  \param icon action icon
  \param menu menu text
  \param tip status bar tip
  \param key keyboard accelerator
  \param parent parent object
  \param toggle if \c true, the action will be toggled
  \param reciever action activation signal receiver object
  \param member action activation signal receiver slot
*/
QAction* CAM_Module::createAction( const int id, const QString& text, const QIcon& icon,
                                   const QString& menu, const QString& tip, const int key,
                                   QObject* parent, const bool toggle, QObject* reciever,
				   const char* member, const QString& shortcutAction )
{
  return createAction( id, text, icon, menu, tip, QKeySequence(key), parent, toggle, reciever, member, shortcutAction );
}

/*!
  \brief Create new instance of QtxAction and register action with specified \a id.

  Resulting action ID may differ from the requested one. This can happen if
  requested ID is already in use.

  If \a id < 0, the action ID is generated automatically.

  \param id required action ID
  \param text tooltip text
  \param icon action icon
  \param menu menu text
  \param tip status bar tip
  \param key keyboard accelerator
  \param parent parent object
  \param toggle if \c true, the action will be toggled
  \param reciever action activation signal receiver object
  \param member action activation signal receiver slot
*/
QAction* CAM_Module::createAction( const int id, const QString& text, const QIcon& icon,
                                   const QString& menu, const QString& tip, const QKeySequence& key,
                                   QObject* parent, const bool toggle, QObject* reciever,
				   const char* member, const QString& shortcutAction )
{
  QtxAction* a = new QtxAction( text, icon, menu, key, parent, toggle, shortcutAction );
  a->setStatusTip( tip );

  if ( reciever && member )
    connect( a, SIGNAL( triggered( bool ) ), reciever, member );

  registerAction( id, a );

  return a;
}

/*!
  \brief Create new action group.
  \param id action group ID
  \param exclusive \c true for exclusive action group
  \return created action group
*/
QtxActionGroup* CAM_Module::createActionGroup( const int id, const bool exclusive )
{
  QtxActionGroup* a = qobject_cast<QtxActionGroup*>( action( id ) );
  if ( !a ) {
    a = new QtxActionGroup( this );
    registerAction( id, a );
  }
  a->setExclusive( exclusive );
  return a;
}

/*!
  \brief Register action in the internal action map.

  If action has been already added previously, its ID is just returned.
  If \a id < 0, the action ID is generated automatically.

  \param id action required ID
  \param a action
  \return action ID
*/
int CAM_Module::registerAction( const int id, QAction* a )
{
  int ident = -1;
  for ( QMap<int, QAction*>::ConstIterator it = myActionMap.begin(); it != myActionMap.end() && ident == -1; ++it )
    if ( it.value() == a )
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

  if ( application() && application()->desktop() )
    application()->desktop()->addAction( a );

  return ident;
}

/*!
  \brief Unregister action from the internal action map.

  \param id action ID
  \return \c true on success or \c false if action is in use
*/
bool CAM_Module::unregisterAction( const int id )
{
  return unregisterAction( action( id ) );
}

/*!
  \brief Unregister action from the internal action map.

  \param a action
  \return \c true on success or \c false if action is in use
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

/*!
  \brief Create separator action.

  Separator action can be used in menus or toolbars.

  \return new separator action
*/
QAction* CAM_Module::separator()
{
  return QtxActionMgr::separator();
}

/*!
  \brief Update visibility state of the module objects.
*/
void CAM_Module::updateModuleVisibilityState() {

}

/*!
  \brief Activate GUI operation of module by its ID.
  This method is called from CAM_Application::startOperation().
  \param actionId is a numerical unique operation id.
*/
bool CAM_Module::activateOperation( int actionId )
{
  return false;
}

/*!
  \brief Activate GUI operation of module by its ID.
  This method is called from CAM_Application::startOperation().
  \param actionId is a string unique operation id.
*/
bool CAM_Module::activateOperation( const QString& actionId )
{
  return false;
}

/*!
  \brief Activate GUI operation of module by its ID and \a pluginName.
  This method is called from CAM_Application::startOperation().
  \param actionId is a string unique operation id.
  \param pluginName is a name of a plugin where the operation is implemented.
*/
bool CAM_Module::activateOperation( const QString& actionId, const QString& pluginName )
{
  return false;
}


/*!
  \brief Connect data model of the module to the active study
  \param camStudy CAM study object
*/
void CAM_Module::connectToStudy( CAM_Study* camStudy )
{
  CAM_Application* app = camStudy ? dynamic_cast<CAM_Application*>( camStudy->application() ) : 0;
  if( !app )
    return;

  CAM_DataModel* prev = 0;
  CAM_Application::ModuleList mods = app->modules();
  for( QList<CAM_Module*>::const_iterator it = mods.begin(); it != mods.end(); ++it )
  {
    CAM_DataModel* dm = (*it)->dataModel();
    if( (*it) == this && !camStudy->containsDataModel( dm ) )
    {
      if ( prev )
        camStudy->insertDataModel( (*it)->dataModel(), prev );
      else
        camStudy->insertDataModel( (*it)->dataModel(), 0 );
    }
    prev = dm;
  }
}

/*!
  \fn void CAM_Module::contextMenuPopup( const QString& type, QMenu* menu, QString& title );
  \brief Create context popup menu.
  \param type popup menu context
  \param menu popup menu
  \param title popup menu title, which can be set by the module if required
*/

/*!
  \fn void CAM_Module::updateCommandsStatus();
  \brief Update menu/toolbar actions.
*/


bool CAM_Module::abortAllOperations()
{
  return true;
}
