#include "SUITApp_Application.h"

#include "SUIT_MessageBox.h"
#include "SUIT_ExceptionHandler.h"

#include <qdir.h>
#include <qfileinfo.h>

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <unistd.h>
#endif

SUITApp_Application::SUITApp_Application( int& argc, char** argv, SUIT_ExceptionHandler* hand )
: QApplication( argc, argv ),
myExceptHandler( hand )
{
  QString path = QFileInfo( argv[0] ).dirPath() + QDir::separator() + "../../resources";
  path = QDir::convertSeparators( QDir( path ).canonicalPath() );

  QTranslator* strTbl = new QTranslator( 0 );
  if ( strTbl->load( "SUITApp_msg_en.po", path  ) )
    installTranslator( strTbl );
  else
    delete strTbl;
}

SUITApp_Application::SUITApp_Application( int& argc, char** argv, Type type, SUIT_ExceptionHandler* hand )
: QApplication( argc, argv, type ),
myExceptHandler( hand )
{
    QTranslator* strTbl = new QTranslator( 0 );
    strTbl->load( "resources\\SUITApp_msg_en.po" );
    installTranslator( strTbl );
}

bool SUITApp_Application::notify( QObject* receiver, QEvent* e )
{
  return myExceptHandler ? myExceptHandler->handle( receiver, e ) :
                           QApplication::notify( receiver, e );
}

void SUITApp_Application::setHandler( SUIT_ExceptionHandler* hand )
{
	myExceptHandler = hand;
}

SUIT_ExceptionHandler* SUITApp_Application::handler() const
{
  return myExceptHandler;
}
