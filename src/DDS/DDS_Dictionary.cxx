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

/*!
  \class DDS_Dictionary
  
  This class to provide information about used datums, reading them from 'xml' file

  Datum is the set of parameters described a phisical characteristic. These parameters loaded from
  special XML which has following format:

  \hr

  <D_URI>
  <COMPONENT COMPONENT_NAME="component_name">
    <UNIT_SYSTEMS>
      <UNIT_SYSTEM UNIT_SYSTEM_NAME="[system_internal_name]" UNIT_SYSTEM_LABEL="[system_label]">
      <UNIT_SYSTEM UNIT_SYSTEM_NAME="[system_internal_name]" UNIT_SYSTEM_LABEL="[system_label]">
      ...
      <UNIT_SYSTEM UNIT_SYSTEM_NAME="[system_internal_name]" UNIT_SYSTEM_LABEL="[system_label]">
    </UNIT_SYSTEMS>

    <DATUM DATUM_UNITS="[base_system_internal_name]" DATUM_FORMAT="[sprintf_format_specification]"
           DATUM_ID="[datum_id]" DATUM_LABEL="[datum_label]" DATUM_REQUIRED="[requred_value]"
           DATUM_FILTER="[string_regular_expression]">
      <DY_DOMAIN>
        <VALUE_DESCR VD_DEFV="[default_value]" VD_MAXV="[max_value]" VD_MINV="[min_value]" VD_TYPE="[value_type]"/>
        <VALUE_LIST_REF VLR_LIST="[referenced_list_id]"/>
      </DY_DOMAIN>
      <DESCR>
        <SHORT_D>[brief_desription_text]</SHORT_D>
        <LONG_D>[full_description_text]</LONG_D>
      </DESCR>
      <OPTIONS>
        <OPTION OPTION_NAME="[option_name_1]">[option_value_1]</OPTION>
        <OPTION OPTION_NAME="[option_name_2]">[option_value_2]</OPTION>
        ...
        <OPTION OPTION_NAME="[option_name_n]">[option_value_n]</OPTION>
      </OPTIONS>
    </DATUM>

    ...

    <VALUE_LIST VALUE_LIST_ID="[list_id]" VALUE_LIST_NAME="[list_name]">
      <VALUE_LIST_VALUE VALUE_LIST_VALUEID="[list_element_id]" VALUE_LIST_VALUEICON="[list_element_icon_file]">[list_element_name]</VALUE_LIST_VALUE>
      <VALUE_LIST_VALUE VALUE_LIST_VALUEID="[list_element_id]" VALUE_LIST_VALUEICON="[list_element_icon_file]">[list_element_name]</VALUE_LIST_VALUE>
      ...
      <VALUE_LIST_VALUE VALUE_LIST_VALUEID="[list_element_id]" VALUE_LIST_VALUEICON="[list_element_icon_file]">[list_element_name]</VALUE_LIST_VALUE>
    </VALUE_LIST>

    ...

    </COMPONENT>
  </D_URI>

  \hr

  In description of datum file format used internal keys as XML tag and attributes names.
  Concrete XML keywords defined by DDS_KeyWords class.

  Describe datum file format more detailed. XML file should have one main tag named as "dictionary" (key "D_URI").
  This tag should contains one or several components. Component is a independent set of datums and unit systems.
  Components defined as XML tag named "component" (key "COMPONENT") with attribute "name" (key COMPONENT_NAME).
  Component name used as component identifier and should be unique. Component tag can contains:

  \li Tag "unit_systems" (key UNIT_SYSTEMS) defines set of used unit systems. Should exist at least one unit
  system named SI ("System International"). If this system not exist then it will be defined automatically.
  Each unit system defined by XML tag "unit system" (key UNIT_SYSTEM) under tag "unit_systems" with attributes
  "name" (key UNIT_SYSTEM_NAME) and "label" (key UNIT_SYSTEM_LABEL). Name is identifier of unit system. Label is
  human readable description.

  \li One or several tag "datum" (key DATUM). For this tag can be defined following attributes:

      \lo Identifier (key DATUM_ID) specify the unique id string for the datum.
      \lo Label (key DATUM_LABEL) specify human readable name of the datum.
      \lo Units of measure (key DATUM_UNITS) for given unit system. Attribute name consist of a name
          of unit system and a keyword got from DDS_KeyWords by key DATUM_UNITS. For example for unit system
          named SI and default keyword attribute name will be "SIunits". This attribute should be specified
          for each deaclared unit system. Value of this attribute should be string describs units of measure.
          Possible designations for units of measure and their multiple prefixes see in package UnitsAPI of
          library OpenCascade (files Units.dat and Lexi_Expr.dat). Units of measure will be used for numeric
          value conversion from one unit system to another one.
      \lo Format (key DATUM_FORMAT) specify the format string which will be used during initial formatting
          of value. This string should be specified in sprintf() format.
      \lo Filter (key DATUM_FILTER) specify the regualr expression. Each user entered string will be checked up
          on matching to this expression (if it defined). Non matched strings will be rejected.
      \lo Required value (key DATUM_REQUIRED). If this attributed defined and value is true then user can't
          leave a input filed blank (parameter must be always entered).

      Tag "description" (key DESCR) can be defined under the tag "datum". This tag contains two sub tags:
         \lo "short description" (key SHORT_D) specify a brief datum description text
         \lo "long description" (key LONG_D) specify a detailed description text

      Tag "options" (key OPTIONS) can be defined under the tag "datum". This tag contains one or more sub
      tags "option" (key OPTION). Each of these XML elements should contain text option value and attribute
      "name" (key OPTION_NAME) which specified option name.

      Each tag "datum" define most common parameters of phisical characteristic.
      This parameters placed in two groups:
      \lo Domain parameters under tag "domain" (key DY_DOMAIN). This tag can contains value description tag
          (key VALUE_DESCR) for discrete data or list reference tag (key VALUE_LIST_REF) for enumerable data.
          Discrete data described following parameters:
            default value (key VD_DEFV)
            maximum value (key VD_MAXV)
            minimum value (key VD_MINV)
            type of value (key VD_TYPE) - possible values: String, Integer, Float, List
          Enumerable data described by "list reference" attribute (key VLR_LIST) which reference on
          list (see "list definition" tag) by list id.

  \li One or several tag "list definition" (key VALUE_LIST). This tag define the list of items for enumerable
      data. Attribute "list id" (key VALUE_LIST_ID) specify the identifier string for the list and attribute
      "list name" (key VALUE_LIST_NAME) define a list name string. Each list item described by tag "list value"
      (key VALUE_LIST_VALUE) under tag "list definition". Each this tag contains item string text and have
      following attributes:
        \lo "list item id" (key VALUE_LIST_VALUEID) - integer numerical identifier for item
        \lo "list item icon" (key VALUE_LIST_VALUEICON) - icon file name for item

  Below the example of a XML file with use default keywords.

  <datadictionary version="1.0">
    <component name="My Component">

      <!-- specify two unit systems -->

      <unitSystems>
        <system name="SI" label="System international">
        <system name="AS" label="Anglo - sacson system">
      </unitSystems>

      <!-- specify datum -->
      <!-- units of measure for SI - meters (m), for AS - inches (in) -->
      <datum SIunits="m" ASunits="in" format="%.25f" id="X" label="X coordinate" required="">
        <domain>
          <!-- default value not specified -->
          <valueDescr default="" max="999999999.999" min="0.000" type="Float"/>
        </domain>
        <description>
          <shortDescr>X coordinate for object</shortDescr>
          <longDescr>X multiplier of object coordinates. Describe position of object in direction of X axis</longDescr>
        </description>
      </datum>

      <datum SIunits="m" ASunits="in" format="%.25f" id="Y" label="Y coordinate" required="">
        <domain>
          <valueDescr default="" max="999999999.999" min="0.000" type="Float"/>
        </domain>
        <description>
          <shortDescr>Y coordinate for object</shortDescr>
          <longDescr>Y multiplier of object coordinates. Describe position of object in direction of Y axis</longDescr>
        </description>
      </datum>

      <!-- datum for object name with filter which not allow to input more that 8 letters,
           numbers or unerscores with first letter only -->
      <datum format="%.8us" id="ObjectName" label="Name" required="yes"
             filter="^([A-Z]+)([A-Z,0-9,_]*)$">
        <domain>
          <!-- limits and default not specified, type is string -->
          <valueDescr default="" max="" min="" type="String" />
        </domain>
        <description>
          <!-- long description not specified -->
          <shortDescr>Name of object</shortDescr>
        </description>
      </datum>


      <!-- datum for enumeration of side -->
      <datum format="" id="Side" label="Side" required="">
        <domain>
          <!-- default list item is item with id 0 -->
          <valueDescr default="0" type="List"/>
          <!-- list reference on list named "side_list" -->
          <listRef list="side_list"/>
        </domain>
        <description>
          <shortDescr>Side of object</shortDescr>
        </description>
      </datum>

      <!-- list definition for enumeration of side -->
      <valueList listid="side_list" name="Side">
        <value id="1">left</value>
        <value id="2">right</value>
        <value id="3">top</value>
        <value id="4">bottom</value>
        <value id="0">undefined</value>
      </valueList>

    </component>
  </datadictionary>


*/

