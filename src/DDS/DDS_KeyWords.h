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

#ifndef DDS_KEYWORDS_H
#define DDS_KEYWORDS_H

#include "DDS.h"

#include <MMgt_TShared.hxx>

#include <NCollection_DataMap.hxx>

DEFINE_STANDARD_HANDLE(DDS_KeyWords, MMgt_TShared)

class TCollection_AsciiString;

class DDS_KeyWords : public MMgt_TShared
{
public:
  Standard_EXPORT static Handle(DDS_KeyWords) Get();

  Standard_EXPORT TCollection_AsciiString     GetKeyWord( const TCollection_AsciiString& ) const;
  Standard_EXPORT void                        SetKeyWord( const TCollection_AsciiString&,
                                                          const TCollection_AsciiString& );

private:
  DDS_KeyWords();

private:
  typedef NCollection_DataMap<TCollection_AsciiString,
                              TCollection_AsciiString> KeyWordMap;

private:
  KeyWordMap                                  myKeyWord;

public:
  OCCT_DEFINE_STANDARD_RTTIEXT(DDS_KeyWords,MMgt_TShared)
};

#endif
