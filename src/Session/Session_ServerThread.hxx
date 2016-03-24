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

//  SALOME Session : implementation of Session_ServerThread.hxx
//  File   : Session_ServerThread.hxx
//  Author : Paul RASCLE, EDF

#ifndef _SESSION_SERVERTHREAD_HXX_
#define _SESSION_SERVERTHREAD_HXX_

#include "SALOME_Session.hxx"

#include <omniORB4/CORBA.h> 
#include <string>

void WaitForServerReadiness(std::string serverName);

class SALOME_NamingService;
class Engines_Container_i;

class SESSION_EXPORT Session_ServerThread
{
public:
  static const int NB_SRV_TYP;
  static const char* _serverTypes[];

  Session_ServerThread();
  Session_ServerThread(int argc,
                       char ** argv, 
                       CORBA::ORB_ptr orb, 
                       PortableServer::POA_ptr poa);
  virtual ~Session_ServerThread();
  void         Init();
  void         Shutdown();
protected:
  void         ActivateModuleCatalog   ( int argc, char ** argv );
  void         ActivateSALOMEDS        ( int argc, char ** argv );
  void         ActivateRegistry        ( int argc, char ** argv );
  void         ActivateContainer       ( int argc, char ** argv );
  virtual void ActivateSession         ( int argc, char ** argv );
  void         ActivateEngine          ( int argc, char ** argv );
  void         ActivateContainerManager( int argc, char ** argv );
protected:
  int                     _argc;
  char **                 _argv;
  int                     _servType;
  CORBA::ORB_var          _orb;
  PortableServer::POA_var _root_poa;
  SALOME_NamingService *  _NS;
  Engines_Container_i*    _container;
};

class QMutex;
class QWaitCondition;

class SESSION_EXPORT Session_SessionThread : public Session_ServerThread
{
public:
  Session_SessionThread() {}
  Session_SessionThread(int argc,
			char** argv, 
			CORBA::ORB_ptr orb, 
			PortableServer::POA_ptr poa,
			QMutex* GUIMutex,
			QWaitCondition* GUILauncher);
  virtual ~Session_SessionThread();  

protected:
  virtual void ActivateSession       ( int argc, char ** argv );
private:
  QMutex*                 _GUIMutex;
  QWaitCondition*         _GUILauncher;
};

#endif

