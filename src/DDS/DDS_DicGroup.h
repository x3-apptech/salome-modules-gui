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

#ifndef DDS_DICGROUP_H
#define DDS_DICGROUP_H

#include "DDS.h"

#include "DDS_DicItem.h"

#include <MMgt_TShared.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <NCollection_List.hxx>

class LDOM_Element;

DEFINE_STANDARD_HANDLE(DDS_DicGroup, MMgt_TShared)

class DDS_DicGroup : public MMgt_TShared
{
public:
  Standard_EXPORT DDS_DicGroup( const TCollection_AsciiString& );

  Standard_EXPORT TCollection_AsciiString     GetName() const;
  Standard_EXPORT void                        GetKeys( TColStd_SequenceOfAsciiString& ) const;

  Standard_EXPORT Handle(DDS_DicItem)         GetDicItem( const TCollection_AsciiString& ) const;

  Standard_EXPORT void                        GetUnitSystems( TColStd_SequenceOfAsciiString& ) const;
  Standard_EXPORT TCollection_ExtendedString  GetUnitSystemLabel( const TCollection_AsciiString& ) const;

  Standard_EXPORT TCollection_AsciiString     GetActiveUnitSystem() const;
  Standard_EXPORT void                        SetActiveUnitSystem( const TCollection_AsciiString& );

  virtual Standard_EXPORT Handle(DDS_DicItem) CreateItem() const;
  
protected:

  Standard_EXPORT DDS_DicGroup( const DDS_DicGroup& );

  Standard_EXPORT void                        operator=( const DDS_DicGroup& );

  Standard_EXPORT virtual void                FillDataMap( const LDOM_Element&, const LDOM_Element& );

  Standard_EXPORT Standard_Boolean            AddDicItem( const TCollection_AsciiString&,
                                                          const Handle(DDS_DicItem)& );

  Standard_EXPORT Standard_Boolean            HasDicItem( const TCollection_AsciiString& ) const;

  Standard_EXPORT void                        RemoveAllDicItems();

  Standard_EXPORT const 
    DDS_IndexedDataMapOfDicItems&             GetItemMap() const;

  Standard_EXPORT Standard_Boolean            AddUnitSystem( const TCollection_AsciiString&,
                                                             const TCollection_ExtendedString& );

protected:

  typedef NCollection_DataMap<TCollection_AsciiString,
                              TCollection_ExtendedString> UnitSystemMap;

private:
  TCollection_AsciiString                    myName;
  DDS_IndexedDataMapOfDicItems               myDataMap;
  UnitSystemMap                              myUnitSystem;
  TCollection_AsciiString                    myActiveSystem;
  TColStd_SequenceOfAsciiString              myKeys;

  friend class DDS_Dictionary;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(DDS_DicGroup,MMgt_TShared)
};

typedef NCollection_IndexedDataMap<TCollection_AsciiString, Handle(DDS_DicGroup)> DDS_IndexedDataMapOfDicGroups;

#endif
