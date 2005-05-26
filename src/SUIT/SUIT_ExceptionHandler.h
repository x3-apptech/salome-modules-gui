#ifndef SUIT_EXCEPTIONHANDLER_H
#define SUIT_EXCEPTIONHANDLER_H

#include "SUIT.h"

class QEvent;
class QObject;
class QString;

class SUIT_EXPORT SUIT_ExceptionHandler
{
public:
  virtual bool handle( QObject*, QEvent* );

protected:
  bool         internalHandle( QObject*, QEvent* );
  void         showMessage( const QString&, const QString& );
};

extern "C"
{
  typedef SUIT_ExceptionHandler* (*APP_GET_HANDLER_FUNC)();
}

#define APP_GET_HANDLER_NAME "getExceptionHandler"

#endif
