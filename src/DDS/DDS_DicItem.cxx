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
#include "DDS_DicItem.h"
#include "DDS_Dictionary.h"

#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

#include <LDOM_Text.hxx>
#include <LDOMString.hxx>
#include <LDOM_Element.hxx>

#include <UnitsAPI.hxx>
#include <Units_Dimensions.hxx>

#include <TColStd_MapOfReal.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>

IMPLEMENT_STANDARD_HANDLE(DDS_DicItem, MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(DDS_DicItem, MMgt_TShared)

DDS_DicItem::DDS_DicItem()
: myType( 0 ),
myDefValue( 0 ),
myMax( 0 ),
myMin( 0 ),
myMinZoom( 0.1 ),
myMaxZoom( 10 ),
myZoomOrder( 2 )
{
}

DDS_DicItem::DDS_DicItem( const DDS_DicItem& )
{
}

void DDS_DicItem::operator=( const DDS_DicItem& )
{
}

TCollection_AsciiString DDS_DicItem::GetId() const
{
  return myId;
}

DDS_DicItem::Type DDS_DicItem::GetType() const
{
  return (DDS_DicItem::Type)myType;
}

TCollection_ExtendedString DDS_DicItem::GetLabel() const
{
  return myLabel;
}

TCollection_ExtendedString DDS_DicItem::GetFilter() const
{
  return myFilter;
}

TCollection_ExtendedString DDS_DicItem::GetRequired() const
{
  return myRequired;
}

DDS_MsgType DDS_DicItem::GetWarningLevel() const
{
  return (DDS_MsgType)myWarnLevel;
}

TCollection_ExtendedString DDS_DicItem::GetLongDescription() const
{
  return myLongDescr;
}

TCollection_ExtendedString DDS_DicItem::GetShortDescription() const
{
  return myShortDescr;
}

TCollection_AsciiString DDS_DicItem::GetComponent() const
{
  TCollection_AsciiString aCompName;
  Handle(DDS_DicGroup) aComponent = Handle(DDS_DicGroup)::DownCast(myComponent);
  if ( !aComponent.IsNull() )
    aCompName = aComponent->GetName();
  return aCompName;
}

TCollection_AsciiString DDS_DicItem::GetUnits() const
{
  return GetUnits( GetActiveUnitSystem() );
}

TCollection_AsciiString DDS_DicItem::GetUnits( const UnitSystem& theSystem ) const
{
  TCollection_AsciiString anUnits;
  UnitData* unitData = GetUnitData( theSystem );
  if ( unitData )
    anUnits = unitData->myUnits;
  return anUnits;
}

Standard_Real DDS_DicItem::GetMinValue() const
{
  return GetMinValue( GetActiveUnitSystem() );
}

Standard_Real DDS_DicItem::GetMinValue( const UnitSystem& theUnitsSystem ) const
{
  return FromSI( myMin, theUnitsSystem );
}

Standard_Real DDS_DicItem::GetMaxValue() const
{
  return GetMaxValue( GetActiveUnitSystem() );
}

Standard_Real DDS_DicItem::GetMaxValue( const UnitSystem& theUnitsSystem ) const
{
  return FromSI( myMax, theUnitsSystem );
}

Standard_Integer DDS_DicItem::GetPrecision() const
{
  return GetPrecision( GetActiveUnitSystem() );
}

Standard_Integer DDS_DicItem::GetPrecision( const UnitSystem& theSystem ) const
{
  Standard_Integer aRes = 0;
  UnitData* unitData = GetUnitData( theSystem );
  if ( unitData )
    aRes = unitData->myPrecision;
  return aRes;
}

TCollection_ExtendedString DDS_DicItem::GetDefaultValue() const
{
  return GetDefaultValue( GetActiveUnitSystem() );
}

TCollection_ExtendedString DDS_DicItem::GetDefaultValue( const UnitSystem& theSystem ) const
{
  if ( !myDefString.Length() )
    return myDefString;

  TCollection_ExtendedString aStr;

  switch ( myType )
  {
  case Float:
  case Integer:
    aStr = FromSI( myDefValue, theSystem );
    break;
  case List:
  case String:
    aStr = myDefString;
    break;
  default:
    break;
  }
  return aStr;
}

TCollection_AsciiString DDS_DicItem::GetFormat( const Standard_Boolean theCanonical ) const
{
  return GetFormat( GetActiveUnitSystem(), theCanonical );
}

TCollection_AsciiString DDS_DicItem::GetFormat( const UnitSystem& theSystem,
                                                const Standard_Boolean theCanonical ) const
{
  TCollection_AsciiString aFormat;
  UnitData* unitData = GetUnitData( theSystem );
  if ( unitData )
    aFormat = unitData->myFormat;

  if ( theCanonical && aFormat.Length() > 1 )
  {
    static TCollection_AsciiString f;
    f = aFormat;
    Standard_Boolean isRemoved = false;
    while ( !isRemoved )
    {
      char ch = f.Value( f.Length() - 1 );
      if ( ( ch != '%' && ch != '.' && !IsDigit( ch ) ) && f.Length() > 1 )
        f.Remove( f.Length() - 1 );
      else
        isRemoved = true;
    }
    aFormat = f;
  }

  return aFormat;
}

/*!
  Access valueList:name of the parameter. This string is void if the list is
  not defined - then use other properties: Type, DefaultValue, MaxValue, MinValue
*/
TCollection_ExtendedString DDS_DicItem::GetNameOfValues() const
{
  return myListName;
}

/*!
  Access valueList of the parameter. This sequence is empty if the list is
  not defined - then use other properties: Type, DefaultValue, MaxValue, MinValue
*/
Standard_Boolean DDS_DicItem::GetListOfValues( Handle(TColStd_HArray1OfExtendedString)& theStrings,
                                               Handle(TColStd_HArray1OfInteger)& theIntegers ) const
{
  theStrings  = myListRef;
  theIntegers = myListRefID;
  return !theIntegers.IsNull() && !theStrings.IsNull();
}

/*!
  Access valueList of the parameter. This sequence is empty if the list is not
  defined - then use other properties: Type, DefaultValue, MaxValue, MinValue
*/
Standard_Boolean DDS_DicItem::GetListOfValues( Handle(TColStd_HArray1OfExtendedString)& theStrings,
                                               Handle(TColStd_HArray1OfInteger)& theIntegers,
                                               Handle(TColStd_HArray1OfExtendedString)& theIcons ) const
{
  theStrings  = myListRef;
  theIntegers = myListRefID;
  theIcons    = myListRefIcons;
  return !theIntegers.IsNull() && !theStrings.IsNull() && !theIcons.IsNull();
}

Standard_Boolean DDS_DicItem::GetSpecialValues( TColStd_MapOfReal& theMap ) const
{
  theMap.Clear();
  if ( !myListRef.IsNull() )
  {
    for ( Standard_Integer i = myListRef->Lower(); i <= myListRef->Upper(); i++ )
    {
      if ( myListRef->Value( i ).IsAscii() )
      {
        TCollection_AsciiString aStr( myListRef->Value( i ) );
        if ( aStr.IsRealValue() )
          theMap.Add( aStr.RealValue() );
      }
    }
  }

  return theMap.Extent() > 0;
}

/*!
  Returns min value of lateral zooming
*/
Standard_Real DDS_DicItem::GetMinZoom() const
{
  return myMinZoom;
}

/*!
  Returns Max Value of lateral zooming
*/
Standard_Real DDS_DicItem::GetMaxZoom() const
{
  return myMaxZoom;
}

/*!
  Get Order of lateral zooming
*/
Standard_Real DDS_DicItem::GetZoomOrder() const
{
  return myZoomOrder;
}

Standard_Real DDS_DicItem::ToSI( const Standard_Real theVal ) const
{
  return ToSI( theVal, GetActiveUnitSystem() );
}

Standard_Real DDS_DicItem::FromSI( const Standard_Real theVal ) const
{
  return FromSI( theVal, GetActiveUnitSystem() );
}

/*!
  Convert value to default SI units according to current units
*/
Standard_Real DDS_DicItem::ToSI( const Standard_Real theVal, const UnitSystem& theUnitsSystem ) const
{
  Standard_Real aRes = theVal;
  UnitData* anUnitData = GetUnitData( theUnitsSystem );
  if ( anUnitData )
    aRes = anUnitData->myZero + aRes * anUnitData->myScale;
  return aRes;
}

/*!
  Convert value from default SI units according to current units
*/
Standard_Real DDS_DicItem::FromSI( const Standard_Real theVal, const UnitSystem& theUnitsSystem ) const
{
  Standard_Real aRes = theVal;
  UnitData* anUnitData = GetUnitData( theUnitsSystem );
  if ( anUnitData )
    aRes = ( aRes - anUnitData->myZero ) / anUnitData->myScale;
  return aRes;
}

/*!
  Returns 'true' if specified data exist.
*/
Standard_Boolean DDS_DicItem::HasData( const Standard_Integer flag ) const
{
  return ( myData & flag ) == flag;
}

/*!
  Parse record in XML file and retrieve information relevant for this data dic item
*/
void DDS_DicItem::FillDataMap( TCollection_AsciiString theID, const LDOM_Element& theDatum,
                               const LDOM_Element& theCompElement, const LDOM_Element& theDocElement,
                               const TColStd_SequenceOfAsciiString& theSystems )
{
  TCollection_AsciiString aLabel    = theDatum.getAttribute( DDS_Dictionary::KeyWord( "DATUM_LABEL" ) );
  TCollection_AsciiString aFormat   = theDatum.getAttribute( DDS_Dictionary::KeyWord( "DATUM_FORMAT" ) );
  TCollection_AsciiString aFilter   = theDatum.getAttribute( DDS_Dictionary::KeyWord( "DATUM_FILTER" ) );
  TCollection_AsciiString aRequired = theDatum.getAttribute( DDS_Dictionary::KeyWord( "DATUM_REQUIRED" ) );

  TCollection_AsciiString aBaseKeyWord = DDS_Dictionary::KeyWord( "DATUM_UNITS" );

  for ( Standard_Integer j = 1; j <= theSystems.Length(); j++ )
  {
    UnitSystem anUnitSystem = theSystems.Value( j );
    if ( !anUnitSystem.Length() )
      continue;

    TCollection_AsciiString aUnitKeyword = anUnitSystem + aBaseKeyWord;

    if ( !myUnitData.IsBound( anUnitSystem ) )
      myUnitData.Bind( anUnitSystem, UnitData() );

    UnitData& anUnitData = myUnitData.ChangeFind( anUnitSystem );
    anUnitData.myUnits = theDatum.getAttribute( LDOMString( aUnitKeyword.ToCString() ) );
  }

  if ( theSystems.Length() && myUnitData.IsBound( theSystems.First() ) &&
       !myUnitData.Find( theSystems.First() ).myUnits.Length() )
  {
    TCollection_AsciiString units = theDatum.getAttribute( LDOMString( aBaseKeyWord.ToCString() ) );
    if ( units.Length() )
      myUnitData.ChangeFind( theSystems.First() ).myUnits = units;
  }

  TCollection_AsciiString units;
  for ( NCollection_DataMap<UnitSystem, UnitData>::Iterator iter( myUnitData ); iter.More() && units.IsEmpty(); iter.Next() )
    units = iter.Value().myUnits;

  for ( NCollection_DataMap<UnitSystem, UnitData>::Iterator itr( myUnitData ); itr.More(); itr.Next() )
  {
    UnitData& dataUnits = itr.ChangeValue();
    if ( dataUnits.myUnits.IsEmpty() )
      dataUnits.myUnits = units;
  }
  
  // 2. Elements ( domain, description )
  Standard_Real aRealMinV = 0;
  Standard_Real aRealMaxV = 0;
  Standard_Real aRealDefV = 0;

  TCollection_AsciiString aType;

  DDS_MsgType aWrongValue = DDS_MT_NONE;
  DDS_DicItem::Type aEnumType = DDS_DicItem::Unknown;

  TCollection_AsciiString aMinV;
  TCollection_AsciiString aMaxV;
  TCollection_AsciiString aDefV;
  TCollection_AsciiString aListName;

  TCollection_AsciiString aLongD;
  TCollection_AsciiString aShortD;

  TColStd_SequenceOfInteger aSeqOfValueID;
  TColStd_SequenceOfExtendedString aSeqOfValue;
  TColStd_SequenceOfExtendedString aSeqOfValueIconName;

  // Presentation
  Standard_Real aMinZoom   = 0;
  Standard_Real aMaxZoom   = 0;
  Standard_Real aZoomOrder = 0;

  // Datum::Reports tags (if any)
  LDOM_Element aWLev = theDatum.GetChildByTagName( DDS_Dictionary::KeyWord( "WARNING_LEVEL" ) );
  if ( !aWLev.isNull() )
  {
    TCollection_AsciiString aWrongValWL = aWLev.getAttribute( DDS_Dictionary::KeyWord( "WRONG_VALUE" ) );
    if ( aWrongValWL.IsEqual( "Info" ) )
      aWrongValue = DDS_MT_INFO;
    else if ( aWrongValWL.IsEqual( "Warning" ) )
      aWrongValue = DDS_MT_WARNING;
    else if ( aWrongValWL.IsEqual( "Alarm" ) )
      aWrongValue = DDS_MT_ALARM;
    else if ( aWrongValWL.IsEqual( "Error" ) )
      aWrongValue = DDS_MT_ERROR;
  }

  // Datum::Presentation
  LDOM_Element aPrs = theDatum.GetChildByTagName( DDS_Dictionary::KeyWord( "PRS" ) );
  if ( !aPrs.isNull() )
  {
    LDOM_Element aLateralZoom = aPrs.GetChildByTagName( DDS_Dictionary::KeyWord( "LATERAL_ZOOM" ) );
    if ( !aLateralZoom.isNull() )
    {
      TCollection_AsciiString aMinZoomStr   = aLateralZoom.getAttribute( DDS_Dictionary::KeyWord( "LZ_MINV" ) );
      TCollection_AsciiString aMaxZoomStr   = aLateralZoom.getAttribute( DDS_Dictionary::KeyWord( "LZ_MAXV" ) );
      TCollection_AsciiString aZoomOrderStr = aLateralZoom.getAttribute( DDS_Dictionary::KeyWord( "LZ_ORDER" ) );
      
      aMinZoomStr.RemoveAll( ' ' );
      if ( aMinZoomStr.IsRealValue() )
        aMinZoom = aMinZoomStr.RealValue();

      aMaxZoomStr.RemoveAll( ' ' );
      if ( aMaxZoomStr.IsRealValue() )
        aMaxZoom = aMaxZoomStr.RealValue();

      aZoomOrderStr.RemoveAll( ' ' );
      if ( aZoomOrderStr.IsRealValue() )
        aZoomOrder = aZoomOrderStr.RealValue();
    }
  }

  // Quantity::Domain record as the only child of that tag name
  LDOM_Element aDomain = theDatum.GetChildByTagName( DDS_Dictionary::KeyWord( "DY_DOMAIN" ) );
  if ( !aDomain.isNull() )
  {
    LDOM_Element aValueDescr = aDomain.GetChildByTagName( DDS_Dictionary::KeyWord( "VALUE_DESCR" ) );
    if ( !aValueDescr.isNull() )
    {
      // read: valueDescr? (type?,min?,max?,default?)
      aType = aValueDescr.getAttribute( DDS_Dictionary::KeyWord( "VD_TYPE" ) );
      if ( aType.IsEqual( "String" ) )
        aEnumType = String;
      else if ( aType.IsEqual( "Float" ) )
        aEnumType = Float;
      else if ( aType.IsEqual( "Integer" ) )
        aEnumType = Integer;

      if ( !aValueDescr.getAttributeNode( DDS_Dictionary::KeyWord( "VD_MINV" ) ).isNull() )
        myData |= MinValue;
      aMinV = aValueDescr.getAttribute( DDS_Dictionary::KeyWord( "VD_MINV" ) );
      aMinV.RemoveAll( ' ' );
      if ( aMinV.IsRealValue() )
        aRealMinV = aMinV.RealValue();
      if ( !aValueDescr.getAttributeNode( DDS_Dictionary::KeyWord( "VD_MAXV" ) ).isNull() )
        myData |= MaxValue;
      aMaxV = aValueDescr.getAttribute( DDS_Dictionary::KeyWord( "VD_MAXV" ) );
      aMaxV.RemoveAll( ' ' );
      if ( aMaxV.IsRealValue() )
        aRealMaxV = aMaxV.RealValue();
      aDefV = aValueDescr.getAttribute( DDS_Dictionary::KeyWord( "VD_DEFV" ) );
      if ( !aValueDescr.getAttributeNode( DDS_Dictionary::KeyWord( "VD_DEFV" ) ).isNull() )
        myData |= DefaultValue;

      aDefV.RemoveAll( ' ' );
      if ( aDefV.IsRealValue() )
        aRealDefV = aDefV.RealValue();

      TCollection_AsciiString aSpecVal = aValueDescr.getAttribute( DDS_Dictionary::KeyWord( "VD_SPEC" ) );
      Split( aSpecVal, myListRef );
    }
    else
    {
      //  read: listRef? (list?)
      LDOM_Element aListRef = aDomain.GetChildByTagName( DDS_Dictionary::KeyWord( "VALUE_LIST_REF" ) );
      if ( !aListRef.isNull() )
      {
        aType = "List";
        aEnumType = List;                       
        LDOMString aListId = aListRef.getAttribute( DDS_Dictionary::KeyWord( "VLR_LIST" ) );
        aDefV = aListRef.getAttribute( DDS_Dictionary::KeyWord( "VD_DEFV" ) );
        aDefV.RemoveAll( ' ' );
        LDOM_Element foundListItem;
        for ( LDOM_Element aListItem = theCompElement.GetChildByTagName( DDS_Dictionary::KeyWord( "VALUE_LIST" ) );
              aListItem != NULL && foundListItem == NULL; aListItem = aListItem.GetSiblingByTagName() )
        {
          if ( aListItem.getAttribute( DDS_Dictionary::KeyWord( "VALUE_LIST_ID" ) ).equals( aListId ) )
            foundListItem = aListItem;

        }
        for ( LDOM_Element aLstItem = theDocElement.GetChildByTagName( DDS_Dictionary::KeyWord( "VALUE_LIST" ) );
              aLstItem != NULL && foundListItem == NULL; aLstItem = aLstItem.GetSiblingByTagName() )
        {
          if ( aLstItem.getAttribute( DDS_Dictionary::KeyWord( "VALUE_LIST_ID" ) ).equals( aListId ) )
            foundListItem = aLstItem;
        }

        if ( foundListItem != NULL )
        {
          //  The appropriate list of values is found: store the list name
          aListName = foundListItem.getAttribute( DDS_Dictionary::KeyWord( "VALUE_LIST_NAME" ) );
          //  Iteration through the list of values
          LDOM_Element aListItemValue = foundListItem.GetChildByTagName( DDS_Dictionary::KeyWord( "VALUE_LIST_VALUE" ) );
          while ( aListItemValue != NULL )
          {
            // read value ID
            TCollection_AsciiString aListValueID = aListItemValue.getAttribute( DDS_Dictionary::KeyWord( "VALUE_LIST_VALUEID" ) );
            if ( aListValueID.IsIntegerValue() )
            {
              //  Read the text in the element "value"
              //LDOM_Text aListItemTxt = (const LDOM_Text&)aListItemValue.getFirstChild();
	      LDOM_Node aNode = aListItemValue.getFirstChild();
	      const LDOM_Text& aText = (const LDOM_Text&) aNode;
	      LDOM_Text aListItemTxt(aText);
              if ( !aListItemTxt.isNull() )
              {
                // adding ID and text value to sequence
                aSeqOfValueID.Append( aListValueID.IntegerValue() );
                aSeqOfValue.Append( aListItemTxt.getData() );
                // adding icon file name (optional) to sequence
                TCollection_ExtendedString aListValueIcon = aListItemValue.getAttribute( DDS_Dictionary::KeyWord( "VALUE_LIST_VALUEICON" ) );
                aSeqOfValueIconName.Append( aListValueIcon );
              }
              aListItemValue = aListItemValue.GetSiblingByTagName();
            }
          }
        }
      }
    }
  }

  // Quantity::Description record as the only child of that tag name
  LDOM_Element aDescr = theDatum.GetChildByTagName( DDS_Dictionary::KeyWord( "DESCR" ) );
  if ( !aDescr.isNull() )
  {
    // short description (#PCDATA)*
    LDOM_Element aShDescr = aDescr.GetChildByTagName( DDS_Dictionary::KeyWord( "SHORT_D" ) );
    if ( !aShDescr.isNull() )
    {
      // text is always a sub-node of element, containing it
      //LDOM_Text aShDescrTxt = (const LDOM_Text&)aShDescr.getFirstChild();
      LDOM_Node aNode = aShDescr.getFirstChild();
      const LDOM_Text& aText = (const LDOM_Text&) aNode;
      LDOM_Text aShDescrTxt(aText);
      if ( !aShDescrTxt.isNull() )
        aShortD = aShDescrTxt.getData();
    }

    // long description (#PCDATA)*
    LDOM_Element aLDescr = aDescr.GetChildByTagName( DDS_Dictionary::KeyWord( "LONG_D" ) );
    if ( !aLDescr.isNull() )
    {
      // text is always a sub-node of element, containing it
      //LDOM_Text aLDescrTxt = (const LDOM_Text&)aLDescr.getFirstChild();
      LDOM_Node aNode = aLDescr.getFirstChild();
      const LDOM_Text& aText = (const LDOM_Text&) aNode;
      LDOM_Text aLDescrTxt(aText);
      if ( !aLDescrTxt.isNull() )
        aLongD = aLDescrTxt.getData();
    }
  }

  NCollection_DataMap<UnitSystem, Handle(Units_Dimensions)> aDimMap;

  for ( NCollection_DataMap<UnitSystem, UnitData>::Iterator it( myUnitData ); it.More(); it.Next() )
  {
    UnitData& anUnitData = it.ChangeValue();

    // check units
    anUnitData.myZero  = 0.;
    anUnitData.myScale = 1.;
    try {
      Standard_CString aUnitDataStr;
      aUnitDataStr = (Standard_CString)anUnitData.myUnits.ToCString();
      if ( anUnitData.myUnits.ToCString()[0] && strcmp( anUnitData.myUnits.ToCString(), "%" ) )
      {
        Handle(Units_Dimensions) aDim;
        anUnitData.myZero  = UnitsAPI::AnyToSI( 0.0, aUnitDataStr, aDim );
        anUnitData.myScale = UnitsAPI::AnyToSI( 1.0, aUnitDataStr, aDim ) - anUnitData.myZero;
        UnitsAPI::AnyFromSI( 1.0, aUnitDataStr );
        if ( !aDimMap.IsBound( it.Key() ) )
          aDimMap.Bind( it.Key(), aDim );
      }
      else if ( anUnitData.myUnits.ToCString()[0] ) // treat '%' as unit with scale 100
        anUnitData.myScale = 0.01;
    }
	  catch( Standard_Failure ) {
      anUnitData.myUnits.Clear();
    }

    Handle(Units_Dimensions) aPrev;
    Standard_Boolean aStatus = Standard_True;
    for ( NCollection_DataMap<UnitSystem, Handle(Units_Dimensions)>::Iterator itr( aDimMap );
          itr.More() && aStatus; itr.Next() )
    {
      if ( itr.Value().IsNull() )
        continue;

      if ( aPrev.IsNull() )
        aPrev = itr.Value();

      aStatus = aPrev->IsEqual( itr.Value() );
    }

    if ( !aStatus )
      printf( "Error in DataDictionary: Different dimensions for %s item", theID.ToCString() );
  }

  myId                = theID;
  myType              = aEnumType;
  myWarnLevel         = aWrongValue;
  myLabel             = aLabel.ToCString();
  myFilter            = aFilter.ToCString();
  myLongDescr         = aLongD.ToCString();
  myShortDescr        = aShortD.ToCString();
  myMin               = aRealMinV;
  myMax               = aRealMaxV;
  myDefValue          = aRealDefV;
  myDefString         = aDefV.ToCString();
  myRequired          = aRequired.ToCString();
  myListName          = aListName.ToCString();
  myMinZoom           = aMinZoom;
  myMaxZoom           = aMaxZoom;
  myZoomOrder         = aZoomOrder;

  // prepare formats
  PrepareFormats( aFormat );

  const Standard_Integer aLength = aSeqOfValue.Length();
  if ( aLength > 0 )
  {
    myListRef      = new TColStd_HArray1OfExtendedString( 1, aLength );
    myListRefID    = new TColStd_HArray1OfInteger( 1, aLength );
    myListRefIcons = new TColStd_HArray1OfExtendedString( 1, aLength );
    for ( Standard_Integer i = aLength; i > 0; i-- )
    {
      myListRef->ChangeValue( i ) = aSeqOfValue.Value( i );
      myListRefID->ChangeValue( i ) = aSeqOfValueID.Value( i );
      myListRefIcons->ChangeValue( i ) = aSeqOfValueIconName.Value( i );
    }
  }

  if ( myType == List && myDefString == "" && !myListRef.IsNull() && myListRef->Length() > 0 )
    myDefString = myListRef->Value( myListRef->Lower() );
}

/*!
  Returns default formats for each unit systems
*/
void DDS_DicItem::GetDefaultFormat()
{
  for ( NCollection_DataMap<UnitSystem, UnitData>::Iterator it( myUnitData ); it.More(); it.Next() )
  {
    UnitData& anUnitData = it.ChangeValue();

    switch ( myType )
    {
    case Integer:
      anUnitData.myFormat = "%d";
      break;
    case Float:
      anUnitData.myFormat = "%g";
      break;
    case String:
    default:
      anUnitData.myFormat.Clear();
      break;;
    }
  }
}

/*!
  Returns format for the string
*/
void DDS_DicItem::GetStringFormat( const TCollection_AsciiString& theFlags,
                                   const TCollection_AsciiString& theWidth,
                                   const TCollection_AsciiString& thePrecision,
                                   const TCollection_AsciiString& theTypePrefix,
                                   TCollection_AsciiString& theFormat )
{
  theFormat = "%";
  theFormat += theFlags;
  theFormat += theWidth;

  if ( !thePrecision.IsEmpty() ) 
  {
    theFormat += ".";
    theFormat += thePrecision;
  }

  theFormat += theTypePrefix;
  theFormat += "s";
}

/*!
  Returns format for the integer
*/
void DDS_DicItem::GetIntegerFormat( const TCollection_AsciiString& theFlags,
                                    const TCollection_AsciiString& theWidth,
                                    const TCollection_AsciiString& thePrecision,
                                    const TCollection_AsciiString& theTypePrefix,
                                    const Standard_Character theType,
                                    TCollection_AsciiString& theFormat )
{
  Standard_Integer aPrecision = 0;
  if ( !thePrecision.IsEmpty() )
    aPrecision = thePrecision.IntegerValue();
  Standard_Integer aWidth = 0;

  if ( !theWidth.IsEmpty() )
    aWidth = theWidth.IntegerValue();

  if ( !thePrecision.IsEmpty() && aPrecision < 0 )
  {
    // possible value 0.1 will be 10.0
    aWidth -= aPrecision;
    aPrecision = 0;
  }

  if ( !thePrecision.IsEmpty() && aPrecision > ( aWidth - 2 ) )
    aWidth = aPrecision + 2;

  theFormat = "%";

  theFormat += theFlags;
  if ( !theWidth.IsEmpty() )
    theFormat += aWidth;

  theFormat += theTypePrefix;
  theFormat += theType;
}

/*!
  Returns format for the float
*/
void DDS_DicItem::GetFloatFormat( const TCollection_AsciiString& theFlags,
                                  const TCollection_AsciiString& theWidth,
                                  const TCollection_AsciiString& thePrecision,
                                  const TCollection_AsciiString& theTypePrefix,
                                  const Standard_Character theType,
                                  TCollection_AsciiString& theFormat )
{
  Standard_Integer aPrecision = 0;
  if ( !thePrecision.IsEmpty() )
    aPrecision = thePrecision.IntegerValue();
  Standard_Integer aWidth = 0;

  if (!theWidth.IsEmpty() )
    aWidth = theWidth.IntegerValue();

  if (!thePrecision.IsEmpty() && aPrecision < 0 )
  {
    // possible value 0.1 will be 10.0
    aWidth -= aPrecision;
    aPrecision = 0;
  }

  if ( !thePrecision.IsEmpty() && aPrecision > ( aWidth - 2 ) )
  {
    aWidth = aPrecision + 2;
  }

  theFormat = "%";
  theFormat += theFlags;

  if ( !theWidth.IsEmpty() ) 
    theFormat += aWidth;

  if ( !thePrecision.IsEmpty() ) 
  {
    theFormat += ".";
    theFormat += aPrecision;
  }

  theFormat += theTypePrefix;
  theFormat += theType;
}

/*!
  Prepares three formants for each unit systems
*/
void DDS_DicItem::PrepareFormats( const TCollection_AsciiString&  theFormat )
{
  for ( NCollection_DataMap<UnitSystem, UnitData>::Iterator it( myUnitData ); it.More(); it.Next() )
  {
    UnitData& anUnitData = it.ChangeValue();

    anUnitData.myFormat = theFormat;
    anUnitData.myPrecision = 0;
  }

  TCollection_AsciiString aPrecisionStr;
  if ( theFormat.IsEmpty() && myType == List )
    return;

  // checking % presenting
  if ( *theFormat.ToCString() != '%' )
  {
    GetDefaultFormat();
    return;
  }

  TCollection_AsciiString aStr = ( theFormat.ToCString() + 1 );
  Standard_Character aType = aStr.Value( aStr.Length() );

  if ( ( aType != 's' && myType == String ) ||
       ( aType != 'd' && myType == Integer ) ||
       ( aType != 'f' && aType != 'g' && aType != 'e' && aType != 'G' && aType != 'E' && myType == Float ) )
  {
    GetDefaultFormat();
    return;
  }

  // removing type character
  aStr.Trunc( aStr.Length() - 1 );

  TCollection_AsciiString aFlags;
  while ( !aStr.IsEmpty() && aStr.Value( 1 ) != '.' && ( aStr.Value( 1 ) < '0' || aStr.Value( 1 ) > '9' ) )
  {
    aFlags = aFlags + aStr.Value( 1 );
    aStr.Remove( 1 );
  }

  Standard_Integer aPos = 1;
  while ( aPos <= aStr.Length() && ( aStr.Value( aPos ) == '.' ||
          ( aStr.Value( aPos ) >= '0' && aStr.Value( aPos ) <= '9' ) ) )
    aPos++;

  TCollection_AsciiString aTypePrefix;
  if ( aPos <= aStr.Length() )
  {
    aTypePrefix = aStr.SubString( aPos, aStr.Length() );
    aStr.Trunc( aPos - 1 );
  }

  Standard_Integer aBasePrecision = 0;

  // taking width and precision
  TCollection_AsciiString aPrecision;

  aPos = aStr.Search( "." );
  if ( aPos >= 0 ) 
  {
    // aPrecision is defined
    aPrecision = aStr.Split( aPos );
    aStr.Remove( aStr.Length() );
    if ( !aPrecision.IsEmpty() )
    {
      if ( !aPrecision.IsIntegerValue() ) 
      { 
        GetDefaultFormat();
        return;
      }
      else
      {
        aPrecisionStr  = aPrecision;
        aBasePrecision = aPrecision.IntegerValue();
      }
    }
  }

  if ( !aStr.IsEmpty() && !aStr.IsIntegerValue() )
  {
    GetDefaultFormat();
    return;
  }

  NCollection_DataMap<UnitSystem, UnitData>::Iterator itr;

  switch ( myType )
  {
  case String:
    for ( itr.Initialize( myUnitData ); itr.More(); itr.Next() )
    {
      if ( aType != 'f' && aType != 'g' && aType != 'e' && aType != 'G' && aType != 'E' )
        GetStringFormat( aFlags, aStr, aPrecisionStr, aTypePrefix, itr.ChangeValue().myFormat );
    }
    break;
  case Float:
  case Integer:
    for ( itr.Initialize( myUnitData ); itr.More(); itr.Next() )
    {
      UnitData& anUnitData = itr.ChangeValue();
      Standard_Integer aAmendment =
        (Standard_Integer)log10( 10.0 / DDS_Dictionary::FromSI( 10.0, anUnitData.myUnits.ToCString() ) );
      anUnitData.myPrecision = aBasePrecision + aAmendment;
      aPrecisionStr = TCollection_AsciiString( anUnitData.myPrecision );

      // create a formats
      if ( myType == Integer )
        GetIntegerFormat( aFlags, aStr, aPrecisionStr, aTypePrefix, aType, anUnitData.myFormat );
      else
        GetFloatFormat( aFlags, aStr, aPrecisionStr, aTypePrefix, aType, anUnitData.myFormat );
    }
    break;
  default:;
    GetDefaultFormat();
    break;
  }
}

void DDS_DicItem::Split( const TCollection_AsciiString& theStr, Handle(TColStd_HArray1OfExtendedString)& aRes )
{
  aRes.Nullify();

  if ( theStr.Length() > 0 )
  {
    TCollection_AsciiString aStr = theStr;
    TColStd_SequenceOfAsciiString aSeq;
    Standard_Integer anIndex = aStr.SearchFromEnd( (Standard_CString)" " );
    while( anIndex > 1 )
    {
      TCollection_AsciiString tmpStr = aStr.Split( anIndex - 1 );
      tmpStr.RemoveAll( ( Standard_Character )' ' );
      if ( tmpStr.Length() > 0 )
        aSeq.Append( tmpStr );
      anIndex = aStr.SearchFromEnd( (Standard_CString)" " );
    }

    aStr.RemoveAll( ( Standard_Character )' ' );
    if ( aStr.Length() > 0 )
      aSeq.Append( aStr );

    if ( aSeq.Length() > 0 )
    {
      aRes = new TColStd_HArray1OfExtendedString( 1, aSeq.Length() );
      for ( int i = 1, n = aSeq.Length(); i <= n; i++ )
        aRes->ChangeValue( i ) = aSeq( i );
    }
  }
}

DDS_DicItem::UnitData* DDS_DicItem::GetUnitData( const UnitSystem& sys ) const
{
  UnitData* unit = 0;

  if ( myUnitData.IsBound( sys ) )
    unit = (UnitData*)&myUnitData.Find( sys );

  return unit;
}

DDS_DicItem::UnitSystem DDS_DicItem::GetActiveUnitSystem() const
{
  UnitSystem aSystem;
  Handle(DDS_DicGroup) aComponent = Handle(DDS_DicGroup)::DownCast(myComponent);
  if ( !aComponent.IsNull() )
    aSystem = aComponent->GetActiveUnitSystem();
  return aSystem;
}
