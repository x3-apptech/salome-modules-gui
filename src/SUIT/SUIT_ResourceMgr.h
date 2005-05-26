#ifndef SUIT_RESOURCEMGR_H
#define SUIT_RESOURCEMGR_H

#include "SUIT.h"

#include <QtxResourceMgr.h>

class SUIT_EXPORT SUIT_ResourceMgr : public QtxResourceMgr
{
public:
  SUIT_ResourceMgr( const QString&, const QString& = QString::null );
  virtual ~SUIT_ResourceMgr();

  QString loadDoc( const QString&, const QString& ) const;
};

#endif
