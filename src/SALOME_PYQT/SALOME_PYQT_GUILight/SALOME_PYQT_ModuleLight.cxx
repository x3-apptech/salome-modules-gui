// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : SALOME_PYQT_Module.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "SALOME_PYQT_PyInterp.h"
#include <SUITApp_init_python.hxx>
#include <PyInterp_Interp.h>
#include <PyConsole_Interp.h>
#include <PyConsole_Console.h>
#include <PyInterp_Dispatcher.h>

#include "SALOME_PYQT_ModuleLight.h"
#include "SALOME_PYQT_DataModelLight.h"

#ifndef GUI_DISABLE_CORBA
#include <Container_init_python.hxx>
#endif

#include <SUIT_ResourceMgr.h>
#include <SUIT_DataObjectIterator.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewModel.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <STD_MDIDesktop.h>
#include <STD_TabDesktop.h>
#include <LightApp_Preferences.h>
#include <LightApp_Application.h>
#include <LightApp_Study.h>

#include <QtxWorkstack.h>
#include <QtxWorkspace.h>
#include <QtxActionGroup.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>

#include <QFile>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>
#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "sipAPISalomePyQtGUILight.h"

#include <sip.h>
#if SIP_VERSION < 0x040700
#include "sipQtGuiQWidget.h"
#include "sipQtGuiQMenu.h"
#endif

#include <utilities.h>

/*!
  \brief Default name of the module, replaced at the moment
  of module creation.
  \internal
*/
const char* DEFAULT_NAME  = "SALOME_PYQT_ModuleLight";

/*!
  \brief Default menu group number.
  \internal
*/
const int DEFAULT_GROUP = 40;

/*!
  \var IsCallOldMethods
  \brief Allow calling obsolete callback methods.
  \internal
  
  If the macro CALL_OLD_METHODS is not defined, the invoking
  of obsolete Python module's methods like setSetting(), definePopup(), 
  etc. is blocked.

  CALL_OLD_METHODS macro can be defined for example by adding 
  -DCALL_OLD_METHODS compilation option to the Makefile.
*/
#ifdef CALL_OLD_METHODS
const bool IsCallOldMethods = true;
#else
const bool IsCallOldMethods = false;
#endif

/* Py_ssize_t for old Pythons */
/* This code is as recommended by: */
/* http://www.python.org/dev/peps/pep-0353/#conversion-guidelines */
#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
# define PY_SSIZE_T_MAX INT_MAX
# define PY_SSIZE_T_MIN INT_MIN
#endif

//
// NB: Python requests.
// General rule for Python requests created by SALOME_PYQT_Module:
// all requests should be executed SYNCHRONOUSLY within the main GUI thread.
// However, it is obligatory that ANY Python call is wrapped with a request object,
// so that ALL Python API calls are serialized with PyInterp_Dispatcher.
//

/*!
  \class SALOME_PYQT_Module::XmlHandler
  \brief XML resource files parser.
  \internal

  This class is used to provide backward compatibility with
  existing Python modules in which obsolete menu definition system
  (via XML files) is used.
*/

class SALOME_PYQT_ModuleLight::XmlHandler
{
public:
  XmlHandler( SALOME_PYQT_ModuleLight* module, const QString& fileName );
  void createActions();
  void createPopup  ( QMenu*         menu,
                      const QString& context,
                      const QString& parent,
                      const QString& object );
  void activateMenus( bool );

protected:
  void createToolBar   ( QDomNode&   parentNode );
  void createMenu      ( QDomNode&   parentNode,
                         const int   parentMenuId = -1,
                         QMenu*      parentPopup = 0 );

  void insertPopupItems( QDomNode&   parentNode,
                         QMenu*      menu );

private:
  SALOME_PYQT_ModuleLight* myModule;
  QDomDocument             myDoc;
  QList<int>               myMenuItems;
};

//
// NB: Library initialization
// Since the SalomePyQtGUILight library is not imported in Python it's initialization function
// should be called manually (and only once) in order to initialize global sip data
// and to get C API from sip : sipBuildResult for example
//

#define INIT_FUNCTION initSalomePyQtGUILight
#if defined(SIP_STATIC_MODULE)
extern "C" void INIT_FUNCTION();
#else
PyMODINIT_FUNC INIT_FUNCTION();
#endif

/*!
  \fn CAM_Module* createModule()
  \brief Module factory function.
  \internal
  
  Creates an instance of SALOME_PYQT_Module object by request
  of an application when the module is loaded and initialized.

  \return new module object
*/

extern "C" {
  SALOME_PYQT_LIGHT_EXPORT CAM_Module* createModule() {

    static bool alreadyInitialized = false;
    if ( !alreadyInitialized ) {
      // call only once (see comment above) !
      static PyThreadState *gtstate = 0;
#ifndef GUI_DISABLE_CORBA
      if(SUIT_PYTHON::initialized)
        gtstate = SUIT_PYTHON::_gtstate;
      else
        gtstate = KERNEL_PYTHON::_gtstate;
#else
      gtstate = SUIT_PYTHON::_gtstate;
#endif
      PyEval_RestoreThread( gtstate );
      INIT_FUNCTION();
      PyEval_ReleaseThread( gtstate );
      alreadyInitialized = !alreadyInitialized;
    }
    return new SALOME_PYQT_ModuleLight();
  }
}

/*!
  \class FuncMsg
  \brief Function call in/out tracer.
  \internal
*/

class FuncMsg
{
public:
  FuncMsg( const QString& funcName )
  {
    myName = funcName;
    MESSAGE( myName.toLatin1().constData() << " [ begin ]" );
  }
  ~FuncMsg()
  {
    MESSAGE( myName.toLatin1().constData() << " [ end ]" );
  }
  void message( const QString& msg )
  {
    MESSAGE( myName.toLatin1().constData() << " : " << msg.toLatin1().constData() );
  }
private:
  QString myName;
};

/*!
  \class SALOME_PYQT_ModuleLight
  \brief This class implements module API for all the Python-based 
  SALOME modules.
*/

//
// Static variables definition
//
SALOME_PYQT_ModuleLight::InterpMap SALOME_PYQT_ModuleLight::myInterpMap;
SALOME_PYQT_ModuleLight* SALOME_PYQT_ModuleLight::myInitModule = 0;

/*!
  \brief Get the module being initialized.
  
  This is a little trick :) needed to provide an access from Python
  (SalomePyQt) to the module being currently activated. The problem
  that during the process of module initialization (initialize() 
  function) it is not yet available via application->activeModule()
  call.
  
  This method returns valid pointer only if called in scope of
  initialize() function.

  \return the module being currently initialized
*/
SALOME_PYQT_ModuleLight* SALOME_PYQT_ModuleLight::getInitModule()
{
  return myInitModule;
}

/*!
  \brief Constructor
*/
SALOME_PYQT_ModuleLight::SALOME_PYQT_ModuleLight()
: LightApp_Module( DEFAULT_NAME ),
  myInterp( 0 ),
  myModule( 0 ), 
  myXmlHandler ( 0 ),
  myLastActivateStatus( true )
{
}

/*!
  \brief Destructor
*/
SALOME_PYQT_ModuleLight::~SALOME_PYQT_ModuleLight()
{
  if ( myXmlHandler )
    delete myXmlHandler;
  if ( myInterp && myModule ) {
    PyLockWrapper aLock = myInterp->GetLockWrapper();
    Py_XDECREF(myModule);
  }
}

/*!
  \brief Initialization of the module.
  
  This method can be used for creation of the menus, toolbars and 
  other such staff.
  
  There are two ways to do this:
  - for obsolete modules this method first tries to read
  <module>_<language>.xml resource file which contains a menu,
  toolbars and popup menus description;
  - new modules can create menus by direct calling of the
  corresponding methods of SalomePyQt Python API in the Python
  module's initialize() method which is called from here.

  NOTE: SALOME supports two modes of modules loading:
  - immediate (all the modules are created and initialized 
  immediately when the application object is created;
  - postponed modules loading (used currently); in this mode
  the module is loaded only be request.
  If postponed modules loading is not used, the active
  study might be not yet defined at this stage, so initialize()
  method should not perform any study-based initialization.

  \param app parent application object
*/
void SALOME_PYQT_ModuleLight::initialize( CAM_Application* app )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::initialize()" );

  // call base implementation
  LightApp_Module::initialize( app );

  // try to get XML resource file name
  SUIT_ResourceMgr* aResMgr = getApp()->resourceMgr();
  if ( !myXmlHandler && aResMgr ) {
    // get current language
    QString aLang = aResMgr->stringValue( "language", "language", QString() );
    if ( aLang.isEmpty() ) 
      aLang = "en";
    // define resource file name
    QString aFileName = name() + "_" + aLang + ".xml";
    aFileName = aResMgr->path( "resources", name(), aFileName );
    // create XML handler instance
    if ( !aFileName.isEmpty() && QFile::exists( aFileName ) )
      myXmlHandler = new SALOME_PYQT_ModuleLight::XmlHandler( this, aFileName );
    // create menus & toolbars from XML file if required
    if ( myXmlHandler )
      myXmlHandler->createActions();
  }

  // perform internal initialization and call module's initialize() funtion
  // InitializeReq: request class for internal init() operation
  class InitializeReq : public PyInterp_Request
  {
  public:
    InitializeReq( CAM_Application*    _app,
                   SALOME_PYQT_ModuleLight* _obj )
      : PyInterp_Request( 0, true ), // this request should be processed synchronously (sync == true)
        myApp( _app ),
        myObj( _obj ) {}

  protected:
    virtual void execute()
    {
      myObj->init( myApp );
    }

  private:
    CAM_Application*    myApp;
    SALOME_PYQT_ModuleLight* myObj;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new InitializeReq( app, this ) );
}

/*!
  \brief Activation of the module.

  This function is usually used in order to show the module's 
  specific menus and toolbars, update actions state and perform
  other such actions required when the module is activated.
  
  Note, that returning \c false in this function prevents the 
  module activation.

  \param theStudy parent study
  \return \c true if activation is successful and \c false otherwise
*/
bool SALOME_PYQT_ModuleLight::activateModule( SUIT_Study* theStudy )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::activateModule()" );

  // call base implementation
  bool res = LightApp_Module::activateModule( theStudy );

  if ( !res )
    return res;

  // internal activation
  return activateModuleInternal( theStudy );
}

