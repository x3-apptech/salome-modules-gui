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
#ifndef DDS_DICITEM_H
#define DDS_DICITEM_H

#include "DDS.h"

#include <MMgt_TShared.hxx>

#include <TCollection_AsciiString.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_DefineIndexedDataMap.hxx>

class LDOM_Element;
class DDS_Dictionary;
class TColStd_MapOfReal;
class TColStd_SequenceOfInteger;
class TColStd_SequenceOfAsciiString;
class TColStd_SequenceOfExtendedString;

//  Class, containing all information about one parameter:
//  unique    : id
//  obligative: label, type, short description, required
//  optional  : format, units,
//              min value, max value, default value.

class DDS_DicItem : public MMgt_TShared
{
public:
  /*! Enum describes type of datum value */
  enum Type
  {
    String,  //!< String type of value
    Float,   //!< Real (double) numeric type of value
    Integer, //!< Integer (int) numeric type of value
    List,    //!< List type of enumerable value
    Unknown  //!< Unknown or undefined type of value
  };

  /*! Enum describes the flags for existance of domain data */
  enum Data
  {
    MinValue = 0x01,    //!< Flag of minimum value definition existence
    MaxValue = 0x02,    //!< Flag of maximum value definition existence
    DefaultValue = 0x04 //!< Flag of default value definition existence
  };

  // This struct is intended for map of Format, Units, Precision and Scale
  struct UnitData
  {
    Standard_Real           myZero;
    Standard_Real           myScale;
    TCollection_AsciiString myUnits;
    TCollection_AsciiString myFormat;
    Standard_Integer        myPrecision;
  };

  typedef TCollection_AsciiString UnitSystem;

public:
  DDS_DicItem();

  Standard_EXPORT TCollection_AsciiString    GetId() const;
  Standard_EXPORT DDS_DicItem::Type          GetType() const;
  Standard_EXPORT TCollection_ExtendedString GetLabel() const;
  Standard_EXPORT TCollection_ExtendedString GetFilter() const;
  Standard_EXPORT TCollection_ExtendedString GetRequired() const;
  Standard_EXPORT DDS_MsgType                GetWarningLevel() const;
  Standard_EXPORT TCollection_ExtendedString GetLongDescription() const;
  Standard_EXPORT TCollection_ExtendedString GetShortDescription() const;
  Standard_EXPORT TCollection_AsciiString    GetComponent() const;

  Standard_EXPORT TCollection_AsciiString    GetUnits() const;
  Standard_EXPORT TCollection_AsciiString    GetUnits( const UnitSystem& ) const;

  Standard_EXPORT TCollection_ExtendedString GetDefaultValue() const;
  Standard_EXPORT TCollection_ExtendedString GetDefaultValue( const UnitSystem& ) const;

  Standard_EXPORT Standard_Real              GetMinValue() const;
  Standard_EXPORT Standard_Real              GetMinValue( const UnitSystem& ) const;

  Standard_EXPORT Standard_Real              GetMaxValue() const;
  Standard_EXPORT Standard_Real              GetMaxValue( const UnitSystem& ) const;

  Standard_EXPORT Standard_Integer           GetPrecision() const;
  Standard_EXPORT Standard_Integer           GetPrecision( const UnitSystem& ) const;

  Standard_EXPORT TCollection_AsciiString    GetFormat( const Standard_Boolean = Standard_True ) const;
  Standard_EXPORT TCollection_AsciiString    GetFormat( const UnitSystem&,
                                                        const Standard_Boolean = Standard_True ) const;
  Standard_EXPORT TCollection_ExtendedString GetNameOfValues() const;
  Standard_EXPORT Standard_Boolean           GetListOfValues( Handle(TColStd_HArray1OfExtendedString)&,
                                                              Handle(TColStd_HArray1OfInteger)& ) const;
  Standard_EXPORT Standard_Boolean           GetListOfValues( Handle(TColStd_HArray1OfExtendedString)&,
                                                              Handle(TColStd_HArray1OfInteger)&,
                                                              Handle(TColStd_HArray1OfExtendedString)& ) const;
  Standard_EXPORT Standard_Boolean           GetSpecialValues( TColStd_MapOfReal& ) const;

