#include "SUIT_ExceptionHandler.h"

#include "SUIT_MessageBox.h"

#include <qapplication.h>

bool SUIT_ExceptionHandler::handle( QObject* o, QEvent* e )
{
  return internalHandle( o, e );
}

bool SUIT_ExceptionHandler::internalHandle( QObject* o, QEvent* e )
{
  return qApp ? qApp->QApplication::notify( o, e ) : false;
}

void SUIT_ExceptionHandler::showMessage( const QString& title, const QString& msg )
{
  if ( !qApp )
    return;

  SUIT_MessageBox::error1( qApp->mainWidget(), title, msg, "OK" );
}
