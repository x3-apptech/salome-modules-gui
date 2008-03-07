//  SALOME SALOME_PY : binding of VTK graphics and Python
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SalomePy.cxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME
//  $Header$

#include <Python.h>
#include <vtkPythonUtil.h>

#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include "SALOME_Event.hxx"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"

#include "SVTK_ViewManager.h"
#include "SVTK_ViewWindow.h"

using namespace std;

#define PUBLISH_ENUM(i)                              \
{                                                    \
  PyObject *w;                                       \
  int rc;                                            \
  if ( ( w = PyInt_FromLong( i ) ) == NULL ) return; \
  rc = PyDict_SetItemString( aModuleDict, #i, w );   \
  Py_DECREF( w );                                    \
  if ( rc < 0 ) return;                              \
}

// enumeration : view type
enum {
  ViewFront,     // fron view
  ViewBack,      // back view
  ViewTop,       // top view
  ViewBottom,    // bottom view
  ViewRight,     // right view
  ViewLeft       // left view
};


/*!
  VSR : 19.04.05 : Reimplemented for new SALOME GUI (SUIT-based)
  All methods are implemented using Event mechanism:
  - getRenderer()
  - getRenderWindow()
  - getRenderWindowInteractor()
  - showTrihedron()
  These methods (except showTrihedron() ) open new VTK viewer
  if there is no one opened.
  In case of error all methods return None object in Python.
*/

static PyObject* GetPyClass(const char* theClassName){
  static PyObject *aVTKModule = NULL;
  if(!aVTKModule){
    if (VTK_MAJOR_VERSION > 3)
      aVTKModule = PyImport_ImportModule("libvtkRenderingPython"); 
    else
      aVTKModule = PyImport_ImportModule("libVTKGraphicsPython"); 
    if(PyErr_Occurred()){
      PyErr_Print();
      return NULL;
    }
  }
  PyObject* aVTKDict = PyModule_GetDict(aVTKModule);
  char* aClassName = const_cast<char*>(theClassName);
  PyObject* aPyClass = PyDict_GetItemString(aVTKDict,aClassName);
  //Py_DECREF(aVTKModule);
  return aPyClass;
}

// internal enum: find or create VTK window
enum { __Find,          // try to find only 
       __FindOrCreate,  // try to find: if not found - create new 
       __Create };      // try to find: if not found - create new 

static SVTK_ViewWindow* GetVTKViewWindow( int toCreate = __FindOrCreate ) {
  SVTK_ViewWindow* aVW = NULL;
  if ( SUIT_Session::session() ) {
    // get application
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if ( anApp ) {
      // get active study
      SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
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
	      aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->getViews()[ 0 ] );
	  }
	}
	else {
	  SVTK_ViewManager* aVM = dynamic_cast<SVTK_ViewManager*>( anApp->getViewManager( "VTKViewer", toCreate == __FindOrCreate ) );
	  if ( aVM ) {
	    aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->getActiveView() );
	    // VSR : When new view window is created it can be not active yet at this moment,
	    // so the following is a some workaround
	    if ( !aVW && !aVM->getViews().isEmpty() )
	      aVW = dynamic_cast<SVTK_ViewWindow*>( aVM->getViews()[ 0 ] );
	  }
	}
      }
    }
  }
  return aVW;
}

/*!
  Get VTK renderer.
  Always opens new VTK window if <toCreate> parameter is non zero.
  Otherwise opens new VTK window only if there is no one opened.
*/
class TGetRendererEvent: public SALOME_Event {
public:
  typedef PyObject* TResult;
  TResult myResult;
  int     myCreate;
  TGetRendererEvent( bool toCreate )
    : myResult( Py_None ), myCreate( toCreate )  {}
  virtual void Execute() {
    if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( myCreate ? __Create : __FindOrCreate ) ) {
      PyObject* aPyClass = GetPyClass("vtkRenderer");
      vtkRenderer* aVTKObject = aVTKViewWindow->getRenderer();
      myResult = PyVTKObject_New(aPyClass,aVTKObject);
    }
  }
};
extern "C" PyObject *libSalomePy_getRenderer(PyObject *self, PyObject *args)
{
  PyObject* aResult = Py_None;
  int toCreate = 0;
  if ( !PyArg_ParseTuple(args, "|i:getRenderer", &toCreate) )
    PyErr_Print();
  else
    aResult = ProcessEvent( new TGetRendererEvent( toCreate ) );
  return aResult;
}

