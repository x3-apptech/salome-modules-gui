// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

//#ifdefined WIN32
//#ifndef DISABLE_PYCONSOLE
//#define DISABLE_PYCONSOLE
//#endif
//#else //#if defined WIN32
//#ifdef DISABLE_PYCONSOLE
// NOTE: DO NOT DELETE THIS DEFINITION ON LINUX
// or make sure Python is initialized in main() in any case
// Otherwise, application based on light SALOME and using Python 
// are unlikely to work properly.
//#undef DISABLE_PYCONSOLE
//#include <Python.h>
//#endif
//
#ifndef DISABLE_PYCONSOLE
#include "SUITApp_init_python.hxx"
#endif

//#endif //#if defined WIN32

#include "GUI_version.h"
#include "SUITApp_Application.h"
#include "SUIT_Desktop.h"
#include "SUIT_LicenseDlg.h"
#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"
#ifdef USE_SALOME_STYLE
#include "Style_Salome.h"
#endif // USE_SALOME_STYLE
#include "QtxSplash.h"

#include <QDir>
#include <QFile>
#include <QLocale>
#include <QRegExp>
#include <QString>
#include <QStringList>

#include <stdlib.h>

#ifdef WIN32
#include <UserEnv.h>
#endif

static QString salomeVersion()
{
  return GUI_VERSION_STR;
}

static QString getAppName( const QString& libName )
{
  QString appName = QFileInfo( libName ).baseName();
  if ( appName.startsWith( "lib" ) ) appName = appName.mid( 3 );
  return appName;
}

// static void MessageOutput( QtMsgType type, const char* msg )
// {
//   switch ( type )
//   {
//   case QtDebugMsg:
// #ifdef _DEBUG_
//     printf( "Debug: %s\n", msg );
// #endif
//     break;
//   case QtWarningMsg:
// #ifdef _DEBUG_
//     printf( "Warning: %s\n", msg );
// #endif
//     break;
//   case QtFatalMsg:
// #ifdef _DEBUG_
//     printf( "Fatal: %s\n", msg );
// #endif
//     break;
//   default:
//     break;
//   }
// }

class SUITApp_Session: public SUIT_Session
{
public:
  SUITApp_Session( bool theIniFormat, int argc, char** argv ) : SUIT_Session( argc, argv ), myIniFormat ( theIniFormat ) {}
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
      resMgr->setOption( "translators", QString( "%P_msg_%L.qm|%P_icons.qm|%P_images.qm" ) );
    }
    return resMgr;
  }

private:
  bool  myIniFormat;
};

int main( int argc, char* argv[] )
{
  //#ifndef DISABLE_PYCONSOLE
  //  // First of all initialize Python, as in complex multi-component applications
  //  // someone else might initialize it some way unsuitable for light SALOME!
  //  Py_SetProgramName( argv[0] );
  //  Py_Initialize(); // Initialize the interpreter
  //  PySys_SetArgv( argc,  argv );
  //  PyEval_InitThreads(); // Create (and acquire) the interpreter lock
  //  PyEval_ReleaseLock(); // Let the others use Python API until we need it again
  //#endif

  //qInstallMsgHandler( MessageOutput );

  QStringList argList;
  bool noExceptHandling = false;
  bool iniFormat        = false;
  bool noSplash         = false;
  bool useLicense       = false;
  for ( int i = 1; i < argc; i++ )
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

  // set "C" locale if requested via preferences
  {
    SUITApp_Session stmp( iniFormat, argc, argv );
    QApplication::setApplicationName( "salome" );
    SUIT_ResourceMgr* resMgr = stmp.createResourceMgr( "LightApp" );
    bool isCloc = resMgr->booleanValue( "language", "locale", true );
    if ( isCloc ) {
      QLocale::setDefault( QLocale::c() );
    }
    else {
      QLocale::setDefault( QLocale::system() );
    }
  }

  // add <qtdir>/plugins directory to the pluins search path for image plugins
  QString qtdir = Qtx::qtDir( "plugins" );
  if ( !qtdir.isEmpty() )
    QApplication::addLibraryPath( qtdir );

// TODO (QT5 PORTING) Below is a temporary solution, to allow compiling with Qt 5
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  //Set a "native" graphic system in case if application runs on the remote host
  QString remote(::getenv("REMOTEHOST"));
  QString client(::getenv("SSH_CLIENT"));
  if(remote.length() > 0 || client.length() > 0 ) {
    QApplication::setGraphicsSystem(QLatin1String("native"));
  }
#endif
  
  SUITApp_Application app( argc, argv );
  QString cfgAppName = getAppName( argList.isEmpty() ? QString() : argList.first() );
  // hard-coding for LightApp :( no other way to this for the moment
  if ( cfgAppName == "LightApp" ) {
    app.setOrganizationName( "salome" );
    app.setApplicationName( "salome" );
    app.setApplicationVersion( salomeVersion() );
  }

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
    SUITApp_Session aSession( iniFormat, argc, argv );
    QtxSplash* splash = 0;
    SUIT_ResourceMgr* resMgr = aSession.createResourceMgr( argList.first() );
    if ( !noSplash ) 
    {
      if ( resMgr )
      {
        resMgr->loadLanguage();

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

#ifndef DISABLE_PYCONSOLE
    //...Initialize python 
    int   _argc   = 1;
    char* _argv[] = {(char*)""};
    SUIT_PYTHON::init_python(_argc,_argv);
#endif

    SUIT_Application* theApp = aSession.startApplication( argList.first() );
    if ( theApp )
    {
#ifdef USE_SALOME_STYLE
      Style_Salome::initialize( theApp->resourceMgr() );
      if ( theApp->resourceMgr()->booleanValue( "Style", "use_salome_style", true ) )
        Style_Salome::apply();
#endif // USE_SALOME_STYLE

      if ( !noExceptHandling )
        app.setHandler( aSession.handler() );

      if ( splash )
        splash->finish( theApp->desktop() );

      result = app.exec();
    }
  }

  return result;
}