/*!
  \brief Perform internal activation of the module.

  The only goal of this function is to extract common functionality
  for LightApp_Module and SalomeApp_module classes requried by the
  specific architecture aspects of "light" / "full" SALOME modes.

  \sa activateModule()
*/
bool SALOME_PYQT_ModuleLight::activateModuleInternal( SUIT_Study* theStudy )
{
  // reset the activation status to the default value
  myLastActivateStatus = true;

  // perform internal activation
  // ActivateReq: request class for internal activate() operation
  class ActivateReq : public PyInterp_Request
  {
  public:
    ActivateReq( SUIT_Study*         _study,
                 SALOME_PYQT_ModuleLight* _obj )
      : PyInterp_Request( 0, true ), // this request should be processed synchronously (sync == true)
        myStudy ( _study ),
        myObj   ( _obj   ) {}

  protected:
    virtual void execute()
    {
      myObj->activate( myStudy );
    }

  private:
    SUIT_Study*         myStudy;
    SALOME_PYQT_ModuleLight* myObj;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new ActivateReq( theStudy, this ) );

  // check activation status (set by activate())
  if ( !lastActivationStatus() )
    return false;

  // activate menus, toolbars, etc
  if ( myXmlHandler ) myXmlHandler->activateMenus( true );
  setMenuShown( true );
  setToolShown( true );

  // connect preferences changing signal
  connect( getApp(), SIGNAL( preferenceChanged( const QString&, const QString&, const QString& ) ),
           this,     SLOT(   preferenceChanged( const QString&, const QString&, const QString& ) ) );

  // perform custom activation actions
  // CustomizeReq: request class for internal customize() operation
  class CustomizeReq : public PyInterp_Request
  {
  public:
    CustomizeReq( SUIT_Study*         _study,
                  SALOME_PYQT_ModuleLight* _obj )
      : PyInterp_Request( 0, true ), // this request should be processed synchronously (sync == true)
        myStudy ( _study ),
        myObj   ( _obj   ) {}

  protected:
    virtual void execute()
    {
      myObj->customize( myStudy );
    }

  private:
    SUIT_Study*         myStudy;
    SALOME_PYQT_ModuleLight* myObj;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new CustomizeReq( theStudy, this ) );

  return true;
}

/*!
  \brief Deactivation of the module.

  This function is usually used in order to hide the module's 
  specific menus and toolbars and perform other such actions
  required when the module is deactivated.

  \param theStudy parent study
  \return \c true if deactivation is successful and \c false otherwise
*/
bool SALOME_PYQT_ModuleLight::deactivateModule( SUIT_Study* theStudy )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::deactivateModule()" );

  // disconnect preferences changing signal
  disconnect( getApp(), SIGNAL( preferenceChanged( const QString&, const QString&, const QString& ) ),
              this,     SLOT(   preferenceChanged( const QString&, const QString&, const QString& ) ) );

  // perform internal deactivation
  // DeactivateReq: request class for internal deactivate() operation
  class DeactivateReq : public PyInterp_LockRequest
  {
  public:
    DeactivateReq( PyInterp_Interp*    _py_interp,
                   SUIT_Study*         _study,
                   SALOME_PYQT_ModuleLight* _obj )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myStudy ( _study ),
        myObj   ( _obj   ) {}

  protected:
    virtual void execute()
    {
      myObj->deactivate( myStudy );
    }

  private:
    SUIT_Study*         myStudy;
    SALOME_PYQT_ModuleLight* myObj;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new DeactivateReq( myInterp, theStudy, this ) );

  // deactivate menus, toolbars, etc
  if ( myXmlHandler ) myXmlHandler->activateMenus( false );
  setMenuShown( false );
  setToolShown( false );

  // call base implementation
  return LightApp_Module::deactivateModule( theStudy );
}

/*!
 \brief Get last activation status.
 \return status of last module activation operation
 \sa activateModule()
*/
bool SALOME_PYQT_ModuleLight::lastActivationStatus() const
{
  return myLastActivateStatus;
}

/*!
  \breif Process application preferences changing.

  Called when any application setting is changed.

  \param module preference module
  \param section preference resource file section
  \param setting preference resource name
*/
void SALOME_PYQT_ModuleLight::preferenceChanged( const QString& module, 
                                            const QString& section, 
                                            const QString& setting )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::preferenceChanged()" );

  // perform synchronous request to Python event dispatcher
  class Event : public PyInterp_LockRequest
  {
  public:
    Event( PyInterp_Interp*    _py_interp,
           SALOME_PYQT_ModuleLight* _obj,
           const QString&      _section,
           const QString&      _setting )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj    ( _obj ),
        mySection( _section ),
        mySetting( _setting ) {}

  protected:
    virtual void execute()
    {
      myObj->prefChanged( mySection, mySetting );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    QString mySection, mySetting;
  };

  if ( module != moduleName() ) {
    // module's own preferences are processed by preferencesChanged() method
    // ...
    // post the request only if dispatcher is not busy!
    // execute request synchronously
    if ( !PyInterp_Dispatcher::Get()->IsBusy() )
      PyInterp_Dispatcher::Get()->Exec( new Event( myInterp, this, section, setting ) );
  }
}

/*!
  \brief Process study activation.
  
  Called when study desktop is activated. Used for notifying the Python
  module about changing of the active study.
*/
void SALOME_PYQT_ModuleLight::studyActivated()
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::studyActivated()" );

  // StudyChangedReq: request class for internal studyChanged() operation
  class StudyChangedReq : public PyInterp_Request
  {
  public:
    StudyChangedReq( SUIT_Study*         _study,
                     SALOME_PYQT_ModuleLight* _obj )
      : PyInterp_Request( 0, true ), // this request should be processed synchronously (sync == true)
        myStudy ( _study ),
        myObj   ( _obj   ) {}

  protected:
    virtual void execute()
    {
      myObj->studyChanged( myStudy );
    }

  private:
    SUIT_Study*         myStudy;
    SALOME_PYQT_ModuleLight* myObj;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new StudyChangedReq( application()->activeStudy(), this ) );
}

/*!
  \brief Process GUI action (from main menu, toolbar or 
  context popup menu action).
*/
void SALOME_PYQT_ModuleLight::onGUIEvent()
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::onGUIEvent()" );

  // get sender action
  QAction* action = qobject_cast<QAction*>( sender() );
  if ( !action )
    return;

  // get action ID
  int id = actionId( action );
  fmsg.message( QString( "action id = %1" ).arg( id ) );

  // perform synchronous request to Python event dispatcher
  class GUIEvent : public PyInterp_LockRequest
  {
  public:
    GUIEvent( PyInterp_Interp*    _py_interp,
              SALOME_PYQT_ModuleLight* _obj,
              int                 _id )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myId    ( _id  ),
        myObj   ( _obj ) {}

  protected:
    virtual void execute()
    {
      myObj->guiEvent( myId );
    }

  private:
    int                 myId;
    SALOME_PYQT_ModuleLight* myObj;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new GUIEvent( myInterp, this, id ) );
}

/*!
  \brief Process context popup menu request.
  
  Called when user activates popup menu in some window
  (view, object browser, etc).

  \param theContext popup menu context (e.g. "ObjectBrowser")
  \param thePopupMenu popup menu
  \param title popup menu title (not used)
*/
void SALOME_PYQT_ModuleLight::contextMenuPopup( const QString& theContext, 
                                           QMenu*         thePopupMenu, 
                                           QString&       /*title*/ )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::contextMenuPopup()" );
  fmsg.message( QString( "context: %1" ).arg( theContext ) );

  // perform synchronous request to Python event dispatcher
  class PopupMenuEvent : public PyInterp_LockRequest
  {
  public:
    PopupMenuEvent( PyInterp_Interp*    _py_interp,
                    SALOME_PYQT_ModuleLight* _obj,
                    const QString&      _context,
                    QMenu*        _popup )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myContext( _context ),
        myPopup  ( _popup  ),
        myObj    ( _obj )   {}

  protected:
    virtual void execute()
    {
      myObj->contextMenu( myContext, myPopup );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    QString             myContext;
    QMenu*         myPopup;
  };

  // post request only if dispatcher is not busy!
  // execute request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new PopupMenuEvent( myInterp, this, theContext, thePopupMenu ) );
}

/*!
  \brief Export preferences for the Python module.
  
  Called only once when the first instance of the module is created.
*/
void SALOME_PYQT_ModuleLight::createPreferences()
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::createPreferences()" );

  // perform synchronous request to Python event dispatcher
  class Event : public PyInterp_LockRequest
  {
  public:
    Event( PyInterp_Interp*    _py_interp,
           SALOME_PYQT_ModuleLight* _obj )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj    ( _obj )   {}

  protected:
    virtual void execute()
    {
      myObj->initPreferences();
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
  };

  // post request only if dispatcher is not busy!
  // execute request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new Event( myInterp, this ) );
}

/*!
  \brief Define the dockable windows associated with the module.
  
  To fill the list of windows the correspondind Python module's windows()
  method is called from SALOME_PYQT_ModuleLight::init() method.

  By default, ObjectBrowser, PythonConsole and LogWindow windows are 
  associated to the module.

  Allowed dockable windows:
  - LightApp_Application::WT_ObjectBrowser : object browser
  - LightApp_Application::WT_PyConsole : python console
  - LightApp_Application::WT_LogWindow : log messages output window

  Dock area is defined by Qt::DockWidgetArea enumeration:
  - Qt::TopDockWidgetArea : top dock area
  - Qt::BottomDockWidgetArea : bottom dock area
  - Qt::LeftDockWidgetArea : left dock area
  - Qt::RightDockWidgetArea : right dock area

  \param mappa map of dockable windows: { <window_type> : <dock_area> }
*/
void SALOME_PYQT_ModuleLight::windows( QMap<int, int>& mappa ) const
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::windows()" );

  mappa = myWindowsMap;
}

/*!
  \brief Define the compatible view windows associated with the module.

  The associated view windows are opened automatically when the module
  is activated.

  To fill the list of views the correspondind Python module's views()
  method is called from SALOME_PYQT_ModuleLight::init() method.
  By default, the list is empty.

  \param listik list of view windows types
*/
void SALOME_PYQT_ModuleLight::viewManagers( QStringList& lst ) const
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::viewManagers()" );

  lst = myViewMgrList;
}

/*!
  \brief Process module's preferences changing.

  Called when the module's preferences are changed.
  
  \param section setting section
  \param setting setting name
*/
void SALOME_PYQT_ModuleLight::preferencesChanged( const QString& section, const QString& setting )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::preferencesChanged()" );

  // perform synchronous request to Python event dispatcher
  class Event : public PyInterp_LockRequest
  {
  public:
    Event( PyInterp_Interp*    _py_interp,
           SALOME_PYQT_ModuleLight* _obj,
           const QString&      _section,
           const QString&      _setting )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj    ( _obj ),
        mySection( _section ),
        mySetting( _setting ) {}

  protected:
    virtual void execute()
    {
      myObj->prefChanged( mySection, mySetting );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    QString mySection, mySetting;
  };

  // post request only if dispatcher is not busy!
  // execut request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new Event( myInterp, this, section, setting ) );
}

