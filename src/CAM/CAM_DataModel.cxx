#include "CAM_DataModel.h"

#include "CAM_Module.h"
#include "CAM_RootObject.h"

/*!Constructor. Initialise module by \a module.*/
CAM_DataModel::CAM_DataModel( CAM_Module* module )
: myRoot( 0 ),
myModule( module )
{
}

/*!Destructor. Do nothing.*/
CAM_DataModel::~CAM_DataModel()
{
}

void CAM_DataModel::initialize()
{
  //! Default implementation, does nothing.\n
  //! Can be used for creation of root object.
}

/*!Get root object.
 *\retval CAM_DataObject pointer - root object.
 */
CAM_DataObject* CAM_DataModel::root() const
{
  return myRoot;
}

/*!Sets root object to \a newRoot.\n
 *Emit root changed, if it was.
 *\param newRoot - new root object
 */
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

/*!Gets module.
 *\retval CAM_Module pointer - module.
 */
CAM_Module* CAM_DataModel::module() const
{
  return myModule;
}

/*!Nullify root, if \a obj equal root.*/
void CAM_DataModel::onDestroyed( SUIT_DataObject* obj )
{
  if ( myRoot == obj )
    myRoot = 0;
}
