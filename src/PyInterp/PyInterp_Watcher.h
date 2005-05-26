#ifndef _PYINTERP_WATCHER_H_
#define _PYINTERP_WATCHER_H_

#include <PyInterp.h>

#include <PyInterp_Dispatcher.h>

#include <qobject.h>

// Private class that keeps track of destructions of request listeners
class PYINTERP_EXPORT PyInterp_Watcher : public QObject
{					    
  Q_OBJECT

public:
  PyInterp_Watcher() : QObject( 0 ) {}
  virtual ~PyInterp_Watcher() {}

public slots:
  void onDestroyed( QObject* o ) { PyInterp_Dispatcher::Get()->objectDestroyed( o ); }
};

#endif // _PYINTERP_WATCHER_H_
