#include "DDS_DicGroup.h"

#include "DDS_Dictionary.h"

#include <LDOMString.hxx>
#include <LDOM_Element.hxx>

#include <UnitsAPI.hxx>

#include <TColStd_SequenceOfAsciiString.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx>

IMPLEMENT_STANDARD_HANDLE(DDS_DicGroup, MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(DDS_DicGroup, MMgt_TShared)

DDS_DicGroup::DDS_DicGroup( const TCollection_AsciiString& name )
: MMgt_TShared(),
myName( name ),
myActiveSystem( UNIT_SYSTEM_SI )
{
}

DDS_DicGroup::DDS_DicGroup( const DDS_DicGroup& )
{
}

TCollection_AsciiString DDS_DicGroup::GetName() const
{
  return myName;
}

void DDS_DicGroup::GetUnitSystems( TColStd_SequenceOfAsciiString& theSystemSeq ) const
{
  theSystemSeq.Clear();
  for ( UnitSystemMap::Iterator it( myUnitSystem ); it.More(); it.Next() )
  {
    if ( it.Key() == TCollection_AsciiString( UNIT_SYSTEM_SI ) )
      theSystemSeq.Prepend( it.Key() );
    else
      theSystemSeq.Append( it.Key() );
  }
}

TCollection_ExtendedString DDS_DicGroup::GetUnitSystemLabel( const TCollection_AsciiString& name ) const
{
  TCollection_ExtendedString aLabel;
  if ( myUnitSystem.IsBound( name ) )
    aLabel = myUnitSystem.Find( name );
  return aLabel;
}

TCollection_AsciiString DDS_DicGroup::GetActiveUnitSystem() const
{
  return myActiveSystem;
}

void DDS_DicGroup::SetActiveUnitSystem( const TCollection_AsciiString& theSystem )
{
  if ( myUnitSystem.IsBound( theSystem ) )
    myActiveSystem = theSystem;
}

void DDS_DicGroup::operator=( const DDS_DicGroup& )
{
}

void DDS_DicGroup::FillDataMap( const LDOM_Element& theComponentData, const LDOM_Element& theDocElement )
{
  TCollection_AsciiString aCompName = theComponentData.getAttribute( DDS_Dictionary::KeyWord( "COMPONENT_NAME" ) );

  LDOM_Element systems = theComponentData.GetChildByTagName( DDS_Dictionary::KeyWord( "UNIT_SYSTEMS" ) );
  if ( !systems.isNull() )
  {
    LDOM_NodeList systemList = systems.getElementsByTagName( DDS_Dictionary::KeyWord( "UNIT_SYSTEM" ) );
    for ( Standard_Integer i = 0; i < systemList.getLength(); i++ )
    {
      LDOM_Element aSystem = (const LDOM_Element &)systemList.item( i );
      TCollection_AsciiString aName = aSystem.getAttribute( DDS_Dictionary::KeyWord( "UNIT_SYSTEM_NAME" ) );
      TCollection_ExtendedString aLabel = aSystem.getAttribute( DDS_Dictionary::KeyWord( "UNIT_SYSTEM_LABEL" ) );

      if ( aName.IsEmpty() )
        continue;

      if ( !myUnitSystem.IsBound( aName ) )
        myUnitSystem.Bind( aName, aLabel );
    }
  }

  if ( !myUnitSystem.IsBound( UNIT_SYSTEM_SI ) )
  {
    printf( "Warning: Mandatory unit system SI not defined in component: \"%s\". Added automaticaly", aCompName.ToCString() );
    myUnitSystem.Bind( UNIT_SYSTEM_SI, TCollection_ExtendedString( "System international" ) );
  }

  TColStd_SequenceOfAsciiString unitSystems;
  GetUnitSystems( unitSystems );

  LDOM_NodeList aData = theComponentData.getElementsByTagName( DDS_Dictionary::KeyWord( "DATUM" ) );
  if ( !aData.getLength() )
    return;

  for ( Standard_Integer i = 0; i < aData.getLength(); i++ )
  {
    LDOM_Element aQuantity = (const LDOM_Element&)aData.item( i );

    // 1. Attributes (id,label,units?,format?,required?)
    TCollection_AsciiString anID = aQuantity.getAttribute( DDS_Dictionary::KeyWord( "DATUM_ID" ) );
    Handle(DDS_DicItem) aDicItem = new DDS_DicItem();

    aDicItem->myComponent = this;
    aDicItem->FillDataMap( anID, aQuantity, theComponentData, theDocElement, unitSystems );
    myDataMap.Add( anID, aDicItem );
  }
}

/*!
  Returns DicItem with all attached data
*/

Handle(DDS_DicItem) DDS_DicGroup::GetDicItem( const TCollection_AsciiString& theID ) const
{
  Handle(DDS_DicItem) aDicItem;
  // get dictionary item by id
  if ( myDataMap.Contains( theID ) )
    aDicItem = myDataMap.FindFromKey( theID );

  return aDicItem;
}
