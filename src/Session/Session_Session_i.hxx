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
//  File   : Session_Session_i.hxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME

#ifndef _SESSION_SESSION_I_HXX_
#define _SESSION_SESSION_I_HXX_

#include "SALOME_Session.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Component)
#include CORBA_SERVER_HEADER(SALOME_Session)
class SALOME_NamingService;

class QMutex;
class QWaitCondition;

class SESSION_EXPORT SALOME_Session_i:  public virtual POA_SALOME::Session,
                         public virtual PortableServer::ServantBase
{
public:
  SALOME_Session_i(int argc, 
                   char ** argv, 
                   CORBA::ORB_ptr orb, 
                   PortableServer::POA_ptr poa, 
                   QMutex* GUIMutex,
                   QWaitCondition* GUILauncher);
  ~SALOME_Session_i();

  //! Launch Graphical User Interface
  void GetInterface();

  //! Return VISU component
  Engines::EngineComponent_ptr GetComponent(const char* theLibraryName);

  //! Stop the Session (must be idle): kill servant & server
  void StopSession();

  //! Get session state
  SALOME::StatSession GetStatSession();

  //! Shutdown session
  void Shutdown();

  //! Register the servant to Naming Service
  void NSregister();
  //! Unregister the servant from Naming Service
  void NSunregister();

  CORBA::Long GetActiveStudyId();

  void ping(){};
  CORBA::Long getPID();
  char* getHostname();

  //! Restors a visual state of the study at theSavePoint
  bool restoreVisualState(CORBA::Long theSavePoint);

  //! Get list of selected entities
  SALOME::StringSeq* getSelection();

  //! Emit a qt signal from the session GUI desktop window.
  void emitMessage(const char* theMessage);
  void emitMessageOneWay(const char* theMessage);

protected:

  //! Naming service interface
  SALOME_NamingService *_NS;

  int _argc ;
  char **_argv;
  CORBA::Boolean _isGUI ;
  QMutex* _GUIMutex ;
  QWaitCondition* _GUILauncher;
  int _runningStudies ;
  CORBA::ORB_var _orb;
  PortableServer::POA_var _poa;
  bool _isShuttingDown;
};

#endif
