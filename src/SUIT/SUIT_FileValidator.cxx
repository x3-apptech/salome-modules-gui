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

// File   : SUIT_FileValidator.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "SUIT_FileValidator.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Tools.h"   
#include <QFileInfo>

/*!
  \class SUIT_FileValidator
  \brief Provides functionality to check the file or directory
  existance and permissions.
  \sa SUIT_FileDlg class
*/

/*!
  \brief Constructor.
  \param parent parent widget (used as parent when displaying 
  information message boxes)
*/
SUIT_FileValidator::SUIT_FileValidator( QWidget* parent )
: myParent( parent ) 
{
}
  
/*!
  \brief Check if the specified file exists and (optionally) can be read.

  If file does not exists or can not be read (if \a checkPermission is \c true)
  and parent() is not null, shows error message box.

  \param fileName file path
  \param checkPermission if \c true (default) check also file permissions
  \return \c false if file does not exist or if it does not have 
  read permissions (if \a checkPermission is \c true)
*/
bool SUIT_FileValidator::canOpen( const QString& fileName, bool checkPermission ) 
{
  if ( !QFile::exists( fileName ) ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_FILE_NOT_EXIST" ).arg( fileName ) );
    return false;
  }
  if ( checkPermission && !QFileInfo( fileName ).isReadable() ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_OPEN_PERMISSION_DENIED" ).arg( fileName ) );
    return false; 
  }
  return true;
}

/*!
  \brief Check if the specified file can be written.

  If file already exists and parent() is not null, prompts
  question message box to the user to confirm file overwriting.

  If file can not be written (if \a checkPermission is \c true)
  and parent() is not null, shows error message box.

  \param fileName file path
  \param checkPermission if \c true (default) check also file permissions
  \return \c false if file exists and user rejects file overwriting 
  or if file does not have write permissions (if \a checkPermission is \c true)
*/
bool SUIT_FileValidator::canSave( const QString& fileName, bool checkPermission ) 
{
  if ( QFile::exists( fileName ) ) {
    if ( parent() )
      if ( SUIT_MessageBox::question( parent(), QObject::tr( "WRN_WARNING" ),
                                      QObject::tr( "QUE_DOC_FILEEXISTS" ).arg( fileName ),
                                      SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                      SUIT_MessageBox::No ) != SUIT_MessageBox::Yes )
        return false;
    
    if ( checkPermission && !QFileInfo( fileName ).isWritable() ) {
      if ( parent() ) 
        SUIT_MessageBox::critical( myParent, QObject::tr( "ERR_ERROR" ),
                                   QObject::tr( "ERR_PERMISSION_DENIED" ).arg( fileName ) );
      return false; 
    }
  }
  else {
    QString dirName = SUIT_Tools::dir( fileName );
    if ( !QFile::exists( dirName ) ) {
      SUIT_MessageBox::critical( parent(), QObject::tr( "WRN_WARNING" ),
                                 QObject::tr( "ERR_DIR_NOT_EXIST" ).arg( dirName ) );
      return false;
    }
    if ( checkPermission && !QFileInfo( dirName ).isWritable() ) {
      if ( parent() )
        SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                   QObject::tr( "ERR_PERMISSION_DENIED" ).arg( fileName ) );
      return false;
    }
  }
  return true;
}

/*!
  \brief Check if the specified directory exists and (optionally) can be read.

  If directory does not exists or can not be read (if \a checkPermission is \c true)
  and parent() is not null, shows error message box.

  \param dirName directory path
  \param checkPermission if \c true (default) check also directory permissions
  \return \c false if directory does not exist or if it does not have 
  read permissions (if \a checkPermission is \c true)
*/
bool SUIT_FileValidator::canReadDir( const QString& dirName, bool checkPermission )
{
  QFileInfo info( dirName );
  if ( !info.exists() ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_DIR_NOT_EXIST" ).arg( dirName ) );
    return false;
  }
  if ( !info.isDir() ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_FILE_NOT_DIR" ).arg( dirName ) );
    return false;
  }
  if ( checkPermission && !info.isReadable() ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_DIR_READ_PERMISSION_DENIED" ).arg( dirName ) );
    return false; 
  }
  return true;
}

/*!
  \brief Check if the specified directory can be written.

  If directory does not exists or can not be modified (if \a checkPermission is \c true)
  and parent() is not null, shows error message box.

  \param dirName directory path
  \param checkPermission if \c true (default) check also directory permissions
  \return \c false if directory does not exist or if it does not have 
  write permissions (if \a checkPermission is \c true)
*/
bool SUIT_FileValidator::canWriteDir( const QString& dirName, bool checkPermission )
{
  QFileInfo info( dirName );
  if ( !info.exists() ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_DIR_NOT_EXIST" ).arg( dirName ) );
    return false;
  }
  if ( !info.isDir() ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_FILE_NOT_DIR" ).arg( dirName ) );
    return false;
  }
  if ( checkPermission && !info.isWritable() ) {
    if ( parent() )
      SUIT_MessageBox::critical( parent(), QObject::tr( "ERR_ERROR" ),
                                 QObject::tr( "ERR_DIR_WRITE_PERMISSION_DENIED" ).arg( dirName ) );
    return false; 
  }
  return true;
}

/*!
  \brief Get parent widget.
  \return parent widget
*/
QWidget* SUIT_FileValidator::parent() const
{ 
  return myParent; 
}
