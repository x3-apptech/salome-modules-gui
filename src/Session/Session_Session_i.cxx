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
//  File   : SALOME_Session_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME

#include "utilities.h"

#include "Session_Session_i.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_Event.h"
#include "SalomeApp_Engine_i.h"
#include "LightApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SALOME_ListIO.hxx"
#include "SUIT_Session.h"
#include "SUIT_Desktop.h"
#include "SUIT_Study.h"

#include "Basics_Utils.hxx"

#include <QMutex>
#include <QWaitCondition>

// Open CASCADE Includes
#include <OSD_SharedLibrary.hxx>
#include <OSD_Function.hxx>

#ifdef WIN32
# include <process.h>
#else
#include <unistd.h>
#endif

/*!
  constructor
*/
SALOME_Session_i::SALOME_Session_i(int argc,
                                   char ** argv,
                                   CORBA::ORB_ptr orb,
                                   PortableServer::POA_ptr poa,
                                   QMutex* GUIMutex,
                                   QWaitCondition* GUILauncher)
{
  _argc = argc ;
  _argv = argv ;
  _isGUI = false ;
  _runningStudies= 0 ;
  _orb = CORBA::ORB::_duplicate(orb) ;
  _poa = PortableServer::POA::_duplicate(poa) ;
  _GUIMutex = GUIMutex;
  _GUILauncher = GUILauncher;
  _NS = new SALOME_NamingService(_orb);
  _isShuttingDown = false;
  //MESSAGE("constructor end");
}

/*!
  returns Visu component
*/
Engines::EngineComponent_ptr SALOME_Session_i::GetComponent(const char* theLibraryName)
{
  typedef Engines::EngineComponent_ptr TGetImpl(CORBA::ORB_ptr,
                                                PortableServer::POA_ptr,
                                                SALOME_NamingService*,QMutex*);
  OSD_SharedLibrary aSharedLibrary(const_cast<char*>(theLibraryName));
  if (aSharedLibrary.DlOpen(OSD_RTLD_LAZY)) {
    if (OSD_Function anOSDFun = aSharedLibrary.DlSymb("GetImpl"))
      return ((TGetImpl (*)) anOSDFun)(_orb,_poa,_NS,_GUIMutex);
  }
  CORBA::Object_var obj = SalomeApp_Engine_i::EngineForComponent(theLibraryName, true);
  if (!CORBA::is_nil(obj)){
    Engines::EngineComponent_var anEngine = Engines::EngineComponent::_narrow(obj);
    return anEngine._retn();
  }
  return Engines::EngineComponent::_nil();
}

/*!
  destructor
*/
SALOME_Session_i::~SALOME_Session_i()
{
  delete _NS;
  //MESSAGE("destructor end");
}

/*!
  tries to find the Corba Naming Service and to register the session,
  gives naming service interface to _IAPPThread
*/
void SALOME_Session_i::NSregister()
{
  CORBA::Object_var obref=_this();
  SALOME::Session_var pSession = SALOME::Session::_narrow(obref);
  try
    {
      _NS->Register(pSession, "/Kernel/Session");
    }
  catch (ServiceUnreachable&)
    {
      INFOS("Caught exception: Naming Service Unreachable");
      exit(1) ;
    }
  catch (...)
    {
      INFOS("Caught unknown exception from Naming Service");
    }
  //MESSAGE("Session registered in Naming Service");
}

/*!
  Unregister session server from CORBA Naming Service
*/
void SALOME_Session_i::NSunregister()
{
  try
    {
      _NS->Destroy_Name("/Kernel/Session");
    }
  catch (ServiceUnreachable&)
    {
      INFOS("Caught exception: Naming Service Unreachable");
    }
  catch (...)
    {
      INFOS("Caught unknown exception from Naming Service");
    }
}

/*!
  Launches the GUI if there is none.
  The Corba method is oneway (corba client does'nt wait for GUI completion)
*/
void SALOME_Session_i::GetInterface()
{
  _GUIMutex->lock();
  _GUIMutex->unlock();
  if ( !SUIT_Session::session() )
  {
    _GUILauncher->wakeAll();
    MESSAGE("SALOME_Session_i::GetInterface() called, starting GUI...");
  }
}

/*!
  Kills the session if there are no active studies nore GUI
*/
class CloseEvent : public SALOME_Event
{
public:
  virtual void Execute() {
    if ( SUIT_Session::session() )
      SUIT_Session::session()->closeSession( SUIT_Session::DONT_SAVE );
  }
};

/*!
  Stop session (close all GUI windows)
*/
void SALOME_Session_i::StopSession()
{
  _GUIMutex->lock();
  _GUIMutex->unlock();
  if ( SUIT_Session::session() ) {
    ProcessVoidEvent( new CloseEvent() );
  }
}

