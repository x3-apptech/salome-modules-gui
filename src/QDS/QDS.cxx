#include "QDS.h"

#include "QDS_Datum.h"

#include <qtextcodec.h>

#include <DDS_DicItem.h>
#include <DDS_Dictionary.h>

#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

QValueList<QDS_Datum*> QDS::_datumList;

QString QDS::toQString( const TCollection_AsciiString& src )
{
  QTextCodec* codec = QTextCodec::codecForLocale();
  QString res;
  if ( !src.IsEmpty() )
    res = codec ? codec->toUnicode( (char*)src.ToCString(), src.Length() ) :
                  QString( (char*)src.ToCString() );
  return res;
}

QString QDS::toQString( const TCollection_ExtendedString& src )
{
  if ( src.IsAscii() )
    return toQString( TCollection_AsciiString( src ) );
  else
    return QString( (QChar*)src.ToExtString(), src.Length() );
}

QString QDS::toQString( const Handle(TCollection_HAsciiString)& src )
{
  if ( src.IsNull() )
    return QString::null;
  else
    return toQString( src->String() );
}

QString QDS::toQString( const Handle(TCollection_HExtendedString)& src )
{
  if ( src.IsNull() )
    return QString::null;
  else
    return toQString( src->String() );
}

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

TCollection_AsciiString QDS::toAsciiString( const TCollection_ExtendedString& src )
{
  return TCollection_AsciiString( src );
}

TCollection_AsciiString QDS::toAsciiString( const Handle(TCollection_HExtendedString)& src )
{
  TCollection_AsciiString res;
  if ( !src.IsNull() )
    res = toAsciiString( src->String() );
  return res;
}

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

TCollection_ExtendedString QDS::toExtString( const TCollection_AsciiString& src )
{
  return TCollection_ExtendedString( src );
}

bool QDS::load( const QString& dictPath )
{
  if ( dictPath.isEmpty() )
    return false;

  return DDS_Dictionary::Load( toAsciiString( dictPath ) );
}

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

QString QDS::activeUnitSystem( const QString& comp )
{
  QString sys;
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( !dic.IsNull() )
    sys = toQString( comp.isEmpty() ? dic->GetActiveUnitSystem() :
                                      dic->GetActiveUnitSystem( toAsciiString( comp ) ) );
  return sys;
}

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

void QDS::insertDatum( QDS_Datum* datum )
{
  if ( !datum )
    return;

  _datumList.append( datum );
}

void QDS::removeDatum( QDS_Datum* datum )
{
  if ( !datum )
    return;

  _datumList.remove( datum );
}
