#include "SalomeApp_DataOwner.h"

#include "SalomeApp_DataObject.h"

#ifdef WNT
#include <typeinfo.h>
#endif

#include <iostream.h>

SalomeApp_DataOwner::SalomeApp_DataOwner( const QString& entry )
: myEntry( entry )
{
  //cout << "---> created DataOwner with entry = " << entry << endl;
}

SalomeApp_DataOwner::~SalomeApp_DataOwner()
{
}

bool SalomeApp_DataOwner::isEqual( const SUIT_DataOwner& obj ) const
{
  const SalomeApp_DataOwner* other = dynamic_cast<const SalomeApp_DataOwner*>( &obj );

  return other && entry() == other->entry();
}

QString SalomeApp_DataOwner::entry() const
{
  return myEntry;
}
