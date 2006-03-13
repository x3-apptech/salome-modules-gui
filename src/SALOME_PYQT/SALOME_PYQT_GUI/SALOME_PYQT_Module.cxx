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
// See http://www.salome-platform.org/
//
//=============================================================================
// File      : SALOME_PYQT_Module.cxx
// Created   : 25/04/05
// Author    : Vadim SANDLER
// Project   : SALOME
// Copyright : 2003-2005 CEA/DEN, EDF R&D
// $Header   : $
//=============================================================================

#include "SALOME_PYQT_Module.h"

#include "PyInterp_Dispatcher.h"
#include "SUIT_ResourceMgr.h"
#include "STD_MDIDesktop.h"
#include "STD_TabDesktop.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"

#include "QtxWorkstack.h"
#include "QtxActionMenuMgr.h"
#include "QtxActionToolMgr.h"
#include <SALOME_LifeCycleCORBA.hxx>
#include <Container_init_python.hxx>

#include <qfile.h>
#include <qdom.h>
#include <qworkspace.h>
#include <qpopupmenu.h>

#include "SALOME_PYQT_SipDefs.h"
#if defined(SIP_VERS_v4_old) || defined(SIP_VERS_v4_new)
#include "sipAPISalomePyQtGUI.h"
#else
#include "sipSalomePyQtGUIDeclSalomePyQtGUI.h"
#endif

#include <sipqtQWidget.h>
#include <sipqtQPopupMenu.h>

#include <CORBA.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Default name of the module, replaced at the moment of module creation
const char* __DEFAULT_NAME__  = "SALOME_PYQT_Module";

///////////////////////////////////////////////////////////////////////////////
// Default menu group number
const int   __DEFAULT_GROUP__ = 40;

///////////////////////////////////////////////////////////////////////////////
// If __CALL_OLD_METHODS__ macro is not defined the invoking of obsolete Python
// module's methods like setSetting(), definePopup(), etc. is blocked.
// This macro is defined by default (in Makefile)
#ifdef __CALL_OLD_METHODS__
const bool IsCallOldMethods = true;
#else
const bool IsCallOldMethods = false;
#endif

///////////////////////////////////////////////////////////////////////////////
// NB: Python requests.
// General rule for Python requests created by SALOME_PYQT_Module:
// all requests should be executed SYNCHRONOUSLY within the main GUI thread.
// However, it is obligatory that ANY Python call is wrapped with a request object,
// so that ALL Python API calls are serialized with PyInterp_Dispatcher.
///////////////////////////////////////////////////////////////////////////////

//=============================================================================
// The class for parsing of the XML resource files.
// Used for backward compatibility with existing Python modules.
//=============================================================================
class SALOME_PYQT_XmlHandler
{
public:
  SALOME_PYQT_XmlHandler( SALOME_PYQT_Module* module, const QString& fileName );
  void createActions();
  void createPopup  ( QPopupMenu*    menu,
		      const QString& context,
		      const QString& parent,
		      const QString& object );

protected:
  void createToolBar   ( QDomNode& parentNode );
  void createMenu      ( QDomNode& parentNode,
			 const int parentMenuId = -1 );

  void insertPopupItems( QDomNode&   parentNode,
			 QPopupMenu* menu );

private:
  SALOME_PYQT_Module* myModule;
  QDomDocument        myDoc;
};

//=============================================================================
// SALOME_PYQT_Module class implementation (implements CAM_Module API for
// all Python-based SALOME module
//=============================================================================

// While the SalomePyQtGUI library is not imported in Python it's initialization function
// should be called manually (and only once) in order to initialize global sip data
// and to get C API from sip : sipBuildResult for example
#if defined(SIP_VERS_v4_old) || defined(SIP_VERS_v4_new)
#define INIT_FUNCTION initSalomePyQtGUI
#if defined(SIP_STATIC_MODULE)
extern "C" void INIT_FUNCTION();
#else
PyMODINIT_FUNC INIT_FUNCTION();
#endif
#else
#define INIT_FUNCTION initlibSalomePyQtGUIc
extern "C" void INIT_FUNCTION();
#endif

/*!
 * This function creates an instance of SALOME_PYQT_Module object by request
 * of and application object when the module is loaded.
 */
extern "C" {
  SALOME_PYQT_EXPORT CAM_Module* createModule() {
    static bool alreadyInitialized = false;
    if ( !alreadyInitialized ) {
      // call only once (see above) !
      PyEval_RestoreThread( KERNEL_PYTHON::_gtstate );
      INIT_FUNCTION();
      PyEval_ReleaseThread( KERNEL_PYTHON::_gtstate );
      alreadyInitialized = !alreadyInitialized;
    }
    return new SALOME_PYQT_Module();
  }
}

/*!
 * Static variables definition
 */
SALOME_PYQT_Module::InterpMap SALOME_PYQT_Module::myInterpMap;
SALOME_PYQT_Module* SALOME_PYQT_Module::myInitModule = 0;

/*!
 * Little trick : provide an access to being activated Python module from outside;
 * needed by the SalomePyQt library :(
*/
SALOME_PYQT_Module* SALOME_PYQT_Module::getInitModule()
{
  return myInitModule;
}

/*!
 * Constructor
 */
SALOME_PYQT_Module::SALOME_PYQT_Module() :
       SalomeApp_Module( __DEFAULT_NAME__ ), myModule( 0 ), myXmlHandler ( 0 )
{
}

/*!
 * Destructor
 */
SALOME_PYQT_Module::~SALOME_PYQT_Module()
{
  if ( myXmlHandler )
    delete myXmlHandler;
}

