#include "SUIT_ExceptionHandler.h"

#include "SUIT_MessageBox.h"

#include <qapplication.h>

/*!\class SUIT_ExceptionHandler
 * Show exception message on error handler.
 */

/*!
  Checks: is internal handle on object \a o?
*/
bool SUIT_ExceptionHandler::handle( QObject* o, QEvent* e )
{
  return internalHandle( o, e );
}

/*!
  Checks: is internal handle on object \a o?
*/
bool SUIT_ExceptionHandler::internalHandle( QObject* o, QEvent* e )
{
  return qApp ? qApp->QApplication::notify( o, e ) : false;
}

/*!
  Show error message \a mgs, if application is not null.
*/
void SUIT_ExceptionHandler::showMessage( const QString& title, const QString& msg )
{
  if ( !qApp )
    return;

  SUIT_MessageBox::error1( qApp->mainWidget(), title, msg, "OK" );
}