//! Shutdown session
void SALOME_Session_i::Shutdown()
{
  _GUIMutex->lock();
  bool isBeingShuttingDown = _isShuttingDown;
  _isShuttingDown = true;
  _GUIMutex->unlock();
  if ( !isBeingShuttingDown ) {
    if ( SUIT_Session::session() ) {
      ProcessVoidEvent( new CloseEvent() );
    }
    else {
      _GUILauncher->wakeAll();
    }
  }
}

/*!
  Send a SALOME::StatSession structure (see idl) to the client
  (number of running studies and presence of GUI)
*/
/*class QtLock
{
public:
  QtLock() { if ( qApp ) qApp->lock(); }
  ~QtLock() { if ( qApp ) qApp->unlock(); }
};*/


SALOME::StatSession SALOME_Session_i::GetStatSession()
{
  // update Session state
  _GUIMutex->lock();

  _runningStudies = 0;
  {
    //QtLock lock;
    _isGUI = SUIT_Session::session();
    if ( _isGUI && SUIT_Session::session()->activeApplication() )
      _runningStudies = SUIT_Session::session()->activeApplication()->getNbStudies();
  }

  // getting stat info
  SALOME::StatSession_var myStats = new SALOME::StatSession ;
  if (_runningStudies)
    myStats->state = SALOME::running ;
  else if (_isShuttingDown)
    myStats->state = SALOME::shutdown ;
  else
    myStats->state = SALOME::asleep ;
  myStats->runningStudies = _runningStudies ;
  myStats->activeGUI = _isGUI ;

  _GUIMutex->unlock();

  return myStats._retn() ;
}

CORBA::Long SALOME_Session_i::GetActiveStudyId()
{
  long aStudyId=-1;
  if ( SUIT_Session::session() && SUIT_Session::session()->activeApplication() ) {
    if ( SUIT_Session::session()->activeApplication()->activeStudy() ) // mkr : IPAL12128
      aStudyId = SUIT_Session::session()->activeApplication()->activeStudy()->id();
  }
  return aStudyId;
}

CORBA::Long SALOME_Session_i::getPID() {
  return (CORBA::Long)
#ifndef WIN32
    getpid();
#else
    _getpid();
#endif
}

char* SALOME_Session_i::getHostname()
{
  std::string aHostName = Kernel_Utils::GetHostname();
  return CORBA::string_dup( aHostName.data() );
}

bool SALOME_Session_i::restoreVisualState(CORBA::Long theSavePoint)
{
  class TEvent: public SALOME_Event {
    int _savePoint;
  public:
    TEvent(int savePoint) { _savePoint = savePoint; }
    virtual void Execute() {
      SUIT_Study* study = SUIT_Session::session()->activeApplication()->activeStudy();
      if ( study ) {
        study->restoreState(_savePoint);
      }
    }
  };
  
  if(SUIT_Session::session() && SUIT_Session::session()->activeApplication() ) {
    SUIT_Study* study = SUIT_Session::session()->activeApplication()->activeStudy();
    if(!study) SUIT_Session::session()->activeApplication()->createEmptyStudy();      
    ProcessVoidEvent( new TEvent(theSavePoint) );
    return true;
  }
 
  return false;
}

void SALOME_Session_i::emitMessage(const char* theMessage)
{
  class TEvent: public SALOME_Event {
  public:
    TEvent(const char * msg) {
      _msg = msg;
    }
    virtual void Execute() {
      SUIT_Session::session()->activeApplication()->desktop()->emitMessage(_msg);
    }
  private:
    const char* _msg;
  };
  if ( SUIT_Session::session() ) {
    if ( SUIT_Session::session()->activeApplication() ) {
      if ( SUIT_Session::session()->activeApplication()->desktop() ) {
        ProcessVoidEvent( new TEvent(theMessage) );
      }
      else {
	MESSAGE("try to emit message '"<<theMessage<<"' but there is no desktop");
      }
    }
    else {
      MESSAGE("try to emit message '"<<theMessage<<"' but there is no application");
    }
  }
  else {
    MESSAGE("try to emit message '"<<theMessage<<"' but there is no session");
  }
}

void SALOME_Session_i::emitMessageOneWay(const char* theMessage)
{
  emitMessage(theMessage);
}

SALOME::StringSeq* SALOME_Session_i::getSelection()
{
  SALOME::StringSeq_var selection = new SALOME::StringSeq;
  _GUIMutex->lock();
  if ( SUIT_Session::session() ) {
    LightApp_Application* app = dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
    if ( app ) {
      LightApp_SelectionMgr* selMgr = dynamic_cast<LightApp_SelectionMgr*>( app->selectionMgr() );
      SALOME_ListIO selected;
      selMgr->selectedObjects( selected );
      selection->length( selected.Extent() );
      int nbSel = 0;
      for ( SALOME_ListIteratorOfListIO it( selected ); it.More(); it.Next() ) {
        Handle( SALOME_InteractiveObject ) io = it.Value();
        if ( io->hasEntry() )
          selection[nbSel++] = CORBA::string_dup( io->getEntry() );
      }
      selection->length( nbSel );
    }
  }
  _GUIMutex->unlock();
  return selection._retn();
}
