#include "CAM_Study.h"

#include "CAM_DataModel.h"
#include "CAM_DataObject.h"
#include "CAM_RootObject.h"
#include "CAM_Module.h"

CAM_Study::CAM_Study( SUIT_Application* app )
: SUIT_Study( app )
{
}

CAM_Study::~CAM_Study()
{
}

void CAM_Study::closeDocument(bool permanently)
{
  for ( ModelListIterator it( myDataModels ); it.current(); ++it )
    it.current()->close();

  SUIT_Study::closeDocument(permanently);
}

bool CAM_Study::appendDataModel( const CAM_DataModel* dm )
{
  return insertDataModel( dm, myDataModels.count() );
}

bool CAM_Study::insertDataModel( const CAM_DataModel* dm, const CAM_DataModel* other )
{
  int idx = myDataModels.findRef( other );
  return insertDataModel( dm, idx < 0 ? idx : idx + 1 );
}

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

bool CAM_Study::removeDataModel( const CAM_DataModel* dm )
{
  if ( !dm )
    return true;

  CAM_RootObject* aModelRoot = dynamic_cast<CAM_RootObject*>( dm->root() );
  if ( aModelRoot )
    aModelRoot->setDataModel( 0 );

  return myDataModels.remove( dm );
}

bool CAM_Study::containsDataModel( const CAM_DataModel* dm ) const
{
  return myDataModels.contains( dm );
}

void CAM_Study::dataModels( ModelList& lst ) const
{
  lst.clear();
  for ( ModelListIterator it( myDataModels ); it.current(); ++it )
    lst.append( it.current() );
}

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

bool CAM_Study::openDataModel( const QString&, CAM_DataModel* )
{
  return false;
}

bool CAM_Study::saveDataModel( const QString&, CAM_DataModel* )
{
  return false;
}

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
