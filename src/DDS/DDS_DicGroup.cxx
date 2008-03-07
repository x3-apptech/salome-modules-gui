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

/*!
  \class DDS_DicGroup
  
  This class to provide set of DDS_DicItem objects from one component.
*/

/*!
  Constructor. Create the group with name \aname.
*/
DDS_DicGroup::DDS_DicGroup( const TCollection_AsciiString& name )
: MMgt_TShared(),
myName( name ),
myActiveSystem( UNIT_SYSTEM_SI )
{
}

/*!
  Copy constructor.
*/
DDS_DicGroup::DDS_DicGroup( const DDS_DicGroup& )
{
}

/*!
  Get the name of group (component).
*/
TCollection_AsciiString DDS_DicGroup::GetName() const
{
  return myName;
}

/*!
  Returns the names list of defined unit systems.
  Parameter \atheSystems will contains the sequence of string names.
*/
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

/*!
  Returns the label of unit system \aname. If unit system not found then empty string returned.
*/
TCollection_ExtendedString DDS_DicGroup::GetUnitSystemLabel( const TCollection_AsciiString& name ) const
{
  TCollection_ExtendedString aLabel;
  if ( myUnitSystem.IsBound( name ) )
    aLabel = myUnitSystem.Find( name );
  return aLabel;
}

/*!
  Gets the name of active unit system.
*/
TCollection_AsciiString DDS_DicGroup::GetActiveUnitSystem() const
{
  return myActiveSystem;
}

/*!
  Sets the name of active unit system.
*/
void DDS_DicGroup::SetActiveUnitSystem( const TCollection_AsciiString& theSystem )
{
  if ( myUnitSystem.IsBound( theSystem ) )
    myActiveSystem = theSystem;
}

/*!
  Assignment operator.
*/
void DDS_DicGroup::operator=( const DDS_DicGroup& )
{
}

/*!
  Fill the internal data structures from XML parsed structures. Internal.
*/
void DDS_DicGroup::FillDataMap( const LDOM_Element& theComponentData, const LDOM_Element& theDocElement )
{
  TCollection_AsciiString aCompName = theComponentData.getAttribute( DDS_Dictionary::KeyWord( "COMPONENT_NAME" ) );

  LDOM_Element systems = theComponentData.GetChildByTagName( DDS_Dictionary::KeyWord( "UNIT_SYSTEMS" ) );
  if ( !systems.isNull() )
  {
    LDOM_NodeList systemList = systems.getElementsByTagName( DDS_Dictionary::KeyWord( "UNIT_SYSTEM" ) );
    for ( Standard_Integer i = 0; i < systemList.getLength(); i++ )
    {
      //const LDOM_Element& aSystem = (const LDOM_Element &)systemList.item( i );
      LDOM_Node aNode = systemList.item( i );
      const LDOM_Element& anElem = (const LDOM_Element&) aNode;
      LDOM_Element aSystem(anElem);
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
    //LDOM_Element aQuantity = (const LDOM_Element&)aData.item( i );
    LDOM_Node aNode = aData.item( i );
    const LDOM_Element& anElem = (const LDOM_Element&) aNode;
    LDOM_Element aQuantity(anElem);

    // 1. Attributes (id,label,units?,format?,required?)
    TCollection_AsciiString anID = aQuantity.getAttribute( DDS_Dictionary::KeyWord( "DATUM_ID" ) );
    Handle(DDS_DicItem) aDicItem = new DDS_DicItem();

    aDicItem->myComponent = this;
    aDicItem->FillDataMap( anID, aQuantity, theComponentData, theDocElement, unitSystems );
    myDataMap.Add( anID, aDicItem );
  }
}

/*!
  Gets dictionary item with specified identifier \atheID.
  If dictionary item not found then null handle returned.
*/
Handle(DDS_DicItem) DDS_DicGroup::GetDicItem( const TCollection_AsciiString& theID ) const
{
  Handle(DDS_DicItem) aDicItem;
  // get dictionary item by id
  if ( myDataMap.Contains( theID ) )
    aDicItem = myDataMap.FindFromKey( theID );

  return aDicItem;
}
