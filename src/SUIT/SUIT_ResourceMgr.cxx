// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
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
#include "SUIT_ResourceMgr.h"

#include <qfileinfo.h>
#include <qdir.h>
#include <qapplication.h>
#include <qregexp.h>

/*!
    Constructor
*/
SUIT_ResourceMgr::SUIT_ResourceMgr( const QString& app, const QString& resVarTemplate )
: QtxResourceMgr( app, resVarTemplate )
{
}

/*!
    Destructor
*/
SUIT_ResourceMgr::~SUIT_ResourceMgr()
{
}

/*!
    Returns the version of application
*/
QString SUIT_ResourceMgr::version() const
{
  return myVersion;
}

/*!
    Sets the version of application
*/
void SUIT_ResourceMgr::setVersion( const QString& ver )
{
  myVersion = ver;
}

/*!
    Loads a doc page from 'prefix' resources and indetified by 'id'
*/
QString SUIT_ResourceMgr::loadDoc( const QString& prefix, const QString& id ) const
{
  QString docSection = option( "doc_section_name" );
  if ( docSection.isEmpty() )
    docSection = QString( "docs" );

  return path( docSection, prefix, id );
}

#ifndef WIN32
#include <unistd.h>
#endif
/*!
    Returns the user file name for specified application
*/
QString SUIT_ResourceMgr::userFileName( const QString& appName, const bool for_load ) const
{
  QString pathName;

  // Try config file, given in arguments
  for (int i = 1; i < qApp->argc(); i++) {
    QRegExp rx ("--resources=(.+)");
    if ( rx.search( QString(qApp->argv()[i]) ) >= 0 && rx.capturedTexts().count() > 1 ) {
      QString file = rx.capturedTexts()[1];
      QFileInfo fi (file);
      pathName = fi.absFilePath();
    }
  }

  if (!pathName.isEmpty())
    return pathName;

  // QtxResourceMgr::userFileName() + '.' + version()
  pathName = QtxResourceMgr::userFileName( appName );

  if ( !version().isEmpty() )
    pathName += QString( "." ) + version();

  if ( !QFileInfo( pathName ).exists() && for_load )
  {
    QString newName = findAppropriateUserFile( pathName );
    if ( !newName.isEmpty() )
      pathName = newName;
  }

  return pathName;
}

/*!
    Finds other the most appropriate user file instead missing one
*/
QString SUIT_ResourceMgr::findAppropriateUserFile( const QString& fname ) const
{
  QDir d( QFileInfo( fname ).dir( true ) );
  d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
  QStringList l = d.entryList();
  QString appr_file;
  int id0 = userFileId( fname ), id, appr=-1;
  if( id0<0 )
    return appr_file;

  for( QStringList::const_iterator anIt = l.begin(), aLast = l.end(); anIt!=aLast; anIt++ )
  {
    id = userFileId( *anIt );
    if( id<0 )
      continue;

    if( appr < 0 || abs( id-id0 ) < abs( appr-id0 ) )
    {
      appr = id;
      appr_file = d.absFilePath( *anIt );
    }
  }
  return appr_file;
}

/*!
    Calculates integer extended version number by user file name for comparing
*/
int SUIT_ResourceMgr::userFileId( const QString& ) const
{
  return -1;
}
