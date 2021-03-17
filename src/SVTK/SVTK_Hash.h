// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  SALOME SALOMEGUI : 
//  File   : SVTK_Hash.h
//  Author : Roman NIKOLAEV

#ifndef SVTK_HASH_H
#define SVTK_HASH_H

#include <vector>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <Standard_Integer.hxx>
#include <vtkType.h>
#include <limits>

typedef std::vector<Standard_Integer> SVTK_ListOfInteger;
typedef std::vector<vtkIdType> SVTK_ListOfVtk;

class SVTK_Hasher {

public:
  static Standard_Integer HashCode(const std::vector<Standard_Integer> ids,
                                   const Standard_Integer              upper)
  {
    Standard_Integer seed = (Standard_Integer)ids.size();
    for ( Standard_Integer v : ids )
      seed ^= v + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );

    return ::HashCode(seed,upper);
  }

  static Standard_Boolean IsEqual(const SVTK_ListOfInteger& theKey1,
                                  const SVTK_ListOfInteger& theKey2)
  {
    return theKey1 == theKey2;
  }
};

class SVTK_vtkHasher {

public:
  static vtkIdType HashCode(const std::vector<vtkIdType> ids,
                            const Standard_Integer       upper)
  {
    vtkIdType seed = (vtkIdType)ids.size();
    for ( vtkIdType v : ids )
      seed ^= v + 0x9e3779b97f4a7c15 + ( seed << 6 ) + ( seed >> 2 );

    return ::HashCode((Standard_Integer) seed, upper);
  }

  static vtkIdType IsEqual(const SVTK_ListOfVtk& theKey1,
                           const SVTK_ListOfVtk& theKey2)
  {
    return theKey1 == theKey2;
  }
};

struct svtkIdHasher
{
  static int HashCode(const vtkIdType theValue,  const int theUpperBound)
  {
    return static_cast<int> ((theValue & (std::numeric_limits<vtkIdType>::max)()) % theUpperBound + 1);
  }

  static bool IsEqual( const vtkIdType& id1, const vtkIdType& id2 )
  {
    return id1 == id2;
  }
};

typedef NCollection_IndexedMap<SVTK_ListOfInteger,SVTK_Hasher> SVTK_IndexedMapOfIds;
typedef NCollection_IndexedMap<SVTK_ListOfVtk, SVTK_vtkHasher> SVTK_IndexedMapOfVtkIds;
typedef NCollection_Map< vtkIdType, svtkIdHasher > SVTK_TVtkIDsMap;
typedef NCollection_Map< vtkIdType, svtkIdHasher >::Iterator SVTK_TVtkIDsMapIterator;
typedef NCollection_IndexedMap<vtkIdType,svtkIdHasher> SVTK_TIndexedMapOfVtkId;

#endif // SVTK_HASH_H