/*!
  \brief Internal module initialization:

  Performs the following actions:
  - initialize or get the Python interpreter (one per study)
  - import the Python module
  - pass the workspace widget to the Python module
  - call Python module's initialize() method
  - call Python module's windows() method
  - call Python module's views() method

  \param app parent application object
*/
void SALOME_PYQT_ModuleLight::init( CAM_Application* app )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::init()" );

  // reset interpreter to NULL
  myInterp = NULL;

  // get study Id
  LightApp_Application* anApp = dynamic_cast<LightApp_Application*>( app );
  if ( !anApp )
    return;
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( app->activeStudy() );
  if ( !aStudy )
    return;
  int aStudyId = aStudy ? aStudy->id() : 0;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp( aStudyId );
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myModule )
    return; // Error

  // this module is being activated now!
  myInitModule = this;

  // then call Python module's initialize() method
  // ... first get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();
  // ... (the Python module is already imported)
  // ... finally call Python module's initialize() method
  if ( PyObject_HasAttrString( myModule, (char*)"initialize" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"initialize", (char*)"" ) );
    if ( !res ) {
      PyErr_Print();
    }
  }

  // get required dockable windows list from the Python module 
  // by calling windows() method
  // ... first put default values
  myWindowsMap.insert( LightApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
  myWindowsMap.insert( LightApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
  myWindowsMap.insert( LightApp_Application::WT_LogWindow,     Qt::BottomDockWidgetArea );

  if ( PyObject_HasAttrString( myModule , (char*)"windows" ) ) {
    PyObjWrapper res1( PyObject_CallMethod( myModule, (char*)"windows", (char*)"" ) );
    if ( !res1 ) {
      PyErr_Print();
    }
    else {
      myWindowsMap.clear();
      if ( PyDict_Check( res1 ) ) {
        PyObject* key;
        PyObject* value;
        Py_ssize_t pos = 0;
        while ( PyDict_Next( res1, &pos, &key, &value ) ) {
          // parse the return value
          // it should be a map: {integer:integer}
          int aKey, aValue;
          if( key && PyInt_Check( key ) && value && PyInt_Check( value ) ) {
            aKey   = PyInt_AsLong( key );
            aValue = PyInt_AsLong( value );
            myWindowsMap[ aKey ] = aValue;
          }
        }
      }
    }
  }

  // get compatible view windows types from the Python module 
  // by calling views() method
  if ( PyObject_HasAttrString( myModule , (char*)"views" ) ) {
    PyObjWrapper res2( PyObject_CallMethod( myModule, (char*)"views", (char*)"" ) );
    if ( !res2 ) {
      PyErr_Print();
    }
    else {
      // parse the return value
      // result can be one string...
      if ( PyString_Check( res2 ) ) {
        myViewMgrList.append( PyString_AsString( res2 ) );
      }
      // ... or list of strings
      else if ( PyList_Check( res2 ) ) {
        int size = PyList_Size( res2 );
        for ( int i = 0; i < size; i++ ) {
          PyObject* value = PyList_GetItem( res2, i );
          if( value && PyString_Check( value ) ) {
            myViewMgrList.append( PyString_AsString( value ) );
          }
        }
      }
    }
  }
  // module is already activated!
  myInitModule = 0;
}

/*!
  \brief Internal activation:

  Performs the following actions:
  - initialize or get the Python interpreter (one per study)
  - import the Python GUI module
  - call Python module's activate() method

  \param theStudy parent study object
*/
void SALOME_PYQT_ModuleLight::activate( SUIT_Study* theStudy )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::activate()" );

  // get study Id
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( theStudy );
  int aStudyId = aStudy ? aStudy->id() : 0;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp( aStudyId );
  if ( !myInterp ) {
    myLastActivateStatus = false;
    return; // Error
  }

  // import Python GUI module
  importModule();
  if ( !myModule ) {
    myLastActivateStatus = false;
    return; // Error
  }

  // get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();

  // call Python module's activate() method (for the new modules)
  if ( PyObject_HasAttrString( myModule , (char*)"activate" ) ) {
    PyObject* res1 = PyObject_CallMethod( myModule, (char*)"activate", (char*)"" );
    if ( !res1 || !PyBool_Check( res1 ) ) {
      PyErr_Print();
      // always true for old modules (no return value)
      myLastActivateStatus = true;
    }
    else {
      // detect return status
      myLastActivateStatus = PyObject_IsTrue( res1 );
    }
  } 
  
  // Connect the SUIT_Desktop signal windowActivated() to this->onActiveViewChanged()
  SUIT_Desktop* aDesk = theStudy->application()->desktop();
  if ( aDesk )
  {
    connect( aDesk, SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
             this,  SLOT( onActiveViewChanged( SUIT_ViewWindow* ) ) );
    // If a active window exists send activeViewChanged
    // If a getActiveView() in SalomePyQt available we no longer need this 
    SUIT_ViewWindow* aView = aDesk->activeWindow();
    if ( aView ) 
      activeViewChanged( aView );
    
    // get all view currently opened in the study and connect their signals  to 
    // the corresponding slots of the class.
    QList<SUIT_ViewWindow*> wndList = aDesk->windows();
    SUIT_ViewWindow* wnd;
    foreach ( wnd, wndList )
      connectView( wnd );
  }
}

/*!
  \brief Additional customization after module is activated:

  Performs the following actions:
  - get the Python interpreter (one per study)
  - import the Python GUI module
  - call Python module's setSettings() method (obsolete function, 
  used for compatibility with old code)

  \param theStudy parent study object
*/
void SALOME_PYQT_ModuleLight::customize( SUIT_Study* theStudy )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::customize()" );

  // get study Id
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( theStudy );
  int aStudyId = aStudy ? aStudy->id() : 0;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp( aStudyId );
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myModule )
    return; // Error

  if ( IsCallOldMethods ) {
    // call Python module's setWorkspace() method
    setWorkSpace();
  }

  // get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();

  if ( IsCallOldMethods ) {
    // call Python module's setSettings() method (obsolete)
    if ( PyObject_HasAttrString( myModule , (char*)"setSettings" ) ) {
      PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"setSettings", (char*)"" ) );
      if( !res ) {
        PyErr_Print();
      }
    }
  }
}

/*!
  \brief Internal deactivation:

  Performs the following actions:
  - call Python module's deactivate() method

  \param theStudy parent study object
*/
void SALOME_PYQT_ModuleLight::deactivate( SUIT_Study* theStudy )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::deactivate()" );

  // check if the subinterpreter is initialized and Python module is imported
  if ( !myInterp || !myModule ) {
    // Error! Python subinterpreter should be initialized and module should be imported first!
    return;
  }
  // then call Python module's deactivate() method
  if ( PyObject_HasAttrString( myModule , (char*)"deactivate" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"deactivate", (char*)"" ) );
    if( !res ) {
      PyErr_Print();
    }
  }
  
  // Disconnect the SUIT_Desktop signal windowActivated()
  SUIT_Desktop* aDesk = theStudy->application()->desktop();
  if ( aDesk )
  {
    disconnect( aDesk, SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
                this,  SLOT( onActiveViewChanged( SUIT_ViewWindow* ) ) );      
  }
}

/*!
  \brief Perform internal actions when active study is changed.

  Called when active the study is actived (user brings its 
  desktop to top):
  - initialize or get the Python interpreter (one per study)
  - import the Python GUI module
  - call Python module's activeStudyChanged() method

  \param theStudy study being activated
*/
void SALOME_PYQT_ModuleLight::studyChanged( SUIT_Study* theStudy )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::studyChanged()" );

  // get study Id
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( theStudy );
  int aStudyId = aStudy ? aStudy->id() : 0;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp( aStudyId );
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myModule )
    return; // Error

  if ( IsCallOldMethods ) {
    // call Python module's setWorkspace() method
    setWorkSpace();
  }

  // get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();

  // call Python module's activeStudyChanged() method
  if ( PyObject_HasAttrString( myModule, (char*)"activeStudyChanged" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"activeStudyChanged", (char*)"i", aStudyId ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Process (internally) context popup menu request.

  Performs the following actions:
  - calls Python module's definePopup(...) method (obsolete function, 
  used for compatibility with old code) to define the popup menu context
  - parses XML resourses file (if exists) and fills the popup menu with the items)
  - calls Python module's customPopup(...) method (obsolete function, 
  used for compatibility with old code) to allow module to customize the popup menu
  - for new modules calls createPopupMenu() function to allow the 
  modules to build the popup menu by using insertItem(...) Qt functions.

  \param theContext popup menu context
  \param thePopupMenu popup menu
*/
void SALOME_PYQT_ModuleLight::contextMenu( const QString& theContext, QMenu* thePopupMenu )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::contextMenu()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  QString aContext( "" ), aObject( "" ), aParent( theContext );

  if ( IsCallOldMethods && PyObject_HasAttrString( myModule, (char*)"definePopup" ) ) {
    // call definePopup() Python module's function
    // this is obsolete function, used only for compatibility reasons
    PyObjWrapper res( PyObject_CallMethod( myModule,
                                           (char*)"definePopup",
                                           (char*)"sss",
                                           theContext.toLatin1().constData(),
                                           aObject.toLatin1().constData(),
                                           aParent.toLatin1().constData() ) );
    if( !res ) {
      PyErr_Print();
    }
    else {
      // parse return value
      char *co, *ob, *pa;
      if( PyArg_ParseTuple( res, "sss", &co, &ob, &pa ) ) {
        aContext = co;
        aObject  = ob;
        aParent  = pa;
      }
    }
  } // if ( IsCallOldMethods ... )

  // first try to create menu via XML parser:
  // we create popup menus without help of QtxPopupMgr
  if ( myXmlHandler )
    myXmlHandler->createPopup( thePopupMenu, aContext, aParent, aObject );

#if SIP_VERSION < 0x040800
  PyObjWrapper sipPopup( sipBuildResult( 0, "M", thePopupMenu, sipClass_QMenu) );
#else
  PyObjWrapper sipPopup( sipBuildResult( 0, "D", thePopupMenu, sipType_QMenu, NULL) );
