#include "SalomeApp_DataSubOwner.h"

#include "SalomeApp_DataObject.h"

#ifdef WNT
#include <typeinfo.h>
#endif

SalomeApp_DataSubOwner::SalomeApp_DataSubOwner( const QString& entry, const int index )
: SalomeApp_DataOwner( entry ),
myIndex( index )
{
}

SalomeApp_DataSubOwner::~SalomeApp_DataSubOwner()
{
}

bool SalomeApp_DataSubOwner::isEqual( const SUIT_DataOwner& obj ) const
{  
  const SalomeApp_DataSubOwner* other = dynamic_cast<const SalomeApp_DataSubOwner*>( &obj );

  return other && entry() == other->entry() && index() == other->index();
}

int SalomeApp_DataSubOwner::index() const
{
  return myIndex;
}
