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

//  SALOME SALOMEGUI : 
//  File   : SVTK_Hash.h
//  Author : Roman NIKOLAEV

#ifndef SVTK_HASH_H
#define SVTK_HASH_H

#include <vector>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Integer.hxx>

typedef std::vector<Standard_Integer> SVTK_ListOfInteger;

class SVTK_Hasher {

public:
    static Standard_Integer HashCode(const std::vector<Standard_Integer> ids,
				     const Standard_Integer upper) {
        Standard_Integer seed = ids.size();
        for( Standard_Integer i = 0; i <  (Standard_Integer) ids.size(); i++ ) {
            Standard_Integer v = ids[i];
            seed ^= v + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
        }
        return ::HashCode(seed,upper);
    }

    static Standard_Boolean IsEqual(const SVTK_ListOfInteger& theKey1,
                                    const SVTK_ListOfInteger& theKey2) {
        return theKey1 == theKey2;
    }
};

typedef NCollection_IndexedMap<SVTK_ListOfInteger,SVTK_Hasher> SVTK_IndexedMapOfIds;

#endif // SVTK_HASH_H
