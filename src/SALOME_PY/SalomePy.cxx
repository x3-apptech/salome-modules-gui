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

//  SALOME SALOME_PY : binding of VTK graphics and Python
//  File   : SalomePy.cxx
//  Author : Paul RASCLE, EDF
//
#ifdef WIN32
// E.A. : On windows with python 2.6, there is a conflict
// E.A. : between pymath.h and Standard_math.h which define
// E.A. : some same symbols : acosh, asinh, ...
#include <Standard_math.hxx>
#include <pymath.h>
#endif

#include <Python.h>
#include <vtkPythonUtil.h>

#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <SALOME_Event.h>

#include <SUIT_Session.h>
#include <LightApp_Application.h>
#include <LightApp_Study.h>

#include <SVTK_ViewManager.h>
#include <SVTK_ViewWindow.h>

#define VTK_XVERSION (VTK_MAJOR_VERSION*10000+VTK_MINOR_VERSION*100+VTK_BUILD_VERSION)

/*!
  \brief Python wrappings for VTK viewer of the SALOME desktop.

  All methods are implemented using Event mechanism. The module
  provides the following functions:
  - getRenderer()
  - getRenderWindow()
  - getRenderWindowInteractor()
  - showTrihedron()
  - fitAll()
  - setView()
  - resetView()

  Usage in Python:
  \code
  import SalomePy
  renderer = SalomePy.getRenderer()     # get VTK renderer
  window   = SalomePy.getRenderWindow() # get render window
  \endcode

  The methods getRenderer(), getRenderWindow() and getRenderWindowInteractor()
  open new VTK viewer if there is no one opened.
  In case of any error these methods return None object to the Python.
*/

#ifdef WIN32
 #if defined SALOMEPY_EXPORTS || defined SalomePy_EXPORTS
  #define SALOMEPY_EXPORT __declspec( dllexport )
 #else
  #define SALOMEPY_EXPORT __declspec( dllimport )
 #endif
#else
 #define SALOMEPY_EXPORT
#endif


#define PUBLISH_ENUM(i)                              \
{                                                    \
  PyObject *w;                                       \
  int rc;                                            \
  if ( ( w = PyInt_FromLong( i ) ) == NULL ) return; \
  rc = PyDict_SetItemString( aModuleDict, #i, w );   \
  Py_DECREF( w );                                    \
  if ( rc < 0 ) return;                              \
}

//! View operation type
enum {
  ViewFront,     //!< front view
  ViewBack,      //!< back view
  ViewTop,       //!< top view
  ViewBottom,    //!< bottom view
  ViewRight,     //!< right view
  ViewLeft       //!< left view
};

/*!
  \brief Get Python class object by name
  \internal
  \param theClassName Python class name
  \return Python class object or None object if class is not found
*/
static PyTypeObject* GetPyClass( const char* theClassName )
{
  static PyObject* aVTKModule = 0;
  PyObject* aPyClass = 0;
  if( !aVTKModule ) {
#if VTK_XVERSION < 30000
    aVTKModule = PyImport_ImportModule( "libVTKGraphicsPython" ); 
#elif VTK_XVERSION < 50700
    aVTKModule = PyImport_ImportModule( "vtk.libvtkRenderingPython" ); 
#elif VTK_XVERSION < 60000
    aVTKModule = PyImport_ImportModule( "vtkRenderingPython" ); 
#else
    aVTKModule = PyImport_ImportModule( "vtkRenderingCorePython" ); 
#endif
    if( PyErr_Occurred() ) {
      PyErr_Print();
    }
  }
  if ( aVTKModule ) {
    PyObject* aVTKDict = PyModule_GetDict( aVTKModule );
    aPyClass = PyDict_GetItemString(aVTKDict, const_cast<char*>( theClassName ) );
    if (!PyType_Check(aPyClass))
      return 0;
  }
  return (PyTypeObject *)aPyClass;
}

/*!
  \brief VTK window find/create mode
  \internal
*/
enum { 
  __Find,          // try to find VTK window; if not found, do nothing
  __FindOrCreate,  // try to find VTK window; if not found, create new one
  __Create };      // create new VTK window

/*!
  \brief Find or create VTK window.
  \internal
  \param toCreate window find/create mode
  \return VTK window pointer or 0 if it could not be found/created
*/
static SVTK_ViewWindow* GetVTKViewWindow( int toCreate = __FindOrCreate ) {
  SVTK_ViewWindow* aVW = 0;
  if ( SUIT_Session::session() ) {
    // get application
    LightApp_Application* anApp = dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
    if ( anApp ) {
      // get active study
      LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>( anApp->activeStudy() );
      if ( aStudy ) {
        // find or create VTK view manager
        if ( toCreate == __Create ) {
          SVTK_ViewManager* aVM = dynamic_cast<SVTK_ViewManager*>( anApp->createViewManager( "VTKViewer" ) );
          if ( aVM ) {
            aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->getActiveView() );
            if ( !aVW )
              aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->createViewWindow() );
            // VSR : When new view window is created it can be not active yet at this moment,
            // so the following is a some workaround
            if ( !aVW && !aVM->getViews().isEmpty() )
              aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->getViews()[0] );
          }
        }
        else {
          SVTK_ViewManager* aVM = dynamic_cast<SVTK_ViewManager*>( anApp->getViewManager( "VTKViewer", toCreate == __FindOrCreate ) );
          if ( aVM ) {
            aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->getActiveView() );
            // VSR : When new view window is created it can be not active yet at this moment,
            // so the following is a some workaround
            if ( !aVW && !aVM->getViews().isEmpty() )
              aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->getViews()[0] );
          }
        }
      }
    }
  }
  return aVW;
}

