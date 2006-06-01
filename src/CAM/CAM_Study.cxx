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
#include "CAM_Study.h"

#include "CAM_DataModel.h"
#include "CAM_DataObject.h"
#include "CAM_RootObject.h"
#include "CAM_Module.h"

/*!Constructor.*/
CAM_Study::CAM_Study( SUIT_Application* app )
: SUIT_Study( app )
{
}

/*!Destructor*/
CAM_Study::~CAM_Study()
{
}

/*!Closing all data models and close document permanently(if \a permanently = true.)
 * \param permanently - flag
 */
void CAM_Study::closeDocument(bool permanently)
{
  for ( ModelListIterator it( myDataModels ); it.current(); ++it )
    it.current()->close();

  SUIT_Study::closeDocument(permanently);
}

/*!Append data model to list.
 * \param dm - data model for adding
 */
bool CAM_Study::appendDataModel( const CAM_DataModel* dm )
{
  return insertDataModel( dm, myDataModels.count() );
}

/*!Insert data model \a dm after \a other
 * \param dm - data model for adding
 * \param other - previus data model for \a dm
 */
bool CAM_Study::insertDataModel( const CAM_DataModel* dm, const CAM_DataModel* other )
{
  int idx = myDataModels.findRef( other );
  return insertDataModel( dm, idx < 0 ? idx : idx + 1 );
}

/*!Insert data model with index \a idx. \n
 * \param dm - data model
 * \param idx - index for inserting(must be no less zero)
 * \retval true - if model added successful, else false.
 */
bool CAM_Study::insertDataModel( const CAM_DataModel* dm, const int idx )
{
  if ( !dm || myDataModels.findRef( dm ) != -1 )
    return false;

  int pos = idx < 0 ? myDataModels.count() : idx;
  myDataModels.insert( QMIN( pos, (int)myDataModels.count() ), dm );

  connect( dm, SIGNAL( rootChanged( const CAM_DataModel* ) ), SLOT( updateModelRoot( const CAM_DataModel* ) ) );

  dataModelInserted( dm );

  return true;
}

/*! Remove data model from list
 * \param dm data model
 * \retval true - if all ok, else false.
 */
bool CAM_Study::removeDataModel( const CAM_DataModel* dm )
{
  if ( !dm )
    return true;

  CAM_RootObject* aModelRoot = dynamic_cast<CAM_RootObject*>( dm->root() );
  if ( aModelRoot )
    aModelRoot->setDataModel( 0 );

  return myDataModels.remove( dm );
}

/*!Check data model contains in list.
 * \param dm - data model
 * \retval true - if data model in list, else false.
 */
bool CAM_Study::containsDataModel( const CAM_DataModel* dm ) const
{
  return myDataModels.contains( dm );
}

/*!Gets list of all data models.
 * \param lst - output data model list.
 */
void CAM_Study::dataModels( ModelList& lst ) const
{
  lst.clear();
  for ( ModelListIterator it( myDataModels ); it.current(); ++it )
    lst.append( it.current() );
}

/*! Open data model \a dModel, if it saved*/
void CAM_Study::dataModelInserted( const CAM_DataModel* dModel )
{
  CAM_DataModel* dm = (CAM_DataModel*)dModel;

  if ( isSaved() ) // need to load data model from an exisitng file?
    openDataModel( studyName(), dm );
  else // no, just need to update data model's connection to study tree 
       //(some application may want to show model's root in a study tree even if a model is empty)
  {
    dm->create( this );
    updateModelRoot( dm );
  }
}

/*! \retval false*/
bool CAM_Study::openDataModel( const QString&, CAM_DataModel* )
{
  return false;
}

/*! \retval false*/
bool CAM_Study::saveDataModel( const QString&, CAM_DataModel* )
{
  return false;
}

/*! Public slot. Update model root.*/
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
    int idx = myDataModels.findRef( dm );
    if ( idx != -1 )
      root()->insertChild( dm->root(), idx );
  }
}