/*!
  Get VTK render window.
  Always opens new VTK window if <toCreate> parameter is non zero.
  Otherwise opens new VTK window only if there is no one opened.
*/
class TGetRenderWindowEvent: public SALOME_Event {
public:
  typedef PyObject* TResult;
  TResult myResult;
  int     myCreate;
  TGetRenderWindowEvent( bool toCreate )
    : myResult( Py_None ), myCreate( toCreate )  {}
  virtual void Execute() {
    if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( myCreate ? __Create : __FindOrCreate ) ) {
      PyObject* aPyClass = GetPyClass("vtkRenderWindow");
      vtkRenderWindow* aVTKObject = aVTKViewWindow->getRenderWindow();
      myResult = PyVTKObject_New(aPyClass,aVTKObject);
    }
  }
};
extern "C" PyObject *libSalomePy_getRenderWindow(PyObject *self, PyObject *args)
{
  PyObject* aResult = Py_None;
  int toCreate = 0;
  if ( !PyArg_ParseTuple(args, "|i:getRenderWindow", &toCreate) )
    PyErr_Print();
  else
    aResult = ProcessEvent( new TGetRenderWindowEvent( toCreate ) );
  return aResult;
}

/*!
  Get VTK render window interactor.
  Always opens new VTK window if <toCreate> parameter is non zero.
  Otherwise opens new VTK window only if there is no one opened.
*/
class TGetRenderWindowInteractorEvent: public SALOME_Event {
public:
  typedef PyObject* TResult;
  TResult myResult;
  int     myCreate;
  TGetRenderWindowInteractorEvent( bool toCreate )
    : myResult( Py_None ), myCreate( toCreate )  {}
  virtual void Execute() {
    if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( myCreate ? __Create : __FindOrCreate ) ) {
      PyObject* aPyClass = GetPyClass("vtkRenderWindowInteractor");
      vtkRenderWindowInteractor* aVTKObject = aVTKViewWindow->getInteractor();
      myResult = PyVTKObject_New(aPyClass,aVTKObject);
    }
  }
};
extern "C" PyObject *libSalomePy_getRenderWindowInteractor(PyObject *self, PyObject *args)
{
  PyObject* aResult = Py_None;
  int toCreate = 0;
  if ( !PyArg_ParseTuple(args, "|i:getRenderWindowInteractor", &toCreate) )
    PyErr_Print();
  else
    aResult = ProcessEvent( new TGetRenderWindowInteractorEvent( toCreate ) );
  return aResult;
}

/*!
  Show/hide trihedron in the current VTK viewer (if there is one)
*/
extern "C" PyObject *libSalomePy_showTrihedron(PyObject *self, PyObject *args)
{
  class TEvent: public SALOME_Event {
  public:
    int myShow;
    TEvent( int bShow )
      : myShow( bShow )  {}
    virtual void Execute() {
      if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( __Find ) ) {
	if ( aVTKViewWindow->isTrihedronDisplayed() != myShow )
	  aVTKViewWindow->onViewTrihedron();
      }
    }
  };
  
  PyObject* aResult = Py_None;
  int bShow = 0;
  if ( !PyArg_ParseTuple(args, "i:showTrihedron", &bShow) )
    PyErr_Print();
  else
    ProcessVoidEvent( new TEvent( bShow ) );
  return aResult;
}

/*!
  Fit all the contents in the current VTK viewer (if there is one)
*/
extern "C" PyObject *libSalomePy_fitAll(PyObject *self, PyObject *args)
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( __Find ) ) {
	aVTKViewWindow->onFitAll();
      }
    }
  };
  
  ProcessVoidEvent( new TEvent() );
  return Py_None;
}

/*!
  Set view type fot the current VTK viewer (if there is one)
*/
extern "C" PyObject *libSalomePy_setView(PyObject *self, PyObject *args)
{
  class TEvent: public SALOME_Event {
  public:
    long myType;
    TEvent( long type ) : myType( type) {}
    virtual void Execute() {
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
  if ( !PyArg_ParseTuple(args, "l:setView", &type) )
    PyErr_Print();
  else {
    ProcessVoidEvent( new TEvent( type ) );
    if( PyErr_Occurred() )
      PyErr_Print();
  }
  return Py_None;
}

/*!
  Reset contents of the current VTK viewer (if there is one)
  to the default state
*/
extern "C" PyObject *libSalomePy_resetView(PyObject *self, PyObject *args)
{
  class TEvent: public SALOME_Event {
  public:
    TEvent() {}
    virtual void Execute() {
      if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow( __Find ) ) {
	aVTKViewWindow->onResetView();
      }
    }
  };
  
  ProcessVoidEvent( new TEvent() );
  return Py_None;
}

/*!
  Library initialization
*/
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

extern "C" void initlibSalomePy()
{
  static char modulename[] = "libSalomePy";
  // init module
  PyObject* aModule = Py_InitModule(modulename, Module_Methods);
  if( PyErr_Occurred() ) {
    PyErr_Print();
    return;
  }
  // get module's dictionary
  PyObject *aModuleDict = PyModule_GetDict( aModule );
  if ( aModuleDict == NULL )
    return;
  // add View type enumeration
  PUBLISH_ENUM( ViewFront );
  PUBLISH_ENUM( ViewBack );
  PUBLISH_ENUM( ViewTop );
  PUBLISH_ENUM( ViewBottom );
  PUBLISH_ENUM( ViewRight );
  PUBLISH_ENUM( ViewLeft );
}