/*!
 * Initialization of the module.
 * Inherited from CAM_Module.
 *
 * This method is used for creation of the menus, toolbars and other staff.
 * There are two ways:
 * - for obsolete modules this method first tries to read <module>_<language>.xml
 *   resource file which contains a menu, toolbars and popup menus description.
 * - new modules can create menus by by calling the corresponding methods of SalomePyQt
 *   Python API in the Python module's initialize() method which is called from here.
 * NOTE: if postponed modules loading is not used, the active study might be not defined
 * yet at this stage, so initialize() method should not perform any study-based initialization.
 */
void SALOME_PYQT_Module::initialize( CAM_Application* app )
{
  SalomeApp_Module::initialize( app );

  // Try to get XML resource file name
  SUIT_ResourceMgr* aResMgr = getApp()->resourceMgr();
  QString aLang = aResMgr->stringValue( "language", "language", QString::null );
  if ( aLang.isEmpty() ) aLang = QString( "en" );
  QString aName = name( "" );
  QString aFileName = aName + "_" + aLang + ".xml";
  aFileName = aResMgr->path( "resources", aName, aFileName );

  // parse XML file if it is found and create actions
  if ( !myXmlHandler && !aFileName.isEmpty() ) {
    myXmlHandler = new SALOME_PYQT_XmlHandler( this, aFileName );
    myXmlHandler->createActions();
  }

  // perform internal initialization and call module's initialize() method
  // InitializeReq: request class for internal init() operation
  class InitializeReq : public PyInterp_Request
  {
  public:
    InitializeReq( CAM_Application*    _app,
		   SALOME_PYQT_Module* _obj )
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
    SALOME_PYQT_Module* myObj;
  };

  // Posting the request
  PyInterp_Dispatcher::Get()->Exec( new InitializeReq( app, this ) );
}

/*!
 * Activation of the module.
 * Inherited from CAM_Module.
 */
bool SALOME_PYQT_Module::activateModule( SUIT_Study* theStudy )
{
  MESSAGE( "SALOME_PYQT_Module::activateModule" );

  bool res = SalomeApp_Module::activateModule( theStudy );

  if ( !res )
    return res;

  // ActivateReq: request class for internal activate() operation
  class ActivateReq : public PyInterp_Request
  {
  public:
    ActivateReq( SUIT_Study*         _study,
		 SALOME_PYQT_Module* _obj )
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
    SALOME_PYQT_Module* myObj;
  };

  // Posting the request
  PyInterp_Dispatcher::Get()->Exec( new ActivateReq( theStudy, this ) );

  // activate menus, toolbars, etc
  setMenuShown( true );
  setToolShown( true );

  if ( menuMgr() )
    connect( menuMgr(), SIGNAL( menuHighlighted( int, int ) ),
	     this,      SLOT( onMenuHighlighted( int, int ) ) );

  return true;
}

/*!
 * Deactivation of the module.
 * Inherited from CAM_Module.
 */
bool SALOME_PYQT_Module::deactivateModule( SUIT_Study* theStudy )
{
  MESSAGE( "SALOME_PYQT_Module::deactivateModule" );

  if ( menuMgr() )
    disconnect( menuMgr(), SIGNAL( menuHighlighted( int, int ) ),
		this,      SLOT( onMenuHighlighted( int, int ) ) );

  bool res = SalomeApp_Module::deactivateModule( theStudy );

  // deactivate menus, toolbars, etc
  setMenuShown( false );
  setToolShown( false );

  // DeactivateReq: request class for internal deactivate() operation
  class DeactivateReq : public PyInterp_LockRequest
  {
  public:
    DeactivateReq( PyInterp_base*      _py_interp,
		   SUIT_Study*         _study,
		   SALOME_PYQT_Module* _obj )
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
    SALOME_PYQT_Module* myObj;
  };

  // Posting the request
  PyInterp_Dispatcher::Get()->Exec( new DeactivateReq( myInterp, theStudy, this ) );

  return res;
}

/*!
 * Processes GUI action (from main menu, toolbar or context popup menu)
 */
