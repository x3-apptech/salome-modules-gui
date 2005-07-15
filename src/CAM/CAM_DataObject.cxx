#include "CAM_DataObject.h"

#include "CAM_Module.h"
#include "CAM_DataModel.h"

/*!Constructor. Sets parent object.*/
CAM_DataObject::CAM_DataObject( SUIT_DataObject* parent )
: SUIT_DataObject( parent )
{
}

/*!Destructor.Do nothing*/
CAM_DataObject::~CAM_DataObject()
{
}

/*!Get module.
 *\retval const CAM_Module pointer - module
 */
CAM_Module* CAM_DataObject::module() const
{ 
  CAM_Module* mod = 0;

  CAM_DataModel* data = dataModel();
  if ( data )
    mod = data->module();

  return mod;
}

/*!Get data model.
 *Return 0 - if no parent obbject.
 *\retval const CAM_DataModel pointer - data model
 */
CAM_DataModel* CAM_DataObject::dataModel() const
{
  CAM_DataObject* parentObj = dynamic_cast<CAM_DataObject*>( parent() );

  if ( !parentObj )
    return 0;

  return parentObj->dataModel();
}
