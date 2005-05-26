#include "CAM_RootObject.h"

#include "CAM_DataModel.h"

CAM_RootObject::CAM_RootObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
myDataModel( 0 )
{
}

CAM_RootObject::CAM_RootObject( CAM_DataModel* data, SUIT_DataObject* parent )
: CAM_DataObject( parent ),
myDataModel( data )
{
}

CAM_RootObject::~CAM_RootObject()
{
}

CAM_DataModel* CAM_RootObject::dataModel() const
{
  return myDataModel;
}

void CAM_RootObject::setDataModel( CAM_DataModel* dm )
{
  myDataModel = dm;
}
