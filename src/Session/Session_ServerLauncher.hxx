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

//  SALOME Session : implementation of Session_ServerLauncher.hxx
//  File   : Session_ServerLauncher.hxx
//  Author : Paul RASCLE, EDF

#ifndef _SESSION_SERVERLAUNCHER_HXX_
#define _SESSION_SERVERLAUNCHER_HXX_

#include "SALOME_Session.hxx"

#include <omniORB4/CORBA.h> 
#include <list>
#include <vector>
#include <string>
#include <QThread>

class SESSION_EXPORT ServArg
  {
  public:
    int _servType;
    int _firstArg;
    int _lastArg;
    inline ServArg(int servType=0, int firstArg=0, int lastArg=0);
};

inline ServArg::ServArg(int servType, int firstArg, int lastArg):
  _servType(servType),_firstArg(firstArg),_lastArg(lastArg)
{}

class Session_ServerThread;

class QMutex;
class QWaitCondition;

class SESSION_EXPORT Session_ServerLauncher: public QThread
{
public:
  Session_ServerLauncher();
  Session_ServerLauncher(int argc,
                         char ** argv, 
                         CORBA::ORB_ptr orb, 
                         PortableServer::POA_ptr poa,
                         QMutex *GUIMutex,
                         QWaitCondition *ServerLaunch,
                         QMutex *SessionMutex,
                         QWaitCondition *SessionStarted);
  virtual ~Session_ServerLauncher();
  void run();
  void ShutdownAll();
  void KillAll();

protected:
  void CheckArgs();
  void ActivateAll();

private:
  int                              _argc;
  char**                           _argv;
  CORBA::ORB_var                    _orb;
  PortableServer::POA_var          _root_poa;
  QMutex*                          _GUIMutex;
  QWaitCondition*                  _ServerLaunch;
  QMutex*                          _SessionMutex;
  QWaitCondition*                  _SessionStarted;
  std::list<ServArg>               _argServToLaunch;
  std::vector<std::string>         _argCopy;
  std::list<Session_ServerThread*> _serverThreads;
};

#endif

