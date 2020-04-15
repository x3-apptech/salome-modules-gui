// Copyright (C) 2007-2020  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef DISABLE_PYCONSOLE
#include "SUITApp_init_python.hxx"
#endif

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

namespace
{
  //! Get version of SALOME GUI module
  static QString salomeVersion()
  {
    return GUI_VERSION_STR;
  }

  //! Extract application name from the library filename
  static QString getAppName( const QString& libName )
  {
    QString appName = QFileInfo( libName ).baseName();
    if ( appName.startsWith( "lib" ) )
      appName = appName.mid( 3 );
    return appName;
  }

  //! Custom resources manager, that allows customization of application name/version
  //  via configuration/translation files.
  class ResourceMgr : public SUIT_ResourceMgr
  {
  public:
    ResourceMgr( bool iniFormat = false, const QString& appName = "LightApp" )
    : SUIT_ResourceMgr( appName, "%1Config" )
    {
      customize( iniFormat, appName ); // activate customization
      setCurrentFormat( iniFormat ? "ini" : "xml" );
      setOption( "translators", QString( "%P_msg_%L.qm|%P_icons.qm|%P_images.qm" ) );
    }

    QString customName() const { return myCustomAppName; }
    QString version() const { return myCustomAppVersion; }

  private:
    static void customize( bool iniFormat, const QString& appName )
    {
      // Try to retrieve actual application name and version from translation files.
      // We create temporary resource manager and load translations.
      // This procedure is supposed to be done only once, at first call.
      if ( myCustomAppName.isNull() ) {
        SUIT_ResourceMgr mgr( appName, "%1Config" );
        mgr.setCurrentFormat( iniFormat ? "ini" : "xml" );
        mgr.setWorkingMode( IgnoreUserValues ); // prevent reading data from user's file
        mgr.loadLanguage( appName, "en" );

        // actual application name can be customized via APP_NAME resource key
        myCustomAppName = QObject::tr( "APP_NAME" ).trimmed();
        if ( myCustomAppName == "APP_NAME" || myCustomAppName.toLower() == "salome" ) 
          myCustomAppName = appName; // fallback name

        // actual application name can be customized via APP_VERSION resource key
        myCustomAppVersion = QObject::tr( "APP_VERSION" ).trimmed();
        if ( myCustomAppVersion == "APP_VERSION" )
          myCustomAppVersion = myCustomAppName == appName ? salomeVersion() : ""; // fallback version
      }
    }

  protected:
    QString userFileName( const QString& appName, const bool forLoad ) const
    {
      if ( version().isEmpty() ) return ""; 
      return SUIT_ResourceMgr::userFileName( myCustomAppName, forLoad );
    }

