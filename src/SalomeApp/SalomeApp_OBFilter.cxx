#include "SalomeApp_OBFilter.h"

#include "SalomeApp_SelectionMgr.h" 
#include "SalomeApp_DataObject.h"
#include "SalomeApp_DataOwner.h"

SalomeApp_OBFilter::SalomeApp_OBFilter( SalomeApp_SelectionMgr* theSelMgr )
{
  mySelMgr = theSelMgr;
}

SalomeApp_OBFilter::~SalomeApp_OBFilter()
{
}

bool SalomeApp_OBFilter::isOk( const SUIT_DataObject* theDataObj ) const
{
  const SalomeApp_DataObject* obj = dynamic_cast<const SalomeApp_DataObject*>( theDataObj );
  if ( obj )
    return mySelMgr->isOk( new SalomeApp_DataOwner( obj->entry() ) );

  return true;
}

