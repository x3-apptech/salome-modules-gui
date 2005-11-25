#include "LightApp_DataOwner.h"

#include "LightApp_DataObject.h"

#ifndef WNT
#include <typeinfo>
#define _typeinfo std::type_info
#else
#include <typeinfo.h>
#define _typeinfo type_info
#endif

#include <iostream>

/*!Constructor. Initialize by \a theEntry.*/
LightApp_DataOwner
::LightApp_DataOwner( const QString& theEntry ): 
  myEntry( theEntry )
{
}

/*!Constructor. Initialize by \a SALOME_InteractiveObject.*/
LightApp_DataOwner
::LightApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO ):
  myEntry(!theIO.IsNull()? theIO->getEntry(): ""),
  myIO(theIO)
{
}

/*!Destructor. Do nothing.*/
LightApp_DataOwner
::~LightApp_DataOwner()
{
}

/*!Checks: Is current data owner equal \a obj.*/
bool
LightApp_DataOwner
::isEqual( const SUIT_DataOwner& obj ) const
{
  const _typeinfo& ti1 = typeid( obj );
  const _typeinfo& ti2 = typeid( *(this) );

  if (ti1 != ti2)
    return false;

  const LightApp_DataOwner* other = dynamic_cast<const LightApp_DataOwner*>( &obj );

  return other && entry() == other->entry();
}

/*!Gets entry.*/
QString
LightApp_DataOwner
::entry() const
{
  return myEntry;
}

/*!Gets SALOME_InteractiveObject.*/
const Handle(SALOME_InteractiveObject)&
LightApp_DataOwner
::IO() const
{
  return myIO;
}