    virtual long userFileId( const QString& _fname ) const
    {
      //////////////////////////////////////////////////////////////////////////////////////////////
      // In SALOME and SALOME-based applications the user preferences file is named as
      // - <AppName>.xml.<AppVersion> on Windows
      // - <AppName>rc.<AppVersion> on Linux
      // where
      //   * AppName is application name, defaults to LightApp. Can be customized in SALOME-based
      //     applications, see ResourceMgr above for more details.
      //   * AppVersion is application version, defaults to current version of SALOME GUI module
      //     if AppName is not customize, otherwise empty. Can be customized in SALOME-based
      //     applications, see ResourceMgr above for more details.
      //
      // Since version 6.5.0 of SALOME, user file is stored in the ~/.config/salome
      // directory. For backward compatibility, when user preferences from nearest
      // version of application is searched, user home directory is also looked through,
      // with lower priority.
      // 
      // Since version 6.6.0 of SALOME, user file name on Linux is no more prefixed by dot
      // symbol since it is stored in the hidden ~/.config/salome directory. However, dot-prefixed
      // files are also taken into account (with lower priority) for backward compatibility.
      //
      // Notes:
      // - Currently the following format of version number is supported:
      //   <major>[.<minor>[.<release>[<type><dev>]]]
      //   Parts in square brackets are considered optional. Here:
      //   * major   - major version id
      //   * minor   - minor version id
      //   * release - maintenance version id
      //   * type    - dev or patch marker; it can be either one alphabetical symbol (from 'a' to 'z')
      //               or 'rc' to point release candidate (case-insensitive)
      //   * dev     - dev version or patch number
      //   All numerical values must be of range [1-99].
      //   Examples: 1.0, 6.5.0, 1.2.0a1, 3.3.3rc3 (release candidate 3), 11.0.0p1 (patch 1)
      //
      // - Versioning approach can be customized by implementing and using own resource manager class,
      //   see QtxResurceMgr, SUIT_ResourceMgr classes, and ResourceMgr class above in this file.
      //////////////////////////////////////////////////////////////////////////////////////////////

      long id = -1;
      if ( !myCustomAppName.isEmpty() ) {
#ifdef WIN32
        // On Windows, user file name is something like LightApp.xml.6.5.0 where
        // - LightApp is an application name (can be customized)
        // - xml is a file format (xml or ini)
        // - 6.5.0 is an application version, can include alfa/beta/rc marks, e.g. 6.5.0a3, 6.5.0rc1
        QRegExp exp( QString( "%1\\.%2\\.([a-zA-Z0-9.]+)" ).arg( myCustomAppName ).arg( currentFormat() ) );
#else
        // On Linux, user file name is something like LightApprc.6.5.0 where
        // - LightApp is an application name (can be customized)
        // - 6.5.0 is an application version, can include alfa/beta/rc marks, e.g. 6.5.0a3, 6.5.0rc1

        // VSR 24/09/2012: issue 0021781: since version 6.6.0 user filename is not prepended with "."
        // when it is stored in the ~/.config/<appname> directory;
        // for backward compatibility we also check files prepended with "." with lower priority
        QRegExp exp( QString( "\\.?%1rc\\.([a-zA-Z0-9.]+)" ).arg( myCustomAppName ) );
#endif
        QString fname = QFileInfo( _fname ).fileName();
        if ( exp.exactMatch( fname ) ) {
          long fid = Qtx::versionToId( exp.cap( 1 ) );
          if ( fid > 0 ) id = fid;
        }
      }
      return id;
    }

  private:
    static QString myCustomAppName;
    static QString myCustomAppVersion;
  };
  QString ResourceMgr::myCustomAppName;
  QString ResourceMgr::myCustomAppVersion;

  //! Custom session, to use custom resource manager class.
  class Session : public SUIT_Session
  {
  public:
    Session( bool theIniFormat = false ) : SUIT_Session(), myIniFormat( theIniFormat ) {}
    virtual SUIT_ResourceMgr* createResourceMgr( const QString& appName ) const
    {
      return new ResourceMgr( myIniFormat, appName );
    }
  private:
    bool myIniFormat;
  };
} // end of anonymous namespace

