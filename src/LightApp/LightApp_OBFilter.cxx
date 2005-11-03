#include "LightApp_OBFilter.h"

#include "LightApp_SelectionMgr.h" 
#include "LightApp_DataObject.h"
#include "LightApp_DataOwner.h"

/*!
  Constructor.
*/
LightApp_OBFilter::LightApp_OBFilter( LightApp_SelectionMgr* theSelMgr )
{
  mySelMgr = theSelMgr;
}

/*!Destructor.*/
LightApp_OBFilter::~LightApp_OBFilter()
{
}

/*!Checks: data object is ok?*/
bool LightApp_OBFilter::isOk( const SUIT_DataObject* theDataObj ) const
{
  const LightApp_DataObject* obj = dynamic_cast<const LightApp_DataObject*>( theDataObj );
  if ( obj )
    return mySelMgr->isOk( new LightApp_DataOwner( obj->entry() ) );

  return true;
}

