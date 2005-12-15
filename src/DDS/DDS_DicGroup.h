#ifndef DDS_DICGROUP_H
#define DDS_DICGROUP_H

#include "DDS.h"

#include "DDS_DicItem.h"

#include <MMgt_TShared.hxx>

#include <TCollection_AsciiString.hxx>

#include <NCollection_List.hxx>

class LDOM_Element;
class TColStd_SequenceOfAsciiString;

DEFINE_STANDARD_HANDLE(DDS_DicGroup, MMgt_TShared)

class DDS_DicGroup : public MMgt_TShared
{
public:
  DDS_DicGroup( const TCollection_AsciiString& );

  TCollection_AsciiString                    GetName() const;

  Standard_EXPORT Handle(DDS_DicItem)        GetDicItem( const TCollection_AsciiString& ) const;

  Standard_EXPORT void                       GetUnitSystems( TColStd_SequenceOfAsciiString& ) const;
  Standard_EXPORT TCollection_ExtendedString GetUnitSystemLabel( const TCollection_AsciiString& ) const;

  Standard_EXPORT TCollection_AsciiString    GetActiveUnitSystem() const;
  Standard_EXPORT void                       SetActiveUnitSystem( const TCollection_AsciiString& );

private:
  DDS_DicGroup( const DDS_DicGroup& );

  void                                       operator=( const DDS_DicGroup& );

  void                                       FillDataMap( const LDOM_Element&, const LDOM_Element& );

private:
  typedef NCollection_DataMap<TCollection_AsciiString,
                              TCollection_ExtendedString> UnitSystemMap;

private:
  TCollection_AsciiString                    myName;
  DDS_IndexedDataMapOfDicItems               myDataMap;
  UnitSystemMap                              myUnitSystem;
  TCollection_AsciiString                    myActiveSystem;

  friend class DDS_Dictionary;

public:
  DEFINE_STANDARD_RTTI(DDS_DicGroup)
};

DEFINE_BASECOLLECTION(DDS_BaseCollectionOfDicGroups, Handle(DDS_DicGroup))
DEFINE_INDEXEDDATAMAP(DDS_IndexedDataMapOfDicGroups, DDS_BaseCollectionOfDicGroups,
                      TCollection_AsciiString, Handle(DDS_DicGroup))

#endif