int main( int argc, char* argv[] )
{
  // Set-up application settings configuration (as for QSettings)
  // Note: these are default settings which can be customized (see below)
  QApplication::setOrganizationName( "salome" );
  QApplication::setApplicationName( "salome" );
  QApplication::setApplicationVersion( salomeVersion() );

  // Add <qtdir>/plugins dir to the pluins search path for image plugins
  QString qtdir = Qtx::qtDir( "plugins" );
  if ( !qtdir.isEmpty() )
    QApplication::addLibraryPath( qtdir );

  // Add application library path (to search style plugin etc...)
  QString path = Qtx::addSlash( Qtx::getenv( "GUI_ROOT_DIR" ) ) + "bin/salome";
  QApplication::addLibraryPath( QDir::toNativeSeparators( path ) );

  // QSurfaceFormat should be set before creation of QApplication,  
  // so to avoid conflicts beetween SALOME and ParaView QSurfaceFormats we should merge theirs formats
  // (see void Qtx::initDefaultSurfaceFormat()) and set the resultant format here.
  Qtx::initDefaultSurfaceFormat(); 

  // Create Qt application instance: this should be done as early as possible!
  // Note: QApplication forces setting locale LC_ALL to system one: setlocale(LC_ALL, "").
  SUITApp_Application app( argc, argv );

  // Initialize Python (if necessary)
  // Note: Python forces setting locale LC_CTYPE to system one: setlocale(LC_CTYPE, "").
#ifndef DISABLE_PYCONSOLE
  char* py_argv[] = {(char*)""};
  SUIT_PYTHON::init_python( 1, py_argv );
#endif

  // Treat command line arguments
  bool debugExceptions  = false;
  bool iniFormat        = false;
  bool noSplash         = false;
  bool useLicense       = false;
  QStringList args;
  foreach( QString arg, QApplication::arguments().mid(1) ) // omit 1st argument: app executable
  {
    if ( arg == "--no-exception-handler" )
      debugExceptions = true;
    else if ( arg == "--format=ini" )
      iniFormat = true;
    else if ( arg == "--no-splash" )
      noSplash = true;
    else if ( arg == "--show-license" )
      useLicense = true;
    else if ( !arg.startsWith( "-" ) )
      args << arg;
  }
  if ( args.empty() )
    args << "LightApp"; // fallback application library

  QString appName = getAppName( args.first() );

  // Create auxiliary resource manager to access application settings
  ResourceMgr resMgr( iniFormat, appName );
  resMgr.setWorkingMode( ResourceMgr::IgnoreUserValues );
  resMgr.loadLanguage( appName, "en" );

  // Set-up application settings configuration possible customized via resources
  if ( resMgr.customName() != "LightApp" ) {
    QApplication::setApplicationName( resMgr.customName() );
    QApplication::setApplicationVersion( resMgr.version() );
  }

  // Force default "C" locale if requested via user's preferences
  // Note: this does not change whole application locale (changed via setlocale() function),
  // but only affects GUI behavior
  resMgr.setWorkingMode( ResourceMgr::AllowUserValues ); // we must take into account user preferences
  if ( resMgr.booleanValue( "language", "locale", true ) )
    QLocale::setDefault( QLocale::c() );
  resMgr.setWorkingMode( ResourceMgr::IgnoreUserValues );

  if ( !debugExceptions )
    debugExceptions = resMgr.booleanValue( "launch", "noexcepthandler", false );
  if ( !noSplash )
    noSplash = !resMgr.booleanValue( "launch", "splash", true );
  if ( !useLicense )
    useLicense = resMgr.booleanValue( "launch", "license", false );

  // If asked, read the text from a file show a license dialog
  // TODO: path to license file, and option to check license, may be defined in XML cfg file.
  if ( useLicense ) {
    QFile file( QDir::home().filePath( "ReadLicense.log" ) );
    if ( !file.exists() ) {
      SUIT_LicenseDlg aLicense;
      if ( aLicense.exec() != QDialog::Accepted ) 
        return 1; // license is not accepted!
    }
  }

  // If not disabled, show splash screen
  QtxSplash* splash = 0;
  if ( !noSplash ) 
  {
    splash = QtxSplash::splash( QPixmap() );
    splash->readSettings( &resMgr );
    if ( splash->pixmap().isNull() )
      splash->setPixmap( resMgr.loadPixmap( appName, QObject::tr( "ABOUT_SPLASH" ) ) );
    if ( splash->pixmap().isNull() )
      splash->setPixmap( resMgr.loadPixmap( "LightApp", QObject::tr( "ABOUT_SPLASH" ) ) );
    if ( splash->pixmap().isNull() ) {
      delete splash;
      splash = 0;
    }
    else {
      splash->setOption( "%A", QObject::tr( "APP_NAME" ) );
      splash->setOption( "%V", QObject::tr( "ABOUT_VERSION" ).arg( resMgr.version() ) );
      splash->setOption( "%L", QObject::tr( "ABOUT_LICENSE" ) );
      splash->setOption( "%C", QObject::tr( "ABOUT_COPYRIGHT" ) );
      splash->show();
      QApplication::instance()->processEvents();
    }
  }

  // Create session
  Session session( iniFormat );

  // Initialize and start application supplied by the library specified via the parameter
  SUIT_Application* sessionApp = session.startApplication( appName );
  if ( sessionApp )
  {
#ifdef USE_SALOME_STYLE
    Style_Salome::initialize( session->resourceMgr() );
    if ( session->resourceMgr()->booleanValue( "Style", "use_salome_style", true ) )
      Style_Salome::apply();
#endif // USE_SALOME_STYLE

    if ( !debugExceptions )
      app.setHandler( session.handler() );

    if ( splash )
      splash->finish( sessionApp->desktop() );

    return app.exec();
  }

  return 1;
}
