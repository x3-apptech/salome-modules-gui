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
#include "DDS_Dictionary.h"

#include "DDS_KeyWords.h"

#include <LDOMString.hxx>
#include <LDOMParser.hxx>

#include <UnitsAPI.hxx>

#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

#include <NCollection_Map.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

IMPLEMENT_STANDARD_HANDLE(DDS_Dictionary, MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(DDS_Dictionary, MMgt_TShared)

DDS_Dictionary::DDS_Dictionary()
: MMgt_TShared()
{
}

DDS_Dictionary::DDS_Dictionary( const DDS_Dictionary& )
{
}

void DDS_Dictionary::operator=( const DDS_Dictionary& )
{
}

void DDS_Dictionary::GetUnitSystems( TColStd_SequenceOfAsciiString& theSystems ) const
{
  theSystems.Clear();

  NCollection_Map<TCollection_AsciiString> aMap;
  for ( Standard_Integer i = 1; i <= myGroupMap.Extent(); i++ )
  {
    TColStd_SequenceOfAsciiString theSeq;
    myGroupMap.FindFromIndex( i )->GetUnitSystems( theSeq );
    for ( Standard_Integer s = 1; s <= theSeq.Length(); s++ )
    {
      if ( aMap.Contains( theSeq.Value( s ) ) )
        continue;

      theSystems.Append( theSeq.Value( s ) );
      aMap.Add( theSeq.Value( s ) );
    }
  }

}

void DDS_Dictionary::GetUnitSystems( TColStd_SequenceOfAsciiString& theSystems,
                                     const TCollection_AsciiString& theComponent ) const
{
  theSystems.Clear();
  if ( myGroupMap.Contains( theComponent ) )
    myGroupMap.FindFromKey( theComponent )->GetUnitSystems( theSystems );
}

TCollection_ExtendedString DDS_Dictionary::GetUnitSystemLabel( const TCollection_AsciiString& theSystem ) const
{
  TCollection_ExtendedString aLabel;
  for ( Standard_Integer i = 1; i <= myGroupMap.Extent() && !aLabel.Length(); i++ )
    aLabel = myGroupMap.FindFromIndex( i )->GetUnitSystemLabel( theSystem );
  return aLabel;
}

TCollection_ExtendedString DDS_Dictionary::GetUnitSystemLabel( const TCollection_AsciiString& theSystem,
                                                               const TCollection_AsciiString& theComponent ) const
{
  TCollection_ExtendedString aLabel;
  if ( myGroupMap.Contains( theComponent ) )
    aLabel = myGroupMap.FindFromKey( theComponent )->GetUnitSystemLabel( theSystem );
  return aLabel;
}

TCollection_AsciiString DDS_Dictionary::GetActiveUnitSystem() const
{
  TCollection_AsciiString aSystem;
  if ( myGroupMap.Extent() )
    aSystem = myGroupMap.FindFromIndex( 1 )->GetActiveUnitSystem();
  return aSystem;
}

TCollection_AsciiString DDS_Dictionary::GetActiveUnitSystem( const TCollection_AsciiString& theComponent ) const
{
  TCollection_AsciiString aSystem;
  if ( myGroupMap.Contains( theComponent ) )
    aSystem = myGroupMap.FindFromKey( theComponent )->GetActiveUnitSystem();
  return aSystem;
}

void DDS_Dictionary::SetActiveUnitSystem( const TCollection_AsciiString& theSystem )
{
  for ( Standard_Integer i = 1; i <= myGroupMap.Extent(); i++ )
    myGroupMap.FindFromIndex( i )->SetActiveUnitSystem( theSystem );
}

void DDS_Dictionary::SetActiveUnitSystem( const TCollection_AsciiString& theSystem,
                                          const TCollection_AsciiString& theComponent )
{
  if ( myGroupMap.Contains( theComponent ) )
    myGroupMap.FindFromKey( theComponent )->SetActiveUnitSystem( theSystem );
}

/*!
  Returns the instance of dictionary. Create instance if it is NULL.
*/
Handle(DDS_Dictionary) DDS_Dictionary::Get()
{
  static Handle(DDS_Dictionary) sDictionary;

  if ( sDictionary.IsNull() )
    sDictionary = new DDS_Dictionary();

  return sDictionary;
}

Standard_Boolean DDS_Dictionary::Load( const TCollection_AsciiString theFileName )
{
  static NCollection_Map<TCollection_AsciiString> _LoadMap;

  if ( _LoadMap.Contains( theFileName ) )
    return Standard_True;

  Handle(DDS_Dictionary) aDic = Get();
  if ( aDic.IsNull() )
    return Standard_False;

  LDOMParser aParser;
  if ( aParser.parse( theFileName.ToCString() ) )
    return Standard_False;

  LDOM_Document aDoc = aParser.getDocument();
  LDOM_Element aDocElement = aDoc.getDocumentElement();
  for ( LDOM_Element aComponentElem = aDocElement.GetChildByTagName( KeyWord( "COMPONENT" ) );
        !aComponentElem.isNull(); aComponentElem = aComponentElem.GetSiblingByTagName() )
    aDic->FillDataMap( aComponentElem, aDocElement );

  _LoadMap.Add( theFileName );

  return Standard_True;
}

LDOMString DDS_Dictionary::KeyWord( const TCollection_AsciiString& key )
{
  LDOMString keyWord;
  Handle(DDS_KeyWords) aKeyWords = DDS_KeyWords::Get();
  if ( !aKeyWords.IsNull() )
  {
    TCollection_AsciiString aStr = aKeyWords->GetKeyWord( key );
    if ( aStr.Length() )
      keyWord = LDOMString( aStr.ToCString() );
  }
  return keyWord;
}

/*!
  Returns DicItem from specified group with all attached data
*/

Handle(DDS_DicItem) DDS_Dictionary::GetDicItem( const TCollection_AsciiString& theID,
                                                const TCollection_AsciiString& theGroup ) const
{
  Handle(DDS_DicItem) aDicItem;
  Handle(DDS_DicGroup) aDicGroup;
  if ( myGroupMap.Contains( theGroup ) )
    aDicGroup = myGroupMap.FindFromKey( theGroup );
  if ( !aDicGroup.IsNull() )
    aDicItem = aDicGroup->GetDicItem( theID );
  return aDicItem;
}

/*!
  Returns DicItem with all attached data
*/

Handle(DDS_DicItem) DDS_Dictionary::GetDicItem( const TCollection_AsciiString& theID ) const
{
  Handle(DDS_DicItem) aDicItem;
  for ( Standard_Integer i = 1; i <= myGroupMap.Extent() && aDicItem.IsNull(); i++ )
    aDicItem = myGroupMap.FindFromIndex( i )->GetDicItem( theID );
  return aDicItem;
}

void DDS_Dictionary::FillDataMap( const LDOM_Element& theComponentData, const LDOM_Element& theDocElement )
{
  TCollection_AsciiString aCompName = theComponentData.getAttribute( KeyWord( "COMPONENT_NAME" ) );
  if ( !myGroupMap.Contains( aCompName ) )
    myGroupMap.Add( aCompName, new DDS_DicGroup( aCompName ) );
  Handle(DDS_DicGroup) aDicGroup = myGroupMap.FindFromKey( aCompName );
  aDicGroup->FillDataMap( theComponentData, theDocElement );
  myGroupMap.Add( aCompName, aDicGroup );
}

Standard_Real DDS_Dictionary::ToSI( const Standard_Real theValue, const Standard_CString theUnits )
{
  Standard_Real aRetValue = theValue;
  if ( theUnits && *theUnits && strcmp( theUnits, "%" ) )
  {
    try {
      aRetValue = UnitsAPI::AnyToSI( theValue, theUnits );
    }
    catch( Standard_Failure ) {
    }
  }
  else if ( theUnits && *theUnits )
    aRetValue = theValue / 100.0;

  return aRetValue;
}

Standard_Real DDS_Dictionary::FromSI( const Standard_Real theValue, const Standard_CString theUnits )
{
  Standard_Real aRetValue = theValue;
  if ( theUnits && *theUnits && strcmp( theUnits, "%" ) )
  {
    try {
      aRetValue = UnitsAPI::AnyFromSI( theValue, theUnits );
    }
    catch( Standard_Failure ) {
    }
  }
  else if ( theUnits && *theUnits )
    aRetValue = theValue * 100.0;

  return aRetValue;
}