#endif

  // then call Python module's createPopupMenu() method (for new modules)
  if ( PyObject_HasAttrString( myModule, (char*)"createPopupMenu" ) ) {
    PyObjWrapper res1( PyObject_CallMethod( myModule,
                                            (char*)"createPopupMenu",
                                            (char*)"Os",
                                            sipPopup.get(),
                                            theContext.toLatin1().constData() ) );
    if( !res1 ) {
      PyErr_Print();
    }
  }

  if ( IsCallOldMethods && PyObject_HasAttrString( myModule, (char*)"customPopup" ) ) {
    // call customPopup() Python module's function
    // this is obsolete function, used only for compatibility reasons
    PyObjWrapper res2( PyObject_CallMethod( myModule,
                                            (char*)"customPopup",
                                            (char*)"Osss",
                                            sipPopup.get(),
                                            aContext.toLatin1().constData(),
                                            aObject.toLatin1().constData(),
                                            aParent.toLatin1().constData() ) );
    if( !res2 ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Internal GUI event handling.

  Performs the following actions:
  - calls Python module's OnGUIEvent() method

  \param theId GUI action ID
*/
void SALOME_PYQT_ModuleLight::guiEvent( const int theId )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::guiEvent()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  if ( PyObject_HasAttrString( myModule, (char*)"OnGUIEvent" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"OnGUIEvent", (char*)"i", theId ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Initialize (internally) preferences for the module.

  Performs the following actions:
  - calls Python module's createPreferences() method
*/
void SALOME_PYQT_ModuleLight::initPreferences()
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::initPreferences()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  // temporary set myInitModule because createPreferences() method
  // might be called during the module intialization process
  myInitModule = this;

  if ( PyObject_HasAttrString( myModule, (char*)"createPreferences" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"createPreferences", (char*)"" ) );
    if( !res ) {
      PyErr_Print();
    }
  }

  myInitModule = 0;
}

/*!
  \brief Initialize python subinterpreter (one per study).
  \param theStudyId study ID
*/
void SALOME_PYQT_ModuleLight::initInterp( int theStudyId )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::initInterp()" );

  // check study Id
  if ( !theStudyId ) {
    // Error! Study Id must not be 0!
    myInterp = NULL;
    return;
  }
  // try to find the subinterpreter
  if( myInterpMap.contains( theStudyId ) ) {
    // found!
    myInterp = myInterpMap[ theStudyId ];
    return;
  }

  myInterp = new SALOME_PYQT_PyInterp();
  if(!myInterp)
    return;
  
  myInterp->initialize();
  myInterpMap[ theStudyId ] = myInterp;
  
#ifndef GUI_DISABLE_CORBA
  if(!SUIT_PYTHON::initialized) {
    // import 'salome' module and call 'salome_init' method;
    // do it only once on interpreter creation
    // ... first get python lock
    PyLockWrapper aLock = myInterp->GetLockWrapper();
    // ... then import a module
    PyObjWrapper aMod = PyImport_ImportModule( "salome" );
    if( !aMod ) {
      // Error!
      PyErr_Print();
      return;
    }
    // ... then call a method
    int embedded = 1;
    PyObjWrapper aRes( PyObject_CallMethod( aMod, (char*)"salome_init", (char*)"ii", theStudyId, embedded ) );
    if( !aRes ) {
      // Error!
      PyErr_Print();
      return;
    }
  }
#endif 
}

/*!
  \brief Import Python GUI module and remember the reference to the module.

  Attention! initInterp() should be called first!!!
*/
void SALOME_PYQT_ModuleLight::importModule()
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::importModule()" );

  // check if the subinterpreter is initialized
  if ( !myInterp ) {
    // Error! Python subinterpreter should be initialized first!
    myModule = 0;
    return;
  }
  // import Python GUI module and puts it in <myModule> attribute
  // ... first get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();
  // ... then import a module
  QString aMod = name() + "GUI";
  try {
    myModule = PyImport_ImportModule( aMod.toLatin1().data() );
  }
  catch (...) {
  }
  if( !myModule ) {
    // Error!
    PyErr_Print();
    return;
  }
}

/*!
  \brief Set study workspace to the Python module.

  Calls setWorkSpace() method of the Pythohn module with 
  PyQt QWidget object to use with interpreter.

  Attention! initInterp() and importModule() should be called first!!!
*/
void SALOME_PYQT_ModuleLight::setWorkSpace()
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::setWorkSpace()" );

  // check if the subinterpreter is initialized and Python module is imported
  if ( !myInterp || !myModule ) {
    // Error! Python subinterpreter should be initialized and module should be imported first!
    return;
  }

  // call setWorkspace() method
  // ... first get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();

  // ... then try to import SalomePyQt module. If it's not possible don't go on.
  PyObjWrapper aQtModule( PyImport_ImportModule( "SalomePyQt" ) );
  if( !aQtModule ) {
    // Error!
    PyErr_Print();
    return;
  }

  if ( IsCallOldMethods ) {
    // ... then get workspace object
    QWidget* aWorkspace = 0;
    if ( getApp()->desktop()->inherits( "STD_MDIDesktop" ) ) {
      STD_MDIDesktop* aDesktop = dynamic_cast<STD_MDIDesktop*>( getApp()->desktop() );
      if ( aDesktop )
        aWorkspace = aDesktop->workspace();
    }
    else if ( getApp()->desktop()->inherits( "STD_TabDesktop" ) ) {
      STD_TabDesktop* aDesktop = dynamic_cast<STD_TabDesktop*>( getApp()->desktop() );
      if ( aDesktop )
        aWorkspace = aDesktop->workstack();
    }
#if SIP_VERSION < 0x040800
    PyObjWrapper pyws( sipBuildResult( 0, "M", aWorkspace, sipClass_QWidget) );
#else
    PyObjWrapper pyws( sipBuildResult( 0, "D", aWorkspace, sipType_QWidget , NULL) );
#endif
    // ... and finally call Python module's setWorkspace() method (obsolete)
    if ( PyObject_HasAttrString( myModule, (char*)"setWorkSpace" ) ) {
      PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"setWorkSpace", (char*)"O", pyws.get() ) );
      if( !res ) {
        PyErr_Print();
      }
    }
  }
}

