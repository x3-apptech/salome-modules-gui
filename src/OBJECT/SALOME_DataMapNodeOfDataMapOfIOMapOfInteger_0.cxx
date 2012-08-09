// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//  File   : SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_0.cxx
//  Module : SALOME
//
#include "SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx"

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _SALOME_InteractiveObject_HeaderFile
#include "SALOME_InteractiveObject.hxx"
#endif
#ifndef _TColStd_IndexedMapOfInteger_HeaderFile
#include <TColStd_IndexedMapOfInteger.hxx>
#endif
#ifndef _TColStd_MapTransientHasher_HeaderFile
#include <TColStd_MapTransientHasher.hxx>
#endif
#ifndef _SALOME_DataMapOfIOMapOfInteger_HeaderFile
#include "SALOME_DataMapOfIOMapOfInteger.hxx"
#endif
#ifndef _SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger_HeaderFile
#include "SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger.hxx"
#endif

/*!
  Destructor
*/
SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::~SALOME_DataMapNodeOfDataMapOfIOMapOfInteger() {}
 


/*!
  \return OCC Handle Type
*/
Standard_EXPORT Handle_Standard_Type& SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("SALOME_DataMapNodeOfDataMapOfIOMapOfInteger",
                                                         sizeof(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger),
                                                         1,
                                                         (Standard_Address)_Ancestors,
                                                         (Standard_Address)NULL);

  return _aType;
}


/*!
  DownCast method: allows safe downcasting
*/
const Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger))) {
       _anOtherObject = Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger)((Handle(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger)&)AnObject);
     }
  }

  return _anOtherObject ;
}

/*!
  \return OCC Handle dynamic Type
*/
const Handle(Standard_Type)& SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::DynamicType() const 
{ 
  return STANDARD_TYPE(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) ; 
}

/*!
  \return true if other type is the same
  \param AType - type to be checked
*/
Standard_Boolean SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::IsKind(const Handle(Standard_Type)& AType) const 
{ 
  return (STANDARD_TYPE(SALOME_DataMapNodeOfDataMapOfIOMapOfInteger) == AType || TCollection_MapNode::IsKind(AType)); 
}
/*!
  Destructor
*/
Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger::~Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger() {}
#define TheKey Handle_SALOME_InteractiveObject
#define TheKey_hxx "SALOME_InteractiveObject.hxx"
#define TheItem TColStd_IndexedMapOfInteger
#define TheItem_hxx <TColStd_IndexedMapOfInteger.hxx>
#define Hasher TColStd_MapTransientHasher
#define Hasher_hxx <TColStd_MapTransientHasher.hxx>
#define TCollection_DataMapNode SALOME_DataMapNodeOfDataMapOfIOMapOfInteger
#define TCollection_DataMapNode_hxx "SALOME_DataMapNodeOfDataMapOfIOMapOfInteger.hxx"
#define TCollection_DataMapIterator SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger
#define TCollection_DataMapIterator_hxx "SALOME_DataMapIteratorOfDataMapOfIOMapOfInteger.hxx"
#define Handle_TCollection_DataMapNode Handle_SALOME_DataMapNodeOfDataMapOfIOMapOfInteger
#define TCollection_DataMapNode_Type_() SALOME_DataMapNodeOfDataMapOfIOMapOfInteger_Type_()
#define TCollection_DataMap SALOME_DataMapOfIOMapOfInteger
#define TCollection_DataMap_hxx "SALOME_DataMapOfIOMapOfInteger.hxx"
#include <TCollection_DataMapNode.gxx>

