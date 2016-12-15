// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : SALOME_PYQT_ModuleLight.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "SALOME_PYQT_DataModelLight.h"
#include "SALOME_PYQT_ModuleLight.h"
#include "SALOME_PYQT_PyModule.h"

#include "CAM_Application.h"
#include "SUITApp_init_python.hxx"
#include "SUIT_DataObjectIterator.h"
#include "LightApp_Application.h"
#include "SUIT_DataBrowser.h"
#include "sipAPISalomePyQtGUILight.h"

#ifndef GUI_DISABLE_CORBA
#include <Container_init_python.hxx>
#endif

#include <QCoreApplication>

// Py_ssize_t for old Pythons
// This code is as recommended by"
// http://www.python.org/dev/peps/pep-0353/#conversion-guidelines
//#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
//typedef int Py_ssize_t;
//# define PY_SSIZE_T_MAX INT_MAX
//# define PY_SSIZE_T_MIN INT_MIN
//#endif

//
// NB: Python requests.
// General rule for Python requests created by Python-based GUI modules
// (SALOME_PYQT_ModuleLight and other ones):
// all requests should be executed SYNCHRONOUSLY within the main GUI thread.
// However, it is obligatory that ANY Python call is wrapped with a request object,
// so that ALL Python API calls are serialized with PyInterp_Dispatcher.
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

extern "C" 
{
  SALOME_PYQT_LIGHT_EXPORT CAM_Module* createModule() 
  {
    QCoreApplication* app = QCoreApplication::instance();
    bool alreadyInitialized = app && app->property( "salome_pyqt_gui_light_initialized" ).toBool();

    // make initialization only once (see comment above) !
    if ( !alreadyInitialized ) {
      PyLockWrapper lck; // GIL acquisition
      INIT_FUNCTION();
      if ( app ) app->setProperty( "salome_pyqt_gui_light_initialized", true );
    }

    return new SALOME_PYQT_ModuleLight();
  }
}

/*!
  \class SALOME_PYQT_ModuleLight
  \brief This class implements GUI module for "light-weight" (no-CORBA-engine)
  Python-based SALOME modules.
*/

/*!
  \brief Constructor
*/
SALOME_PYQT_ModuleLight::SALOME_PYQT_ModuleLight()
  : LightApp_Module( "noname" ) // name is set explicitly at the module initialization
{
  // initialize helper
  myHelper = new PyModuleHelper( this );
}

/*!
  \brief Destructor
*/
SALOME_PYQT_ModuleLight::~SALOME_PYQT_ModuleLight()
{
  // as myHelper is a QObject, it should be deleted automatically
}

/*!
  \brief Initialization of the module.
  \param app parent application object
  \sa PyModuleHelper::initialize()
*/
void SALOME_PYQT_ModuleLight::initialize( CAM_Application* app )
{
  // call base implementation
  LightApp_Module::initialize( app );

  // ... then call helper
  myHelper->initialize( app );
  SUIT_DataBrowser* ob = getApp()->objectBrowser();
  if (ob && ob->model()) {
    connect( ob->model(), SIGNAL( clicked( SUIT_DataObject*, int ) ),
             myHelper, SLOT( onObjectBrowserClicked( SUIT_DataObject*, int ) ), Qt::UniqueConnection );
  }
}

/*!
  \brief Activation of the module.
  \param study parent study
  \return \c true if activation is successful and \c false otherwise
  \sa PyModuleHelper::activate()
*/
bool SALOME_PYQT_ModuleLight::activateModule( SUIT_Study* study )
{
  // call base implementation and then helper
  return LightApp_Module::activateModule( study ) && myHelper->activate( study );
}

/*!
  \brief Deactivation of the module.
  \param study parent study
  \return \c true if deactivation is successful and \c false otherwise
  \sa PyModuleHelper::deactivate()
*/
bool SALOME_PYQT_ModuleLight::deactivateModule( SUIT_Study* study )
{  
  // call helper
  bool res = myHelper->deactivate( study );
    
  // ... then call base implementation
  return LightApp_Module::deactivateModule( study ) && res;
}

/*!
  \brief Close of the module.

  This function is usually used in order to close the module's 
  specific menus and toolbars and perform other such actions
  required when the module is closed.
*/
void SALOME_PYQT_ModuleLight::onModelClosed()
{
  // call helper
  myHelper->modelClosed(application()->activeStudy());
  LightApp_Module::onModelClosed();
}


/*!
  \brief Get the dockable windows associated with the module.
  \param winMap output map of dockable windows in form { <window_type> : <dock_area> }
  \sa PyModuleHelper::windows()
*/
void SALOME_PYQT_ModuleLight::windows( QMap<int, int>& winMap ) const
{
  // get list of dockable windows from helper
  winMap = myHelper->windows();
}

