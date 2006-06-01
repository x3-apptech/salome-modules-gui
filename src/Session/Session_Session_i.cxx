//  SALOME Session : implementation of Session.idl
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SALOME_Session_i.cxx
//  Author : Paul RASCLE, EDF
//  Module : SALOME
//  $Header$

#include "utilities.h"

#include "Session_Session_i.hxx"

#include "SALOME_NamingService.hxx"
#include "SALOME_Event.hxx"

#include "SUIT_Session.h"
#include "SUIT_Application.h"

#include <qapplication.h>

// Open CASCADE Includes
#include <OSD_SharedLibrary.hxx>
#include <OSD_LoadMode.hxx>
#include <OSD_Function.hxx>

using namespace std;

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
  _isGUI = FALSE ;
  _runningStudies= 0 ;
  _orb = CORBA::ORB::_duplicate(orb) ;
  _poa = PortableServer::POA::_duplicate(poa) ;
  _GUIMutex = GUIMutex;
  _GUILauncher = GUILauncher;
  //MESSAGE("constructor end");
}

/*!
  returns Visu component
*/
Engines::Component_ptr SALOME_Session_i::GetComponent(const char* theLibraryName)
{
  typedef Engines::Component_ptr TGetImpl(CORBA::ORB_ptr,
					  PortableServer::POA_ptr,
					  SALOME_NamingService*,QMutex*);
  OSD_SharedLibrary  aSharedLibrary(const_cast<char*>(theLibraryName));
  if(aSharedLibrary.DlOpen(OSD_RTLD_LAZY))
    if(OSD_Function anOSDFun = aSharedLibrary.DlSymb("GetImpl"))
      return ((TGetImpl (*)) anOSDFun)(_orb,_poa,_NS,_GUIMutex);
  return Engines::Component::_nil();
}

/*!
  destructor
*/
SALOME_Session_i::~SALOME_Session_i()
{
  //MESSAGE("destructor end");
}

/*!
  tries to find the Corba Naming Service and to register the session,
  gives naming service interface to _IAPPThread
*/
void SALOME_Session_i::NSregister()
{
  SALOME::Session_ptr pSession = SALOME::Session::_narrow(_this());
  try
    {
      _NS = new SALOME_NamingService(_orb);
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
    MESSAGE("SALOME_Session_i::GetInterface() called, starting GUI...")
      }
}

/*!
  Kills the session if there are no active studies nore GUI
*/
class CloseEvent : public SALOME_Event
{
public:
  virtual void Execute() {
    SUIT_Session* session = SUIT_Session::session();
    session->closeSession( SUIT_Session::DONT_SAVE );
    //if ( SUIT_Application::getDesktop() )
    //  QAD_Application::getDesktop()->closeDesktop( true );
  }
};

/*!
  Processes event to close session
*/
void SALOME_Session_i::StopSession()
{
  ProcessVoidEvent( new CloseEvent() );
}

/*!
  Send a SALOME::StatSession structure (see idl) to the client
  (number of running studies and presence of GUI)
*/
class QtLock
{
public:
  QtLock() { if ( qApp ) qApp->lock(); }
  ~QtLock() { if ( qApp ) qApp->unlock(); }
};


SALOME::StatSession SALOME_Session_i::GetStatSession()
{
  // update Session state
  _GUIMutex->lock();

  _runningStudies = 0;
  {
    QtLock lock;
    _isGUI = SUIT_Session::session();
    if ( _isGUI && SUIT_Session::session()->activeApplication() )
      _runningStudies = SUIT_Session::session()->activeApplication()->getNbStudies();
  }

  _GUIMutex->unlock();

  // getting stat info
  SALOME::StatSession_var myStats = new SALOME::StatSession ;
  if (_runningStudies)
    myStats->state = SALOME::running ;
  else
    myStats->state = SALOME::asleep ;
  myStats->runningStudies = _runningStudies ;
  myStats->activeGUI = _isGUI ;
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
