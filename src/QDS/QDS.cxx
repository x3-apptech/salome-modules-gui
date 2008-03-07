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
#include "QDS.h"

#include "QDS_Datum.h"

#include <qtextcodec.h>

#include <DDS_DicItem.h>
#include <DDS_Dictionary.h>

#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

QValueList<QDS_Datum*> QDS::_datumList;

/*!
  Convert the OpenCascade ascii string to Qt string.
*/
QString QDS::toQString( const TCollection_AsciiString& src )
{
  QTextCodec* codec = QTextCodec::codecForLocale();
  QString res;
  if ( !src.IsEmpty() )
    res = codec ? codec->toUnicode( (char*)src.ToCString(), src.Length() ) :
                  QString( (char*)src.ToCString() );
  return res;
}

/*!
  Convert the OpenCascade unicode string to Qt string.
*/
QString QDS::toQString( const TCollection_ExtendedString& src )
{
  if ( src.IsAscii() )
    return toQString( TCollection_AsciiString( src ) );
  else
    return QString( (QChar*)src.ToExtString(), src.Length() );
}

/*!
  Convert the OpenCascade ascii string to Qt string.
*/
QString QDS::toQString( const Handle(TCollection_HAsciiString)& src )
{
  if ( src.IsNull() )
    return QString::null;
  else
    return toQString( src->String() );
}

/*!
  Convert the OpenCascade unicode string to Qt string.
*/
QString QDS::toQString( const Handle(TCollection_HExtendedString)& src )
{
  if ( src.IsNull() )
    return QString::null;
  else
    return toQString( src->String() );
}

/*!
  Convert the Qt string to OpenCascade ascii string.
*/
TCollection_AsciiString QDS::toAsciiString( const QString& src )
{
  TCollection_AsciiString res;
  if ( src.latin1() )
  {
    QTextCodec* codec = QTextCodec::codecForLocale();
    if ( codec )
    {
      int len = -1;
      QCString str = codec->fromUnicode( src, len );
      res = TCollection_AsciiString( (Standard_CString)(const char*)str, len );
    }
    else
      res = TCollection_AsciiString( (char*)src.latin1() );
  }
  return res;
}

/*!
  Convert the OpenCascade unicode string to OpenCascade ascii string.
*/
TCollection_AsciiString QDS::toAsciiString( const TCollection_ExtendedString& src )
{
  return TCollection_AsciiString( src );
}

/*!
  Convert the OpenCascade unicode string to OpenCascade ascii string.
*/
TCollection_AsciiString QDS::toAsciiString( const Handle(TCollection_HExtendedString)& src )
{
  TCollection_AsciiString res;
  if ( !src.IsNull() )
    res = toAsciiString( src->String() );
  return res;
}

/*!
  Convert the Qt string to OpenCascade unicode string.
*/
TCollection_ExtendedString QDS::toExtString( const QString& src )
{
  if ( src.isEmpty() )
    return TCollection_ExtendedString();

  Standard_Integer len = src.length();
  Standard_ExtString extStr = new Standard_ExtCharacter[( len + 1 ) * 2];
  memcpy( extStr, src.unicode(), len * 2 );
  extStr[len] = 0;

  TCollection_ExtendedString trg( extStr );

  delete [] extStr;

  return trg;
}

/*!
  Convert the OpenCascade ascii string to OpenCascade unicode string.
*/
TCollection_ExtendedString QDS::toExtString( const TCollection_AsciiString& src )
{
  return TCollection_ExtendedString( src );
}

/*!
  Load datum definitions in the dictionary from XML file \adictPath.
  Returns true if load successed or false otherwise.
*/
bool QDS::load( const QString& dictPath )
{
  if ( dictPath.isEmpty() )
    return false;

  return DDS_Dictionary::Load( toAsciiString( dictPath ) );
}

/*!
  Returns the label of unit system \asys. If component \acomp specified and not empty then
  function find the given unit system in the given component otherwise all components will be searched.
  If unit system not found then empty string returned.
*/
QString QDS::unitSystemLabel( const QString& sys, const QString& comp )
{
  QString lab;
  TCollection_AsciiString system = toAsciiString( sys );
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( !dic.IsNull() )
    lab = toQString( comp.isEmpty() ? dic->GetUnitSystemLabel( system ) :
                                      dic->GetUnitSystemLabel( system, toAsciiString( comp ) ) );
  return lab;
}

/*!
  Gets the name of active unit system from the specified component \acomp.
  If component not specified or component is empty string then first got component will be used.
  If component exist then active unit system name returned or empty string otherwise.
*/
QString QDS::activeUnitSystem( const QString& comp )
{
  QString sys;
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( !dic.IsNull() )
    sys = toQString( comp.isEmpty() ? dic->GetActiveUnitSystem() :
                                      dic->GetActiveUnitSystem( toAsciiString( comp ) ) );
  return sys;
}

/*!
  Sets the active unit system named \asys. If not empty component name \acomp specified then
  unit system will be activated in the given component otherwise all components will be processed.

  After the changing of active unit system function notify about it to all registered datums
  from processed components using method QDS_Datum::unitSystemChanged();
*/
void QDS::setActiveUnitSystem( const QString& sys, const QString& comp )
{
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( dic.IsNull() )
    return;

  TCollection_AsciiString system = toAsciiString( sys );
  comp.isEmpty() ? dic->SetActiveUnitSystem( system ) :
                   dic->SetActiveUnitSystem( system, toAsciiString( comp ) );

  QString unitSys = activeUnitSystem( comp );
  if ( sys == unitSys )
    return;

  TCollection_AsciiString aComp = toAsciiString( comp );
  for ( QValueList<QDS_Datum*>::iterator it = _datumList.begin(); it != _datumList.end(); ++it )
  {
    QDS_Datum* datum = *it;
    if ( !datum )
      continue;

    bool ok = aComp.IsEmpty();
    if ( !ok )
    {
      Handle(DDS_DicItem) item = datum->dicItem();
      ok = !item.IsNull() && aComp == item->GetComponent();
    }

    if ( ok )
      datum->unitSystemChanged( unitSys );
  }
}

/*!
  Register given datum \adatum in the static list.
  This function invoked by QDS_Datum constructor.
*/
void QDS::insertDatum( QDS_Datum* datum )
{
  if ( !datum )
    return;

  _datumList.append( datum );
}

/*!
  Remove given datum \adatum from the static list.
  This function invoked by QDS_Datum destructor.
*/
void QDS::removeDatum( QDS_Datum* datum )
{
  if ( !datum )
    return;

  _datumList.remove( datum );
}
