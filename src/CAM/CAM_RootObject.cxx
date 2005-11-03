#include "CAM_RootObject.h"

#include "CAM_DataModel.h"
#include "CAM_Module.h"

/*!Constructor. Initialize by \a parent.
 * Set data model to 0.
 */
CAM_RootObject::CAM_RootObject( SUIT_DataObject* parent )
: CAM_DataObject( parent ),
myDataModel( 0 )
{
}

/*!Constructor. Initialize by \a parent and \a data - data object
 *\param data - data object
 *\param parent - parent data object
 */
CAM_RootObject::CAM_RootObject( CAM_DataModel* data, SUIT_DataObject* parent )
: CAM_DataObject( parent ),
myDataModel( data )
{
}

/*!Destructor. Do nothing.*/
CAM_RootObject::~CAM_RootObject()
{
}

/*!
    Returns module name
*/
QString CAM_RootObject::name() const
{
  QString aName = "";
  if (myDataModel)
    aName = myDataModel->module()->moduleName();
  return aName;
}

/*!Get data model
 *\retval const CAM_DataModel pointer to data model.
 */
CAM_DataModel* CAM_RootObject::dataModel() const
{
  return myDataModel;
}

/*!Set data model.
 *\param dm - data model to set.
 */
void CAM_RootObject::setDataModel( CAM_DataModel* dm )
{
  myDataModel = dm;
}
