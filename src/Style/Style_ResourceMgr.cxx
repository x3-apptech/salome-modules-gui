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

// File   : Style_ResourceMgr.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
//
#include "Style_ResourceMgr.h"

/*!
  \class Style_ResourceMgr
  \brief SALOME style resources manager.

  This class is used to manage the SALOME style themes throughout the application
  in the similar way as QtxResourceMgr does it with application preferences.

  Standard SALOME themes are stored in the global application settings files
  (named as SalomeStyle.xml). User-defined themes are stored in user's home
  directory - in the file .SalomeStylerc.

  The Style_ResourceMgr class is used by SALOME style preferences dialog box
  (Style_PrefDlg class).
*/

/*!
  \brief Constructor
*/
Style_ResourceMgr::Style_ResourceMgr()
  : QtxResourceMgr( "SalomeStyle", "%1Config" )
{
  if ( dirList().isEmpty() && ::getenv( "GUI_ROOT_DIR" ) )
    setDirList( QStringList() << Qtx::addSlash( ::getenv( "GUI_ROOT_DIR" ) ) + "share/salome/resources/gui" );
  setCurrentFormat( "xml" );
}

/*!
  \brief Destructor
*/
Style_ResourceMgr::~Style_ResourceMgr()
{
}

/*!
  \brief Get list of avaiable SALOME themes
  \param type themes type
  \param sort if \c true (default), returns a list of themes sorted by name
  \return list of avaiable themes names
*/
QStringList Style_ResourceMgr::styles( StyleType type, bool sort )
{
  QStringList sl;
  
  WorkingMode m = workingMode();

  switch ( type ) {
  case Global:
    setWorkingMode( IgnoreUserValues );
    sl = sections();
    break;
  case User:
    {
      setWorkingMode( AllowUserValues );
      sl = sections();
      setWorkingMode( IgnoreUserValues );
      QMutableListIterator<QString> it( sl );
      while ( it.hasNext() ) {
        QString s = it.next();
        if ( hasSection( s ) ) it.remove();
      }
    }
    break;
  case All:
    setWorkingMode( AllowUserValues );
    sl = sections();
    break;
  default:
    break;
  }

  setWorkingMode( m );

  if ( sort )
    qSort( sl );

  return sl;
}
