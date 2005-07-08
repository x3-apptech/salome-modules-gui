//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : SUIT_FileValidator.cxx
//  Author : 
//  Module : SALOME
//  $Header$

#include "SUIT_FileValidator.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Session.h"

#include <qfile.h>
#include <qfileinfo.h>

/*! constructor */
SUIT_FileValidator::SUIT_FileValidator(QWidget* parent) :
myParent(parent) 
{
}
  
/*! returns false if can't open file */
bool SUIT_FileValidator::canOpen( const QString& file ) 
{
  if ( !QFile::exists( file ) ) {
    SUIT_MessageBox::error1( myParent,
          QObject::tr( "ERR_ERROR" ),
          QObject::tr( "ERR_FILE_NOT_EXIST" ).arg( file ),
          QObject::tr( "BUT_OK" ) );
      return false;
    }
  if ( !QFileInfo( file ).isReadable() ) {
    SUIT_MessageBox::error1( myParent,
          QObject::tr( "ERR_ERROR" ),
          QObject::tr( "ERR_PERMISSION_DENIED" ).arg( file ),
          QObject::tr( "BUT_OK" ) );
    return false; 
  }
  return true;
}

/*! returns false if can't save file */
bool SUIT_FileValidator::canSave( const QString& file ) 
{
  if ( QFile::exists( file ) ) {
    // if file exists - raise warning...
    if ( SUIT_MessageBox::warn2( myParent,
              QObject::tr( "WRN_WARNING" ),
              QObject::tr( "QUE_DOC_FILEEXISTS" ).arg( file ),
              QObject::tr( "BUT_YES" ), 
              QObject::tr( "BUT_NO" ),
              SUIT_YES, 
              SUIT_NO, 
              SUIT_NO ) == SUIT_NO ) {
      return false;
    }
    // ... and if user wants to overwrite file, check it for writeability
    if ( !QFileInfo( file ).isWritable() ) {
      SUIT_MessageBox::error1( myParent,
            QObject::tr( "ERR_ERROR" ),
            QObject::tr( "ERR_PERMISSION_DENIED" ).arg( file ),
            QObject::tr( "BUT_OK" ) );
      return false; 
    }
  }
  else {
    // if file doesn't exist - try to create it
    QFile qf( file );
    if ( !qf.open( IO_WriteOnly ) ) {
      SUIT_MessageBox::error1( myParent,
            QObject::tr( "ERR_ERROR" ),
            QObject::tr( "ERR_PERMISSION_DENIED" ).arg( file ),
            QObject::tr( "BUT_OK" ) );
      return false;
    }
    else {
      // remove just created file
      qf.close();
      qf.remove();
    }
  }
  return true;
}