/*!
  \brief Preference changing callback function (internal).

  Performs the following actions:
  - call Python module's preferenceChanged() method

  \param section setting section name
  \param setting setting name
*/
void SALOME_PYQT_ModuleLight::prefChanged( const QString& section, const QString& setting )
{
  FuncMsg fmsg( "SALOME_PYQT_ModuleLight::prefChanged()" );

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  if ( PyObject_HasAttrString( myModule, (char*)"preferenceChanged" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myModule,
                                           (char*)"preferenceChanged", 
                                           (char*)"ss", 
                                           section.toLatin1().constData(), 
                                           setting.toLatin1().constData() ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
  \brief Get default menu group identifier
  \return menu group ID (40 by default)
*/
int SALOME_PYQT_ModuleLight::defaultMenuGroup()
{
  return DEFAULT_GROUP; 
}

//
// The next methods call the parent implementation.
// This is done to open protected methods from CAM_Module class.
//

/*!
  \brief Create toolbar with specified \a name.
  \param name toolbar name
  \return toolbar ID or -1 if toolbar creation is failed
*/
int SALOME_PYQT_ModuleLight::createTool( const QString& name )
{
  return LightApp_Module::createTool( name );
}

/*! 
  \brief Insert action with specified \a id to the toolbar.
  \param id action ID
  \param tBar toolbar ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createTool( const int id, const int tBar, const int idx )
{
  return LightApp_Module::createTool( id, tBar, idx );
}

/*!
  \brief Insert action with specified \a id to the toolbar.
  \param id action ID
  \param tBar toolbar name
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createTool( const int id, const QString& tBar, const int idx )
{
  return LightApp_Module::createTool( id, tBar, idx );
}

/*!
  \brief Insert action to the toolbar.
  \param a action
  \param tBar toolbar ID
  \param id required action ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  return LightApp_Module::createTool( a, tBar, id, idx );
}

/*!
  \brief Insert action to the toolbar.
  \param a action
  \param tBar toolbar name
  \param id required action ID
  \param idx required index in the toolbar
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  return LightApp_Module::createTool( a, tBar, id, idx );
}

/*!
  \brief Create main menu.
  \param subMenu menu name
  \param menu parent menu ID
  \param id required menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return menu ID or -1 if menu could not be added
*/
int SALOME_PYQT_ModuleLight::createMenu( const QString& subMenu, const int menu, const int id, const int group, const int idx )
{
  return LightApp_Module::createMenu( subMenu, menu, id, group, idx );
}

/*!
  \brief Create main menu.
  \param subMenu menu name
  \param menu parent menu name (list of menu names separated by "|")
  \param id required menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return menu ID or -1 if menu could not be added
*/
int SALOME_PYQT_ModuleLight::createMenu( const QString& subMenu, const QString& menu, const int id, const int group, const int idx )
{
  return LightApp_Module::createMenu( subMenu, menu, id, group, idx );
}

/*!
  \brief Insert action to the main menu.
  \param id action ID
  \param menu parent menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createMenu( const int id, const int menu, const int group, const int idx )
{
  return LightApp_Module::createMenu( id, menu, group, idx );
}

/*!
  \brief Insert action to the main menu.
  \param id action ID
  \param menu parent menu name (list of menu names separated by "|")
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createMenu( const int id, const QString& menu, const int group, const int idx )
{
  return LightApp_Module::createMenu( id, menu, group, idx );
}

/*!
  \brief Insert action to the main menu.
  \param a action
  \param menu parent menu ID
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createMenu( QAction* a, const int menu, const int id, const int group, const int idx )
{
  return LightApp_Module::createMenu( a, menu, id, group, idx );
}

/*!
  \brief Insert action to the main menu.
  \param a action
  \param menu parent menu name (list of menu names separated by "|")
  \param group menu group ID
  \param idx required index in the menu
  \return action ID or -1 if action could not be added
*/
int SALOME_PYQT_ModuleLight::createMenu( QAction* a, const QString& menu, const int id, const int group, const int idx )
{
  return LightApp_Module::createMenu( a, menu, id, group, idx );
}

/*!
  \brief Create separator action which can be used in the menu or toolbar.
  \return new separator action
*/
QAction* SALOME_PYQT_ModuleLight::separator()
{
  return LightApp_Module::separator();
}

/*!
  \brief Get action by specified \a id.
  \return action or 0 if it is not found
*/
QAction* SALOME_PYQT_ModuleLight::action( const int id ) const
{
  QAction* a = LightApp_Module::action( id );
  if ( !a ) {
    // try menu
    QMenu* m = menuMgr()->findMenu( id );
    if ( m ) a = m->menuAction();
  }
  return a;
}

/*!
  \brief Get action identifier.
  \return action ID or -1 if action is not registered
*/
int SALOME_PYQT_ModuleLight::actionId( const QAction* a ) const
{
  return LightApp_Module::actionId( a );
}

/*!
  \brief Create new action.
  
  If the action with specified identifier already registered
  it is not created, but its attributes are only modified.

  \param id action ID
  \param text tooltip text
  \param icon icon
  \param menu menu text
  \param tip status tip
  \param key keyboard shortcut
  \param toggle \c true for checkable action
  \return created action
*/
QAction* SALOME_PYQT_ModuleLight::createAction( const int id, const QString& text, const QString& icon,
                                           const QString& menu, const QString& tip, const int key,
                                           const bool toggle, QObject* parent )
{
  QIcon anIcon = loadIcon( icon );
  QAction* a = action( id );
  if ( a ) {
    if ( a->toolTip().isEmpty()   && !text.isEmpty() )  a->setToolTip( text );
    if ( a->text().isEmpty()      && !menu.isEmpty() )  a->setText( menu );
    if ( a->icon().isNull()       && !anIcon.isNull() ) a->setIcon( anIcon );
    if ( a->statusTip().isEmpty() && !tip.isEmpty() )   a->setStatusTip( tip );
    if ( a->shortcut().isEmpty()  && key )              a->setShortcut( key );
    if ( a->isCheckable() != toggle )                   a->setCheckable( toggle );
    disconnect( a, SIGNAL( triggered( bool ) ), this, SLOT( onGUIEvent() ) );
    connect(    a, SIGNAL( triggered( bool ) ), this, SLOT( onGUIEvent() ) );
  }
  else {
    a = LightApp_Module::createAction( id, 
                                        text, 
                                        anIcon, 
                                        menu, 
                                        tip, 
                                        key, 
                                        parent ? parent : this, 
                                        toggle, 
                                        this, 
                                        SLOT( onGUIEvent() ) );
  }
  return a;
}

/*!
  \brief Create new action group.
  
  If the action with specified identifier already registered
  it is not created, but its attributes are only modified.

  \param id action ID
  \param text tooltip text
  \param icon icon
  \param menu menu text
  \param tip status tip
  \param key keyboard shortcut
  \param toggle \c true for checkable action
  \return created action
*/
QtxActionGroup* SALOME_PYQT_ModuleLight::createActionGroup(const int id, const bool exclusive)
{
  QtxActionGroup* a = qobject_cast<QtxActionGroup*>( action( id ) );
  if ( !a ) {
    a = new QtxActionGroup( this );
    LightApp_Module::registerAction( id, a );
  }
  a->setExclusive( exclusive );
  return a;
}

/*! 
  \brief Load icon from resource file.
  \param fileName icon file name
  \return icon (null icon if loading failed)
*/
QIcon SALOME_PYQT_ModuleLight::loadIcon( const QString& fileName )
{
  QIcon anIcon;
  if ( !fileName.isEmpty() ) {
    QPixmap pixmap = getApp()->resourceMgr()->loadPixmap( name(), tr( fileName.toLatin1() ) );
    if ( !pixmap.isNull() )
      anIcon = QIcon( pixmap );
  }
  return anIcon;
}

/*!
  \brief Add global application preference (for example, 
  application specific section).
  \param label preference name
  \return preference ID
*/
int SALOME_PYQT_ModuleLight::addGlobalPreference( const QString& label )
{
  LightApp_Preferences* pref = preferences();
  if ( !pref )
    return -1;

  return pref->addPreference( label, -1 );
}

/*!
  \brief Add preference.
  \param label preference name
  \return preference ID
*/
int SALOME_PYQT_ModuleLight::addPreference( const QString& label )
{
  return LightApp_Module::addPreference( label );
}
                                       
/*!
  \brief Add preference.
  \param label preference name
  \param pId parent preference ID
  \param type preference type
  \param section resource file section name
  \param param resource file setting name
  \return preference ID
*/
int SALOME_PYQT_ModuleLight::addPreference( const QString& label, 
                                       const int pId, const int type,
                                       const QString& section,
                                       const QString& param )
{
  return LightApp_Module::addPreference( label, pId, type, section, param );
}

/*!
  \brief Get the preference property.
  \param id preference ID
  \param prop property name
  \return property value (invalid QVariant() if property is not found)
*/
QVariant SALOME_PYQT_ModuleLight::preferenceProperty( const int id, 
                                                 const QString& prop ) const
{
  QVariant v = LightApp_Module::preferenceProperty( id, prop );
  return v;
}

/*!
  \brief Set the preference property.
  \param id preference ID
  \param prop property name
  \param var property value
*/
void SALOME_PYQT_ModuleLight::setPreferenceProperty( const int id, 
                                                const QString& prop, 
                                                const QVariant& var )
{
  LightApp_Module::setPreferenceProperty( id, prop, var );
}


/*!
  \brief Signal handler windowActivated(SUIT_ViewWindow*) of SUIT_Desktop
  \param pview view being activated
*/
void SALOME_PYQT_ModuleLight::onActiveViewChanged( SUIT_ViewWindow* pview )
{
  class ActiveViewChange : public PyInterp_LockRequest
  {
  public:
    ActiveViewChange( PyInterp_Interp* _py_interp, SALOME_PYQT_ModuleLight* _obj, const SUIT_ViewWindow* _pview )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myObj(_obj),myView(_pview) {}

  protected:
    virtual void execute()
    {
      myObj->activeViewChanged( myView );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    const SUIT_ViewWindow * myView;
  };
  
  PyInterp_Dispatcher::Get()->Exec( new ActiveViewChange( myInterp, this, pview ) ); 
}

/*!
  \brief Processes the view changing, calls Python module's activeViewChanged() method 
  \param pview view being activated
*/
void SALOME_PYQT_ModuleLight::activeViewChanged( const SUIT_ViewWindow* pview )
{
  if ( !myInterp || !myModule ) 
    return;
  
  // Do not use SUIT_ViewWindow::closing() signal here. View manager reacts on 
  // this signal and deletes view. So our slot does not works if it is connected 
  // on this signal. SUIT_ViewManager::deleteView(SUIT_ViewWindow*) is used here
  
  connectView( pview );

  if ( PyObject_HasAttrString( myModule, (char*)"activeViewChanged" ) ) 
  {
    if ( !pview ) 
      return;   

    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"activeViewChanged", (char*)"i" , pview->getId() ) );
    if( !res )
      PyErr_Print();
  }
}

/*!
  \brief Signal handler cloneView() of OCCViewer_ViewWindow
  \param pview view being cloned
*/
void SALOME_PYQT_ModuleLight::onViewCloned( SUIT_ViewWindow* pview )
{
  class ViewClone : public PyInterp_LockRequest
  {
  public:
    ViewClone( PyInterp_Interp* _py_interp, SALOME_PYQT_ModuleLight* _obj, const SUIT_ViewWindow* _pview )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myObj(_obj), myView(_pview) {}

  protected:
    virtual void execute()
    {
      myObj->viewCloned( myView );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;    
    const SUIT_ViewWindow* myView;
  };
  
  PyInterp_Dispatcher::Get()->Exec( new ViewClone( myInterp, this, pview ) );
}

/*!
  \brief Processes the view cloning, calls Python module's activeViewCloned() method
  \param pview view being cloned
*/
void SALOME_PYQT_ModuleLight::viewCloned( const SUIT_ViewWindow* pview )
{
  if ( !myInterp || !myModule || !pview ) 
    return;  

  if ( PyObject_HasAttrString( myModule, (char*)"viewCloned" ) ) 
  {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"viewCloned", (char*)"i", pview->getId() ) );
    if( !res )
      PyErr_Print();
  }
}

/*!
  \brief Signal handler tryClose(SUIT_ViewWindow*) of a view
  \param pview view being closed
*/
void SALOME_PYQT_ModuleLight::onViewTryClose( SUIT_ViewWindow* pview )
{
  class ViewTryClose : public PyInterp_LockRequest
  {
  public:
    ViewTryClose( PyInterp_Interp* _py_interp, SALOME_PYQT_ModuleLight* _obj, const SUIT_ViewWindow* _pview )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myObj(_obj),myView(_pview) {}

  protected:
    virtual void execute()
    {
      myObj->viewTryClose( myView );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    const SUIT_ViewWindow * myView;    
  };

  PyInterp_Dispatcher::Get()->Exec( new ViewTryClose( myInterp, this, pview ) );
}

/*!
  \brief Processes the view closing attempt, calls Python module's viewTryClose() method
  \param pview view user tries to close
*/
void SALOME_PYQT_ModuleLight::viewTryClose( const SUIT_ViewWindow* pview )
{
  if ( !myInterp || !myModule ) 
    return;  

  if ( PyObject_HasAttrString( myModule, (char*)"viewTryClose" ) ) 
  {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"viewTryClose", (char*)"i", pview->getId() ) );
    if ( !res )
    {
      PyErr_Print();
    }
  }
}

/*!
  \brief Signal handler closing(SUIT_ViewWindow*) of a view
  \param pview view being closed
*/
void SALOME_PYQT_ModuleLight::onViewClosed( SUIT_ViewWindow* pview )
{
  class ViewClose : public PyInterp_LockRequest
  {
  public:
    ViewClose( PyInterp_Interp* _py_interp, SALOME_PYQT_ModuleLight* _obj, const SUIT_ViewWindow* _pview )
      : PyInterp_LockRequest( _py_interp, 0, true ),
        myObj(_obj),myView(_pview) {}

  protected:
    virtual void execute()
    {
      myObj->viewClosed( myView );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    const SUIT_ViewWindow * myView;    
  };

  PyInterp_Dispatcher::Get()->Exec( new ViewClose( myInterp, this, pview ) );
}

/*!
  \brief Processes the view closing, calls Python module's viewClosed() method
  \param pview view being closed
*/
void SALOME_PYQT_ModuleLight::viewClosed( const SUIT_ViewWindow* pview )
{
  if ( !myInterp || !myModule ) 
    return;  

  if ( PyObject_HasAttrString( myModule, (char*)"viewClosed" ) ) 
  {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"viewClosed", (char*)"i", pview->getId() ) );
    if ( !res )
    {
      PyErr_Print();
    }
  }
}

/*!
  \brief Connects or disconnects signals about activating and cloning view on the module slots
  \param pview view which is connected/disconnected
*/
void SALOME_PYQT_ModuleLight::connectView( const SUIT_ViewWindow* pview )
{
  SUIT_ViewManager* viewMgr = pview->getViewManager();
  SUIT_ViewModel* viewModel = viewMgr ? viewMgr->getViewModel() : 0;
      
  if ( viewMgr )
  {
    disconnect( viewMgr, SIGNAL( tryCloseView( SUIT_ViewWindow* ) ),
		this, SLOT( onViewTryClose( SUIT_ViewWindow* ) ) );
    disconnect( viewMgr, SIGNAL( deleteView( SUIT_ViewWindow* ) ),
		this, SLOT( onViewClosed( SUIT_ViewWindow* ) ) );
  
    connect( viewMgr, SIGNAL( tryCloseView( SUIT_ViewWindow* ) ),
	     this, SLOT( onViewTryClose( SUIT_ViewWindow* ) ) );
    connect( viewMgr, SIGNAL( deleteView( SUIT_ViewWindow* ) ),
             this, SLOT( onViewClosed( SUIT_ViewWindow* ) ) );
  }
  
  // Connect cloneView() signal of an OCC View
  if ( pview->inherits( "OCCViewer_ViewWindow" ) )
  {
    disconnect( pview, SIGNAL( viewCloned( SUIT_ViewWindow* ) ), 
                this, SLOT( onViewCloned( SUIT_ViewWindow* ) ) );
    connect( pview, SIGNAL( viewCloned( SUIT_ViewWindow* ) ), 
             this, SLOT( onViewCloned( SUIT_ViewWindow* ) ) );
  }
  // Connect cloneView() signal of Plot2d View manager
  else if ( viewModel && viewModel->inherits( "Plot2d_Viewer" ) )
  {
    disconnect( viewModel, SIGNAL( viewCloned( SUIT_ViewWindow* ) ), 
                this, SLOT( onViewCloned( SUIT_ViewWindow* ) ) );
    connect( viewModel, SIGNAL( viewCloned( SUIT_ViewWindow* ) ), 
             this, SLOT( onViewCloned( SUIT_ViewWindow* ) ) );
  }
}

