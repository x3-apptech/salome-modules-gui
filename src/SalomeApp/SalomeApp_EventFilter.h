#ifndef SALOMEAPP_EVENTFILTER_H
#define SALOMEAPP_EVENTFILTER_H

#include "SalomeApp.h"
#include <qobject.h>

#if defined WNT
#pragma warning( disable: 4251 )
#endif

class SALOME_Event;

class SALOMEAPP_EXPORT SalomeApp_EventFilter: public QObject 
{
public:
  static void Init();
  static void Destroy();

protected:
  SalomeApp_EventFilter();
  virtual ~SalomeApp_EventFilter();

private:
  /* global event filter for qapplication */
  virtual bool eventFilter( QObject* o, QEvent* e );
  void processEvent( SALOME_Event* );

private:
  static SalomeApp_EventFilter* myFilter;
};

#if defined WNT
#pragma warning( default: 4251 )
#endif

#endif
