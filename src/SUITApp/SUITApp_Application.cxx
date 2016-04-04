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

#include "SUITApp_Application.h"

#include <SUIT_Tools.h>
#include <SUIT_ExceptionHandler.h>

#include <Qtx.h>

#include <QDir>

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <unistd.h>
#endif

/*!
  Constructor
*/
SUITApp_Application::SUITApp_Application( int& argc, char** argv, SUIT_ExceptionHandler* hand )
// TODO (QT5 PORTING) Below is a temporary solution, to allow compiling with Qt 5
#if !defined(WIN32) && (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  // san: Opening an X display and choosing a visual most suitable for 3D visualization
  // in order to make SALOME viewers work with non-native X servers
  : QApplication( (Display*)Qtx::getDisplay(), argc, argv, Qtx::getVisual() ),
#else
  : QApplication( argc, argv ), 
#endif
myExceptHandler( hand )
{
}

/*!
  Constructor
*/
// TODO (QT5 PORTING) Below is a temporary solution, to allow compiling with Qt 5
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
SUITApp_Application::SUITApp_Application( int& argc, char** argv, Type type, SUIT_ExceptionHandler* hand )
  : QApplication( argc, argv, type ),
myExceptHandler( hand )
{
}
#endif
/*!
  Sends event to receiver
  \return the value that is returned from the receiver's event handler
  \param e - event
  \param receiver - receiver
*/
bool SUITApp_Application::notify( QObject* receiver, QEvent* e )
{
  return myExceptHandler ? myExceptHandler->handle( receiver, e ) :
                           QApplication::notify( receiver, e );
}

/*!
  Changes exception handler
  \param hand - new handler
*/
void SUITApp_Application::setHandler( SUIT_ExceptionHandler* hand )
{
  myExceptHandler = hand;
}

/*!
  \return exception handler
*/
SUIT_ExceptionHandler* SUITApp_Application::handler() const
{
  return myExceptHandler;
}
