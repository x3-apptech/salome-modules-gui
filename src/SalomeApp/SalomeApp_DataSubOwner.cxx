#include "SalomeApp_DataSubOwner.h"

#include "SalomeApp_DataObject.h"

#ifdef WNT
#include <typeinfo.h>
#endif

/*!Constructor.Initialize by \a entry and \a index*/
SalomeApp_DataSubOwner::SalomeApp_DataSubOwner( const QString& entry, const int index )
: SalomeApp_DataOwner( entry ),
myIndex( index )
{
}

/*!Destructor. Do nothing.*/
SalomeApp_DataSubOwner::~SalomeApp_DataSubOwner()
{
}

/*!Checks: Is current data sub owner equal \a obj.*/
bool SalomeApp_DataSubOwner::isEqual( const SUIT_DataOwner& obj ) const
{  
  const SalomeApp_DataSubOwner* other = dynamic_cast<const SalomeApp_DataSubOwner*>( &obj );

  return other && entry() == other->entry() && index() == other->index();
}

/*!Gets index.*/
int SalomeApp_DataSubOwner::index() const
{
  return myIndex;
}