/*!
  Constructor. Creates the instance of dictionary. Private method. Use DDS_Dictionary::Get() instead.
*/
DDS_Dictionary::DDS_Dictionary()
: MMgt_TShared()
{
}

/*!
  Copy constructor. Internal.
*/
DDS_Dictionary::DDS_Dictionary( const DDS_Dictionary& )
{
}

/*!
  Assigment operator. Internal.
*/
void DDS_Dictionary::operator=( const DDS_Dictionary& )
{
}

/*!
  Returns the names list of defined unit systems from all components.
  Parameter \atheSystems will contains the sequence of string names.
*/
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

/*!
  Returns the names list of defined unit systems from the specified component \atheComponent.
  Parameter \atheSystems will contains the sequence of string names. If component not found then
  empty list returned.
*/
void DDS_Dictionary::GetUnitSystems( TColStd_SequenceOfAsciiString& theSystems,
                                     const TCollection_AsciiString& theComponent ) const
{
  theSystems.Clear();
  if ( myGroupMap.Contains( theComponent ) )
    myGroupMap.FindFromKey( theComponent )->GetUnitSystems( theSystems );
}

/*!
  Returns the label of unit system \atheSystem. Function find the given unit system in
  all components. If unit system not found in any component then empty string returned.
*/
TCollection_ExtendedString DDS_Dictionary::GetUnitSystemLabel( const TCollection_AsciiString& theSystem ) const
{
  TCollection_ExtendedString aLabel;
  for ( Standard_Integer i = 1; i <= myGroupMap.Extent() && !aLabel.Length(); i++ )
    aLabel = myGroupMap.FindFromIndex( i )->GetUnitSystemLabel( theSystem );
  return aLabel;
}

