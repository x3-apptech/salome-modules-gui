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
// See http://www.salome-platform.org/
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
    Converts Qt string to TCollection_ExtendedString. [ static ]
*/
TCollection_ExtendedString CAF_Tools::toExtString ( const QString& src )
{
    TCollection_ExtendedString result;
    for ( int i = 0; i < (int)src.length(); i++ )
        result.Insert( i + 1, src[ i ].unicode() );
    return result;
}