/*!
  \fn PyObject* getRenderer( int toCreate = 0 );
  \brief Get VTK renderer (vtkRenderer).
  
  If \a toCreate parameter is 0 (by default) the function tries to find
  and reuse existing VTK window; if it is not found, the new VTK window
  is opened.

  If \a toCreate parameter is non-zero, the function always creates
  new VTK window.

  If VTK window could not be found and or created, the None Python object
  is returned.

  \param toCreate window creation mode
  \return VTK window renderer object
*/

class SALOMEPY_EXPORT TGetRendererEvent: public SALOME_Event
{
public:
  typedef PyObject* TResult;
  TResult myResult;
  int     myCreate;
  TGetRendererEvent( bool toCreate )
    : myResult( Py_None ), myCreate( toCreate ) {}
  virtual void Execute()
  {
    PyTypeObject* aPyClass = ::GetPyClass( "vtkRenderer" );
    SVTK_ViewWindow* aVTKViewWindow = 
      ::GetVTKViewWindow( myCreate ? __Create : __FindOrCreate );
    if( aVTKViewWindow && aPyClass ) {
      vtkRenderer* aVTKObject = aVTKViewWindow->getRenderer();
#if VTK_XVERSION < 50700
      myResult = PyVTKObject_New( aPyClass, aVTKObject );
#else
      myResult = PyVTKObject_FromPointer( aPyClass, NULL, aVTKObject );
#endif
    }
  }
};

extern "C" SALOMEPY_EXPORT PyObject* libSalomePy_getRenderer( PyObject* self, PyObject* args )
{
  PyObject* aResult = Py_None;
  int toCreate = 0;
  if ( !PyArg_ParseTuple( args, "|i:getRenderer", &toCreate ) )
    PyErr_Print();
  else
    aResult = ProcessEvent( new TGetRendererEvent( toCreate ) );
  return aResult;
}

/*!
  \fn PyObject* getRenderWindow( int toCreate = 0 );
  \brief Get VTK render window (vtkRenderWindow).
  
  If \a toCreate parameter is 0 (by default) the function tries to find 
  and reuse existing VTK window; if it is not found, the new VTK window
  is opened.

  If \a toCreate parameter is non-zero, the function always creates
  new VTK window.

  If VTK window could not be found and or created, the None Python object
  is returned.

  \param toCreate window creation mode
  \return VTK window render window object
*/

