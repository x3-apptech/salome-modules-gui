#ifndef SALOMEAPP_TOOLS_H
#define SALOMEAPP_TOOLS_H

#include "SalomeApp.h"

#include <SUIT_Tools.h>

#include <qcolor.h>
#include <qstring.h>

#include <Quantity_Color.hxx>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Exception)

class SALOMEAPP_EXPORT SalomeApp_Tools : public SUIT_Tools
{
public:
  Quantity_Color  color( const QColor& );
  QColor          color( const Quantity_Color& );

  static QString  ExceptionToString( const SALOME::SALOME_Exception& );
  static void     QtCatchCorbaException( const SALOME::SALOME_Exception& );
};

#endif
