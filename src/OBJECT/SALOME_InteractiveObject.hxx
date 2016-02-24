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

#ifndef SALOME_INTERACTIVEOBJECT_HXX
#define SALOME_INTERACTIVEOBJECT_HXX

#include <MMgt_TShared.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

#include <Basics_OCCTVersion.hxx>

#include <string>

class SALOME_InteractiveObject;
DEFINE_STANDARD_HANDLE(SALOME_InteractiveObject, MMgt_TShared)

class SALOME_InteractiveObject : public MMgt_TShared
{
public:
  Standard_EXPORT   SALOME_InteractiveObject();
  Standard_EXPORT   SALOME_InteractiveObject(const char* anEntry, 
					     const char* aComponentDataType,
					     const char* aName = "");
  Standard_EXPORT   virtual ~SALOME_InteractiveObject();

  Standard_EXPORT   void setEntry(const char* anEntry);
  Standard_EXPORT   const char* getEntry();
  Standard_EXPORT   Standard_Boolean hasEntry();
  
  Standard_EXPORT   void setName(const char* aName);
  Standard_EXPORT   const char* getName();
  
  Standard_EXPORT   virtual  Standard_Boolean isSame(const Handle(SALOME_InteractiveObject)& anIO);

  Standard_EXPORT   void setReference(const char* aReference);
  Standard_EXPORT   const char* getReference();
  Standard_EXPORT   Standard_Boolean hasReference();

  Standard_EXPORT   void setComponentDataType(const char* ComponentDataType);
  Standard_EXPORT   const char* getComponentDataType();
  Standard_EXPORT   Standard_Boolean isComponentType(const char* ComponentDataType); 

  // Must be defined for using by NCollection for ceratin of data map
  Standard_EXPORT friend Standard_Boolean IsEqual(const Handle(SALOME_InteractiveObject)& anIO1,
				  const Handle(SALOME_InteractiveObject)& anIO2);

protected:
  std::string myEntry;
  std::string myName;
  std::string myComponentDataType;
  std::string myReference;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(SALOME_InteractiveObject,MMgt_TShared)
};

#endif // SALOME_INTERACTIVEOBJECT_HXX
