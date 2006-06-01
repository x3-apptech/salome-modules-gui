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
