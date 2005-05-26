#include "CAM_DataModel.h"

#include "CAM_Module.h"
#include "CAM_RootObject.h"

CAM_DataModel::CAM_DataModel( CAM_Module* module )
: myRoot( 0 ),
myModule( module )
{
}

CAM_DataModel::~CAM_DataModel()
{
}

void CAM_DataModel::initialize()
{
  // Default implementation, does nothing.
  // Can be used for creation of root object.
}

CAM_DataObject* CAM_DataModel::root() const
{
  return myRoot;
}

void CAM_DataModel::setRoot( const CAM_DataObject* newRoot )
{
  if ( myRoot == newRoot )
    return;

  if ( myRoot )
    myRoot->disconnect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );

  myRoot = (CAM_DataObject*)newRoot;

  if ( myRoot )
    myRoot->connect( this, SLOT( onDestroyed( SUIT_DataObject* ) ) );

  emit rootChanged( this );
}

CAM_Module* CAM_DataModel::module() const
{
  return myModule;
}

void CAM_DataModel::onDestroyed( SUIT_DataObject* obj )
{
  if ( myRoot == obj )
    myRoot = 0;
}
