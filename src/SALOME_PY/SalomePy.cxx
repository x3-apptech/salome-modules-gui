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

/*!
  VSR : 19.04.05 : Reimplemented for new SALOME GUI (SUIT-based)
  All methods are implemented using Event mechanism:
  - getRenderer()
  - getRenderWindow()
  - getRenderWindowInteractor()
  These methods open new VTK viewer if there is no one opened.
  In case of error methods return None object in Python.
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

static SVTK_ViewWindow* GetVTKViewWindow() {
  SVTK_ViewWindow* aVW = NULL;
  if ( SUIT_Session::session() ) {
    // get application
    SalomeApp_Application* anApp = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    if ( anApp ) {
      // get active study
      SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( anApp->activeStudy() );
      if ( aStudy ) {
	// find or create VTK view manager
	SVTK_ViewManager* aVM = dynamic_cast<SVTK_ViewManager*>( anApp->getViewManager( "VTKViewer", true ) );
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
  return aVW;
}

/*!
  Get VTK renderer (opens new VTK window if there is no one opened)
*/
class TGetRendererEvent: public SALOME_Event {
public:
  typedef PyObject* TResult;
  TResult myResult;
  TGetRendererEvent() : myResult( Py_None ) {}
  virtual void Execute() {
    if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow() ) {
      PyObject* aPyClass = GetPyClass("vtkRenderer");
      vtkRenderer* aVTKObject = aVTKViewWindow->getRenderer();
      myResult = PyVTKObject_New(aPyClass,aVTKObject);
    }
  }
};
extern "C" PyObject *libSalomePy_getRenderer(PyObject *self, PyObject *args)
{
  return ProcessEvent( new TGetRendererEvent() );
}

/*!
  Get VTK render window (opens new VTK window if there is no one opened)
*/
class TGetRenderWindowEvent: public SALOME_Event {
public:
  typedef PyObject* TResult;
  TResult myResult;
  TGetRenderWindowEvent() : myResult( Py_None ) {}
  virtual void Execute() {
    if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow() ) {
      PyObject* aPyClass = GetPyClass("vtkRenderWindow");
      vtkRenderWindow* aVTKObject = aVTKViewWindow->getRenderWindow();
      myResult = PyVTKObject_New(aPyClass,aVTKObject);
    }
  }
};
extern "C" PyObject *libSalomePy_getRenderWindow(PyObject *self, PyObject *args)
{
  return ProcessEvent( new TGetRenderWindowEvent() );
}

/*!
  Get VTK render window interactor (opens new VTK window if there is no one opened)
*/
class TGetRenderWindowInteractorEvent: public SALOME_Event {
public:
  typedef PyObject* TResult;
  TResult myResult;
  TGetRenderWindowInteractorEvent() : myResult( Py_None ) {}
  virtual void Execute() {
    if( SVTK_ViewWindow* aVTKViewWindow = GetVTKViewWindow() ) {
      PyObject* aPyClass = GetPyClass("vtkRenderWindowInteractor");
      vtkRenderWindowInteractor* aVTKObject = aVTKViewWindow->getInteractor();
      myResult = PyVTKObject_New(aPyClass,aVTKObject);
    }
  }
};
extern "C" PyObject *libSalomePy_getRenderWindowInteractor(PyObject *self, PyObject *args)
{
  return ProcessEvent( new TGetRenderWindowInteractorEvent() );
}

/*!
  Library initialization
*/
static PyMethodDef Module_Methods[] = 
{
  { "getRenderer",               libSalomePy_getRenderer,     METH_NOARGS },
  { "getRenderWindow",           libSalomePy_getRenderWindow, METH_NOARGS },
  { "getRenderWindowInteractor", libSalomePy_getRenderWindow, METH_NOARGS },
  { NULL, NULL }
};

extern "C" void initlibSalomePy()
{
  static char modulename[] = "libSalomePy";
  /*PyObject* aModule = */Py_InitModule(modulename, Module_Methods);
  if(PyErr_Occurred()){
    PyErr_Print();
    return;
  }
}
