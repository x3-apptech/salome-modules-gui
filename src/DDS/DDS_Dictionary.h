#ifndef DDS_DICTIONARY_H
#define DDS_DICTIONARY_H

#include "DDS_DicGroup.h"

#include <LDOMString.hxx>

#include <MMgt_TShared.hxx>

class LDOM_Element;
class TCollection_AsciiString;

DEFINE_STANDARD_HANDLE(DDS_Dictionary, MMgt_TShared)

//  Class to provide information about used parameters,
//  reading them from 'xml' file
class DDS_Dictionary : public MMgt_TShared
{
public:
  Standard_EXPORT static Handle(DDS_Dictionary)  Get();

  // Return instance of data dictionary. Create instance if it is NULL.

  Standard_EXPORT Handle(DDS_DicItem)            GetDicItem( const TCollection_AsciiString& ) const;
  Standard_EXPORT Handle(DDS_DicItem)            GetDicItem( const TCollection_AsciiString&,
                                                             const TCollection_AsciiString& ) const;

  Standard_EXPORT void                           GetUnitSystems( TColStd_SequenceOfAsciiString& ) const;
  Standard_EXPORT void                           GetUnitSystems( TColStd_SequenceOfAsciiString&,
                                                                 const TCollection_AsciiString& ) const;
  Standard_EXPORT TCollection_ExtendedString     GetUnitSystemLabel( const TCollection_AsciiString& ) const;
  Standard_EXPORT TCollection_ExtendedString     GetUnitSystemLabel( const TCollection_AsciiString&,
                                                                     const TCollection_AsciiString& ) const;
  Standard_EXPORT TCollection_AsciiString        GetActiveUnitSystem() const;
  Standard_EXPORT TCollection_AsciiString        GetActiveUnitSystem( const TCollection_AsciiString& ) const;
  Standard_EXPORT void                           SetActiveUnitSystem( const TCollection_AsciiString& );
  Standard_EXPORT void                           SetActiveUnitSystem( const TCollection_AsciiString&,
                                                                      const TCollection_AsciiString& );


  static Standard_EXPORT Standard_Boolean        Load( const TCollection_AsciiString );

  static Standard_EXPORT Standard_Real           ToSI( const Standard_Real, const Standard_CString );
  static Standard_EXPORT Standard_Real           FromSI( const Standard_Real, const Standard_CString );

  static Standard_EXPORT LDOMString              KeyWord( const TCollection_AsciiString& );

private:
  DDS_Dictionary();
  DDS_Dictionary( const DDS_Dictionary& );

  void                                           operator=( const DDS_Dictionary& );

  // prepares formants for each unit systems
  void                                           FillDataMap( const LDOM_Element&, const LDOM_Element& );

private:
  DDS_IndexedDataMapOfDicGroups                  myGroupMap;

public:
  DEFINE_STANDARD_RTTI(DDS_Dictionary)
};

#endif