/*!
  \brief Define the compatible view windows associated with the module.
  \param viewList output list of view windows types
  \sa PyModuleHelper::viewManagers()
*/
void SALOME_PYQT_ModuleLight::viewManagers( QStringList& viewList ) const
{
  // get list of view types from helper
  viewList = myHelper->viewManagers();
}

/*!
  \brief Process study activation.
  \sa PyModuleHelper::studyActivated()
*/
void SALOME_PYQT_ModuleLight::studyActivated()
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
void SALOME_PYQT_ModuleLight::contextMenuPopup( const QString& context, 
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
void SALOME_PYQT_ModuleLight::createPreferences()
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
void SALOME_PYQT_ModuleLight::preferencesChanged( const QString& section, const QString& parameter )
{
  // call helper
  myHelper->preferencesChanged( section, parameter );
}

/*!
  \brief Save module data. Called when user saves study.
  \param files output list of files where module stores data
  \param url study URL
  \sa PyModuleHelper::save()
*/
void SALOME_PYQT_ModuleLight::save( QStringList& files, const QString& url )
{
  // call helper
  myHelper->save( files, url );
}

/*
  \brief Load module data. Called when user opens study 
  and activates module.
  \param files list of files where module data is stored
  \param url study URL
  \return \c true if loading has been finished successfully or \c false otherwise
  \sa PyModuleHelper::load()
*/
bool SALOME_PYQT_ModuleLight::load( const QStringList& files, const QString& url )
{
  // call helper
  return myHelper->load( files, url );
}

/*!
  \brief Dump module data to the Python script. 
  Called when user activates dump study operation.
  \param files output list of files where module stores python script
  \sa PyModuleHelper::dumpPython()
*/
void SALOME_PYQT_ModuleLight::dumpPython( QStringList& files )
{
  // call helper
  myHelper->dumpPython( files );
}

/*!
  \brief Test if object \a what can be dragged by the user.
  \param what data object being tested
  \return \c true if object can be dragged or \c false otherwise
  \sa PyModuleHelper::isDraggable()
*/
bool SALOME_PYQT_ModuleLight::isDraggable( const SUIT_DataObject* what ) const
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
bool SALOME_PYQT_ModuleLight::isDropAccepted( const SUIT_DataObject* where ) const
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
void SALOME_PYQT_ModuleLight::dropObjects( const DataObjectList& what, SUIT_DataObject* where,
					   const int row, Qt::DropAction action )
{
  // call helper
  myHelper->dropObjects( what, where, row, action );
}

/*!
  \brief Create new empty data object
  \param parent entry of parent data object
  \return entry of created data object
*/
QString SALOME_PYQT_ModuleLight::createObject( const QString& parent )
{
  QString entry;
  SALOME_PYQT_DataObjectLight* obj = 0;

  if ( !parent.isEmpty() ) {
    SALOME_PYQT_DataObjectLight* parentObj = findObject( parent );
    if ( parentObj )
      obj = new SALOME_PYQT_DataObjectLight( parentObj );
  }
  else {
    SALOME_PYQT_DataModelLight* dm =
      dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel() );
    if ( dm )
      obj = new SALOME_PYQT_DataObjectLight( dm->getRoot() );
  }  
  if ( obj )
    entry = obj->entry();
  return entry;
}

/*!
 \brief Create new data object with specified name, icon and tooltip
 \param name data object name
 \param icon data object icon
 \param toolTip data object tooltip
 \param parent entry of parent data object
 \return entry of created data object
*/
QString SALOME_PYQT_ModuleLight::createObject( const QString& name,
					       const QString& icon,
					       const QString& toolTip,
					       const QString& parent )
{
  QString entry = createObject( parent );
  SALOME_PYQT_DataObjectLight* obj = findObject( entry );
  if ( obj ) {
    obj->setName( name);
    obj->setToolTip( toolTip );
    obj->setIcon( icon );
  }
  return entry;
}

/*!
  \brief Set data object name
  \param entry data object entry
  \param name data object name
*/
void SALOME_PYQT_ModuleLight::setName( const QString& entry, const QString& name )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    dataObj->setName( name );
}

/*!
  \brief Get data object name
  \param entry data object entry
  \return data object name
*/
QString SALOME_PYQT_ModuleLight::getName( const QString& entry ) const
{
  QString name;
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    name = dataObj->name();
  return name;
}

/*!
  \brief Set data object icon
  \param entry data object entry
  \param icon data object icon file name (icon is loaded from module resources)
*/
void SALOME_PYQT_ModuleLight::setIcon( const QString& entry, const QString& icon )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    dataObj->setIcon( icon );
}

/*!
  \brief Set data object tooltip
  \param entry data object entry
  \param toolTip data object tooltip
*/
void SALOME_PYQT_ModuleLight::setToolTip( const QString& entry, const QString& toolTip )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    dataObj->setToolTip( toolTip );
}

