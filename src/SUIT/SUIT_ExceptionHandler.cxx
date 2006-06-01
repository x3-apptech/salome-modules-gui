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
#include "SUIT_ExceptionHandler.h"

#include "SUIT_MessageBox.h"

#include <qapplication.h>

/*!\class SUIT_ExceptionHandler
 * Show exception message on error handler.
 */

/*!
  Checks: is internal handle on object \a o?
*/
bool SUIT_ExceptionHandler::handle( QObject* o, QEvent* e )
{
  return internalHandle( o, e );
}

/*!
  Checks: is internal handle on object \a o?
*/
bool SUIT_ExceptionHandler::internalHandle( QObject* o, QEvent* e )
{
  return qApp ? qApp->QApplication::notify( o, e ) : false;
}

/*!
  Show error message \a mgs, if application is not null.
*/
void SUIT_ExceptionHandler::showMessage( const QString& title, const QString& msg )
{
  if ( !qApp )
    return;
  
  while ( QApplication::overrideCursor() )
    QApplication::restoreOverrideCursor();
  
  SUIT_MessageBox::error1( qApp->mainWidget(), title, msg, "OK" );
}
