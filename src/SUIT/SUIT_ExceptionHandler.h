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

#ifndef SUIT_EXCEPTIONHANDLER_H
#define SUIT_EXCEPTIONHANDLER_H

#include "SUIT.h"
#include <QList>

class QEvent;
class QObject;
class QString;

typedef void (*CleanUpFunction)();
typedef QList<CleanUpFunction> CleanUpFuncList;

class SUIT_EXPORT SUIT_ExceptionHandler
{
public:
  static void  addCleanUpRoutine(CleanUpFunction p);
  static void  removeCleanUpRoutine(CleanUpFunction p);

  virtual bool handle( QObject*, QEvent* );

protected:
  bool         internalHandle( QObject*, QEvent* );
  void         showMessage( const QString&, const QString& );

private:
  void         cleanUp();

  static CleanUpFuncList myCleanUpFunctions;
};

extern "C"
{
  typedef SUIT_ExceptionHandler* (*APP_GET_HANDLER_FUNC)();
}

#define APP_GET_HANDLER_NAME "getExceptionHandler"

#endif
