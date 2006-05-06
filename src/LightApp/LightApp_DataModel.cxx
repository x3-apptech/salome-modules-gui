// File:      LightApp_DataModel.cxx
// Created:   10/25/2004 10:36:06 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "LightApp_DataModel.h"
#include "LightApp_Study.h"
#include "LightApp_RootObject.h"
#include "LightApp_DataObject.h"
#include "LightApp_Module.h"
#include "LightApp_Application.h"

#include <OB_Browser.h>

#include <SUIT_Application.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_DataObject.h>

/*!
  Constructor
*/
LightApp_DataModel::LightApp_DataModel( CAM_Module* theModule )
: CAM_DataModel( theModule )
{
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
  if( app )
    app->objectBrowser()->updateTree( 0, false );
}

/*!
  Default behaviour of data model update for light modules
*/
void LightApp_DataModel::update( LightApp_DataObject*, LightApp_Study* )
{
  LightApp_ModuleObject* modelRoot = dynamic_cast<LightApp_ModuleObject*>( root() );
  DataObjectList ch;
  if( modelRoot )
  {
    ch = modelRoot->children();
    for ( DataObjectListIterator it( ch ); it.current(); ++it )
      it.current()->setParent( 0 );
  }
  build();
  updateWidgets();
  for( DataObjectListIterator it( ch ); it.current(); ++it )
    delete it.current();
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