/*!
  \brief Get data object tooltip
  \param entry data object entry
  \return data object tooltip
*/
QString SALOME_PYQT_ModuleLight::getToolTip( const QString& entry ) const
{
  QString toolTip;
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    toolTip = dataObj->toolTip();
  return toolTip;
}

/*!
  \brief Set data object color
  \param entry data object entry
  \param color data object color
 */
void SALOME_PYQT_ModuleLight::setColor( const QString& entry, const QColor& color )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    dataObj->setColor( color );
}

/*!
  \brief Get data object color
  \param entry data object entry
  \return data object color
*/
QColor SALOME_PYQT_ModuleLight::getColor( const QString& entry ) const
{
  QColor color;
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    color = dataObj->color( SUIT_DataObject::Foreground );
  return color;
}

void SALOME_PYQT_ModuleLight::setObjectPosition( const QString& theEntry, int thePos )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( theEntry );
  if ( dataObj )
    dataObj->setPosition(thePos);
}

int SALOME_PYQT_ModuleLight::getObjectPosition( const QString& theEntry )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( theEntry );
  if ( dataObj )
    return dataObj->position();
  return -1;
}


/*!
  \brief Set reference to another data object
  \param entry data object entry
  \param refEntry referenced data object entry
*/
void SALOME_PYQT_ModuleLight::setReference( const QString& entry, const QString& refEntry )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    dataObj->setRefEntry( refEntry );
}

/*
  \brief Get entry of the referenced object (if there's any)
  \param entry data object entry
  \return referenced data object entry
*/
QString SALOME_PYQT_ModuleLight::getReference( const QString& entry ) const
{
  QString refEntry;
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj )
    refEntry = dataObj->refEntry();
  return refEntry;
}

/*!
  \brief Remove object by entry
  \param entry data object entry
*/
void SALOME_PYQT_ModuleLight::removeObject( const QString& entry )
{
  SALOME_PYQT_DataObjectLight* dataObj = findObject( entry );
  if ( dataObj && dataObj->parent() )
    dataObj->parent()->removeChild( dataObj );
}

/*!
  \brief Remove all child data objects from specified data object
  \param entry data object entry
*/
void SALOME_PYQT_ModuleLight::removeChildren( const QString& entry )
{
  SUIT_DataObject* dataObj = 0;
  if ( !entry.isEmpty() ) {
    dataObj = findObject( entry );
  }
  else {
    SALOME_PYQT_DataModelLight* dm =
      dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel() );
    if ( dm ) dataObj = dm->getRoot();
  }
  if ( dataObj ) {
    DataObjectList children;
    dataObj->children( children );
    QListIterator<SUIT_DataObject*> it( children );
    while ( it.hasNext() ) {
      dataObj->removeChild( it.next() );
    }
  }
}

/*!
  \brief Get entries of all child data objects of specified data object
  \param entry data object entry
  \param recursive \c true for recursive processing
*/
QStringList SALOME_PYQT_ModuleLight::getChildren( const QString& entry, const bool recursive ) const
{
  QStringList entryList;
  SUIT_DataObject* dataObj = 0;
  if ( !entry.isEmpty() ) {
    dataObj = findObject( entry );
  }
  else {
    SALOME_PYQT_DataModelLight* dm =
      dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel() );
    if ( dm ) dataObj = dm->getRoot();
  }
  if ( dataObj ) {
    DataObjectList lst;
    dataObj->children( lst, recursive );
    QListIterator<SUIT_DataObject*> it( lst );
    while ( it.hasNext() ) {
      SALOME_PYQT_DataObjectLight* sobj = dynamic_cast<SALOME_PYQT_DataObjectLight*>( it.next() );
      entryList.append( sobj->entry() );
    }
  }
  return entryList;
}

/*!
  \brief Create new instance of data model and return it.
*/
CAM_DataModel* SALOME_PYQT_ModuleLight::createDataModel()
{
  return new SALOME_PYQT_DataModelLight( this );
}

/*!
  \brief Find data object by its entry
  \param entry data object entry
  \return data object with given entry or 0 if object isn't found
*/
SALOME_PYQT_DataObjectLight* SALOME_PYQT_ModuleLight::findObject( const QString& entry ) const
{
  SALOME_PYQT_DataObjectLight* obj = 0;
  SALOME_PYQT_DataModelLight* dm =
    dynamic_cast<SALOME_PYQT_DataModelLight*>( dataModel() );
  if ( !entry.isEmpty() && dm ) {
    for ( SUIT_DataObjectIterator it( dm->getRoot(), SUIT_DataObjectIterator::DepthLeft ); it.current() && !obj; ++it ) { 
      SALOME_PYQT_DataObjectLight* curentobj =
        dynamic_cast<SALOME_PYQT_DataObjectLight*>( it.current() );
      if ( curentobj && curentobj->entry() == entry )
        obj = curentobj;
    }
  }
  return obj;
}
