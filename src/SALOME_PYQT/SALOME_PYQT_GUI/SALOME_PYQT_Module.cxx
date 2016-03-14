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
// File   : SALOME_PYQT_Module.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "SALOME_PYQT_Module.h"
#include "SALOME_PYQT_PyModule.h"
#include "SalomeApp_Application.h"

#include <SALOME_LifeCycleCORBA.hxx>
#include <Container_init_python.hxx>

#include <QCoreApplication>

//
// NB: Python requests.
// General rule for Python requests created by Python-based GUI modules
// (SALOME_PYQT_Module and other ones):
// all requests should be executed SYNCHRONOUSLY within the main GUI thread.
// However, it is obligatory that ANY Python call is wrapped with a request object,
// so that ALL Python API calls are serialized with PyInterp_Dispatcher.
//
// NB: Library initialization
// Since the SalomePyQtGUI library is not imported in Python it's initialization function
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
  SALOME_PYQT_EXPORT CAM_Module* createModule()
  {
    QCoreApplication* app = QCoreApplication::instance();
    bool alreadyInitialized = app && app->property( "salome_pyqt_gui_light_initialized" ).toBool();

    if ( !alreadyInitialized ) {
      PyLockWrapper lck; // GIL acquisition
      INIT_FUNCTION();
      if ( app ) app->setProperty( "salome_pyqt_gui_light_initialized", true );
    }

    return new SALOME_PYQT_Module();
  }
}

/*!
  \class SALOME_PYQT_Module
  \brief This class implements GUI module for CORBA engine-based Python SALOME modules.
*/

/*!
  \brief Constructor
*/
SALOME_PYQT_Module::SALOME_PYQT_Module()
  : SalomeApp_Module( "noname" ) // name is set explicitly at the module initialization
{
  // initialize helper
  myHelper = new PyModuleHelper( this );
}

/*!
  \brief Destructor
*/
SALOME_PYQT_Module::~SALOME_PYQT_Module()
{
  // as myHelper is a QObject, it should be deleted automatically
}

/*!
  \brief Get module engine IOR
  
  This function tries to get engine IOR from the Python module using engineIOR() function.
  That function can load module engine using appropriate container if required.
  If this function is not available in Python module, the default implementation
  is used which loads engine to the default FactoryServer container.
*/
QString SALOME_PYQT_Module::engineIOR() const
{
  // call helper to get IOR from Python module
  static QString ior;

  if ( ior.isEmpty() ) {
    // first call helper to get IOR from Python module
    ior = myHelper->engineIOR();
  }
  if ( ior.isEmpty() ) {
    // if IOR is still not specified, try default implementation
    // which loads engine to the default FactoryServer container.
    Engines::EngineComponent_var comp;
    // temporary solution
    try {
      comp = getApp()->lcc()->FindOrLoad_Component( "FactoryServer", name().toLatin1() );
    }
    catch (CORBA::Exception&) {
    }
    if ( !CORBA::is_nil( comp ) )
      ior = QString( getApp()->orb()->object_to_string( comp.in() ) );
  }

  return ior;
}

/*!
  \brief Initialization of the module.
  \param app parent application object
  \sa PyModuleHelper::initialize()
*/
void SALOME_PYQT_Module::initialize( CAM_Application* app )
{
  // call base implementation
  SalomeApp_Module::initialize( app );

  // ... then call helper
  myHelper->initialize( app );
}

/*!
  \brief Activation of the module.
  \param study parent study
  \return \c true if activation is successful and \c false otherwise
  \sa PyModuleHelper::activate()
*/
bool SALOME_PYQT_Module::activateModule( SUIT_Study* study )
{
  // call base implementation and then helper
  return SalomeApp_Module::activateModule( study ) && myHelper->activate( study );
}

/*!
  \brief Deactivation of the module.
  \param study parent study
  \return \c true if deactivation is successful and \c false otherwise
  \sa PyModuleHelper::deactivate()
*/
bool SALOME_PYQT_Module::deactivateModule( SUIT_Study* study )
{
  // call helper
  bool res = myHelper->deactivate( study );
    
  // ... then call base implementation
  return SalomeApp_Module::deactivateModule( study ) && res;
}

/*!
  \brief Get the dockable windows associated with the module.
  \param winMap output map of dockable windows in form { <window_type> : <dock_area> }
  \sa PyModuleHelper::windows()
*/
void SALOME_PYQT_Module::windows( QMap<int, int>& winMap ) const
{
  // get list of dockable windows from helper
  winMap = myHelper->windows();
}

/*!
  \brief Define the compatible view windows associated with the module.
  \param viewList output list of view windows types
  \sa PyModuleHelper::viewManagers()
*/
void SALOME_PYQT_Module::viewManagers( QStringList& viewList ) const
{
  // get list of view types from helper
  viewList = myHelper->viewManagers();
}

/*!
  \brief Process study activation.
  \sa PyModuleHelper::studyActivated()
*/
void SALOME_PYQT_Module::studyActivated()
{
  // call helper
  myHelper->studyActivated( application()->activeStudy() );
}

/*!
  \brief Process context popup menu request.
  \param context popup menu context (e.g. "ObjectBrowser")
  \param menu popup menu
  \param title popup menu title (not used)
  \sa PyModuleHelper::contextMenu()
*/
void SALOME_PYQT_Module::contextMenuPopup( const QString& context, 
					   QMenu*         menu, 
					   QString&       /*title*/ )
{
  // call helper
  myHelper->contextMenu( context, menu );
}

/*!
  \brief Export preferences for the Python module.
  \sa PyModuleHelper::createPreferences()
*/
void SALOME_PYQT_Module::createPreferences()
{
  // call helper
  myHelper->createPreferences();
}

/*!
  \brief Process module's preferences changing.
  \param section preference resources section
  \param parameter preference resources parameter name
  \sa PyModuleHelper::preferencesChanged()
*/
void SALOME_PYQT_Module::preferencesChanged( const QString& section, const QString& parameter )
{
  // call helper
  myHelper->preferencesChanged( section, parameter );
}

/*!
  \brief Called when study is closed
  \param study study being closed
  \sa PyModuleHelper::studyClosed()
*/
void SALOME_PYQT_Module::studyClosed( SUIT_Study* study )
{
  // call helper
  myHelper->modelClosed( study );
  SalomeApp_Module::studyClosed( study );
}

/*!
  \brief Test if object \a what can be dragged by the user.
  \param what data object being tested
  \return \c true if object can be dragged or \c false otherwise
  \sa PyModuleHelper::isDraggable()
*/
bool SALOME_PYQT_Module::isDraggable( const SUIT_DataObject* what ) const
{
  // call helper
  return myHelper->isDraggable( what );
}

/*!
  \brief Test if drop operation can be done on the \a where object.
  \param where data object being tested
  \return \c true if if drop operation is supported by object or \c false otherwise
  \sa PyModuleHelper::isDropAccepted()
*/
bool SALOME_PYQT_Module::isDropAccepted( const SUIT_DataObject* where ) const
{
  // call helper
  return myHelper->isDropAccepted( where );
}

/*!
  \brief Perform drop operation
  \param what list of data objects being dropped
  \param where target data object for drop operation
  \param row line (child item index) where drop operation is performed to
  \param action current drop action (copy or move)
  \sa PyModuleHelper::dropObjects()
*/
void SALOME_PYQT_Module::dropObjects( const DataObjectList& what, SUIT_DataObject* where,
				      const int row, Qt::DropAction action )
{
  // call helper
  myHelper->dropObjects( what, where, row, action );
}