void SALOME_PYQT_Module::onGUIEvent()
{
  // get sender action
  const QObject* obj = sender();
  if ( !obj || !obj->inherits( "QAction" ) )
    return;
  QAction* action = (QAction*)obj;

  // get action ID
  int id = actionId( action );
  MESSAGE( "SALOME_PYQT_Module::onGUIEvent: id = " << id );

  // perform synchronous request to Python event dispatcher
  class GUIEvent : public PyInterp_LockRequest
  {
  public:
    GUIEvent( PyInterp_base*      _py_interp,
  	      SALOME_PYQT_Module* _obj,
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
    SALOME_PYQT_Module* myObj;
  };

  // Posting the request
  PyInterp_Dispatcher::Get()->Exec( new GUIEvent( myInterp, this, id ) );
}

/*!
 * Processes GUI action (from context popup menu, only for XML-based actions!)
 */
void SALOME_PYQT_Module::onGUIEvent( int id )
{
  // perform synchronous request to Python event dispatcher
  class GUIEvent : public PyInterp_LockRequest
  {
  public:
    GUIEvent( PyInterp_base*      _py_interp,
  	      SALOME_PYQT_Module* _obj,
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
    SALOME_PYQT_Module* myObj;
  };

  // Posting the request
  PyInterp_Dispatcher::Get()->Exec( new GUIEvent( myInterp, this, id ) );
}

/*!
 * Menu highlight processing
 */
void SALOME_PYQT_Module::onMenuHighlighted( int menu, int submenu )
{
  if ( !action( menu ) && registered( menu, submenu ) ) {
    // perform synchronous request to Python event dispatcher
    class MenuHighlightEvent : public PyInterp_LockRequest
    {
    public:
      MenuHighlightEvent( PyInterp_base*      _py_interp,
			  SALOME_PYQT_Module* _obj,
			  int                 _menu,
			  int                 _submenu )
	: PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
	  myMenu   ( _menu ),
	  mySubMenu( _submenu ),
	  myObj    ( _obj ) {}

    protected:
      virtual void execute()
      {
	myObj->menuHighlight( myMenu, mySubMenu );
      }

    private:
      int                 myMenu;
      int                 mySubMenu;
      SALOME_PYQT_Module* myObj;
    };

    // Posting the request
    PyInterp_Dispatcher::Get()->Exec( new MenuHighlightEvent( myInterp, this, menu, submenu ) );
  }
}

/*!
  Context popup menu request.
  Called when user activates popup menu in some window (view, object browser, etc).
  */
void SALOME_PYQT_Module::contextMenuPopup( const QString& theContext, QPopupMenu* thePopupMenu, QString& /*title*/ )
{
  MESSAGE( "SALOME_PYQT_Module::contextMenuPopup : " << theContext.latin1() );
  // perform synchronous request to Python event dispatcher
  class PopupMenuEvent : public PyInterp_LockRequest
  {
  public:
    PopupMenuEvent( PyInterp_base*     _py_interp,
		    SALOME_PYQT_Module* _obj,
		    const QString&      _context,
		    QPopupMenu*        _popup )
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
    SALOME_PYQT_Module* myObj;
    QString             myContext;
    QPopupMenu*         myPopup;
  };

  // Posting the request only if dispatcher is not busy!
  // Executing the request synchronously
  if ( !PyInterp_Dispatcher::Get()->IsBusy() )
    PyInterp_Dispatcher::Get()->Exec( new PopupMenuEvent( myInterp, this, theContext, thePopupMenu ) );
}

/*!
 * Defines the dockable window associated with the module.
 * To fill the list of windows the correspondind Python module's windows()
 * method is called from SALOME_PYQT_Module::init() method.
 * By default, ObjectBrowser, PythonConsole and LogWindow are provided.
 */
void SALOME_PYQT_Module::windows( QMap<int, int>& mappa ) const
{
  // First clear the output parameters
  QMap<int, int>::ConstIterator it;
  for ( it = myWindowsMap.begin(); it != myWindowsMap.end(); ++it ) {
    mappa[ it.key() ] = it.data();
  }
}

/*!
 * Defines the compatible views which should be opened on module activation.
 * To fill the list of views the correspondind Python module's views()
 * method is called from SALOME_PYQT_Module::init() method.
 * By default, the list is empty.
 */
void SALOME_PYQT_Module::viewManagers( QStringList& listik ) const
{
  for ( QStringList::ConstIterator it = myViewMgrList.begin(); it != myViewMgrList.end(); ++it ) {
    listik.append( *it );
  }
}

/*!
 * Performs internal initialization
 * - initializes/gets the Python interpreter (one per study)
 * - imports the Python module
 * - passes the workspace widget to the Python module
 * - calls Python module's initialize() method
 * - calls Python module's windows() method
 * - calls Python module's views() method
 */
void SALOME_PYQT_Module::init( CAM_Application* app )
{
  // reset interpreter to NULL
  myInterp = NULL;

  // get study Id
  SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( app );
  if ( !anApp )
    return;

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  if ( !aStudy )
    return;
  int aStudyId = aStudy ? aStudy->studyDS()->StudyId() : 0;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp( aStudyId );
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myModule )
    return; // Error

  myInitModule = this;

  if ( IsCallOldMethods ) { // __CALL_OLD_METHODS__
    // call Python module's setWorkspace() method
    setWorkSpace();
  }                         //__CALL_OLD_METHODS__

  // then call Python module's initialize() method
  // ... first get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();
  // ... (the Python module is already imported)
  // ... finally call Python module's initialize() method
  if(PyObject_HasAttrString(myModule , "initialize")){
    PyObjWrapper res( PyObject_CallMethod( myModule, "initialize", "" ) );
    if( !res ) {
      PyErr_Print();
    }
  }

  // get the windows list from the Python module by calling windows() method
  // ... first put default values
  myWindowsMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::DockLeft );
  myWindowsMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::DockBottom );
  // VSR: LogWindow is not yet implemented
  // myWindowsMap.insert( SalomeApp_Application::WT_LogWindow,     Qt::DockBottom );

  if(PyObject_HasAttrString(myModule , "windows")){
    PyObjWrapper res1( PyObject_CallMethod( myModule, "windows", "" ) );
    if( !res1 ) {
      PyErr_Print();
    }
    else {
      myWindowsMap.clear();
      if ( PyDict_Check( res1 ) ) {
        PyObject* key;
        PyObject* value;
        int pos = 0;
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
  // get the windows list from the Python module by calling views() method
  if(PyObject_HasAttrString(myModule , "views")){
    PyObjWrapper res2( PyObject_CallMethod( myModule, "views", "" ) );
    if( !res2 ) {
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
  myInitModule = 0;
}

/*!
 * Performs internal activation:
 * - initializes/gets the Python interpreter (one per study)
 * - imports the Python GUI module
 * - calls Python module's setSettings() method (obsolete function, used for compatibility with old code)
 *   or activate() method (for new modules)
 */
void SALOME_PYQT_Module::activate( SUIT_Study* theStudy )
{
  // get study Id
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( theStudy );
  int aStudyId = aStudy ? aStudy->studyDS()->StudyId() : 0;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp( aStudyId );
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myModule )
    return; // Error

  // get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();

  if ( IsCallOldMethods ) { //__CALL_OLD_METHODS__
    // call Python module's setSettings() method (obsolete)
    if(PyObject_HasAttrString(myModule , "setSettings")){
      PyObjWrapper res( PyObject_CallMethod( myModule, "setSettings", "" ) );
      if( !res ) {
        PyErr_Print();
      }
    }
  }                         //__CALL_OLD_METHODS__

  // call Python module's activate() method (for the new modules)
  if(PyObject_HasAttrString(myModule , "activate")){
    PyObjWrapper res1( PyObject_CallMethod( myModule, "activate", "" ) );
    if( !res1 ) {
      PyErr_Print();
    }
  }
}

/*!
 * Performs internal deactivation:
 * - calls Python module's deactivate() method
 */
void SALOME_PYQT_Module::deactivate( SUIT_Study* theStudy )
{
  // check if the subinterpreter is initialized and Python module is imported
  if ( !myInterp || !myModule ) {
    // Error! Python subinterpreter should be initialized and module should be imported first!
    return;
  }
  // then call Python module's deactivate() method
  if(PyObject_HasAttrString(myModule , "deactivate")){
    PyObjWrapper res( PyObject_CallMethod( myModule, "deactivate", "" ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
 * Called when active the study is actived (user brings its desktop to top)
 * - initializes/gets the Python interpreter (one per study)
 * - imports the Python GUI module
 * - calls Python module's activeStudyChanged() method
 */
void SALOME_PYQT_Module::studyChanged( SUIT_Study* theStudy )
{
  // get study Id
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( theStudy );
  int aStudyId = aStudy ? aStudy->studyDS()->StudyId() : 0;

  // initialize Python subinterpreter (on per study) and put it in <myInterp> variable
  initInterp( aStudyId );
  if ( !myInterp )
    return; // Error

  // import Python GUI module
  importModule();
  if ( !myModule )
    return; // Error

  // get python lock
  PyLockWrapper aLock = myInterp->GetLockWrapper();

  // call Python module's activeStudyChanged() method
  if(PyObject_HasAttrString(myModule , "activeStudyChanged")){
    PyObjWrapper res( PyObject_CallMethod( myModule, "activeStudyChanged", "i", aStudyId ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
 * Get module engine, returns nil var if engine is not found in LifeCycleCORBA
 */
Engines::Component_var SALOME_PYQT_Module::getEngine() const
{
  Engines::Component_var comp;
  // temporary solution
  try {
    comp = getApp()->lcc()->FindOrLoad_Component( "FactoryServerPy", name( "" ) );
  }
  catch (CORBA::Exception&) {
  }
  return comp;
}

/*!
 * Get module engine IOR, returns empty string if engine is not found in LifeCycleCORBA
 */
QString SALOME_PYQT_Module::engineIOR() const
{
  if ( !CORBA::is_nil( getEngine() ) )
    return QString( getApp()->orb()->object_to_string( getEngine() ) );
  return QString( "" );
}

/*!
 * Called when study desktop is activated.
 * Used for notifying about changing of the active study.
 */
void SALOME_PYQT_Module::studyActivated()
{
  // StudyChangedReq: request class for internal studyChanged() operation
  class StudyChangedReq : public PyInterp_Request
  {
  public:
    StudyChangedReq( SUIT_Study*         _study,
		     SALOME_PYQT_Module* _obj )
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
    SALOME_PYQT_Module* myObj;
  };

  // Posting the request
  PyInterp_Dispatcher::Get()->Exec( new StudyChangedReq( application()->activeStudy(), this ) );
}

/*!
 * Processes context popup menu request
 * - calls Python module's definePopup(...) method (obsolete function, used for compatibility with old code)
 *   to define the popup menu context
 * - parses XML resourses file (if exists) and fills the popup menu with the items)
 * - calls Python module's customPopup(...) method (obsolete function, used for compatibility with old code)
 *   to allow module to customize the popup menu
 * - for new modules calls createPopupMenu() function to allow the modules to build the popup menu
 *   by using insertItem(...) Qt functions.
 */
void SALOME_PYQT_Module::contextMenu( const QString& theContext, QPopupMenu* thePopupMenu )
{
  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  QString aContext( theContext ), aObject( "" ), aParent( "" );

  if ( IsCallOldMethods && PyObject_HasAttrString(myModule , "definePopup") ) { //__CALL_OLD_METHODS__
    // call definePopup() Python module's function
    // this is obsolete function, used only for compatibility reasons
    PyObjWrapper res(PyObject_CallMethod( myModule,
					  "definePopup",
					  "sss",
					  aContext.latin1(),
					  aObject.latin1(),
					  aParent.latin1() ) );
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
  }                        //__CALL_OLD_METHODS__

  // first try to create menu via XML parser:
  // we create popup menus without help of QtxPopupMgr
  if ( myXmlHandler )
    myXmlHandler->createPopup( thePopupMenu, aContext, aParent, aObject );

  PyObjWrapper sipPopup( sipBuildResult( 0, "M", thePopupMenu, sipClass_QPopupMenu ) );

  // then call Python module's createPopupMenu() method (for new modules)
  if ( PyObject_HasAttrString(myModule , "createPopupMenu") ) {
    PyObjWrapper res1( PyObject_CallMethod( myModule,
					  "createPopupMenu",
 				          "Os",
					  sipPopup.get(),
					  aContext.latin1() ) );
    if( !res1 ) {
      PyErr_Print();
    }
  }

  if ( IsCallOldMethods && PyObject_HasAttrString(myModule , "customPopup") ) { //__CALL_OLD_METHODS__
    // call customPopup() Python module's function
    // this is obsolete function, used only for compatibility reasons
    PyObjWrapper res2( PyObject_CallMethod( myModule,
					    "customPopup",
					    "Osss",
					    sipPopup.get(),
					    aContext.latin1(),
					    aObject.latin1(),
					    aParent.latin1() ) );
    if( !res2 ) {
      PyErr_Print();
    }
  }                        //__CALL_OLD_METHODS__
}

/*!
 * Processes GUI event
 * - calls Python module's OnGUIEvent() method
 */
void SALOME_PYQT_Module::guiEvent( const int theId )
{
  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  if ( PyObject_HasAttrString(myModule , "OnGUIEvent") ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, "OnGUIEvent", "i", theId ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
 * Menu highlight processing
 * - calls Python module's menuActivated(int,int) method
 */
void SALOME_PYQT_Module::menuHighlight( const int menu, const int submenu )
{
  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  if ( PyObject_HasAttrString(myModule , "menuHighlight") ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, "menuHighlight", "ii", menu, submenu ) );
    if( !res ) {
      PyErr_Print();
    }
  }
}

/*!
 *  Initialises python subinterpreter (one per study)
 */
void SALOME_PYQT_Module::initInterp( int theStudyId )
{
  // check study Id
  if ( !theStudyId ) {
    // Error! Study Id must not be 0!
    myInterp = NULL;
    return;
  }
  // try to find the subinterpreter
  if( myInterpMap.find( theStudyId ) != myInterpMap.end() ) {
    // found!
    myInterp = myInterpMap[ theStudyId ];
    return;
  }
  // not found - create a new one!
  ///////////////////////////////////////////////////////////////////
  // Attention: the creation of Python interpretor must be protected
  // by a C++ Lock because of C threads
  ///////////////////////////////////////////////////////////////////
  myInterp = new SALOME_PYQT_PyInterp();
  myInterp->initialize();
  myInterpMap[ theStudyId ] = myInterp;

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
  PyObjWrapper aRes( PyObject_CallMethod( aMod, "salome_init", "" ) );
  if( !aRes ) {
    // Error!
    PyErr_Print();
    return;
  }
}

/*!
 *  Imports Python GUI module and remember the reference to the module
 *  !!! initInterp() should be called first!!!
 */
void SALOME_PYQT_Module::importModule()
{
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
  QString aMod = QString( name("") ) + "GUI";
  myModule = PyImport_ImportModule( (char*)( aMod.latin1() ) );
  if( !myModule ) {
    // Error!
    PyErr_Print();
    return;
  }
}

/*!
 *  Calls <module>.setWorkSpace() method with PyQt QWidget object to use with
 *  interpreter.
 *  !!! initInterp() and importModule() should be called first!!!
 */
void SALOME_PYQT_Module::setWorkSpace()
{
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

  if ( IsCallOldMethods ) { //__CALL_OLD_METHODS__
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
    PyObjWrapper pyws( sipBuildResult( 0, "M", aWorkspace, sipClass_QWidget ) );
    // ... and finally call Python module's setWorkspace() method (obsolete)
    if ( PyObject_HasAttrString(myModule , "setWorkSpace") ) {
      PyObjWrapper res( PyObject_CallMethod( myModule, "setWorkSpace", "O", pyws.get() ) );
      if( !res ) {
        PyErr_Print();
      }
    }
  }                         //__CALL_OLD_METHODS__
}

/*!
 * Returns default menu group
 */
int SALOME_PYQT_Module::defaultMenuGroup()
{
  return __DEFAULT_GROUP__; 
}

/*!
 * The next methods call the parent implementation.
 * This is done to open protected methods from CAM_Module class.
 * Also these methods are used to register created from outside menus
 * in order to enable dynamic menus handling.
 */
int SALOME_PYQT_Module::createTool( const QString& name )
{
  return SalomeApp_Module::createTool( name );
}
int SALOME_PYQT_Module::createTool( const int id, const int tBar, const int idx )
{
  return SalomeApp_Module::createTool( id, tBar, idx );
}
int SALOME_PYQT_Module::createTool( const int id, const QString& tBar, const int idx )
{
  return SalomeApp_Module::createTool( id, tBar, idx );
}
int SALOME_PYQT_Module::createTool( QAction* a, const int tBar, const int id, const int idx )
{
  return SalomeApp_Module::createTool( a, tBar, id, idx );
}
int SALOME_PYQT_Module::createTool( QAction* a, const QString& tBar, const int id, const int idx )
{
  return SalomeApp_Module::createTool( a, tBar, id, idx );
}
int SALOME_PYQT_Module::createMenu( const QString& subMenu, const int menu, const int id, const int group, const int idx, const bool constantMenu )
{
  bool exists = hasMenu( subMenu, menu );
  int regId = SalomeApp_Module::createMenu( subMenu, menu, id, group, idx, true );
  if ( !exists )
    registerMenu( regId, menu, constantMenu );
  return regId;
}
int SALOME_PYQT_Module::createMenu( const QString& subMenu, const QString& menu, const int id, const int group, const int idx, const bool constantMenu )
{
  QStringList menus = QStringList::split( "|", menu, false );
  int pid = -1;
  for (int i = 0; i < menus.count(); i++ ) {
    pid = createMenu( menus[i], pid, -1, -1, -1, constantMenu );
    if ( pid == -1 ) break;
  }
  if ( pid != -1 )
    pid = createMenu( subMenu, pid, id, group, idx, constantMenu );
  return pid;
}
int SALOME_PYQT_Module::createMenu( const int id, const int menu, const int group, const int idx, const bool constantMenu )
{
  int regId = SalomeApp_Module::createMenu( id, menu, group, idx );
  if ( regId != -1 )
    registerMenu( regId, menu, constantMenu );
  return regId;
}
int SALOME_PYQT_Module::createMenu( const int id, const QString& menu, const int group, const int idx, const bool constantMenu )
{
  QStringList menus = QStringList::split( "|", menu, false );
  int pid = -1;
  for (int i = 0; i < menus.count(); i++ ) {
    pid = createMenu( menus[i], pid, -1, -1, -1, constantMenu );
    if ( pid == -1 ) break;
  }
  if ( pid != -1 )
    pid = createMenu( id, pid, group, idx, constantMenu );
  return pid;
}
int SALOME_PYQT_Module::createMenu( QAction* a, const int menu, const int id, const int group, const int idx, const bool constantMenu )
{
  int regId = SalomeApp_Module::createMenu( a, menu, id, group, idx );
  if ( regId != -1 )
    registerMenu( regId, menu, constantMenu );
  return regId;
}
int SALOME_PYQT_Module::createMenu( QAction* a, const QString& menu, const int id, const int group, const int idx, const bool constantMenu )
{
  QStringList menus = QStringList::split( "|", menu, false );
  int pid = -1;
  for (int i = 0; i < menus.count(); i++ ) {
    pid = createMenu( menus[i], pid, -1, -1, -1, constantMenu );
    if ( pid == -1 ) break;
  }
  if ( pid != -1 )
    pid = createMenu( a, pid, id, group, idx, constantMenu );
  return pid;
}
QAction* SALOME_PYQT_Module::createSeparator()
{
  return SalomeApp_Module::separator();
}
QAction* SALOME_PYQT_Module::action( const int id ) const
{
  return SalomeApp_Module::action( id );
}
int SALOME_PYQT_Module::actionId( const QAction* a ) const
{
  return SalomeApp_Module::actionId( a );
}
QAction* SALOME_PYQT_Module::createAction( const int id, const QString& text, const QString& icon,
					   const QString& menu, const QString& tip, const int key,
					   const bool toggle )
{
  QIconSet anIcon;
  if ( !icon.isEmpty() ) {
    QPixmap pixmap  = getApp()->resourceMgr()->loadPixmap( name(""), tr( icon ) );
    if ( !pixmap.isNull() )
      anIcon = QIconSet( pixmap );
  }
  QAction* a = action( id );
  if ( a ) {
    if ( a->text().isEmpty()      && !text.isEmpty() )  a->setText( text );
    if ( a->menuText().isEmpty()  && !menu.isEmpty() )  a->setMenuText( menu );
    if ( a->iconSet().isNull()    && !anIcon.isNull() ) a->setIconSet( anIcon );
    if ( a->statusTip().isEmpty() && !tip.isNull() )    a->setStatusTip( tip );
    if ( a->accel().isEmpty()     && key )              a->setAccel( key );
    if ( a->isToggleAction() != toggle )                a->setToggleAction( toggle );
    disconnect( a, SIGNAL( activated() ), this, SLOT( onGUIEvent() ) );
    connect(    a, SIGNAL( activated() ), this, SLOT( onGUIEvent() ) );
  }
  else {
    a = SalomeApp_Module::createAction( id, text, anIcon, menu, tip, key, getApp()->desktop(), toggle, this, SLOT( onGUIEvent() ) );
  }
  return a;
}

/*!
 * Returns TRUE if menu already exists
 */
bool SALOME_PYQT_Module::hasMenu( const QString& subMenu, const int menu )
{
  return menuMgr() && menuMgr()->containsMenu( subMenu, menu );
}

/*!
 * Register the menu
 */
void SALOME_PYQT_Module::registerMenu( const int id, const int menu, const bool constantMenu )
{
  QAction* a = action( id );
  QAction* s = separator();
  if ( a && a == s )
    return;
  if ( !registered( id, menu ) ) 
    myMenus[menu].append( MenuId( id, constantMenu ) );
}

/*!
 * Unregister the menu
 */
void SALOME_PYQT_Module::unregisterMenu( const int id, const int menu )
{
  if ( myMenus.find( menu ) != myMenus.end() ) {
    MenuIdList::iterator lit;
    for ( lit = myMenus[menu].begin(); lit != myMenus[menu].end(); ++lit ) {
      if ( (*lit).id == id ) {
	myMenus[menu].remove( lit );
	return;
      }
    }
  }
}

/*!
 * Returns TRUE if the menu is registered
 */
bool SALOME_PYQT_Module::registered( const int id, const int menu )
{
  MenuMap::iterator mit;
  for ( mit = myMenus.begin(); mit != myMenus.end(); ++mit ) {
    MenuIdList::iterator lit;
    for ( lit = mit.data().begin(); lit != mit.data().end(); ++lit ) {
      if ( (*lit).id == id && ( menu == 0 || mit.key() == menu ) )
	return true;
    }
  }
  return false;
}

/*!
 * Returns TRUE if the menu is constant (not removed by clearMenu()).
 * This concerns the menus which are created from XML files.
 */
bool SALOME_PYQT_Module::isConstantMenu( const int id, const int menu )
{
  if ( myMenus.find( menu ) != myMenus.end() ) {
    MenuIdList& l = myMenus[ menu ];
    MenuIdList::iterator lit;
    for ( lit = l.begin(); lit != l.end(); ++lit ) {
      if ( (*lit).id == id && (*lit).constantMenu )
	return true;
    }
  }
  return false;
}

/*!
 * Displays/hides the module's menus.
 */
void SALOME_PYQT_Module::setMenuShown( const bool show )
{
  QtxActionMenuMgr* mMgr = menuMgr();
  if ( !mMgr )
    return;

  bool upd = mMgr->isUpdatesEnabled();
  mMgr->setUpdatesEnabled( false );

  SalomeApp_Module::setMenuShown( show );

  for ( MenuMap::iterator mit = myMenus.begin(); mit != myMenus.end(); ++mit ) {
    MenuIdList::iterator lit;
    for ( lit = mit.data().begin(); lit != mit.data().end(); ++lit )
      if ( !action( (*lit).id ) )
	mMgr->setShown( (*lit).id, show );
  }

  mMgr->setUpdatesEnabled( upd );
  if ( upd )
    mMgr->update();
}

/*!
 * Displays/hides the module's toolbars.
 */
void SALOME_PYQT_Module::setToolShown( const bool show )
{
  SalomeApp_Module::setToolShown( show );
}

/*!
 * Clears the given menu.
 * If <id> = 0   : clear all items in the <menu> menu
 * If <menu> = 0 : clear all menus recursively starting from top-level.
 * If <removeActions> = 0 : also unregister all removed actions (not delete!).
 */
bool SALOME_PYQT_Module::clearMenu( const int id, const int menu, const bool removeActions )
{
  QAction* a = action( id );
  QAction* s = separator();
  typedef QValueList<int> IntList;
  if ( a && a != s ) {
    // action
    IntList menus;
    if ( menu != 0 ) {
      if ( registered( id, menu ) ) menus.append( menu );
    }
    else {
      MenuMap::iterator mit;
      for ( mit = myMenus.begin(); mit != myMenus.end(); ++mit )
	if ( registered( id, mit.key() ) ) menus.append( mit.key() );
    }
    for ( int i = 0; i < menus.count(); i++ ) {
      if ( !isConstantMenu( id, menus[i] ) ) {
	menuMgr()->remove( menuMgr()->actionId( a ), menus[ i ] );
	unregisterMenu( id, menus[i] );
      }
    }
    if ( !registered( id ) && removeActions )
      unregisterAction( id );
  }
  else {
    // menu
    if ( id == 0 ) {
      if ( menu == 0 ) {
	// clear all menus recursively starting from top-level (main menu)
	IntList l = myMenus.keys();
	IntList::iterator lit;
	for ( lit = l.begin(); lit != l.end(); ++lit )
	  clearMenu( 0, *lit, removeActions );
      }
      else {
	if ( myMenus.find( menu ) != myMenus.end() ) {
	  // remove all items in the parent menu
	  IntList l;
	  MenuIdList::iterator lit;
	  for ( lit = myMenus[menu].begin(); lit != myMenus[menu].end(); ++lit )
	    l.append( (*lit).id );
	  for ( int i = 0; i < l.count(); i++ )
	    clearMenu( l[i], menu, removeActions );
	  if ( myMenus[menu].empty() )
	    myMenus.remove( menu );
	}
      }
    }
    else {
      MenuMap::iterator mit;
      for ( mit = myMenus.begin(); mit != myMenus.end(); ++mit ) {
	MenuIdList::iterator lit;
	for ( lit = mit.data().begin(); lit != mit.data().end(); ++lit ) {
	  if ( (*lit).id == id && ( menu == 0 || mit.key() == menu ) ) {
	    clearMenu( 0, id, removeActions ); // first remove all sub-menus
	    if ( !isConstantMenu( id, mit.key() ) ) {
	      menuMgr()->remove( id, mit.key() );
	      mit.data().remove( lit );
	      break;
	    }
	  }
	}
      }
    }
  }
  return false;
}

//=============================================================================
// SALOME_PYQT_XmlHandler class implementation
//=============================================================================

// gets an tag name for the dom element [ static ]
// returns an empty string if the element does not have tag name
static QString tagName( const QDomElement& element ) {
  return element.tagName().stripWhiteSpace();
}

// gets an attribute by it's name for the dom element [ static ]
// returns an empty string if the element does not have such attribute
static QString attribute( const QDomElement& element, const QString& attName ) {
  return element.attribute( attName ).stripWhiteSpace();
}

// checks the given value for the boolean value [ static ]
// returns TRUE if string is "true", "yes" or "1"
static bool checkBool( const QString& value ) {
  return ( value == "true" || value == "yes" || value == "1" );
}

// checks the given value for the integer value [ static ]
// returns -1 if item is empty or presents and invalid number
static int checkInt( const QString& value, const int def = -1 )
{
  return value.isEmpty() ? def : value.toInt();
}

/*!
 * Constructor
 */
SALOME_PYQT_XmlHandler::SALOME_PYQT_XmlHandler( SALOME_PYQT_Module* module, const QString& fileName )
     : myModule( module )
{
  QFile aFile( fileName );
  if ( !aFile.open( IO_ReadOnly ) )
    return;
  if ( !myDoc.setContent( &aFile ) ) {
      aFile.close();
      return;
  }
  aFile.close();
}

/*!
  Called by SALOME_PYQT_Module::initialize() in order to create actions
  (menus, toolbars, popup menus)
 */
void SALOME_PYQT_XmlHandler::createActions()
{
  // get document element
  QDomElement aDocElem = myDoc.documentElement();

  // get main menu actions
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
 *  Creates popup menu
 */
void SALOME_PYQT_XmlHandler::createPopup( QPopupMenu*    menu,
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
      QString lab = attribute( e, "label-id"   );
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
  Create main menu with child actions
 */
void SALOME_PYQT_XmlHandler::createMenu( QDomNode& parentNode, const int parentMenuId )
{
  if ( !myModule )
    return;

  if ( parentNode.isNull() )
    return;

  QDomElement parentElement = parentNode.toElement();
  if ( !parentElement.isNull() ) {
    QString plabel = attribute( parentElement, "label-id" );
    int     pid    = checkInt( attribute( parentElement, "item-id" ) );
    int     ppos   = checkInt( attribute( parentElement, "pos-id" ) );
    int     group  = checkInt( attribute( parentElement, "group-id" ), 
			       myModule->defaultMenuGroup() );
    if ( !plabel.isEmpty() ) {
      // create menu
      int menuId = myModule->createMenu( plabel,         // label
					 parentMenuId,   // parent menu ID, should be -1 for main menu
					 pid,            // ID
					 group,          // group ID
					 ppos,           // position
					 true );         // create constant menu (not removed by clearMenu())
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
	    ////QString execute = attribute( elem, "execute-action" );               // not used

	    // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
	    // also check if the action with given ID is already created
	    if ( id != -1 ) {
	      // create menu action
	      QAction* action = myModule->createAction( id,                               // ID
						        tooltip,                          // tooltip
						        icon,                             // icon
						        label,                            // menu text
						        tooltip,                          // status-bar text
						        QKeySequence( accel ),            // keyboard accelerator
						        toggle );                         // toogled action
	      myModule->createMenu( action,   // action
				    menuId,   // parent menu ID
				    id,       // ID (same as for createAction())
				    group,    // group ID
				    pos,      // position
				    true );   // create constant menu (not removed by clearMenu())
	    }
	  }
	  else if ( aTagName == "submenu" ) {
	    // create sub-menu
	    createMenu( node, menuId );
	  }
	  else if ( aTagName == "separator" ) {
	    // create menu separator
	    int id    = checkInt( attribute( elem, "item-id" ) );     // separator can have ID
	    int pos   = checkInt( attribute( elem, "pos-id" ) );
	    int group = checkInt( attribute( elem, "group-id" ), 
				  myModule->defaultMenuGroup() );
	    QAction* action = myModule->createSeparator();
	    myModule->createMenu( action,  // separator action
				  menuId,  // parent menu ID
				  id,      // ID
				  group,   // group ID
				  pos,     // position
				  true );  // create constant menu (not removed by clearMenu())
	  }
	}
	node = node.nextSibling();
      }
    }
  }
}

/*!
  Create a toolbar with child actions
 */
void SALOME_PYQT_XmlHandler::createToolBar( QDomNode& parentNode )
{
  if ( !myModule )
    return;

  if ( parentNode.isNull() )
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
	    ////QString execute = attribute( elem, "execute-action" );               // not used

	    // -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
	    // also check if the action with given ID is already created
            if ( id != -1 ) {
	      // create toolbar action
	      QAction* action = myModule->createAction( id,                               // ID
						        tooltip,                          // tooltip
						        icon,                             // icon
						        label,                            // menu text
						        tooltip,                          // status-bar text
						        QKeySequence( accel ),            // keyboard accelerator
						        toggle );                         // toogled action
	      myModule->createTool( action, tbId, -1, pos );
	    }
	  }
	  else if ( aTagName == "separatorTB" ) {
	    // create toolbar separator
	    int     pos     = checkInt( attribute( elem, "pos-id" ) );
	    QAction* action = myModule->createSeparator();
	    myModule->createTool( action, tbId, -1, pos );
	  }
	}
	node = node.nextSibling();
      }
    }
  }
}

void SALOME_PYQT_XmlHandler::insertPopupItems( QDomNode& parentNode, QPopupMenu* menu )
{
  if ( !myModule )
    return;

  if ( parentNode.isNull() )
    return;

  // we create popup menus without help of QtxPopupMgr
  QDomNode node = parentNode.firstChild();
  while ( !node.isNull() ) {
    if ( node.isElement() ) {
      QDomElement elem = node.toElement();
      QString aTagName = tagName( elem );
      if ( aTagName == "popup-item" ) {
	// insert a command item
	int     id      = checkInt( attribute( elem, "item-id" ) );
	int     pos     = checkInt( attribute( elem, "pos-id" ) );
	QString label   = attribute( elem, "label-id" );
	QString icon    = attribute( elem, "icon-id" );
	/////QString tooltip = attribute( elem, "tooltip-id" );                   // not used
	QString accel   = attribute( elem, "accel-id" );
	/////bool    toggle  = checkBool( attribute( elem, "toggle-id" ) );       // not used
	/////QString execute = attribute( elem, "execute-action" );               // not used

	QIconSet anIcon;
	if ( !icon.isEmpty() ) {
          QPixmap pixmap  = myModule->getApp()->resourceMgr()->loadPixmap( myModule->name(""), icon );
	  if ( !pixmap.isNull() )
	    anIcon = QIconSet( pixmap );
        }

	// -1 action ID is not allowed : it means that <item-id> attribute is missed in the XML file!
	// also check if the action with given ID is already created
	if ( id != -1 ) {
	  menu->insertItem( anIcon, label, myModule, SLOT( onGUIEvent(int) ), QKeySequence( accel ), id, pos );
	}
      }
      else if ( aTagName == "submenu" ) {
	// create sub-menu
	int     id    = checkInt( attribute( elem, "item-id" ) );
	int     pos   = checkInt( attribute( elem, "pos-id" ) );
	QString label = attribute( elem, "label-id" );
	QString icon    = attribute( elem, "icon-id" );

	QIconSet anIcon;
	if ( !icon.isEmpty() ) {
  	  QPixmap pixmap  = myModule->getApp()->resourceMgr()->loadPixmap( myModule->name(""), icon );
	  if ( !pixmap.isNull() )
	    anIcon = QIconSet( pixmap );
        }

	QPopupMenu* newPopup = new QPopupMenu( menu, label );
	menu->insertItem( anIcon, label, newPopup, id, pos );
	insertPopupItems( node, newPopup );
      }
      else if ( aTagName == "separator" ) {
	// create menu separator
	int     pos     = checkInt( attribute( elem, "pos-id" ) );
	menu->insertSeparator( pos );
      }
    }
    node = node.nextSibling();
  }
}
