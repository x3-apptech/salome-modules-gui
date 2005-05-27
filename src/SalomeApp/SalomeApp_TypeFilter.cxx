#include "SalomeApp_TypeFilter.h"

#include "SalomeApp_DataOwner.h"
#include "SalomeApp_Study.h"

#include <SALOMEDS_SObject.hxx>

SalomeApp_TypeFilter::SalomeApp_TypeFilter( SalomeApp_Study* study, const QString& kind )
  : SalomeApp_Filter( study ) 
{
  myKind = kind;
}

SalomeApp_TypeFilter::~SalomeApp_TypeFilter()
{
}

bool SalomeApp_TypeFilter::isOk( const SUIT_DataOwner* sOwner ) const
{  
  const SalomeApp_DataOwner* owner = dynamic_cast<const SalomeApp_DataOwner*> ( sOwner );

  SalomeApp_Study* aDoc =  getStudy();
  if (owner && aDoc && aDoc->studyDS())
    {
      _PTR(Study) aStudy = aDoc->studyDS();
      QString entry = owner->entry();
      
      _PTR(SObject) aSObj( aStudy->FindObjectID( entry.latin1() ) );
      if (aSObj)
	{
	  _PTR(SComponent) aComponent(aSObj->GetFatherComponent());
	  if ( aComponent && (aComponent->ComponentDataType() == myKind) )
	    return true;
	}
    }

  return false;
}
