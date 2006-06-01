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
//  File   : CAF_Tools.cxx
//  Author : UI team

#include "CAF_Tools.h"

#include <string.h>
#include <TCollection_ExtendedString.hxx>

/*!
    Converts TCollection_ExtendedString 'src' to Qt string. [ static ]
*/
QString CAF_Tools::toQString ( const TCollection_ExtendedString& src )
{
  return QString( (const QChar*)src.ToExtString(), src.Length() );
}

/*!
    Converts TCollection_AsciiString'src' to Qt string. [ static ]
*/
QString CAF_Tools::toQString( const TCollection_AsciiString& src )
{
  return QString( src.ToCString() );
}

/*!
    Converts Qt string to TCollection_AsciiString. [ static ]
*/
TCollection_AsciiString CAF_Tools::toAsciiString( const QString& src )
{
  TCollection_AsciiString res;
  if ( !src.isEmpty() )
    res = TCollection_AsciiString( (char*)src.latin1() );
  return res;
}

/*!
    Converts Qt string to TCollection_ExtendedString. [ static ]
*/
TCollection_ExtendedString CAF_Tools::toExtString ( const QString& src )
{
  TCollection_ExtendedString result;
  for ( int i = 0; i < (int)src.length(); i++ )
    result.Insert( i + 1, src[ i ].unicode() );
  return result;
}

/*!
    Converts Qt color to OCC color
*/
Quantity_Color CAF_Tools::color( const QColor& c )
{
  Quantity_Color aColor;
  if ( c.isValid() )
    aColor = Quantity_Color( c.red()   / 255., c.green() / 255.,
                             c.blue()  / 255., Quantity_TOC_RGB );
  return aColor;
}

/*!
    Converts OCC color to Qt color
*/
QColor CAF_Tools::color( const Quantity_Color& c )
{
  return QColor ( int( c.Red()   * 255 ),
                  int( c.Green() * 255 ),
                  int( c.Blue()  * 255 ) );
}
