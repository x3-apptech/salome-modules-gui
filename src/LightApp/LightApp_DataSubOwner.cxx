#include "LightApp_DataSubOwner.h"

#include "LightApp_DataObject.h"

#ifdef WNT
#include <typeinfo.h>
#endif

/*!Constructor.Initialize by \a entry and \a index*/
LightApp_DataSubOwner::LightApp_DataSubOwner( const QString& entry, const int index )
: LightApp_DataOwner( entry ),
myIndex( index )
{
}

/*!Destructor. Do nothing.*/
LightApp_DataSubOwner::~LightApp_DataSubOwner()
{
}

/*!Checks: Is current data sub owner equal \a obj.*/
bool LightApp_DataSubOwner::isEqual( const SUIT_DataOwner& obj ) const
{  
  if (LightApp_DataOwner::isEqual(obj)) {
    const LightApp_DataSubOwner* other = dynamic_cast<const LightApp_DataSubOwner*>( &obj );
    return other && index() == other->index();
  }
  return false;
}

/*!Gets index.*/
int LightApp_DataSubOwner::index() const
{
  return myIndex;
}
