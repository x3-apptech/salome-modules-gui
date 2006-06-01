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
#include "SUIT_Tools.h"

#include <qdir.h>

#include <stdio.h>
#include <stdarg.h>

/*!
  Traces output to log-file.
  If log is NULL, 'Salome_trace' file is created in temp directory.
  Log file is written in 'append' mode.
*/
void SUIT_Tools::trace( const char* lpszLog, const char* lpszFormat, ... )
{
  QString tmpPath = tmpDir();
  if ( !tmpPath.isEmpty() )
    tmpPath += QDir::separator();

  tmpPath += QString( "Salome_trace" );

  FILE*	pStream;
  pStream = fopen( lpszLog ? lpszLog : tmpPath.latin1(), "a" );
  if ( pStream ) 
  {	
    va_list argptr;
    va_start( argptr, lpszFormat );
    fprintf( pStream, "- Trace %s [%d] : %s", __FILE__, __LINE__, lpszFormat );
    va_end( argptr );

    fclose( pStream );
  }
}

/*! 
    Creates a rect with TopLeft = ( min(x1,x2), min(y1,y2) )
    and BottomRight = ( TopLeft + (x2-x1)(y2-y1) )    
*/	
QRect SUIT_Tools::makeRect( const int x1, const int y1, const int x2, const int y2 )
{  
  return QRect( QMIN( x1, x2 ), QMIN( y1, y2 ), QABS( x2 - x1 ), QABS( y2 - y1 ) );
}

/*!
  Creates font from string description
*/
QFont SUIT_Tools::stringToFont( const QString& fontDescription )
{
  QFont font;
  if ( fontDescription.stripWhiteSpace().isEmpty() || !font.fromString( fontDescription ) )
    font = QFont( "Courier", 11 );
  return font;
}

/*!
  Creates font's string description
*/
QString SUIT_Tools::fontToString( const QFont& font )
{
  return font.toString();
}

/*!
  Center widget 'src' relative to widget 'ref'.
*/
void SUIT_Tools::centerWidget( QWidget* src, const QWidget* ref )
{
  SUIT_Tools::alignWidget( src, ref, Qt::AlignCenter );
}
