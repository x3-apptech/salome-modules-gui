#include "SalomeApp_TypeFilter.h"

#include "LightApp_DataOwner.h"
#include "SalomeApp_Study.h"

#include <SALOMEDS_SObject.hxx>

/*!
  Constructor.
*/
SalomeApp_TypeFilter::SalomeApp_TypeFilter( SalomeApp_Study* study, const QString& kind )
  : SalomeApp_Filter( study ) 
{
  myKind = kind;
}

/*!
  Destructor.
*/
SalomeApp_TypeFilter::~SalomeApp_TypeFilter()
{
}

/*!
  Check: data owner is valid?
*/
bool SalomeApp_TypeFilter::isOk( const SUIT_DataOwner* sOwner ) const
{  
  const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*> ( sOwner );

  SalomeApp_Study* aDoc =  getStudy();
  if (owner && aDoc && aDoc->studyDS())
    {
      _PTR(Study) aStudy = aDoc->studyDS();
      QString entry = owner->entry();
      
      _PTR(SObject) aSObj( aStudy->FindObjectID( entry.latin1() ) );
      if (aSObj)
	{
	  _PTR(SComponent) aComponent(aSObj->GetFatherComponent());
	  if ( aComponent && (aComponent->ComponentDataType() == myKind.latin1()) )
	    return true;
	}
    }

  return false;
}
