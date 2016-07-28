// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyEditor.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include "PyEditor_Window.h"
#include "PyEditor_StdSettings.h"

#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QStringList>
#include <QTranslator>

QString qtTrDir()
{
  const char* vars[] = { "QT5_ROOT_DIR", "QT_ROOT_DIR", "QTDIR" };
  QString qtPath;
  for ( uint i = 0; i < sizeof( vars ) / sizeof( vars[0] ) && qtPath.isEmpty(); i++ )
    qtPath = qgetenv( vars[i] );
  if ( !qtPath.isEmpty() )
    qtPath = QDir( qtPath ).absoluteFilePath( "translations" );
  return qtPath;
}

QString resourceDir()
{
  // Try standalone application's dir.
  QDir appDir = QApplication::applicationDirPath();
  appDir.cdUp();
  QFileInfo resFile( appDir.filePath( "share/resources/PyEditor_msg_en.qm" ) );
  if ( resFile.exists() )
    return resFile.absolutePath();

  // Try salome dir.
  appDir = QApplication::applicationDirPath();
  appDir.cdUp(); appDir.cdUp();
  resFile.setFile( appDir.filePath( "share/salome/resources/gui/PyEditor_msg_en.qm" ) );
  if ( resFile.exists() )
    return resFile.absolutePath();

  return QString();
}

int main( int argc, char *argv[] )
{
  QApplication app( argc, argv );
  app.setOrganizationName( "salome" );
  app.setOrganizationDomain( "www.salome-platform.org" );
  app.setApplicationName( "pyeditor" );
  
  PyEditor_StdSettings* settings = new PyEditor_StdSettings();
  PyEditor_Settings::setSettings( settings );
  
  QString language = settings->language();
  
  // Load Qt translations.
  QString qtDirTrSet = QLibraryInfo::location( QLibraryInfo::TranslationsPath );
  QString qtDirTrEnv = qtTrDir();
  
  QStringList qtTrFiles;
  qtTrFiles << "qt" << "qtbase";
  QStringList qtTrDirs;
  qtTrDirs << QLibraryInfo::location( QLibraryInfo::TranslationsPath );
  qtTrDirs << qtTrDir();
  
  foreach( QString qtTrFile, qtTrFiles ) {
    foreach ( QString qtTrDir, qtTrDirs ) {
      QTranslator* translator = new QTranslator;
      if ( translator->load( QString("%1_%2").arg( qtTrFile ).arg( language ), qtTrDir ) ) {
        app.installTranslator( translator );
        break;
      }
      else {
        delete translator;
      }
    }
  }
  
  // Load application's translations.
  QTranslator translator;
  if ( translator.load( QString( "PyEditor_msg_%1" ).arg( language ), resourceDir() ) )
    app.installTranslator( &translator );
  
  PyEditor_Window window;
  window.setWindowIcon( QIcon( ":/images/py_editor.png" ) );
  window.resize( 650, 700 );
  window.show();
  
  return app.exec();
}