/*!
  Returns the label of unit system \atheSystem from component \atheComponent. Function find
  the given unit system in the specified component only. If unit system not found in the
  component then empty string returned.
*/
TCollection_ExtendedString DDS_Dictionary::GetUnitSystemLabel( const TCollection_AsciiString& theSystem,
                                                               const TCollection_AsciiString& theComponent ) const
{
  TCollection_ExtendedString aLabel;
  if ( myGroupMap.Contains( theComponent ) )
    aLabel = myGroupMap.FindFromKey( theComponent )->GetUnitSystemLabel( theSystem );
  return aLabel;
}

/*!
  Gets the name of active unit system from first got component. If any component exist then
  active unit system name returned or empty string otherwise.
*/
TCollection_AsciiString DDS_Dictionary::GetActiveUnitSystem() const
{
  TCollection_AsciiString aSystem;
  if ( myGroupMap.Extent() )
    aSystem = myGroupMap.FindFromIndex( 1 )->GetActiveUnitSystem();
  return aSystem;
}

/*!
  Gets the name of active unit system from component \atheComponent. If this component exist
  active unit system name returned or empty string otherwise.
*/
TCollection_AsciiString DDS_Dictionary::GetActiveUnitSystem( const TCollection_AsciiString& theComponent ) const
{
  TCollection_AsciiString aSystem;
  if ( myGroupMap.Contains( theComponent ) )
    aSystem = myGroupMap.FindFromKey( theComponent )->GetActiveUnitSystem();
  return aSystem;
}

