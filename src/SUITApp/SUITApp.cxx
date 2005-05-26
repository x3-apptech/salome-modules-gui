#if defined WNT
#undef SUIT_ENABLE_PYTHON
//#else
//#include "SUITconfig.h"
#endif

#include <qapplication.h>

#include "SUIT_Session.h"
#include "SUITApp_Application.h"

#ifdef SUIT_ENABLE_PYTHON
#include <Python.h>
#endif

#include <qstringlist.h>

int main( int args, char* argv[] )
{
#ifdef SUIT_ENABLE_PYTHON
  Py_Initialize();
  PySys_SetArgv( args, argv );
#endif
  
  QStringList argList;
  bool noExceptHandling = false;
  for ( int i = 1; i < args && !noExceptHandling; i++ )
  {
    if ( !strcmp( argv[i], "/noexcepthandling" ) )
      noExceptHandling = true;
    else
      argList.append( QString( argv[i] ) );
  }

  SUITApp_Application app( args, argv );

  int result = -1;
  if ( !argList.isEmpty() )
  {
    SUIT_Session* aSession = new SUIT_Session();
	  if ( aSession->startApplication( argList.first() ) )
    {
      if ( !noExceptHandling )
        app.setHandler( aSession->handler() );

      result = app.exec();
    }
    delete aSession;
  }

  return result;
}
