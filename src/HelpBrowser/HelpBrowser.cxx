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
// File   : HelpBrowser.cxx
// Author : Vadim SANDLER, OpenCASCADE S.A.S. (vadim.sandler@opencascade.com)
//          Maxim GLIBIN, OpenCASCADE S.A.S. (maxim.glibin@opencascade.com)

#include "HelpBrowser_Application.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QTimer>
#include <QtxResourceMgr.h>
#include <QtxTranslator.h>
#include <QtxWebBrowser.h>

#include <iostream>

namespace
{
  void printHelp()
  {
    QFileInfo fi( HelpBrowser_Application::arguments().at(0) );

    std::cout << std::endl;
    std::cout << "SALOME Help Browser" << std::endl;
    std::cout << std::endl;
    std::cout << "usage: " << qPrintable( fi.fileName() ) << " [options] file" << std::endl;
    std::cout << "    file is a help file to be opened" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-h, --help         Prints this help and quits." << std::endl;
    std::cout << "--language=LANG    Use LANG language in menus." << std::endl;
    //std::cout << "--add=APP_PID      Adds PID of application into the file." << std::endl;
    //std::cout << "--remove=APP_PID   Removes PID of application from the file." << std::endl;
    std::cout << std::endl;
  }
}

class RaiseWindowHelper: public QDialog
{
public:
  RaiseWindowHelper( QWidget* parent ) : QDialog( parent, Qt::FramelessWindowHint )
  {
    setAttribute( Qt::WA_DeleteOnClose, true );
    resize( 1, 1 );
    show();
    QTimer::singleShot( 100, this, SLOT( close() ) );
    QTimer::singleShot( 500, parent, SLOT( setFocus() ) );
  }
};

class HelpBrowser: public QtxWebBrowser
{
public:
  HelpBrowser() : QtxWebBrowser()
  {
    setAttribute( Qt::WA_DeleteOnClose, false );
  }
  ~HelpBrowser()
  {
    if ( resourceMgr() )
      resourceMgr()->save();
  }
  void about()
  {
    QStringList info;
    QFile f( ":/COPYING" );
    f.open( QIODevice::ReadOnly );
    QTextStream in( &f );

    info << QtxWebBrowser::tr( "%1 has been developed using %2" ).arg( QString( "SALOME %1").arg( tr( "Help Browser" ) ) ).arg( "Qt Solutions Component: Single Application." );
    info << "";
    info << in.readAll().split( "\n" );

    QMessageBox::about( this, tr( "About %1" ).arg( tr( "Help Browser" ) ),
      info.join( "\n" ) );
  }
  void load( const QString& url )
  {
    QtxWebBrowser::load( url );
    RaiseWindowHelper* helper = new RaiseWindowHelper( this );
  }
};

