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
#include "SUITApp_Application.h"

#include "SUIT_MessageBox.h"
#include "SUIT_ExceptionHandler.h"

#include <qdir.h>
#include <qfileinfo.h>

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
: QApplication( argc, argv ),
myExceptHandler( hand )
{
  QString path = QFileInfo( argv[0] ).dirPath() + QDir::separator() + "../../resources";
  path = QDir::convertSeparators( QDir( path ).canonicalPath() );

  QTranslator* strTbl = new QTranslator( 0 );
  if ( strTbl->load( "SUITApp_msg_en.po", path  ) )
    installTranslator( strTbl );
  else
    delete strTbl;
}

/*!
  Constructor
*/
SUITApp_Application::SUITApp_Application( int& argc, char** argv, Type type, SUIT_ExceptionHandler* hand )
: QApplication( argc, argv, type ),
myExceptHandler( hand )
{
    QTranslator* strTbl = new QTranslator( 0 );
    strTbl->load( "resources\\SUITApp_msg_en.po" );
    installTranslator( strTbl );
}

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