/*!
  \brief Get tag name for the DOM element.
  \param element DOM element
  \return empty string if the element does not have tag name
  \internal
*/
static QString tagName( const QDomElement& element )
{
  return element.tagName().trimmed();
}

/*!
  \brief Get DOM element's attribute by its name.
  \param element DOM element
  \param attName attribute name
  \return empty string if the element does not have such attribute
  \internal
*/
static QString attribute( const QDomElement& element, const QString& attName )
{
  return element.attribute( attName ).trimmed();
}

/*!
  \brief Inspect specified string for the boolean value.
  
  This function returns \c true if string represents boolean value: 
  - "true", "yes" or "1" for \c true
  - "false", "no" or "0" for \c false
  Second parameter allows to specify what boolean value is expected:
  - 1: \c true
  - 0: \c false
  - other value is not taken into account (return represented value)

  \param value inspected string
  \param check expected boolean value
  \return boolean value represented by the string (\a check is not 1 or 0)
          or \c true if value correspond to the specified \a check
*/
static bool checkBool( const QString& value, const int check = -1 )
{
  QString v = value.toLower();
  if ( ( v == "true"  || v == "yes" || v == "1" ) && ( check != 0 ) )
    return true;
  if ( ( v == "false" || v == "no"  || v == "0" ) && ( check == 0 ) )
    return true;
  return false;
}

/*!
  \brief Inspect specified string for the integer value.
  
  This function returns returns -1 if item is empty or represents
  an invalid number.
  \param value inspected string
  \param def default value
  \param shift shift value (it is added to the integer value to produce shifted result)
*/
static int checkInt( const QString& value, const int def = -1, const int shift = -1 )
{
  bool bOk;
  int val = value.toInt( &bOk );
  if ( !bOk ) val = def;
  if ( shift > 0 && bOk && val < 0 )
    val += shift;
  return val;
}

/*!
  \brief Constructor
  \internal
  \param module parent module pointer
  \param fileName XML file path
*/
SALOME_PYQT_ModuleLight::XmlHandler::XmlHandler( SALOME_PYQT_ModuleLight* module, 
                                            const QString&      fileName )
: myModule( module )
{
  if ( fileName.isEmpty() ) 
    return;
  QFile aFile( fileName );
  if ( !aFile.open( QIODevice::ReadOnly ) )
    return;
  myDoc.setContent( &aFile );
  aFile.close();
}

/*!
  \brief Parse XML file and create actions.
  \internal
  
  Called by SALOME_PYQT_ModuleLight::activate() in order to create actions
  (menus, toolbars).
*/
void SALOME_PYQT_ModuleLight::XmlHandler::createActions()
{
  // get document element
  QDomElement aDocElem = myDoc.documentElement();

  // create main menu actions
  QDomNodeList aMenuList = aDocElem.elementsByTagName( "menu-item" );
  for ( int i = 0; i < aMenuList.count(); i++ ) {
    QDomNode n = aMenuList.item( i );
    createMenu( n );
  }

  // create toolbars actions
  QDomNodeList aToolsList = aDocElem.elementsByTagName( "toolbar" );
  for ( int i = 0; i < aToolsList.count(); i++ ) {
    QDomNode n = aToolsList.item( i );
    createToolBar( n );
  }
}

/*!
  \brief Create popup menu.
  \internal
  \param menu popup menu
  \param context popup menu context
  \param context popup menu parent object name
  \param context popup menu object name
*/
void SALOME_PYQT_ModuleLight::XmlHandler::createPopup( QMenu*         menu,
                                                  const QString& context,
                                                  const QString& parent,
                                                  const QString& object )
{
  // get document element
  QDomElement aDocElem = myDoc.documentElement();

  // get popup menus actions
  QDomNodeList aPopupList = aDocElem.elementsByTagName( "popupmenu" );
  for ( int i = 0; i < aPopupList.count(); i++ ) {
    QDomNode n = aPopupList.item( i );
    if ( !n.isNull() && n.isElement() ) {
      QDomElement e = n.toElement();
      // QString lab = attribute( e, "label-id" ); // not used // 
      QString ctx = attribute( e, "context-id" );
      QString prt = attribute( e, "parent-id"  );
      QString obj = attribute( e, "object-id"  );
      if ( ctx == context && prt == parent && obj == object )  {
        insertPopupItems( n, menu );
        break;
      }
    }
  }
}

/*!
  \brief Activate menus
  \internal
  \param enable if \c true menus are activated, otherwise menus are deactivated
*/
void SALOME_PYQT_ModuleLight::XmlHandler::activateMenus( bool enable )
{
  if ( !myModule )
    return;

  QtxActionMenuMgr* mgr = myModule->menuMgr();
  int id;
  foreach( id, myMenuItems ) mgr->setEmptyEnabled( id, enable );
}

/*!
  \brief Create main menu item and insert actions to it.
  \internal
  \param parentNode XML node with menu description
  \param parentMenuId parent menu ID (-1 for top-level menu)
  \param parentPopup parent popup menu (0 for top-level menu)
*/
void SALOME_PYQT_ModuleLight::XmlHandler::createMenu( QDomNode& parentNode, 
                                                 const int parentMenuId, 
                                                 QMenu*    parentPopup )
{
  if ( !myModule || parentNode.isNull() )
    return;
  
  QDomElement parentElement = parentNode.toElement();
  if ( !parentElement.isNull() ) {
    QString plabel = attribute( parentElement, "label-id" );
    int     pid    = checkInt( attribute( parentElement, "item-id" ) );
    int     ppos   = checkInt( attribute( parentElement, "pos-id" ) );
    int     group  = checkInt( attribute( parentElement, "group-id" ), 
                               myModule->defaultMenuGroup() );
    if ( !plabel.isEmpty() ) {
      QMenu* popup = 0;
      int menuId = -1;
      // create menu
      menuId = myModule->createMenu( plabel,         // label
                                     parentMenuId,   // parent menu ID, -1 for top-level menu
                                     pid,            // ID
                                     group,          // group ID
                                     ppos );         // position
      myMenuItems.append( menuId );
      QDomNode node = parentNode.firstChild();
      while ( !node.isNull() ) {
        if ( node.isElement() ) {
          QDomElement elem = node.toElement();
          QString aTagName = tagName( elem );
          if ( aTagName == "popup-item" ) {
            int     id      = checkInt( attribute( elem, "item-id" ) );
            int     pos     = checkInt( attribute( elem, "pos-id" ) );
            int     group   = checkInt( attribute( elem, "group-id" ), 
                                        myModule->defaultMenuGroup() );
            QString label   = attribute( elem, "label-id" );
            QString icon    = attribute( elem, "icon-id" );
            QString tooltip = attribute( elem, "tooltip-id" );
            QString accel   = attribute( elem, "accel-id" );
            bool    toggle  = checkBool( attribute( elem, "toggle-id" ) );

            // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
            // also check if the action with given ID is already created
            if ( id != -1 ) {
              // create menu action
              QAction* action = myModule->createAction( id,                     // ID
                                                        tooltip,                // tooltip
                                                        icon,                   // icon
                                                        label,                  // menu text
                                                        tooltip,                // status-bar text
                                                        QKeySequence( accel ),  // keyboard accelerator
                                                        toggle );               // toogled action
              myModule->createMenu( action,   // action
                                    menuId,   // parent menu ID
                                    id,       // ID (same as for createAction())
                                    group,    // group ID
                                    pos );    // position
            }
          }
          else if ( aTagName == "submenu" ) {
            // create sub-menu
            createMenu( node, menuId, popup );
          }
          else if ( aTagName == "separator" ) {
            // create menu separator
            int id    = checkInt( attribute( elem, "item-id" ) ); // separator can have ID
            int pos   = checkInt( attribute( elem, "pos-id" ) );
            int group = checkInt( attribute( elem, "group-id" ), 
                                  myModule->defaultMenuGroup() );
            QAction* action = myModule->separator();
            myModule->createMenu( action,  // separator action
                                  menuId,  // parent menu ID
                                  id,      // ID
                                  group,   // group ID
                                  pos );   // position
          }
        }
        node = node.nextSibling();
      }
    }
  }
}

/*!
  \brief Create a toolbar and insert actions to it.
  \param parentNode XML node with toolbar description
*/
void SALOME_PYQT_ModuleLight::XmlHandler::createToolBar( QDomNode& parentNode )
{
  if ( !myModule || parentNode.isNull() )
    return;

  QDomElement parentElement = parentNode.toElement();
  if ( !parentElement.isNull() ) {
    QString aLabel = attribute( parentElement, "label-id" );
    if ( !aLabel.isEmpty() ) {
      // create toolbar
      int tbId = myModule->createTool( aLabel );
      QDomNode node = parentNode.firstChild();
      while ( !node.isNull() ) {
        if ( node.isElement() ) {
          QDomElement elem = node.toElement();
          QString aTagName = tagName( elem );
          if ( aTagName == "toolbutton-item" ) {
            int     id      = checkInt( attribute( elem, "item-id" ) );
            int     pos     = checkInt( attribute( elem, "pos-id" ) );
            QString label   = attribute( elem, "label-id" );
            QString icon    = attribute( elem, "icon-id" );
            QString tooltip = attribute( elem, "tooltip-id" );
            QString accel   = attribute( elem, "accel-id" );
            bool    toggle  = checkBool( attribute( elem, "toggle-id" ) );

            // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
            // also check if the action with given ID is already created
            if ( id != -1 ) {
              // create toolbar action
              QAction* action = myModule->createAction( id,                    // ID
                                                        tooltip,               // tooltip
                                                        icon,                  // icon
                                                        label,                 // menu text
                                                        tooltip,               // status-bar text
                                                        QKeySequence( accel ), // keyboard accelerator
                                                        toggle );              // toogled action
              myModule->createTool( action, tbId, -1, pos );
            }
          }
          else if ( aTagName == "separatorTB" || aTagName == "separator" ) {
            // create toolbar separator
            int pos = checkInt( attribute( elem, "pos-id" ) );
            QAction* action = myModule->separator();
            myModule->createTool( action, tbId, -1, pos );
          }
        }
        node = node.nextSibling();
      }
    }
  }
}

