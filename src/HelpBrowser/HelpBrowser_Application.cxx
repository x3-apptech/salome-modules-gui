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
// File   : HelpBrowser_Application.cpp
// Author : Maxim GLIBIN, OpenCASCADE S.A.S. (maxim.glibin@opencascade.com)

#include "HelpBrowser_Application.h"

#include <QFile>
#include <QFileSystemWatcher>
#include <QTextStream>
#include <QWidget>

/*!
  \brief Constructor
 */
HelpBrowser_Application::HelpBrowser_Application(const QString& appId, int& argc, char** argv)
    : QtSingleApplication(appId, argc, argv)
{
  watcher = new QFileSystemWatcher();
}

/*!
  \brief Returns the file system watcher for monitoring files for modifications.
*/
QFileSystemWatcher* HelpBrowser_Application::fileSysWatcher() const
{
  return watcher;
}

/*!
  \brief Removes the specified paths from the file system watcher and corresponding
  files if \a isDeleteFile flag is set. By default, this flag is \c true.
*/
void HelpBrowser_Application::clearWatcher(bool isDeleteFile)
{
  QStringList aFiles = watcher->files();
  foreach ( QString aFilePath, aFiles ) 
  {
    removeWatchPath( aFilePath );
    if ( isDeleteFile )
      QFile::remove( aFilePath );
  }
}

/*!
  Adds the path to the file system watcher if the file exists.
*/
void HelpBrowser_Application::addWatchPath(const QString& thePath)
{
  if ( thePath.isEmpty() )
  {
    qWarning("HelpBrowser_Application::addWatchPath: path is empty.");
    return;
  }
  watcher->addPath( thePath );
}

/*!
  Removes the path to the file system watcher.
*/
void HelpBrowser_Application::removeWatchPath(const QString& thePath)
{
  if (thePath.isEmpty())
  {
    qWarning("HelpBrowser_Application::removeWatchPath: path is empty.");
    return;
  }
  watcher->removePath( thePath );
}

/*!
  Updates the statement of watcher. If file becomes empty it is removed and
  calls quit() function for application.
*/
void HelpBrowser_Application::updateWatchStatement(const QString& thePath)
{
  if ( thePath.isEmpty() )
  {
    qWarning("HelpBrowser_Application::updateWatchStatement: path is empty.");
    return;
  }

  if ( !QFile::exists( thePath ) )
    removeWatchPath( thePath );

  // Get list of paths to files that are being watched
  bool isClose = false;
  if ( watcher->files().contains( thePath ) )
  {
    QFile aFile( thePath );
    if ( aFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
      QTextStream anInSteam( &aFile );
      if ( anInSteam.readAll().isEmpty() )
        isClose = true;
      aFile.close();
    }
  }

  if ( isClose )
  {
    QFile::remove( thePath );
    this->quit();
  }
}
