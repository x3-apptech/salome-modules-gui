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
// File:      Session_ServerCheck.hxx
// Author:    Vadim SANDLER

#if !defined(SESSION_SERVERCHECK_HXX)
#define SESSION_SERVERCHECK_HXX

#include <SALOME_Session.hxx>

#include <qthread.h> 
#include <qapplication.h> 

class QMutex;
class QWaitCondition;

/*!
  Class Session_ServerCheck : check SALOME servers
*/
class SESSION_EXPORT Session_ServerCheck : public QThread
{
public:
  // constructor
  Session_ServerCheck( QMutex*, QWaitCondition* );
  // destructor
  virtual ~Session_ServerCheck();

  // thread loop
  virtual void run() ;

private:
  QMutex*         myMutex;             // splash mutex
  QWaitCondition* myWC;                // splash wait condition

  bool            myCheckCppContainer; // flag : check C++ container ?
  bool            myCheckPyContainer;  // flag : check Python container ?
  bool            myCheckSVContainer;  // flag : check supervision container ?
  int             myAttempts;          // number of checks attemtps to get response from server
  int             myDelay;             // delay between two attempts in microseconds
};

#endif
