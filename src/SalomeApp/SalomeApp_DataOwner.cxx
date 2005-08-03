#include "SalomeApp_DataOwner.h"

#include "SalomeApp_DataObject.h"

#ifdef WNT
#include <typeinfo.h>
#endif

#include <iostream>

/*!Constructor. Initialize by \a theEntry.*/
SalomeApp_DataOwner
::SalomeApp_DataOwner( const QString& theEntry ): 
  myEntry( theEntry )
{
}

/*!Constructor. Initialize by \a SALOME_InteractiveObject.*/
SalomeApp_DataOwner
::SalomeApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO ):
  myEntry(!theIO.IsNull()? theIO->getEntry(): ""),
  myIO(theIO)
{
}

/*!Destructor. Do nothing.*/
SalomeApp_DataOwner
::~SalomeApp_DataOwner()
{
}

/*!Checks: Is current data owner equal \a obj.*/
bool
SalomeApp_DataOwner
::isEqual( const SUIT_DataOwner& obj ) const
{
  const SalomeApp_DataOwner* other = dynamic_cast<const SalomeApp_DataOwner*>( &obj );

  return other && entry() == other->entry();
}

/*!Gets entry.*/
QString
SalomeApp_DataOwner
::entry() const
{
  return myEntry;
}

/*!Gets SALOME_InteractiveObject.*/
const Handle(SALOME_InteractiveObject)&
SalomeApp_DataOwner
::IO() const
{
  return myIO;
}