/*!
  \brief Fill popup menu with the items.
  \param parentNode XML node with popup menu description
  \param menu popup menu
*/
void SALOME_PYQT_ModuleLight::XmlHandler::insertPopupItems( QDomNode& parentNode, QMenu* menu )
{
  if ( !myModule && parentNode.isNull() )
    return;

  // we create popup menus without help of QtxPopupMgr
  QDomNode node = parentNode.firstChild();
  while ( !node.isNull() ) { 
    if ( node.isElement() ) {
      QDomElement elem = node.toElement();
      QString aTagName = tagName( elem );
      QList<QAction*> actions = menu->actions();
      if ( aTagName == "popup-item" ) {
        // insert a command item
        int     id      = checkInt( attribute( elem, "item-id" ) );
        int     pos     = checkInt( attribute( elem, "pos-id" ) );
        QString label   = attribute( elem, "label-id" );
        QString icon    = attribute( elem, "icon-id" );
        QString tooltip = attribute( elem, "tooltip-id" );
        QString accel   = attribute( elem, "accel-id" );
        bool    toggle  = checkBool( attribute( elem, "toggle-id" ) );

        // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
        // also check if the action with given ID is already created
        if ( id != -1 ) {
          QAction* action = myModule->createAction( id,                     // ID
                                                    tooltip,                // tooltip
                                                    icon,                   // icon
                                                    label,                  // menu text
                                                    tooltip,                // status-bar text
                                                    QKeySequence( accel ),  // keyboard accelerator
                                                    toggle );               // toogled action
          QAction* before = ( pos >= 0 && pos < actions.count() ) ? actions[ pos ] : 0;
          menu->insertAction( before, action );
        }
      }
      else if ( aTagName == "submenu" ) {
        // create sub-menu
        ////int     id    = checkInt( attribute( elem, "item-id" ) ); // not used //
        int     pos   = checkInt( attribute( elem, "pos-id" ) );
        QString label = attribute( elem, "label-id" );
        QString icon  = attribute( elem, "icon-id" );

        QIcon anIcon;
        if ( !icon.isEmpty() ) {
          QPixmap pixmap  = myModule->getApp()->resourceMgr()->loadPixmap( myModule->name(), icon );
          if ( !pixmap.isNull() )
            anIcon = QIcon( pixmap );
        }

        QMenu* newPopup = menu->addMenu( anIcon, label );
        QAction* before = ( pos >= 0 && pos < actions.count() ) ? actions[ pos ] : 0;
        menu->insertMenu( before, newPopup );
        insertPopupItems( node, newPopup );
      }
      else if ( aTagName == "separator" ) {
        // create menu separator
        int pos = checkInt( attribute( elem, "pos-id" ) );
        QAction* action = myModule->separator();
        QAction* before = ( pos >= 0 && pos < actions.count() ) ? actions[ pos ] : 0;
        menu->insertAction( before, action );
      }
    }
    node = node.nextSibling();
  }
}

/*
 * Save study request.
 * Called when user save study.
 */
void SALOME_PYQT_ModuleLight::save(QStringList& theListOfFiles)
{
  MESSAGE("SALOME_PYQT_ModuleLight::save()")
  // perform synchronous request to Python event dispatcher
  class SaveEvent: public PyInterp_LockRequest
  {
  public:     
    SaveEvent(PyInterp_Interp*           _py_interp,
              SALOME_PYQT_ModuleLight*       _obj,
              QStringList&                   _files_list)
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj( _obj ) ,
        myFilesList(_files_list) {}
  protected:
    virtual void execute()
    {
      myObj->saveEvent(myFilesList);
    }
  private:
    SALOME_PYQT_ModuleLight* myObj;
    QStringList&        myFilesList;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new SaveEvent( myInterp, this, theListOfFiles ) );
}

void SALOME_PYQT_ModuleLight::saveEvent(QStringList& theListOfFiles)
{
  MESSAGE("SALOME_PYQT_ModuleLight::saveEvent()");
  QStringList::Iterator it = theListOfFiles.begin();
  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule || (it == theListOfFiles.end()))
    return;

  if ( PyObject_HasAttrString(myModule, (char*)"saveFiles") ) {
    // temporary set myInitModule because saveEvent() method
    // might be called by the framework when this module is inactive,
    // but still it should be possible to access this module's data
    // from Python
    myInitModule = this;

    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"saveFiles",
                                           (char*)"s", (*it).toLatin1().constData()));

    myInitModule = 0;

    if( !res ) {
      PyErr_Print();
    }
    else{
      // parse the return value
      // result can be one string...
      if ( PyString_Check( res ) ) {
        QString astr = PyString_AsString( res );
        //SCRUTE(astr);
        theListOfFiles.append(astr);
      }
      //also result can be a list...
      else if ( PyList_Check( res ) ) {
        int size = PyList_Size( res );
        for ( int i = 0; i < size; i++ ) {
          PyObject* value = PyList_GetItem( res, i );
          if( value && PyString_Check( value ) ) {
            theListOfFiles.append( PyString_AsString( value ) );
          }
        }
      }
    }
  }
}

/*
 * Python dump request.
 * Called when user activates dump study operation.
 */
void SALOME_PYQT_ModuleLight::dumpPython(QStringList& theListOfFiles)
{
  MESSAGE("SALOME_PYQT_ModuleLight::dumpPython()")
  // perform synchronous request to Python event dispatcher
  class DumpEvent: public PyInterp_LockRequest
  {
  public:     
    DumpEvent(PyInterp_Interp*          _py_interp,
              SALOME_PYQT_ModuleLight*  _obj,
              QStringList&              _files_list)
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj( _obj ) ,
        myFilesList(_files_list) {}
  protected:
    virtual void execute()
    {
      myObj->dumpEvent(myFilesList);
    }
  private:
    SALOME_PYQT_ModuleLight* myObj;
    QStringList&             myFilesList;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new DumpEvent( myInterp, this, theListOfFiles ) );
}

void SALOME_PYQT_ModuleLight::dumpEvent(QStringList& theListOfFiles)
{
  MESSAGE("SALOME_PYQT_ModuleLight::dumpEvent()");
  QStringList::Iterator it = theListOfFiles.begin();
  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule || (it == theListOfFiles.end()))
    return;

  if ( PyObject_HasAttrString(myModule, (char*)"dumpStudy") ) {
    // temporary set myInitModule because dumpEvent() method
    // might be called by the framework when this module is inactive,
    // but still it should be possible to access this module's data
    // from Python
    myInitModule = this;

    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"dumpStudy",
                                           (char*)"s", (*it).toLatin1().constData()));

    myInitModule = 0;

    if( !res ) {
      PyErr_Print();
    }
    else{
      // parse the return value
      // result can be one string...
      if ( PyString_Check( res ) ) {
        QString astr = PyString_AsString( res );
        //SCRUTE(astr);
        theListOfFiles.append(astr);
      }
      //also result can be a list...
      else if ( PyList_Check( res ) ) {
        int size = PyList_Size( res );
        for ( int i = 0; i < size; i++ ) {
          PyObject* value = PyList_GetItem( res, i );
          if( value && PyString_Check( value ) ) {
            theListOfFiles.append( PyString_AsString( value ) );
          }
        }
      }
    }
  }
}

/*
 * Open study request.
 * Called when user open study.
 */
bool SALOME_PYQT_ModuleLight::open(QStringList theListOfFiles)
{
  MESSAGE("SALOME_PYQT_ModuleLight::open()");
  // perform synchronous request to Python event dispatcher
  bool opened = false;
  class OpenEvent: public PyInterp_LockRequest
  {
  public:
    OpenEvent(PyInterp_Interp*              _py_interp,
              SALOME_PYQT_ModuleLight*      _obj,
              QStringList                   _files_list,
              bool&                         _opened)
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj( _obj ) ,
        myFilesList(_files_list),
        myOpened(_opened) {}
  protected:
    virtual void execute()
    {
      myObj->openEvent(myFilesList,myOpened);
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    QStringList        myFilesList;
    bool& myOpened;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new OpenEvent( myInterp, this, theListOfFiles, opened) );
  return opened;
}


void SALOME_PYQT_ModuleLight::openEvent(QStringList theListOfFiles, bool &opened)
{
  MESSAGE("SALOME_PYQT_ModuleLight::openEvent()");
  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule || theListOfFiles.isEmpty())
    return;
  QStringList* theList = new QStringList(theListOfFiles);

#if SIP_VERSION < 0x040800
  PyObjWrapper sipList( sipBuildResult( 0, "M", theList, sipClass_QStringList) );
#else
  PyObjWrapper sipList( sipBuildResult( 0, "D", theList, sipType_QStringList, NULL ) );
#endif
  if ( PyObject_HasAttrString(myModule , (char*)"openFiles") ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"openFiles",
                                           (char*)"O", sipList.get()));
    if( !res || !PyBool_Check( res )) {
      PyErr_Print();
      opened = false;
    }
    else{
      opened = PyObject_IsTrue( res );
      
    }
  }
}

/*
 * Create new empty Data Object and return its entry
 */
QString SALOME_PYQT_ModuleLight::createObject(const QString& parent)
{
  SALOME_PYQT_DataObjectLight* obj=0;
  if(!parent.isEmpty())
  {
    SALOME_PYQT_DataObjectLight* parentObj = findObject(parent);
    if(parentObj)
    {
      obj = new SALOME_PYQT_DataObjectLight(parentObj);
    }
  }
  else
  {
      SALOME_PYQT_DataModelLight* dm =
        dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel());
      if(dm)
      {
        obj = new SALOME_PYQT_DataObjectLight(dm->getRoot());
      }
  }  
  if (obj)
    return obj->entry();
  else
    return QString::null;
}

/*
 * Create new Data Object with name, icon and tooltip
 * and return its entry
 */
QString SALOME_PYQT_ModuleLight::createObject(const QString& aname,
                                              const QString& iconname,
                                              const QString& tooltip,
                                              const QString& parent)
{
  QString entry = createObject(parent);
  SALOME_PYQT_DataObjectLight* obj = findObject(entry);

  if(obj)
  {
    obj->setName(aname);
    obj->setToolTip(tooltip);
    obj->setIcon(iconname);
    return obj->entry();
  }
  else
    return QString::null;
}

/*
 * Find object by entry
 */
SALOME_PYQT_DataObjectLight* SALOME_PYQT_ModuleLight::findObject(const QString& entry)
{
  SALOME_PYQT_DataObjectLight* obj = 0;
  SALOME_PYQT_DataModelLight* dm =
    dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel());
  if(!entry.isEmpty() && dm ){
    for ( SUIT_DataObjectIterator it( dm->getRoot(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) { 
      SALOME_PYQT_DataObjectLight* curentobj =
        dynamic_cast<SALOME_PYQT_DataObjectLight*>( it.current() );
      
      if(curentobj && curentobj->entry() == entry) {
        obj = curentobj;
        return obj;
      }
    }
  }
  return obj;
}

/*
 * Set Name for object
 */
void SALOME_PYQT_ModuleLight::setName(const QString& obj, const QString& name)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    dataObj->setName(name);
  }
}

