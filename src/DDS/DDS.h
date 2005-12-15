#ifndef DDS_H
#define DDS_H

#include <TCollection_AsciiString.hxx>

#include <NCollection_DefineDataMap.hxx>
#include <NCollection_DefineBaseCollection.hxx>

#define UNIT_SYSTEM_SI "SI"

typedef enum { DDS_MT_OK, DDS_MT_WARNING,
               DDS_MT_ERROR, DDS_MT_ALARM,
               DDS_MT_INFO, DDS_MT_NONE } DDS_MsgType;

DEFINE_BASECOLLECTION(DDS_BaseColOfAsciiString,TCollection_AsciiString)

#endif
