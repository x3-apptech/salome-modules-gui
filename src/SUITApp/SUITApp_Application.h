// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef SUITAPP_APPLICATION_H
#define SUITAPP_APPLICATION_H

#ifdef ENABLE_TESTRECORDER
  #include <TestApplication.h>
#else
  #include <QApplication>
#endif

class SUIT_ExceptionHandler;

#ifdef WNT
#  if defined SUITAPP_EXPORTS || defined SUITApp_EXPORTS
#    define SUITAPP_EXPORT __declspec(dllexport)
#  else
#   define SUITAPP_EXPORT __declspec(dllimport)
#  endif
#else
#  define SUITAPP_EXPORT
#endif

#ifdef ENABLE_TESTRECORDER
  class SUITAPP_EXPORT SUITApp_Application : public TestApplication
#else
  class SUITAPP_EXPORT SUITApp_Application : public QApplication
#endif
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
