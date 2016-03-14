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

// File:      LightApp_DataModel.cxx
// Created:   10/25/2004 10:36:06 AM
// Author:    Sergey LITONIN
//
#include "LightApp_DataModel.h"
#include "LightApp_Study.h"
#include "LightApp_DataObject.h"
#include "LightApp_Module.h"
#include "LightApp_Application.h"

#include <SUIT_TreeModel.h>
#include <SUIT_DataBrowser.h>
#include <SUIT_DataObject.h>

/*!
  Constructor
*/
LightApp_DataModel::LightApp_DataModel( CAM_Module* theModule )
: CAM_DataModel( theModule )
{
  myGroupId = 0;
  if( module() )
        myGroupId = qHash( module()->name() );
}

/*!
  Destructor
*/
LightApp_DataModel::~LightApp_DataModel()
{
}

/*!
  Emit opened()
*/
bool LightApp_DataModel::open( const QString&, CAM_Study* study, QStringList )
{
  emit opened(); //TODO: is it really needed? to be removed maybe...
  return true;
}

/*!
  Emit saved()
*/
bool LightApp_DataModel::save( QStringList& )
{
  emit saved();
  return true;
}

/*!
  Emit saved()
*/
bool LightApp_DataModel::saveAs( const QString&, CAM_Study*, QStringList& )
{
  emit saved();
  return true;
}

/*!
  Does nothing by default. Should be redefined in light modules
  that want to participate in "Dump study" operation.
*/
bool LightApp_DataModel::dumpPython( const QString&, CAM_Study*, bool, QStringList& )
{
  return true;
}

/*!
  Emit closed()
*/
bool LightApp_DataModel::close()
{
  emit closed();
  return true;
}

/*!
  Build whole data model tree
*/
void LightApp_DataModel::build()
{
}

/*!
  Updates data model presentation in some widgets (for example, in object browser
*/
void LightApp_DataModel::updateWidgets()
{
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( module()->application() );
  if ( app )
    app->objectBrowser()->updateTree( 0, false );
}

/*!
  Default behaviour of data model update for light modules
*/
void LightApp_DataModel::update( LightApp_DataObject*, LightApp_Study* )
{
  // san: Previously modelRoot was casted to LightApp_ModuleObject*,
  // BUT this is incorrect: in full SALOME the model root has different type.
  // Hopefully LightApp_DataObject* is sufficient here.
  LightApp_DataObject* modelRoot = dynamic_cast<LightApp_DataObject*>( root() );
  DataObjectList ch;
  QMap<SUIT_DataObject*,int> aMap;
  if( modelRoot )
  {
    ch = modelRoot->children();
    QListIterator<SUIT_DataObject*> it( ch );
    while ( it.hasNext() )
      it.next()->setParent( 0 );
  }

  build();

  modelRoot = dynamic_cast<LightApp_DataObject*>( root() );
  if( modelRoot )
  {
    DataObjectList new_ch = modelRoot->children();
    QListIterator<SUIT_DataObject*> it1( new_ch );
    while ( it1.hasNext() )
      aMap.insert( it1.next(), 0 );
  }

  updateWidgets();

  QListIterator<SUIT_DataObject*> it( ch );
  while ( it.hasNext() ) {
    SUIT_DataObject* aDO = it.next();
    if( !aMap.contains( aDO ) )
      delete aDO;
  }
}

/*!
  \return corresponding module 
*/
LightApp_Module* LightApp_DataModel::getModule() const
{
  return dynamic_cast<LightApp_Module*>( module() );
}

/*!
  \return corresponding  study
*/
LightApp_Study* LightApp_DataModel::getStudy() const
{
  LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( root()->root() );
  if ( !aRoot )
    return 0;
  return aRoot->study();
}

/*!
  default implementation, always returns false so as not to mask study's isModified()
*/
bool LightApp_DataModel::isModified() const
{
  return false;
}

/*!
  default implementation, always returns true so as not to mask study's isSaved()
*/
bool LightApp_DataModel::isSaved() const
{
  return true;
}

/*!
  \return data model group id used for custom columns creation
*/
int LightApp_DataModel::groupId() const
{
  return myGroupId;
}

/*!
  Register custom column in the object browser
  \param browser - object browser where new column should be created
  \param name - translated column name
  \param custom_id - custom column identificator passed into data object's methods text(), icon() etc
*/
void LightApp_DataModel::registerColumn( SUIT_DataBrowser* browser, const QString& name, const int custom_id )
{
  SUIT_AbstractModel* m = dynamic_cast<SUIT_AbstractModel*>( browser ? browser->model() : 0 );
  if( m )
    m->registerColumn( groupId(), name, custom_id );
}

/*!
  Remove registered custom column from the object browser
  \param browser - object browser where new column should be created
  \param name - translated column name
*/
void LightApp_DataModel::unregisterColumn( SUIT_DataBrowser* browser, const QString& name )
{
  SUIT_AbstractModel* m = dynamic_cast<SUIT_AbstractModel*>( browser ? browser->model() : 0 );
  if( m )
    m->unregisterColumn( groupId(), name );
}

/*!
  Creates the data model's root (module object) using the study services.
  This is important because different study classes use different moduel object classes.
  Therefore creation of the module object cannot be done at the data model level
  where the type of the current study instance should not be known.
  The module object returned by this method should be then passed to the model's setRoot().
  \return the module object instance corresponding to the study type
  \sa CAM_DataModel class
*/
CAM_ModuleObject* LightApp_DataModel::createModuleObject( SUIT_DataObject* theRoot ) const
{
  LightApp_RootObject* aStudyRoot = dynamic_cast<LightApp_RootObject*>( theRoot );
  if ( !aStudyRoot )
    return 0;

  LightApp_Study* aStudy = aStudyRoot->study();
  if ( aStudy )
    return aStudy->createModuleObject( const_cast<LightApp_DataModel*>( this ), 
				       theRoot );
  return 0;
}
