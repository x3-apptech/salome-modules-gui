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

#include "SalomeApp_Tools.h"

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>

#include <utilities.h>

#include <QColor>
#include <QString>

#include <Quantity_Color.hxx>

/*!
  Convert QColor to Quantity_Color, if QColor is valid.
*/
Quantity_Color SalomeApp_Tools::color( const QColor& c )
{
        Quantity_Color aColor;
        if ( c.isValid() )
                aColor = Quantity_Color( c.red() / 255., c.green() / 255., c.blue() / 255., Quantity_TOC_RGB );
        return aColor;
}

/*!
  Convert Quantity_Color to QColor.
*/
QColor SalomeApp_Tools::color( const Quantity_Color& c )
{
        return QColor( (int)( c.Red() * 255 ), (int)( c.Green() * 255 ), (int)( c.Blue() * 255 ) );
}

/*!
  Gets message on exception \a S_ex.
*/
QString SalomeApp_Tools::ExceptionToString( const SALOME::SALOME_Exception& S_ex )
{
  QString message;
  
  switch ( S_ex.details.type )
  {
  case SALOME::COMM:
  case SALOME::INTERNAL_ERROR:
    {
            message = QString( S_ex.details.text );
            QString source( S_ex.details.sourceFile );
            QString line;
            line.setNum( S_ex.details.lineNumber );
            message = message + " \n" + source + " : " + line;
      break;
    }
  case SALOME::BAD_PARAM:
    {
            message = QString( S_ex.details.text );
#ifdef _DEBUG_
            QString source( S_ex.details.sourceFile );
            QString line;
            line.setNum( S_ex.details.lineNumber );
            message = message + " \n" + source + " : " + line;
#endif
            break;
    }
  default:
    {
            message = QString( "SALOME CORBA Exception Type invalid" );
            QString source( S_ex.details.sourceFile );
            QString line;
            line.setNum( S_ex.details.lineNumber );
            message = message + " \n" + source + " : " + line;
            break;
    }
  }
  return message;
}

/*!
  Gets message box on exception \a S_ex.
*/
void SalomeApp_Tools::QtCatchCorbaException( const SALOME::SALOME_Exception& S_ex )
{
  QString message = ExceptionToString( S_ex );

  QString title;
  bool error = true;
  switch ( S_ex.details.type )
  {
  case SALOME::COMM:
  case SALOME::INTERNAL_ERROR:
    title = QObject::tr( "Engine Error" );
    break;
  case SALOME::BAD_PARAM:
    error = false;
    title = QObject::tr( "Engine Warning" );
          break;
  default:
    title = QObject::tr( "Internal SALOME Error" );
    break;
  }

  if ( error )
    SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                               title, message );
  else
    SUIT_MessageBox::warning( SUIT_Session::session()->activeApplication()->desktop(),
                              title, message );

}