class SALOMEPY_EXPORT TGetRenderWindowEvent: public SALOME_Event
{
public:
  typedef PyObject* TResult;
  TResult myResult;
  int     myCreate;
  TGetRenderWindowEvent( bool toCreate )
    : myResult( Py_None ), myCreate( toCreate ) {}
  virtual void Execute()
  {
    PyTypeObject* aPyClass = ::GetPyClass( "vtkRenderWindow" );
    SVTK_ViewWindow* aVTKViewWindow = 
      ::GetVTKViewWindow( myCreate ? __Create : __FindOrCreate );
    if( aVTKViewWindow && aPyClass ) {
      vtkRenderWindow* aVTKObject = aVTKViewWindow->getRenderWindow();
#if VTK_XVERSION < 50700
      myResult = PyVTKObject_New( aPyClass, aVTKObject );
#else
      myResult = PyVTKObject_FromPointer( aPyClass, NULL, aVTKObject );
#endif
    }
  }
};

extern "C" SALOMEPY_EXPORT PyObject* libSalomePy_getRenderWindow( PyObject* self, PyObject* args )
{
  PyObject* aResult = Py_None;
  int toCreate = 0;
  if ( !PyArg_ParseTuple( args, "|i:getRenderWindow", &toCreate ) )
    PyErr_Print();
  else
    aResult = ProcessEvent( new TGetRenderWindowEvent( toCreate ) );
  return aResult;
}

/*!
  \fn PyObject* getRenderWindowInteractor( int toCreate = 0 );
  \brief Get VTK render window interactor (getRenderWindowInteractor).
  
  If \a toCreate parameter is 0 (by default) the function tries to find 
  and reuse existing VTK window; if it is not found, the new VTK window
  is opened.

  If \a toCreate parameter is non-zero, the function always creates
  new VTK window.

  If VTK window could not be found and or created, the None Python object
  is returned.

  \param toCreate window creation mode
  \return VTK window render window interactor object
*/

class SALOMEPY_EXPORT TGetRenderWindowInteractorEvent: public SALOME_Event
{
public:
  typedef PyObject* TResult;
  TResult myResult;
  int     myCreate;
  TGetRenderWindowInteractorEvent( bool toCreate )
    : myResult( Py_None ), myCreate( toCreate ) {}
  virtual void Execute()
  {
    PyTypeObject* aPyClass = ::GetPyClass( "vtkRenderWindowInteractor" );
    SVTK_ViewWindow* aVTKViewWindow = 
      ::GetVTKViewWindow( myCreate ? __Create : __FindOrCreate );
    if( aVTKViewWindow && aPyClass ) {
      vtkRenderWindowInteractor* aVTKObject = aVTKViewWindow->getInteractor();
#if VTK_XVERSION < 50700
      myResult = PyVTKObject_New( aPyClass, aVTKObject );
#else
      myResult = PyVTKObject_FromPointer( aPyClass, NULL, aVTKObject );
#endif
    }
  }
};

extern "C" SALOMEPY_EXPORT PyObject* libSalomePy_getRenderWindowInteractor( PyObject* self, PyObject* args )
{
  PyObject* aResult = Py_None;
  int toCreate = 0;
  if ( !PyArg_ParseTuple( args, "|i:getRenderWindowInteractor", &toCreate ) )
    PyErr_Print();
  else
    aResult = ProcessEvent( new TGetRenderWindowInteractorEvent( toCreate ) );
  return aResult;
}

/*!
  \fn PyObject* showTrihedron( int show );
  \brief Show/hide trihedron in the current VTK viewer.

  If there is no active VTK viewer, nothing happens.
  
  \param show new trihedron visibility state
  \return nothing (Py_None)
*/

extern "C" SALOMEPY_EXPORT PyObject* libSalomePy_showTrihedron( PyObject* self, PyObject* args )
{
  class TEvent: public SALOME_Event
  {
  public:
    int myShow;
    TEvent( int bShow )
      : myShow( bShow ) {}
    virtual void Execute()
    {
      if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( __Find ) ) {
        if ( aVTKViewWindow->isTrihedronDisplayed() != myShow )
          aVTKViewWindow->onViewTrihedron(myShow);
      }
    }
  };
  
  PyObject* aResult = Py_None;
  int bShow = 0;
  if ( !PyArg_ParseTuple( args, "i:showTrihedron", &bShow ) )
    PyErr_Print();
  else
    ProcessVoidEvent( new TEvent( bShow ) );
  return aResult;
}

