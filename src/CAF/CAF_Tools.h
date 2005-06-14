//  File   : CAF_Tools.h
//  Author : UI team

#ifndef CAF_TOOLS_H
#define CAF_TOOLS_H

#include <CAF.h>

#include <qnamespace.h>
#include <qstring.h>
#include <TCollection_ExtendedString.hxx>

class CAF_EXPORT CAF_Tools : public Qt
{
public:
  /* converting strings */
  static QString                    toQString ( const TCollection_ExtendedString& );
  static TCollection_ExtendedString toExtString ( const QString& );
};

#endif // CAF_TOOLS_H



