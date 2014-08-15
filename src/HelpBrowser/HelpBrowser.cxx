// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "qtsingleapplication.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
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
    QFileInfo fi( QtSingleApplication::arguments().at(0) );

    std::cout << std::endl;
    std::cout << "SALOME Help Browser" << std::endl;
    std::cout << std::endl;
    std::cout << "usage: " << qPrintable( fi.fileName() ) << " [options] file" << std::endl;
    std::cout << "    file is a help file to be opened" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-h, --help         Prints this help and quits." << std::endl;
    std::cout << "--language=LANG    Use LANG language in menus." << std::endl;
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
  // set application name (for preferences)
  
  QtSingleApplication::setApplicationName( "salome" );

  // specify application identifier via its name
  QFileInfo fi( argv[0] );

  // create application instance

  QtSingleApplication instance( fi.fileName(), argc, argv );

  // parse command line arguments

  bool showHelp = false;
  QString language;
  QString helpfile;

  QRegExp rl( "--language=(.+)" );
  rl.setMinimal( false );

  for ( int a = 1; a < argc; ++a ) {
    QString param = argv[a];
    if ( param == "--help" || param == "-h" )
      showHelp = true;
    else if ( rl.exactMatch( param ) )
      language = rl.cap( 1 );
    else
      helpfile = param;
  }

  // show help and exit if --help or -h option has been specified via command line

  if ( showHelp ) {
    printHelp();
    exit( 0 );
  }

  if ( instance.sendMessage( helpfile ) )
    return 0;

  // load translations

  QtxTranslator tqt, tsal;
  if ( !language.isEmpty() ) {
    if ( tqt.load( QString( "qt_%1" ).arg( language ), QLibraryInfo::location( QLibraryInfo::TranslationsPath ) ) )
      instance.installTranslator( &tqt );

    QDir appDir = QtSingleApplication::applicationDirPath();
    appDir.cdUp(); appDir.cdUp(); 
    
    if ( tsal.load( QString( "Qtx_msg_%1" ).arg( language ), appDir.filePath( "share/salome/resources/gui" ) ) )
      instance.installTranslator( &tsal );
  }

  // initialize resource manager (for preferences)

  QtxResourceMgr* resMgr = new QtxResourceMgr( "HelpBrowser", "%1Config" );
  resMgr->setCurrentFormat( "xml" );
  QtxWebBrowser::setResourceMgr( resMgr );

  // show main window

  HelpBrowser browser;
  browser.show();

  // load file specified via command line

  if ( helpfile.isEmpty() ) {
    QString docdir = qgetenv( "DOCUMENTATION_ROOT_DIR" );
    if ( !docdir.isEmpty() )
      helpfile = QDir::toNativeSeparators( QString( "%1/index.html" ).arg( docdir ) );
  }

  if ( !helpfile.isEmpty() ) {
    browser.load( helpfile );
  }

  // finalize main window activation

  instance.setActivationWindow( &browser );
  
  QObject::connect( &instance, SIGNAL( messageReceived( QString ) ),
		    &browser,  SLOT( load ( QString ) ) );

  return instance.exec();
}
