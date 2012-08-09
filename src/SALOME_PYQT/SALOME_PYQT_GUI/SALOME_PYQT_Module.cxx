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
// File   : SALOME_PYQT_Module.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include <PyInterp_Dispatcher.h>

#include "SALOME_PYQT_Module.h"
#include "SalomeApp_Application.h"
#include "SALOME_PYQT_ModuleLight.h"

#include <SALOME_LifeCycleCORBA.hxx>
#include <Container_init_python.hxx>
#include <CORBA.h>


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
  SALOME_PYQT_EXPORT CAM_Module* createModule() {

    static bool alreadyInitialized = false;
    if ( !alreadyInitialized ) {
      // call only once (see comment above) !

      PyEval_RestoreThread( KERNEL_PYTHON::_gtstate);
      INIT_FUNCTION();
      PyEval_ReleaseThread( KERNEL_PYTHON::_gtstate);
      alreadyInitialized = !alreadyInitialized;
    }
    return new SALOME_PYQT_Module();
  }
}


/*!
  \var __DEFAULT_NAME__ - Default name of the module, replaced at the moment of module creation
*/
const char* __DEFAULT_NAME__  = "SALOME_PYQT_Module";

/*!
 * Constructor
 */
SALOME_PYQT_Module::SALOME_PYQT_Module()
  : SalomeApp_Module(__DEFAULT_NAME__),
    LightApp_Module(__DEFAULT_NAME__),
    SALOME_PYQT_ModuleLight()
{
}

/*!
 * Destructor
 */
SALOME_PYQT_Module::~SALOME_PYQT_Module()
{
}

/*!
 * Get module engine, returns nil var if engine is not found in LifeCycleCORBA
 */
Engines::EngineComponent_var SALOME_PYQT_Module::getEngine() const
{
  Engines::EngineComponent_var comp;
  // temporary solution
  try {
    comp = getApp()->lcc()->FindOrLoad_Component( "FactoryServerPy", name().toLatin1() );
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
  class EngineIORReq : public PyInterp_LockRequest
  {
  public:
    EngineIORReq( PyInterp_Interp*    _py_interp,
                  SALOME_PYQT_Module* _obj )
      : PyInterp_LockRequest( _py_interp, 0, true ), // this request should be processed synchronously (sync == true)
        myObj( _obj ) {}

  protected:
    virtual void execute()
    {
      myObj->getEngineIOR();
    }

  private:
    SALOME_PYQT_Module* myObj;
  };

  // post request
  PyInterp_Dispatcher::Get()->Exec( new EngineIORReq( myInterp, const_cast<SALOME_PYQT_Module*>( this ) ) );

  return myIOR;
}


/*!
 * Redefined to invokec correct version
 */
bool SALOME_PYQT_Module::activateModule( SUIT_Study* theStudy )
{
  // call base implementation
  bool res = SalomeApp_Module::activateModule( theStudy );

  if ( !res )
    return res;

  // internal activation
  return activateModuleInternal( theStudy );
}

/*!
 * Tries to get engine IOR from the Python module using engineIOR() function.
 * That function can load module engine using appropriate container if required.
 * If this function is not available in Python module, the default implementation
 * is used which loads engine to the default FactoryServerPy container.
 */
void SALOME_PYQT_Module::getEngineIOR()
{
  myIOR = "";

  // Python interpreter should be initialized and Python module should be
  // import first
  if ( !myInterp || !myModule )
    return;

  if ( PyObject_HasAttrString( myModule , "engineIOR" ) ) {
    PyObjWrapper res( PyObject_CallMethod( myModule, (char*)"engineIOR", (char*)"" ) );
    if ( !res ) {
      PyErr_Print();
    }
    else {
      // parse the return value, result chould be string
      if ( PyString_Check( res ) ) {
        myIOR = PyString_AsString( res );
      }
    }
  }
  else if ( !CORBA::is_nil( getEngine() ) )
    myIOR = QString( getApp()->orb()->object_to_string( getEngine() ) );
}

CAM_DataModel* SALOME_PYQT_Module::createDataModel()
{
  MESSAGE( "SALOME_PYQT_Module::createDataModel()" );
  CAM_DataModel * dm = SalomeApp_Module::createDataModel();
  return dm;
}

/*!
  \brief Process GUI action (from main menu, toolbar or 
  context popup menu action).
*/
void SALOME_PYQT_Module::onGUIEvent(){
  SALOME_PYQT_ModuleLight::onGUIEvent();
}

/*!
  \brief Signal handler closing(SUIT_ViewWindow*) of a view
  \param pview view being closed
*/
void SALOME_PYQT_Module::onViewClosed( SUIT_ViewWindow* pview )
{
  SALOME_PYQT_ModuleLight::onViewClosed( pview );
}

/*!
  \brief Signal handler tryClose(SUIT_ViewWindow*) of a view
  \param pview view user tries to close
*/
void SALOME_PYQT_Module::onViewTryClose( SUIT_ViewWindow* pview )
{
  SALOME_PYQT_ModuleLight::onViewTryClose( pview );
}

/*!
  \breif Process application preferences changing.

  Called when any application setting is changed.

  \param module preference module
  \param section preference resource file section
  \param setting preference resource name
*/
void SALOME_PYQT_Module::preferenceChanged( const QString& module, 
                                            const QString& section, 
                                            const QString& setting )
{
  SALOME_PYQT_ModuleLight::preferenceChanged(module,section,setting);
}

/*!
  \brief Signal handler windowActivated(SUIT_ViewWindow*) of SUIT_Desktop
  \param pview view being activated
*/
void SALOME_PYQT_Module::onActiveViewChanged( SUIT_ViewWindow* pview )
{
  SALOME_PYQT_ModuleLight::onActiveViewChanged(pview);
}

/*!
  \brief Signal handler cloneView() of OCCViewer_ViewWindow
  \param pview view being cloned
*/
void SALOME_PYQT_Module::onViewCloned( SUIT_ViewWindow* pview )
{
  SALOME_PYQT_ModuleLight::onViewCloned(pview);
}
