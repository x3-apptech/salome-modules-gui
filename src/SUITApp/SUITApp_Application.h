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
#ifndef SUITAPP_APPLICATION_H
#define SUITAPP_APPLICATION_H

#include <qapplication.h>

class SUIT_ExceptionHandler;

class SUITApp_Application : public QApplication
{
  Q_OBJECT

public:
  SUITApp_Application( int& argc, char** argv, SUIT_ExceptionHandler* = 0 );
  SUITApp_Application( int& argc, char** argv, Type type, SUIT_ExceptionHandler* = 0 );

  virtual bool notify( QObject* receiver, QEvent* e );

	SUIT_ExceptionHandler* handler() const;
	void                   setHandler( SUIT_ExceptionHandler* );

private:
  SUIT_ExceptionHandler* myExceptHandler;
};

#endif
