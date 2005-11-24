#ifndef QTX_LOGOMGR_H
#define QTX_LOGOMGR_H

#include "Qtx.h"

class QMenuBar;

#include <qobject.h>
#include <qpixmap.h>

#ifdef WIN32
#pragma warning( disable : 4251 )
#endif

class QTX_EXPORT QtxLogoMgr : public QObject
{
  Q_OBJECT

public:
  QtxLogoMgr( QMenuBar* );
  virtual ~QtxLogoMgr();

  int        count() const;

  void       insert( const QString&, const QPixmap&, const int = -1 );
  void       remove( const QString& );
  void       clear();

  QMenuBar*  menuBar() const;

private:
  void       generate();
  int        find( const QString& ) const;

private:
  typedef struct { QString id; QPixmap pix; } LogoInfo;
  typedef QValueList<LogoInfo>                LogoList;

private:
  int        myId;
  QMenuBar*  myMenus;
  LogoList   myLogos;
};

#ifdef WIN32
#pragma warning( default : 4251 )
#endif

#endif