/*!
  \fn PyObject* fitAll();
  \brief Fit all the contents in the current VTK viewer.

  If there is no active VTK viewer, nothing happens.

  \return nothing (Py_None)
*/

extern "C" SALOMEPY_EXPORT PyObject* libSalomePy_fitAll( PyObject* self, PyObject* args )
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( __Find ) ) {
        aVTKViewWindow->onFitAll();
      }
    }
  };
  
  ProcessVoidEvent( new TEvent() );
  return Py_None;
}

/*!
  \fn PyObject* setView( int type );
  \brief Set view type for the current VTK viewer.

  If there is no active VTK viewer, nothing happens.
  
  \param type view type
  \return nothing (Py_None)
*/

extern "C" SALOMEPY_EXPORT PyObject* libSalomePy_setView( PyObject* self, PyObject* args )
{
  class TEvent: public SALOME_Event
  {
  public:
    long myType;
    TEvent( long type ) : myType( type) {}
    virtual void Execute()
    {
      if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( __Find ) ) {
        switch( myType ) {
        case ViewFront:
          aVTKViewWindow->onFrontView();  break;
        case ViewBack:
          aVTKViewWindow->onBackView();   break;
        case ViewTop:
          aVTKViewWindow->onTopView();    break;
        case ViewBottom:
          aVTKViewWindow->onBottomView(); break;
        case ViewRight:
          aVTKViewWindow->onRightView();  break;
        case ViewLeft:
          aVTKViewWindow->onLeftView();   break;
        default:
          PyErr_Format(PyExc_ValueError,"setView%: wrong parameter value; must be between %d and %d", ViewFront, ViewLeft );
          break;
        }
      }
    }
  };
  
  long type = -1;
  if ( !PyArg_ParseTuple( args, "l:setView", &type ) )
    PyErr_Print();
  else {
    ProcessVoidEvent( new TEvent( type ) );
    if( PyErr_Occurred() )
      PyErr_Print();
  }
  return Py_None;
}

/*!
  \fn PyObject* resetView();
  \brief Reset contents of the current VTK viewer.

  If there is no active VTK viewer, nothing happens.
  
  \return nothing (Py_None)
*/

extern "C" SALOMEPY_EXPORT PyObject* libSalomePy_resetView( PyObject* self, PyObject* args )
{
  class TEvent: public SALOME_Event
  {
  public:
    TEvent() {}
    virtual void Execute()
    {
      if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( __Find ) ) {
        aVTKViewWindow->onResetView();
      }
    }
  };
  
  ProcessVoidEvent( new TEvent() );
  return Py_None;
}

static PyMethodDef Module_Methods[] = 
{
  { "getRenderer",               libSalomePy_getRenderer,               METH_VARARGS },
  { "getRenderWindow",           libSalomePy_getRenderWindow,           METH_VARARGS },
  { "getRenderWindowInteractor", libSalomePy_getRenderWindowInteractor, METH_VARARGS },
  { "showTrihedron",             libSalomePy_showTrihedron,             METH_VARARGS },
  { "fitAll",                    libSalomePy_fitAll,                    METH_NOARGS  },
  { "setView",                   libSalomePy_setView,                   METH_VARARGS },
  { "resetView",                 libSalomePy_resetView,                 METH_NOARGS  },
  { NULL, NULL }
};

/*!
  \brief Python module initialization.
  \internal
*/
extern "C" SALOMEPY_EXPORT void initlibSalomePy()
{
  static char* modulename = (char*)"libSalomePy";

  // init module
  PyObject* aModule = Py_InitModule( modulename, Module_Methods );
  if( PyErr_Occurred() ) {
    PyErr_Print();
    return;
  }

  // get module's dictionary
  PyObject *aModuleDict = PyModule_GetDict( aModule );
  if ( aModuleDict == NULL )
    return;

  // export View type enumeration
  PUBLISH_ENUM( ViewFront );
  PUBLISH_ENUM( ViewBack );
  PUBLISH_ENUM( ViewTop );
  PUBLISH_ENUM( ViewBottom );
  PUBLISH_ENUM( ViewRight );
  PUBLISH_ENUM( ViewLeft );
}
