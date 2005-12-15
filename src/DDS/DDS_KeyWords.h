#ifndef DDS_KEYWORDS_H
#define DDS_KEYWORDS_H

#include "DDS.h"

#include <MMgt_TShared.hxx>

#include <NCollection_DataMap.hxx>

DEFINE_STANDARD_HANDLE(DDS_KeyWords, MMgt_TShared)

class TCollection_AsciiString;

class DDS_KeyWords: public MMgt_TShared
{
public:
  Standard_EXPORT static Handle(DDS_KeyWords) Get();

  Standard_EXPORT TCollection_AsciiString     GetKeyWord( const TCollection_AsciiString& ) const;
  Standard_EXPORT void                        SetKeyWord( const TCollection_AsciiString&,
                                                          const TCollection_AsciiString& );

private:
  DDS_KeyWords();

private:
  typedef NCollection_DataMap<TCollection_AsciiString,
                              TCollection_AsciiString> KeyWordMap;

private:
  KeyWordMap                                  myKeyWord;

public:
  DEFINE_STANDARD_RTTI(DDS_KeyWords)
};

#endif
