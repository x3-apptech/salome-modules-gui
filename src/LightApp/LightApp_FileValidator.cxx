// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : LightApp_FileValidator.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "LightApp_FileValidator.h"
#include "SUIT_MessageBox.h"

#include <QFileInfo>

/*!
  \class LightApp_PyFileValidator
  \brief Validator for Python scripts file names. 
*/

/*!
  \brief Constructor
  \param parent parent widget
*/
LightApp_PyFileValidator::LightApp_PyFileValidator( QWidget* parent )
  : SUIT_FileValidator ( parent )
{
}

/*!
  \brief Check if the specified file can be written.

  Checks if file name is valid for Python script.

  \param fileName file path
  \param checkPermission if \c true (default) check also file permissions
  \return \c false if file exists and user rejects file overwriting
  or if file does not have write permissions (if \a checkPermission is \c true)
  \sa SUIT_FileValidator
*/
bool LightApp_PyFileValidator::canSave( const QString& fileName, bool checkPermission )
{
  QFileInfo fi( fileName );
  if ( !QRegExp( "[A-Za-z_][A-Za-z0-9_]*" ).exactMatch( fi.completeBaseName() ) ) {
    SUIT_MessageBox::critical( parent(),
                               QObject::tr( "WRN_WARNING" ),
                               QObject::tr( "WRN_PYTHON_FILE_NAME_BAD" ) );
    return false;
  }
  return SUIT_FileValidator::canSave( fileName, checkPermission );
}
