#include "LightApp_DataObject.h"

#include "LightApp_Study.h"
#include "LightApp_RootObject.h"

#include "CAM_DataModel.h"
#include "CAM_Module.h"

#include <SUIT_Application.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_DataObjectKey.h>

#include <qobject.h>

/*!
	Class: LightApp_DataObject::Key
	Level: Internal
*/
class LightApp_DataObject::Key : public SUIT_DataObjectKey
{
public:
  Key( const QString& );
  virtual ~Key();

  virtual bool isLess( const SUIT_DataObjectKey* ) const;
  virtual bool isEqual( const SUIT_DataObjectKey* ) const;

private:
  QString myEntry;
};

/*!Constructor. Initialize by \a entry.*/
LightApp_DataObject::Key::Key( const QString& entry )
: SUIT_DataObjectKey(),
  myEntry( entry )
{
}

/*!Destructor. Do nothing.*/
LightApp_DataObject::Key::~Key()
{
}

/*!Checks: Is current key less than \a other.*/
bool LightApp_DataObject::Key::isLess( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry < that->myEntry;
}

/*!Checks: Is current key equal with \a other.*/
bool LightApp_DataObject::Key::isEqual( const SUIT_DataObjectKey* other ) const
{
  Key* that = (Key*)other;
  return myEntry == that->myEntry;
}

/*
	Class: LightApp_DataObject
	Level: Public
*/
/*!Constructor. Initialize by \a parent*/
LightApp_DataObject::LightApp_DataObject( SUIT_DataObject* parent )
: CAM_DataObject( parent )
{
}

/*!Destructor. Do nothing.*/
LightApp_DataObject::~LightApp_DataObject()
{
}

/*!Gets object ID.
 *\retval QString
 */
QString LightApp_DataObject::entry() const
{
  return QString::null;
}

/*!Create and return new key object.*/
SUIT_DataObjectKey* LightApp_DataObject::key() const
{
  QString str = entry();
  return new Key( str );
}

/*!Gets component object.
 *\retval SUIT_DataObject.
 */
SUIT_DataObject* LightApp_DataObject::componentObject() const
{
  SUIT_DataObject* compObj = 0; // for root object

  if ( parent() && parent() == root() ) 
    compObj = (SUIT_DataObject*)this; // for component-level objects
  else 
  {
    compObj = parent(); // for lower level objects
    while ( compObj && compObj->parent() != root() )
      compObj = compObj->parent();
  }
  return compObj;
}

/*!Get component type.*/
QString LightApp_DataObject::componentDataType() const
{
  SUIT_DataObject* aCompObj = componentObject();
  LightApp_ModuleObject* anObj = dynamic_cast<LightApp_ModuleObject*>( aCompObj );
  if ( anObj ) {
    CAM_DataModel* aModel = anObj->dataModel();
    if ( aModel )
      return aModel->module()->name();
  }
  return "";
}

/*
	Class: LightApp_ModuleObject
	Level: Public
*/

/*!Constructor.Initialize by \a parent.*/
LightApp_ModuleObject::LightApp_ModuleObject( SUIT_DataObject* parent )
: CAM_RootObject( parent ),
  CAM_DataObject( parent )
{
}

/*!Constructor.Initialize by \a module and parent.*/
LightApp_ModuleObject::LightApp_ModuleObject( CAM_DataModel* dm, SUIT_DataObject* parent )
: CAM_RootObject( dm, parent ),
  CAM_DataObject( parent )
{
}

/*!Destructor. Do nothing.*/
LightApp_ModuleObject::~LightApp_ModuleObject()
{
}

/*!Returns module name */
QString LightApp_ModuleObject::name() const
{
  return CAM_RootObject::name();
}

/*!Insert new child object to the children list at specified position
 *\add component in Study for this module object if it necessary*/
void LightApp_ModuleObject::insertChild( SUIT_DataObject* theObj, int thePosition )
{
  CAM_RootObject::insertChild(theObj, thePosition);

  CAM_DataModel* aModel = dataModel();

  LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>(parent());

  if (aRoot)
    aRoot->study()->addComponent(aModel);


}
