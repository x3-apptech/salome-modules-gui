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

#ifndef SALOME_TYPEFILTER_HXX
#define SALOME_TYPEFILTER_HXX

#include "SALOME_Filter.hxx"

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>

class SALOME_TypeFilter : public SALOME_Filter
{
public:
  Standard_EXPORT SALOME_TypeFilter(const Standard_CString theKind);
  Standard_EXPORT ~SALOME_TypeFilter();

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SALOME_InteractiveObject)& theObj) const;

protected:
  Standard_CString myKind;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(SALOME_TypeFilter,SALOME_Filter)
};

DEFINE_STANDARD_HANDLE(SALOME_TypeFilter, SALOME_Filter)

#endif // SALOME_TYPEFILTER_HXX
