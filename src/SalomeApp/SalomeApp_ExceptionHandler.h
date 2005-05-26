#ifndef SALOMEAPP_EXCEPTIONHANDLER_H
#define SALOMEAPP_EXCEPTIONHANDLER_H

#include "SalomeApp.h"

#include <SUIT_ExceptionHandler.h>

class SALOMEAPP_EXPORT SalomeApp_ExceptionHandler : public SUIT_ExceptionHandler
{
public:
  SalomeApp_ExceptionHandler( const bool );
  virtual bool handle( QObject*, QEvent* );

protected:
  virtual bool handleSignals( QObject*, QEvent* );
};

#endif
