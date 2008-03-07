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

/*!
  \class DDS_KeyWords

  This object contains the pairs of internal keys and XML tag or attribute names. This class allow to user
  define set of XML keywords used in datums definition file.

  Default keywords table:
  \li D_URI - dictionary
  \li COMPONENT - component
  \li COMPONENT_NAME - name
  \li UNIT_SYSTEMS - unitSystems
  \li UNIT_SYSTEM - system
  \li UNIT_SYSTEM_NAME - name
  \li UNIT_SYSTEM_LABEL - label
  \li DATUM - datum
  \li DATUM_ID - id
  \li DATUM_LABEL - label
  \li DATUM_UNITS - units
  \li DATUM_FORMAT - format
  \li DATUM_FILTER - filter
  \li DATUM_REQUIRED - required
  \li VALUE_LIST - valueList
  \li VALUE_LIST_ID - listid
  \li VALUE_LIST_NAME - name
  \li VALUE_LIST_TYPE - type
  \li VALUE_LIST_VALUE - value
  \li VALUE_LIST_VALUEID - id
  \li VALUE_LIST_VALUEICON - icon
  \li DY_DOMAIN - domain
  \li WARNING_LEVEL - warningLevel
  \li WRONG_VALUE - wrongValue
  \li VALUE_DESCR - valueDescr
  \li VALUE_LIST_REF - listRef
  \li DESCR - description
  \li LONG_D - longDescr
  \li SHORT_D - shortDescr
  \li VD_TYPE - type
  \li VD_DEFV - default
  \li VD_MAXV - max
  \li VD_MINV - min
  \li VD_SPEC - specVal
  \li VLR_LIST - list
  \li PRS - presentation
  \li LATERAL_ZOOM - lateralZoom
  \li LZ_MINV - min
  \li LZ_MAXV - max
  \li LZ_ORDER - order
  \li OPTIONS - options
  \li OPTION - option
  \li OPTION_NAME - name
*/

/*!
  Constructor
*/

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

  SetKeyWord( "OPTIONS",              "options" );
  SetKeyWord( "OPTION",               "option" );
  SetKeyWord( "OPTION_NAME",          "name" );
}

/*!
  Returns the static instance of the class DDS_KeyWords
*/

Handle(DDS_KeyWords) DDS_KeyWords::Get()
{
  static Handle(DDS_KeyWords) keyWords;

  if ( keyWords.IsNull() )
    keyWords = new DDS_KeyWords();

  return keyWords;
}

/*!
  Returns the XML keyword by specified internal key \a key. If the internal key isn't registered
  then empty string will be returned.
*/

TCollection_AsciiString DDS_KeyWords::GetKeyWord( const TCollection_AsciiString& key ) const
{
  TCollection_AsciiString keyWord;
  if ( myKeyWord.IsBound( key ) )
    keyWord = myKeyWord.Find( key );
  return keyWord;
}

/*!
  Register the pair of the internal key \a key and XML keyword \a keyWord.
*/

void DDS_KeyWords::SetKeyWord( const TCollection_AsciiString& key,
                               const TCollection_AsciiString& keyWord )
{
  if ( myKeyWord.IsBound( key ) )
    myKeyWord.UnBind( key );

  myKeyWord.Bind( key, keyWord );
}
