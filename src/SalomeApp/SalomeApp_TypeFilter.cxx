// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SalomeApp_TypeFilter.h"

#include "LightApp_DataOwner.h"
#include "SalomeApp_Study.h"

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
      
      _PTR(SObject) aSObj( aStudy->FindObjectID( entry.toStdString() ) );
      if (aSObj)
        {
          _PTR(SComponent) aComponent(aSObj->GetFatherComponent());
          if ( aComponent && (aComponent->ComponentDataType() == myKind.toStdString()) )
            return true;
        }
    }

  return false;
}
