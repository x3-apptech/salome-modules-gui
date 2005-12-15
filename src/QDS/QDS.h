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
