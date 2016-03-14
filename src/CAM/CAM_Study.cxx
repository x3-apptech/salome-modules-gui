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

#include "CAM_Study.h"

#include "CAM_DataModel.h"
#include "CAM_DataObject.h"
#include "CAM_Module.h"

/*!
  \class CAM_Study
  \brief Represents document object in the CAM application architecture.

  For each loaded module study contains the data model instance reference.
  Provides all necessary functionality for data models management.
*/

/*!
  \brief Constructor.
  \param app parent application
*/
CAM_Study::CAM_Study( SUIT_Application* app )
: SUIT_Study( app )
{
}

/*!
  \brief Destructor.
*/
CAM_Study::~CAM_Study()
{
}

/*!
  \brief Called when study is closed.

  Closes all data models.
  
  \param permanently if \c true close study permanently (not used in the base implemetation)
*/
void CAM_Study::closeDocument( bool permanently )
{
  for( QList<CAM_DataModel*>::const_iterator it = myDataModels.begin(); 
       it != myDataModels.end(); ++it )
    (*it)->close();

  SUIT_Study::closeDocument( permanently );
}

/*!
  \brief Append data model to the study.
  \param dm data model being added
  \return \c true on success and \c false on error
*/
bool CAM_Study::appendDataModel( const CAM_DataModel* dm )
{
  return insertDataModel( dm, myDataModels.count() );
}

/*!
  \brief Insert data model \a dm after data model \a other
  
  If \a other is 0, the data model is added to the end of list.

  \param dm data model being added
  \param other data model to be previous in the list
  \return \c true on success and \c false on error
*/
bool CAM_Study::insertDataModel( const CAM_DataModel* dm, const CAM_DataModel* other )
{
  int idx = myDataModels.indexOf( (CAM_DataModel*)other );
  return insertDataModel( dm, idx < 0 ? idx : idx + 1 );
}

/*!
  \brief Insert data model \a dm with index \a idx.
  
  \param dm data model being added
  \param idx data model required index
  \return \c true on success and \c false on error
*/
bool CAM_Study::insertDataModel( const CAM_DataModel* dm, const int idx )
{
  if ( !dm || myDataModels.indexOf( (CAM_DataModel*)dm ) != -1 )
    return false;

  int pos = idx < 0 ? myDataModels.count() : idx;
  myDataModels.insert( qMin( pos, (int)myDataModels.count() ), (CAM_DataModel*)dm );

  connect( dm, SIGNAL( rootChanged( const CAM_DataModel* ) ), SLOT( updateModelRoot( const CAM_DataModel* ) ) );

  dataModelInserted( dm );

  return true;
}

/*!
  \brief Remove data model from the study.
  \param dm data model being removed
  \return \c true on success and \c false on error
*/
bool CAM_Study::removeDataModel( const CAM_DataModel* dm )
{
  if ( !dm )
    return true;

  CAM_ModuleObject* aModelRoot = dynamic_cast<CAM_ModuleObject*>( dm->root() );
  if ( aModelRoot )
    aModelRoot->setDataModel( 0 );

  return myDataModels.removeAll( (CAM_DataModel*)dm );
}

/*!
  \brief Check if data model is contained in the list.
  \param dm data model
  \return \c true if data model is in the list and \c false otherwise.
*/
bool CAM_Study::containsDataModel( const CAM_DataModel* dm ) const
{
  return myDataModels.contains( (CAM_DataModel*)dm );
}

/*!
  \brief Get all data models.
  \param lst returning list of data model.
*/
void CAM_Study::dataModels( ModelList& lst ) const
{
  lst.clear();
  for( QList<CAM_DataModel*>::const_iterator it = myDataModels.begin(); 
       it != myDataModels.end(); ++it )
    lst.append( *it );
}

/*!
  \brief Called when data model is inserted in the study.
  
  Open data model \a dModel, if it is saved and update data tree.

  \param dModel data model
*/
void CAM_Study::dataModelInserted( const CAM_DataModel* dModel )
{
  CAM_DataModel* dm = (CAM_DataModel*)dModel;

  if ( isSaved() ) // need to load data model from an exisitng file?
    openDataModel( studyName(), dm );
  else // no, just need to update data model's connection to study tree 
       //(some application may want to show model's root in a study tree even if a model is empty)
    dm->create( this );
  updateModelRoot( dm );
}

/*!
  \brief Called when data model is opened.

  Base implementation does nothing and returns \c false.
  
  \return \c true on success and \c false on error
*/
bool CAM_Study::openDataModel( const QString&, CAM_DataModel* )
{
  return false;
}

/*!
  \brief Called when data model is saved.

  Base implementation does nothing and returns \c false.
  
  \return \c true on success and \c false on error
*/
bool CAM_Study::saveDataModel( const QString&, CAM_DataModel* )
{
  return false;
}

/*!
  \brief Update data model root object.
  \param dm data model being updated.
*/
void CAM_Study::updateModelRoot( const CAM_DataModel* dm )
{
  if ( !root() )
    return;

  DataObjectList childList;
  root()->children( childList );
  CAM_DataObject* curRoot = 0;
  QString aName = dm->root() ? dm->root()->name() : dm->module()->moduleName();
  int i = 0;
  for ( int n = childList.count(); i < n; i++ ) {
    if ( childList.at( i )->name() == aName ) {
      curRoot = dynamic_cast<CAM_DataObject*>( childList.at( i ) );
      break;
    }
  }

  if ( curRoot == dm->root() )
    return;

  // replacing old data model root with a new one - old root deleted here !
  if ( curRoot )
    root()->replaceChild( curRoot, dm->root(), true );
  else {
    int idx = myDataModels.indexOf( (CAM_DataModel*)dm );
    if ( idx != -1 )
      root()->insertChild( dm->root(), idx );
  }
}
