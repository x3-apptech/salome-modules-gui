#include "SalomeApp_DataOwner.h"

#include "SalomeApp_DataObject.h"

#ifdef WNT
#include <typeinfo.h>
#endif

#include <iostream.h>

SalomeApp_DataOwner
::SalomeApp_DataOwner( const QString& theEntry ): 
  myEntry( theEntry )
{
}

SalomeApp_DataOwner
::SalomeApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO ):
  myEntry(!theIO.IsNull()? theIO->getEntry(): ""),
  myIO(theIO)
{
}

SalomeApp_DataOwner
::~SalomeApp_DataOwner()
{
}

bool
SalomeApp_DataOwner
::isEqual( const SUIT_DataOwner& obj ) const
{
  const SalomeApp_DataOwner* other = dynamic_cast<const SalomeApp_DataOwner*>( &obj );

  return other && entry() == other->entry();
}

QString
SalomeApp_DataOwner
::entry() const
{
  return myEntry;
}

const Handle(SALOME_InteractiveObject)&
SalomeApp_DataOwner
::IO() const
{
  return myIO;
}
