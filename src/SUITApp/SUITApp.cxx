#if defined WNT
#undef SUIT_ENABLE_PYTHON
//#else
//#include "SUITconfig.h"
#endif

#include <qapplication.h>

#include "SUIT_Session.h"
#include "SUIT_ResourceMgr.h"
#include "SUITApp_Application.h"

#ifdef SUIT_ENABLE_PYTHON
#include <Python.h>
#endif

#include <stdlib.h>

#include <qstringlist.h>
#include <qfile.h>
#include <qdir.h>
#include <qstring.h>

QString salomeVersion()
{
  QString path( ::getenv( "GUI_ROOT_DIR" ) );
  if ( !path.isEmpty() )
    path += QDir::separator();
  path += QString( "bin/salome/VERSION" );

  QFile vf( path );
  if ( !vf.open( IO_ReadOnly ) )
    return QString::null;

  QString line;
  vf.readLine( line, 1024 );
  vf.close();

  if ( line.isEmpty() )
    return QString::null;

  while ( !line.isEmpty() && line.at( line.length() - 1 ) == QChar( '\n' ) )
    line.remove( line.length() - 1, 1 );

  QString ver;
  int idx = line.findRev( ":" );
  if ( idx != -1 )
    ver = line.mid( idx + 1 ).stripWhiteSpace();

  return ver;
}

/* XPM */
static const char* pixmap_not_found_xpm[] = {
"16 16 3 1",
"       c None",
".      c #000000",
"+      c #A80000",
"                ",
"                ",
"    .     .     ",
"   .+.   .+.    ",
"  .+++. .+++.   ",
"   .+++.+++.    ",
"    .+++++.     ",
"     .+++.      ",
"    .+++++.     ",
"   .+++.+++.    ",
"  .+++. .+++.   ",
"   .+.   .+.    ",
"    .     .     ",
"                ",
"                ",
"                "};

class SUITApp_Session: public SUIT_Session
{
public:
  SUITApp_Session( bool theIniFormat ) : SUIT_Session(), myIniFormat ( theIniFormat ) {}
  virtual ~SUITApp_Session() {}

protected:
  virtual SUIT_ResourceMgr* createResourceMgr( const QString& appName ) const
  {
    SUIT_ResourceMgr* resMgr;
    if ( myIniFormat )
      resMgr = new SUIT_ResourceMgr( appName );
    else {
      resMgr = new SUIT_ResourceMgr( appName, QString( "%1Config" ) );
      resMgr->setVersion( salomeVersion() );
      resMgr->setCurrentFormat( "xml" );
      resMgr->setOption( "translators", QString( "%P_msg_%L.qm|%P_icons.qm|%P_images.qm" ) );
      static QPixmap defaultPixmap( pixmap_not_found_xpm );
      resMgr->setDefaultPixmap( defaultPixmap );
    }
    return resMgr;
  }
private:
  bool  myIniFormat;
};


int main( int args, char* argv[] )
{
#ifdef SUIT_ENABLE_PYTHON
  Py_Initialize();
  PySys_SetArgv( args, argv );
#endif
  
  QStringList argList;
  bool noExceptHandling = false;
  bool iniFormat = false;
  for ( int i = 1; i < args /*&& !noExceptHandling*/; i++ )
  {
    if ( !strcmp( argv[i], "/noexcepthandling" ) )
      noExceptHandling = true;
    else if ( !strcmp( argv[i], "--format=ini") )
      iniFormat = true;
    else
      argList.append( QString( argv[i] ) );
  }


  SUITApp_Application app( args, argv );

  int result = -1;
  if ( !argList.isEmpty() )
  {
    SUITApp_Session* aSession = new SUITApp_Session( iniFormat );
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