/*
 * Set Icon for object
 */
void SALOME_PYQT_ModuleLight::setIcon(const QString& obj, const QString& iconname)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    dataObj->setIcon(iconname);
  }
}

/*
 * Return Name of object
 */
QString SALOME_PYQT_ModuleLight::getName(const QString& obj)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    return dataObj->name();
  }
  return QString::null;
}

/*
 * Return Tool Tip of object
 */
QString SALOME_PYQT_ModuleLight::getToolTip(const QString& obj)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    return dataObj->toolTip();
  }
  return QString::null;
}


/*
 * Set Tool Tip for object
 */
void SALOME_PYQT_ModuleLight::setToolTip(const QString& obj, const QString& tooltip)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    dataObj->setToolTip(tooltip);
  }
}

/*
 * Return color of object
 */
QColor SALOME_PYQT_ModuleLight::getColor(const QString& obj)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( obj );
  if( dataObj ) {
    return dataObj->color( SUIT_DataObject::Foreground );
  }
  return QColor();
}

/*
 * Set color for object
 */
void SALOME_PYQT_ModuleLight::setColor(const QString& obj, const QColor& color)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( obj );
  if( dataObj ) {
    dataObj->setColor( color );
  }
}

/*
 * Return entry of the referenced object (if any)
 */
QString SALOME_PYQT_ModuleLight::getReference(const QString& obj)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    return dataObj->refEntry();
  }
  return QString::null;
}


/*
 * Set entry of the referenced object
 */
void SALOME_PYQT_ModuleLight::setReference(const QString& obj, const QString& refEntry)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    dataObj->setRefEntry(refEntry);
  }
}

/*
 * Remove object by entry
 */
void SALOME_PYQT_ModuleLight::removeObject(const QString& obj)
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
  if(dataObj) {
    dataObj->parent()->removeChild(dataObj);
  }
}


/*
 * Remove chields from object 
 */
void SALOME_PYQT_ModuleLight::removeChild(const QString& obj)
{
  MESSAGE("SALOME_PYQT_ModuleLight::removeChild()");
  DataObjectList lst;
  if(!obj.isEmpty())
    {
    SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
    if(dataObj)
    {
      dataObj->children(lst);
      QListIterator<SUIT_DataObject*> it( lst );
      while( it.hasNext() )
      {
        SALOME_PYQT_DataObjectLight* sobj = dynamic_cast<SALOME_PYQT_DataObjectLight*>( it.next() );
        dataObj->removeChild(sobj);
      }
    }
  }
  else
  {
    SALOME_PYQT_DataModelLight* dm =
      dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel());
    if(dm)
    {
      dm->getRoot()->children(lst);
      QListIterator<SUIT_DataObject*> it( lst );
      while(it.hasNext() )
      {
        SALOME_PYQT_DataObjectLight* sobj = dynamic_cast<SALOME_PYQT_DataObjectLight*>( it.next() );
        dm->getRoot()->removeChild(sobj);
      }
    }
  }
}

QStringList SALOME_PYQT_ModuleLight::getChildren(const QString& obj, const bool rec)
{
  DataObjectList lst;
  QStringList entryList;
  if(!obj.isEmpty())
  {
    SALOME_PYQT_DataObjectLight* dataObj = findObject(obj);
    if(dataObj)
    {
      dataObj->children(lst,rec);
      QListIterator<SUIT_DataObject*> it( lst );
      while(it.hasNext())
      {
        SALOME_PYQT_DataObjectLight* sobj = dynamic_cast<SALOME_PYQT_DataObjectLight*>( it.next() );
        entryList.append(sobj->entry());
      }
    }
  }
  else
  {
    SALOME_PYQT_DataModelLight* dm =
      dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel());
    if(dm)
    {
      dm->getRoot()->children(lst);
      QListIterator<SUIT_DataObject*> it( lst );
      while( it.hasNext() )
      {
        SALOME_PYQT_DataObjectLight* sobj = dynamic_cast<SALOME_PYQT_DataObjectLight*>( it.next() );
        entryList.append(sobj->entry());
      }
    }
  }
  return entryList;
}

/*!
 * Create new instance of data model and return it.
 */
CAM_DataModel* SALOME_PYQT_ModuleLight::createDataModel()
{
  MESSAGE( "SALOME_PYQT_ModuleLight::createDataModel()" );
  return new SALOME_PYQT_DataModelLight(this);
}

/*!
 * Returns the Python module object currently loaded.
 */
PyObject* SALOME_PYQT_ModuleLight::getPythonModule()
{
  return myModule;
}

bool SALOME_PYQT_ModuleLight::isDraggable( const SUIT_DataObject* what ) const
{
  MESSAGE("SALOME_PYQT_ModuleLight::isDraggable()");
  // perform synchronous request to Python event dispatcher
  bool draggable = false;
  class IsDraggableEvent: public PyInterp_LockRequest
  {
  public:
    IsDraggableEvent(PyInterp_Interp*          _py_interp,
		     SALOME_PYQT_ModuleLight*  _obj,
		     LightApp_DataObject*      _data_object,
		     bool&                     _is_draggable )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj( _obj ) ,
        myDataObject( _data_object ),
        myIsDraggable( _is_draggable ) {}
  protected:
    virtual void execute()
    {
      myIsDraggable = myObj->isDraggableEvent( myDataObject );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    LightApp_DataObject*     myDataObject;
    bool&                    myIsDraggable;
  };

  const LightApp_DataObject* data_object = dynamic_cast<const LightApp_DataObject*>( what );
							      
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new IsDraggableEvent( myInterp,
							    const_cast<SALOME_PYQT_ModuleLight*>( this ),
							    const_cast<LightApp_DataObject*>( data_object ),
							    draggable ) );
  return draggable;
}

bool SALOME_PYQT_ModuleLight::isDraggableEvent( LightApp_DataObject* what )
{
  MESSAGE("SALOME_PYQT_ModuleLight::isDraggableEvent()");

  bool draggable = false;

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule || !what )
    return draggable;

  if ( PyObject_HasAttrString(myModule , (char*)"isDraggable") ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"isDraggable",
					   (char*)"s", what->entry().toLatin1().constData() ) );
    if( !res || !PyBool_Check( res )) {
      PyErr_Print();
      draggable = false;
    }
    else{
      draggable = PyObject_IsTrue( res );
    }
  }

  return draggable;
}

bool SALOME_PYQT_ModuleLight::isDropAccepted( const SUIT_DataObject* where ) const
{
  MESSAGE("SALOME_PYQT_ModuleLight::isDropAccepted()");
  // perform synchronous request to Python event dispatcher
  bool dropAccepted = false;
  class IsDropAcceptedEvent: public PyInterp_LockRequest
  {
  public:
    IsDropAcceptedEvent(PyInterp_Interp*          _py_interp,
			SALOME_PYQT_ModuleLight*  _obj,
			LightApp_DataObject*      _data_object,
			bool&                     _is_drop_accepted )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj( _obj ) ,
        myDataObject( _data_object ),
        myIsDropAccepted( _is_drop_accepted ) {}
  protected:
    virtual void execute()
    {
      myIsDropAccepted = myObj->isDropAcceptedEvent( myDataObject );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    LightApp_DataObject*     myDataObject;
    bool&                    myIsDropAccepted;
  };
  
  const LightApp_DataObject* data_object = dynamic_cast<const LightApp_DataObject*>( where );
							      
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new IsDropAcceptedEvent( myInterp,
							       const_cast<SALOME_PYQT_ModuleLight*>( this ),
							       const_cast<LightApp_DataObject*>( data_object ),
							       dropAccepted ) );
  return dropAccepted;
}

bool SALOME_PYQT_ModuleLight::isDropAcceptedEvent( LightApp_DataObject* where )
{
  MESSAGE("SALOME_PYQT_ModuleLight::isDropAcceptedEvent()");

  bool dropAccepted = false;

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule || !where )
    return dropAccepted;

  if ( PyObject_HasAttrString(myModule , (char*)"isDropAccepted") ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"isDropAccepted",
					   (char*)"s", where->entry().toLatin1().constData() ) );
    if( !res || !PyBool_Check( res )) {
      PyErr_Print();
      dropAccepted = false;
    }
    else{
      dropAccepted = PyObject_IsTrue( res );
    }
  }

  return dropAccepted;
}

void SALOME_PYQT_ModuleLight::dropObjects( const DataObjectList& what, SUIT_DataObject* where,
					   const int row, Qt::DropAction action )
{
  MESSAGE("SALOME_PYQT_ModuleLight::dropObjects()");
  // perform synchronous request to Python event dispatcher
  class DropObjectsEvent: public PyInterp_LockRequest
  {
  public:
    DropObjectsEvent(PyInterp_Interp*          _py_interp,
		     SALOME_PYQT_ModuleLight*  _obj,
		     const DataObjectList&     _what,
		     SUIT_DataObject*          _where,
		     const int                 _row,
		     Qt::DropAction            _action )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj( _obj ) ,
        myWhat( _what ),
	myWhere( _where ),
	myRow ( _row ),
	myAction ( _action ){}
  protected:
    virtual void execute()
    {
      myObj->dropObjectsEvent( myWhat, myWhere, myRow, myAction );
    }

  private:
    SALOME_PYQT_ModuleLight* myObj;
    DataObjectList           myWhat;
    SUIT_DataObject*         myWhere;
    int                      myRow;
    Qt::DropAction           myAction;
  };
  
  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new DropObjectsEvent( myInterp, this, what, where, row, action ) );
}

void SALOME_PYQT_ModuleLight::dropObjectsEvent( const DataObjectList& what, SUIT_DataObject* where,
						const int row, Qt::DropAction action )
{
  MESSAGE("SALOME_PYQT_ModuleLight::dropObjectsEvent()");
  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule || what.isEmpty() || !where )
    return;

  QStringList* theList = new QStringList();

  LightApp_DataObject* whereObject = dynamic_cast<LightApp_DataObject*>( where );
  if ( !whereObject ) return;
  
  for ( int i = 0; i < what.count(); i++ ) {
    LightApp_DataObject* dataObject = dynamic_cast<LightApp_DataObject*>( what[i] );
    if ( dataObject ) theList->append( dataObject->entry() );
  }

#if SIP_VERSION < 0x040800
  PyObjWrapper sipList( sipBuildResult( 0, "M", theList, sipClass_QStringList) );
#else
  PyObjWrapper sipList( sipBuildResult( 0, "D", theList, sipType_QStringList, NULL) );
#endif
  if ( PyObject_HasAttrString(myModule, (char*)"dropObjects") ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"dropObjects", (char*)"Osii",
					   sipList.get(),
					   whereObject->entry().toLatin1().constData(),
					   row, action ) );
    
    if( !res ) {
      PyErr_Print();
    }
  }
}
