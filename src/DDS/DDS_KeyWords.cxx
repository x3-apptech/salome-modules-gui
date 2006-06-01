// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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
#include "DDS_KeyWords.h"

IMPLEMENT_STANDARD_HANDLE(DDS_KeyWords, MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(DDS_KeyWords, MMgt_TShared)

DDS_KeyWords::DDS_KeyWords()
: MMgt_TShared()
{
  SetKeyWord( "D_URI",                "dictionary" );

  SetKeyWord( "COMPONENT",            "component" );
  SetKeyWord( "COMPONENT_NAME",       "name" );

  SetKeyWord( "UNIT_SYSTEMS",         "unitSystems" );
  SetKeyWord( "UNIT_SYSTEM",          "system" );
  SetKeyWord( "UNIT_SYSTEM_NAME",     "name" );
  SetKeyWord( "UNIT_SYSTEM_LABEL",    "label" );

  SetKeyWord( "DATUM",                "datum" );
  SetKeyWord( "DATUM_ID",             "id" );
  SetKeyWord( "DATUM_LABEL",          "label" );
  SetKeyWord( "DATUM_UNITS",          "units" );
  SetKeyWord( "DATUM_FORMAT",         "format" );
  SetKeyWord( "DATUM_FILTER",         "filter" );
  SetKeyWord( "DATUM_REQUIRED",       "required" );

  SetKeyWord( "VALUE_LIST",           "valueList" );
  SetKeyWord( "VALUE_LIST_ID",        "listid" );
  SetKeyWord( "VALUE_LIST_NAME",      "name" );
  SetKeyWord( "VALUE_LIST_TYPE",      "type" );
  SetKeyWord( "VALUE_LIST_VALUE",     "value" );
  SetKeyWord( "VALUE_LIST_VALUEID",   "id" );
  SetKeyWord( "VALUE_LIST_VALUEICON", "icon" );

  SetKeyWord( "DY_DOMAIN",            "domain" );
  SetKeyWord( "WARNING_LEVEL",        "warningLevel" );
  SetKeyWord( "WRONG_VALUE",          "wrongValue" );
  SetKeyWord( "VALUE_DESCR",          "valueDescr" );
  SetKeyWord( "VALUE_LIST_REF",       "listRef" );

  SetKeyWord( "DESCR",                "description" );
  SetKeyWord( "LONG_D",               "longDescr" );
  SetKeyWord( "SHORT_D",              "shortDescr" );

  SetKeyWord( "VD_TYPE",              "type" );
  SetKeyWord( "VD_DEFV",              "default" );
  SetKeyWord( "VD_MAXV",              "max" );
  SetKeyWord( "VD_MINV",              "min" );
  SetKeyWord( "VD_SPEC",              "specVal" );
  SetKeyWord( "VLR_LIST",             "list" );
  SetKeyWord( "PRS",                  "presentation" );
  SetKeyWord( "LATERAL_ZOOM",         "lateralZoom" );
  SetKeyWord( "LZ_MINV",              "min" );
  SetKeyWord( "LZ_MAXV",              "max" );
  SetKeyWord( "LZ_ORDER",             "order" );
}

Handle(DDS_KeyWords) DDS_KeyWords::Get()
{
  static Handle(DDS_KeyWords) keyWords;

  if ( keyWords.IsNull() )
    keyWords = new DDS_KeyWords();

  return keyWords;
}

TCollection_AsciiString DDS_KeyWords::GetKeyWord( const TCollection_AsciiString& key ) const
{
  TCollection_AsciiString keyWord;
  if ( myKeyWord.IsBound( key ) )
    keyWord = myKeyWord.Find( key );
  return keyWord;
}

void DDS_KeyWords::SetKeyWord( const TCollection_AsciiString& key,
                               const TCollection_AsciiString& keyWord )
{
  if ( myKeyWord.IsBound( key ) )
    myKeyWord.UnBind( key );

  myKeyWord.Bind( key, keyWord );
}