int main( int argc, char **argv )
{
  // Set application name (for preferences)
  HelpBrowser_Application::setApplicationName( "salome" );

  // Specify application identifier via its name
  QFileInfo fi( argv[0] );

  // Create application instance
  HelpBrowser_Application instance( fi.fileName(), argc, argv );

  // Parse command line arguments
  bool showHelp  = false;
  bool removeId  = false;

  QString language;
  QString helpfile;
  QString anAppID = QString();

  QRegExp rl( "--(.+)=(.+)" );
  rl.setMinimal( false );

  for ( int i = 1; i < argc; i++ )
  {
    QString param = argv[i];
    if ( param == "--help" || param == "-h" ) {
      showHelp = true;
    }
    else if ( rl.exactMatch( param ) ) {
      QString opt = rl.cap( 1 );
      QString val = rl.cap( 2 );
      if ( opt == "language" )
        language = val;
      else if ( opt == "add" )
        anAppID = val;
      else if ( opt == "remove" ) {
        anAppID = val;
        removeId = true;
      }
    }
    else {
      helpfile = param;
    }
  }

  // Show help and exit if '--help' or '-h' option has been specified via command line
  if ( showHelp )
  {
    printHelp();
    exit( 0 );
  }

  // Create a file with an application PIDs. File will be managed by only current application
  QStringList dirs;
  dirs << QDir::homePath();
  dirs << QString( ".config" );
  dirs << HelpBrowser_Application::applicationName();
  dirs << QString( "HelpBrowser" );
  QString aWatchedFile = dirs.join( QDir::separator() );

  QFile aFile( aWatchedFile );
  if ( instance.sendMessage( helpfile ) )
  {
    // Client application.
    if ( aFile.exists() && !anAppID.isEmpty() )
    {
      // Update the content of monitoring file
      if ( aFile.open( QIODevice::ReadWrite | QIODevice::Text ) )
      {
        QTextStream anInStream( &aFile );
        QString aContent( anInStream.readAll() );

        QRegExp rx("(\\d+)+");
        QStringList anAppIDs;
        int pos = 0;
        while ( pos >= 0 )
        {
          pos = rx.indexIn( aContent, pos );
          if ( pos >= 0 )
          {
            anAppIDs += rx.cap( 1 );
            pos += rx.matchedLength();
          }
        }

        if ( removeId )
        {
          if ( anAppIDs.contains( anAppID ) )
            anAppIDs.removeAt( anAppIDs.indexOf( anAppID ) );
        }
        else
        {
          if ( !anAppIDs.contains( anAppID ) )
            anAppIDs.append( anAppID );
        }

        aFile.resize( 0 );

        QTextStream anOutStream( &aFile );
        
        foreach (QString anAppId, anAppIDs )
          anOutStream << anAppId << endl;
        aFile.close();
      }
    }
    return 0;
  }
  else
  {
    if ( !anAppID.isEmpty() )
    {
      // Clear file system watcher if one has have path
      instance.clearWatcher();

      QFileInfo wfi( aFile.fileName() );
      if ( QDir().mkpath( wfi.absolutePath() ) && aFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
      {
        // Write date and time when the file was created
        QTextStream aOutStream( &aFile );
        aOutStream << anAppID << endl;
        aFile.close();
      }

      // Add file path to file system watcher
      instance.addWatchPath( aWatchedFile );
    }
  }

  if ( removeId )
    return 0;

  // Load translations
  QtxTranslator tqt, tsal;
  if ( !language.isEmpty() ) {
    if ( tqt.load( QString( "qt_%1" ).arg( language ), QLibraryInfo::location( QLibraryInfo::TranslationsPath ) ) )
      instance.installTranslator( &tqt );

    QDir appDir = HelpBrowser_Application::applicationDirPath();
    appDir.cdUp(); appDir.cdUp();

    if ( tsal.load( QString( "Qtx_msg_%1" ).arg( language ), appDir.filePath( "share/salome/resources/gui" ) ) )
      instance.installTranslator( &tsal );
  }

  // Initialize resource manager (for preferences)
  QtxResourceMgr* resMgr = new QtxResourceMgr( "HelpBrowser", "%1Config" );
  resMgr->setCurrentFormat( "xml" );
  QtxWebBrowser::setResourceMgr( resMgr );

  // Show main window
  HelpBrowser browser;
  browser.show();

  // Load file specified via command line
  if ( helpfile.isEmpty() ) {
    QString docdir = qgetenv( "DOCUMENTATION_ROOT_DIR" );
    if ( !docdir.isEmpty() )
      helpfile = QDir::toNativeSeparators( QString( "%1/index.html" ).arg( docdir ) );
  }

  if ( !helpfile.isEmpty() ) {
    browser.load( helpfile );
  }

  // Finalize main window activation
  instance.setActivationWindow( &browser );

  QObject::connect( &instance, SIGNAL( messageReceived( QString ) ),
                    &browser,  SLOT( load ( QString ) ) );

  QObject::connect( instance.fileSysWatcher(), SIGNAL(fileChanged(const QString&)),
                    &instance, SLOT(updateWatchStatement(const QString&)));

  return instance.exec();
}
