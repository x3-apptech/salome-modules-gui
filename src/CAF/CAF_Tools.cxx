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