/*!
  Sets the active unit system named \atheSystem. This unit system will be activated in all
  existing components if component have it.
*/
void DDS_Dictionary::SetActiveUnitSystem( const TCollection_AsciiString& theSystem )
{
  for ( Standard_Integer i = 1; i <= myGroupMap.Extent(); i++ )
    myGroupMap.FindFromIndex( i )->SetActiveUnitSystem( theSystem );
}

/*!
  Sets the active unit system named \atheSystem for component \atheComponent. If specified unit
  system doesn't exist in the component then function do nothing.
*/
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

/*!
  Load datum definitions in the dictionary from XML file \atheFileName. Returns true if load
  successed or false otherwise.
*/
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

/*!
  Gets XML keyword as LDOMString by specified \akey. If key doesn't exist then empty string
  returned. This function provided for convenience.
*/
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
  Gets dictionary item with specified identifier \atheID from specified component \atheComponent.
  If component or item not found then null handle returned.
*/
Handle(DDS_DicItem) DDS_Dictionary::GetDicItem( const TCollection_AsciiString& theID,
                                                const TCollection_AsciiString& theComponent ) const
{
  Handle(DDS_DicItem) aDicItem;
  Handle(DDS_DicGroup) aDicGroup;
  if ( myGroupMap.Contains( theComponent ) )
    aDicGroup = myGroupMap.FindFromKey( theComponent );
  if ( !aDicGroup.IsNull() )
    aDicItem = aDicGroup->GetDicItem( theID );
  return aDicItem;
}

/*!
  Gets dictionary item with specified identifier \atheID. Function find the item in all components.
  If item not found in any component then null handle returned.
*/
Handle(DDS_DicItem) DDS_Dictionary::GetDicItem( const TCollection_AsciiString& theID ) const
{
  Handle(DDS_DicItem) aDicItem;
  for ( Standard_Integer i = 1; i <= myGroupMap.Extent() && aDicItem.IsNull(); i++ )
    aDicItem = myGroupMap.FindFromIndex( i )->GetDicItem( theID );
  return aDicItem;
}

/*!
  Fill the internal data structures from XML parsed structures. Internal.
*/
void DDS_Dictionary::FillDataMap( const LDOM_Element& theComponentData, const LDOM_Element& theDocElement )
{
  TCollection_AsciiString aCompName = theComponentData.getAttribute( KeyWord( "COMPONENT_NAME" ) );
  if ( !myGroupMap.Contains( aCompName ) )
    myGroupMap.Add( aCompName, new DDS_DicGroup( aCompName ) );
  Handle(DDS_DicGroup) aDicGroup = myGroupMap.FindFromKey( aCompName );
  aDicGroup->FillDataMap( theComponentData, theDocElement );
  myGroupMap.Add( aCompName, aDicGroup );
}

/*!
  Convert numeric value \atheValue from specified unit of measure \atheUnits to SI unit of measure
  (mm for Length, radians for Angles, etc). Converted value returned.
*/
Standard_Real DDS_Dictionary::ToSI( const Standard_Real theValue, const Standard_CString theUnits )
{
  Standard_Real aRetValue = theValue;
  if ( theUnits && *theUnits && strcmp( theUnits, "%" ) )
  {
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
      OCC_CATCH_SIGNALS;
#endif
      aRetValue = UnitsAPI::AnyToSI( theValue, theUnits );
    }
    catch( Standard_Failure ) {
    }
  }
  else if ( theUnits && *theUnits )
    aRetValue = theValue / 100.0;

  return aRetValue;
}

/*!
  Convert numeric value \atheValue to specified unit of measure \atheUnits from SI unit of measure
  (mm for Length, radians for Angles, etc). Converted value returned.
*/
Standard_Real DDS_Dictionary::FromSI( const Standard_Real theValue, const Standard_CString theUnits )
{
  Standard_Real aRetValue = theValue;
  if ( theUnits && *theUnits && strcmp( theUnits, "%" ) )
  {
    try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
      OCC_CATCH_SIGNALS;
#endif
      aRetValue = UnitsAPI::AnyFromSI( theValue, theUnits );
    }
    catch( Standard_Failure ) {
    }
  }
  else if ( theUnits && *theUnits )
    aRetValue = theValue * 100.0;

  return aRetValue;
}
