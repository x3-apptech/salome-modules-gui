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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
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
  QMap<SUIT_DataObject*,int> aMap;
  if( modelRoot )
  {
    ch = modelRoot->children();
    for ( DataObjectListIterator it( ch ); it.current(); ++it )
      it.current()->setParent( 0 );
  }

  build();

  modelRoot = dynamic_cast<LightApp_ModuleObject*>( root() );
  if( modelRoot )
  {
    DataObjectList new_ch = modelRoot->children();
    for ( DataObjectListIterator it1( new_ch ); it1.current(); ++it1 )
      aMap.insert( it1.current(), 0 );
  }

  updateWidgets();

  for( DataObjectListIterator it( ch ); it.current(); ++it )
    if( !aMap.contains( it.current() ) )
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