  Standard_EXPORT Standard_Real              GetMinZoom() const;
  Standard_EXPORT Standard_Real              GetMaxZoom() const;
  Standard_EXPORT Standard_Real              GetZoomOrder() const;

  Standard_EXPORT Standard_Real              ToSI( const Standard_Real ) const;
  Standard_EXPORT Standard_Real              FromSI( const Standard_Real ) const;

  Standard_EXPORT Standard_Real              ToSI( const Standard_Real, const UnitSystem& ) const;
  Standard_EXPORT Standard_Real              FromSI( const Standard_Real, const UnitSystem& ) const;

  Standard_EXPORT Standard_Boolean           HasData( const Standard_Integer ) const;

  Standard_EXPORT TCollection_ExtendedString GetOption( const TCollection_AsciiString& ) const;
  Standard_EXPORT Standard_Boolean           GetOptionNames( TColStd_SequenceOfAsciiString& ) const;

private:
  DDS_DicItem( const DDS_DicItem& );
  void                                       operator=( const DDS_DicItem& );
  void                                       FillDataMap( TCollection_AsciiString, const LDOM_Element&,
                                                          const LDOM_Element&, const LDOM_Element&,
                                                          const TColStd_SequenceOfAsciiString& );
  void                                       PrepareFormats( const TCollection_AsciiString& );
  void                                       GetDefaultFormat();
  UnitSystem                                 GetActiveUnitSystem() const;

  void                                       GetStringFormat( const TCollection_AsciiString&,
                                                              const TCollection_AsciiString&,
                                                              const TCollection_AsciiString&,
                                                              const TCollection_AsciiString&,
                                                              TCollection_AsciiString& );
  void                                       GetIntegerFormat( const TCollection_AsciiString&,
                                                               const TCollection_AsciiString&,
                                                               const TCollection_AsciiString&,
                                                               const TCollection_AsciiString&,
                                                               const Standard_Character,
                                                               TCollection_AsciiString& );
  void                                       GetFloatFormat( const TCollection_AsciiString&,
                                                             const TCollection_AsciiString&,
                                                             const TCollection_AsciiString&,
                                                             const TCollection_AsciiString&,
                                                             const Standard_Character,
                                                             TCollection_AsciiString& );
  void                                       Split( const TCollection_AsciiString&,
                                                    Handle(TColStd_HArray1OfExtendedString)& );

  UnitData*                                  GetUnitData( const UnitSystem& ) const;

private:
  typedef NCollection_DataMap<TCollection_AsciiString, TCollection_ExtendedString> OptionsMap;

private:
  TCollection_AsciiString                    myId;
  TCollection_ExtendedString                 myLabel;
  TCollection_ExtendedString                 myFilter;
  TCollection_ExtendedString                 myRequired;

  Standard_Integer                           myType;
  Standard_Integer                           myWarnLevel;

  TCollection_ExtendedString                 myLongDescr;
  TCollection_ExtendedString                 myShortDescr;

  Standard_Real                              myMax;
  Standard_Real                              myMin;
  Standard_Real                              myDefValue;
  TCollection_ExtendedString                 myDefString;

  Standard_Integer                           myData;

  // valueList
  TCollection_ExtendedString                 myListName;

  Handle(TColStd_HArray1OfExtendedString)    myListRef;
  Handle(TColStd_HArray1OfInteger)           myListRefID;
  Handle(TColStd_HArray1OfExtendedString)    myListRefIcons;

  // presentation
  Standard_Real                              myMinZoom;
  Standard_Real                              myMaxZoom;
  Standard_Real                              myZoomOrder;

  Handle(Standard_Transient)                 myComponent;

  OptionsMap                                 myOptions;
  // unitData
  NCollection_DataMap<UnitSystem, UnitData>  myUnitData;

  friend class DDS_DicGroup;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTI(DDS_DicItem)
};

DEFINE_STANDARD_HANDLE(DDS_DicItem, MMgt_TShared)

DEFINE_BASECOLLECTION(DDS_BaseCollectionOfDicItems, Handle(DDS_DicItem))
DEFINE_INDEXEDDATAMAP(DDS_IndexedDataMapOfDicItems, DDS_BaseCollectionOfDicItems,
                      TCollection_AsciiString, Handle(DDS_DicItem))

#endif
