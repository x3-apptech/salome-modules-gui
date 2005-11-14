#ifndef SUIT_TOOLS_H
#define SUIT_TOOLS_H

#include "SUIT.h"

#include <Qtx.h>

#include <qrect.h>
#include <qfont.h>
#include <qstring.h>

class SUIT_EXPORT SUIT_Tools : public Qtx
{
public:
  static void    trace( const char* pLog, const char* szFormat, ... );//!< Traces output to log-file.
  static QRect   makeRect( const int x1, const int y1, const int x2, const int y2 ); 

  static QString fontToString( const QFont& font );
  static QFont   stringToFont( const QString& fontDescription );

  static void    centerWidget( QWidget* src, const QWidget* ref );
};

#endif
