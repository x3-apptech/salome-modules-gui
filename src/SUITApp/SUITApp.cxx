//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#if defined WIN32

#ifdef SUIT_ENABLE_PYTHON
#undef SUIT_ENABLE_PYTHON
#endif

#else //#if defined WIN32

#ifndef SUIT_ENABLE_PYTHON
// NOTE: DO NOT DELETE THIS DEFINITION ON LINUX
// or make sure Python is initialized in main() in any case
// Otherwise, application based on light SALOME and using Python 
// are unlikely to work properly.
#define SUIT_ENABLE_PYTHON
#include <Python.h>
#endif

#endif //#if defined WIN32

#include "SUITApp_Application.h"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <Style_Salome.h>
#include <QtxSplash.h>

#include <SUIT_LicenseDlg.h>

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include <stdlib.h>

#ifdef WIN32
#include <UserEnv.h>
#endif

static QString salomeVersion()
{
  QString path( ::getenv( "GUI_ROOT_DIR" ) );
  if ( !path.isEmpty() )
    path += QDir::separator();

  path += QString( "bin/salome/VERSION" );

  QFile vf( path );
  if ( !vf.open( QFile::ReadOnly ) )
    return QString();

  QString line = vf.readLine( 1024 );
  vf.close();

  if ( line.isEmpty() )
    return QString();

  while ( !line.isEmpty() && line.at( line.length() - 1 ) == QChar( '\n' ) )
    line.remove( line.length() - 1, 1 );

  QString ver;
  int idx = line.lastIndexOf( ":" );
  if ( idx != -1 )
    ver = line.mid( idx + 1 ).trimmed();

  return ver;
}

static void MessageOutput( QtMsgType type, const char* msg )
{
  switch ( type )
  {
  case QtDebugMsg:
#ifdef _DEBUG_
    printf( "Debug: %s\n", msg );
#endif
    break;
  case QtWarningMsg:
#ifdef _DEBUG_
    printf( "Warning: %s\n", msg );
#endif
    break;
  case QtFatalMsg:
#ifdef _DEBUG_
    printf( "Fatal: %s\n", msg );
#endif
    break;
  default:
    break;
  }
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

  virtual SUIT_ResourceMgr* createResourceMgr( const QString& appName ) const
  {
    SUIT_ResourceMgr* resMgr = 0;
    if ( myIniFormat )
    {
      resMgr = new SUIT_ResourceMgr( appName, QString( "%1Config" ) );
      resMgr->setCurrentFormat( "ini" );
    }
    else
    {
      resMgr = new SUIT_ResourceMgr( appName, QString( "%1Config" ) );
      resMgr->setVersion( salomeVersion() );
      resMgr->setCurrentFormat( "xml" );
    }

    if ( resMgr )
    {
      static QPixmap defaultPixmap( pixmap_not_found_xpm );
      resMgr->setDefaultPixmap( defaultPixmap );
      resMgr->setOption( "translators", QString( "%P_msg_%L.qm|%P_icons.qm|%P_images.qm" ) );
    }
    return resMgr;
  }

private:
  bool  myIniFormat;
};

int main( int argc, char* argv[] )
{
#ifdef SUIT_ENABLE_PYTHON
  // First of all initialize Python, as in complex multi-component applications
  // someone else might initialize it some way unsuitable for light SALOME!
  Py_SetProgramName( argv[0] );
  Py_Initialize(); // Initialize the interpreter
  PySys_SetArgv( argc,  argv );
  PyEval_InitThreads(); // Create (and acquire) the interpreter lock
  PyEval_ReleaseLock(); // Let the others use Python API until we need it again
#endif

  //qInstallMsgHandler( MessageOutput );

  QStringList argList;
  bool noExceptHandling = false;
  bool iniFormat        = false;
  bool noSplash         = false;
  bool useLicense       = false;
  for ( int i = 1; i < argc /*&& !noExceptHandling*/; i++ )
  {
    if ( !strcmp( argv[i], "--noexcepthandling" ) )
      noExceptHandling = true;
    else if ( !strcmp( argv[i], "--format=ini") )
      iniFormat = true;
    else if ( !strcmp( argv[i], "--nosplash") )
      noSplash = true;
	else if ( !strcmp( argv[i], "--uselicense" ) )
      useLicense = true;
	else
      argList.append( QString( argv[i] ) );
  }

  SUITApp_Application app( argc, argv );

  int result = -1;

  if ( useLicense ) {
    QString env;

#ifdef WIN32
    DWORD aLen=1024;
    char aStr[1024];
    HANDLE aToken=0;
    HANDLE hProcess = GetCurrentProcess();
    OpenProcessToken(hProcess,TOKEN_QUERY,&aToken);
    if( GetUserProfileDirectory( aToken, aStr, &aLen ) )
      env = aStr;

#else
    if ( ::getenv( "HOME" ) )
      env = ::getenv( "HOME" );
#endif
 
    QFile file( env + "/ReadLicense.log" ); // Read the text from a file    
    if( !file.exists() ) {
      SUIT_LicenseDlg aLicense;
      if ( aLicense.exec() != QDialog::Accepted ) 
        return result;
    }
  }

  if ( !argList.isEmpty() )
  {
    SUITApp_Session* aSession = new SUITApp_Session( iniFormat );
    QtxSplash* splash = 0;
    SUIT_ResourceMgr* resMgr = aSession->createResourceMgr( argList.first() );
    if ( !noSplash ) 
    {
      if ( resMgr )
      {
	resMgr->loadLanguage( false );

	splash = QtxSplash::splash( QPixmap() );
	splash->readSettings( resMgr );
	if ( splash->pixmap().isNull() ) {
	  delete splash;
	  splash = 0;
	}
	else {
	  QString appName    = QObject::tr( "APP_NAME" ).trimmed();
	  QString appVersion = QObject::tr( "APP_VERSION" ).trimmed();
	  if ( appVersion == "APP_VERSION" )
	  {
	    if ( appName == "APP_NAME" || appName.toLower() == "salome" )
	      appVersion = salomeVersion();
	    else
	      appVersion = "";
	  }
	  splash->setOption( "%A", appName );
	  splash->setOption( "%V", QObject::tr( "ABOUT_VERSION" ).arg( appVersion ) );
	  splash->setOption( "%L", QObject::tr( "ABOUT_LICENSE" ) );
	  splash->setOption( "%C", QObject::tr( "ABOUT_COPYRIGHT" ) );
	  splash->show();
	  QApplication::instance()->processEvents();
	}
      }
    }

    SUIT_Application* theApp = aSession->startApplication( argList.first() );
    if ( theApp )
    {
      Style_Salome::initialize( theApp->resourceMgr() );
      if ( theApp->resourceMgr()->booleanValue( "Style", "use_salome_style", true ) )
	Style_Salome::apply();

      if ( !noExceptHandling )
        app.setHandler( aSession->handler() );

      if ( splash )
	splash->finish( theApp->desktop() );

      result = app.exec();
    }
    delete aSession;
  }

  return result;
}
