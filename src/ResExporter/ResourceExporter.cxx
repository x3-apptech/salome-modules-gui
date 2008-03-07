// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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
/*!
 File:      ResourceExporter.cxx
 Created:   27/06/05
 Author:    Vadim SANDLER
 Copyright (C) CEA 2005

 This tool provides command-line interface allowing to modify user preferences.
 The tool can be used by the compilation procedures in order to set default preferences for the module.

 Usage:
    ResourceExporter <parameters-list>
 Parameters have the following format:
    '<section>:<name>=<new_value>'            - to set <new_value> for the user 
                                                preference <name> from the <section> section;
    '<section>:<name>+=<value>[|<separator>]' - the <value> is appended to the current value 
                                                of the preference <name> from the <section> section;
                                                separator is used to concatenate old and new values,
                                                by default comma (',') symbol is used
    '-<section>:<name>'                       - to remove user preference <name> from the <section>
                                                section. Note that only use preference file is 
                                                influenced, you may need to use '<section>:<name>=""'
                                                option
 The number of parameters is limeted only by maximum possible length of the command line.
*/

#include "SUIT_ResourceMgr.h"
#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <iostream>

/*!
  \return the SALOME version number
*/
static QString salomeVersion()
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

/*!
  print the help information
*/
static void help()
{
  std::cout << ""                                                                                                     << std::endl;
  std::cout << "ResourceExporter: provides command-line interface to modify user preferences."                        << std::endl;
  std::cout << ""                                                                                                     << std::endl;
  std::cout << "Usage:"                                                                                               << std::endl;
  std::cout << "   ResourceExporter <parameters-list>"                                                                << std::endl;
  std::cout << "Parameters have the following format:"                                                                << std::endl;
  std::cout << "   '<section>:<name>=<new_value>'            - to set <new_value> for the user "                      << std::endl;
  std::cout << "                                               preference <name> from the <section> section;"         << std::endl;
  std::cout << "   '<section>:<name>+=<value>[|<separator>]' - the <value> is appended to the current value "         << std::endl;
  std::cout << "                                               of the preference <name> from the <section> section;"  << std::endl;
  std::cout << "                                               separator is used to concatenate old and new values,"  << std::endl;
  std::cout << "                                               by default comma (',') symbol is used"                 << std::endl;
  std::cout << "   '-<section>:<name>'                       - to remove user preference <name> from the <section>"   << std::endl;
  std::cout << "                                               section. Note that only use preference file is "       << std::endl;
  std::cout << "                                               influenced, you may need to use '<section>:<name>=""'" << std::endl;
  std::cout << "                                               option"                                                << std::endl;
  std::cout << "The number of parameters is limeted only by maximum possible length of the command line."             << std::endl;
  std::cout << ""                                                                                                     << std::endl;
}

/*!
  application main() function
*/
int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  if ( argc > 1 ) {
    SUIT_ResourceMgr* resMgr = new SUIT_ResourceMgr( QString( "SalomeApp" ), QString( "%1Config" ) );
    resMgr->setVersion( salomeVersion() );
    resMgr->setCurrentFormat( QString( "xml" ) );
    resMgr->loadLanguage();
    for ( int i = 1; i < argc; i ++ ) {
      QString anArg = QString( argv[i] ).stripWhiteSpace();
      if ( anArg.startsWith( "-" ) ) {
	anArg.remove( 0, 1 );
	if ( anArg.contains( ":" ) ) {
	  QStringList vals = QStringList::split( ":", anArg );
	  QString section  = vals[ 0 ].stripWhiteSpace();
	  QString param    = vals[ 1 ].stripWhiteSpace();
	  if ( section.isEmpty() || param.isEmpty() ) continue;
	  resMgr->remove( section, param );
	}
      }
      else if ( anArg.contains( "+=" ) ) {
	QStringList vals = QStringList::split( "+=", anArg );
	if ( vals[ 0 ].contains( ":" ) ) {
	  QStringList vals1 = QStringList::split( ":", vals[ 0 ] );
	  QString section  = vals1[ 0 ].stripWhiteSpace();
	  QString param    = vals1[ 1 ].stripWhiteSpace();
	  QString newValue = vals [ 1 ].stripWhiteSpace();
	  QString separ    = ","; // default separator
	  if ( newValue.contains( "|" ) ) {
	    QStringList vals2 = QStringList::split( "|", newValue );
	    newValue = vals2[ 0 ].stripWhiteSpace();
	    separ  = vals2[ 1 ].stripWhiteSpace();
	  }
	  if ( section.isEmpty() || param.isEmpty() || newValue.isEmpty() || separ.isEmpty() ) continue;
	  QString value = resMgr->stringValue( section, param );
	  QStringList valsOld = QStringList::split( separ, value );
	  QStringList valsNew = QStringList::split( separ, newValue );
	  for ( int i = 0; i < (int)valsNew.count(); i++ )
	    if ( !valsOld.contains( valsNew[i] ) )
	      valsOld.append( valsNew[i] );
	  resMgr->setValue( section, param, valsOld.join( separ ) );
	}
      }
      else if ( anArg.contains( "=" ) ) {
	QStringList vals = QStringList::split( "=", anArg );
	if ( vals[ 0 ].contains( ":" ) ) {
	  QStringList vals1 = QStringList::split( ":", vals[ 0 ] );
	  QString section  = vals1[ 0 ].stripWhiteSpace();
	  QString param    = vals1[ 1 ].stripWhiteSpace();
	  QString value = vals [ 1 ].stripWhiteSpace();
	  if ( section.isEmpty() || param.isEmpty() ) continue;
	  resMgr->setValue( section, param, value );
	}
      }
    }
    resMgr->save();
    delete resMgr;
  }
  else {
    help();
  }
  return 0;
}
