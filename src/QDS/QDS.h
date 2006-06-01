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
#ifndef QDS_H
#define QDS_H

#ifdef WIN32
#ifdef QDS_EXPORTS
#define QDS_EXPORT __declspec(dllexport)
#else
#define QDS_EXPORT __declspec(dllimport)
#endif
#else
#define QDS_EXPORT
#endif

#if defined WIN32
#pragma warning ( disable:4251 )
#pragma warning ( disable:4786 )
#endif

#include <qstring.h>
#include <qvaluelist.h>

#include <TCollection_AsciiString.hxx>

#include <TCollection_ExtendedString.hxx>

class QDS_Datum;
class Handle(TCollection_HAsciiString);
class Handle(TCollection_HExtendedString);

class QDS_EXPORT QDS
{
public:
  typedef enum { None = 0x00, Label = 0x01, Control = 0x02, Units = 0x04,
                 NotFormat = 0x08, NotAccel = 0x10, UnitsWithLabel = 0x20,
                 All = Label | Control | Units } DatumFlags;

public:
  static bool                       load( const QString& );

  static QString                    unitSystemLabel( const QString&,
                                                     const QString& = QString::null );
  static QString                    activeUnitSystem( const QString& = QString::null );
  static void                       setActiveUnitSystem( const QString&,
                                                         const QString& = QString::null );

  static QString                    toQString( const TCollection_AsciiString& );
  static QString                    toQString( const TCollection_ExtendedString& );
  static QString                    toQString( const Handle(TCollection_HAsciiString)& );
  static QString                    toQString( const Handle(TCollection_HExtendedString)& );

  static TCollection_AsciiString    toAsciiString( const QString& );
  static TCollection_AsciiString    toAsciiString( const TCollection_ExtendedString& );
  static TCollection_AsciiString    toAsciiString( const Handle(TCollection_HExtendedString)& );

  static TCollection_ExtendedString toExtString( const QString& );
  static TCollection_ExtendedString toExtString( const TCollection_AsciiString& );

protected:
  static void                       insertDatum( QDS_Datum* );
  static void                       removeDatum( QDS_Datum* );

private:
  static QValueList<QDS_Datum*>     _datumList;
};

#endif
