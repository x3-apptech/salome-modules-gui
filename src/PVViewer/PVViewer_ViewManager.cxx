// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author: Adrien Bruneton (CEA)

#include "PVViewer_ViewManager.h"
#include "PVViewer_ViewWindow.h"
#include "PVViewer_ViewModel.h"
#include "PVViewer_GUIElements.h"
#include "PVViewer_Core.h"
#include "PVServer_ServiceWrapper.h"

#include <utilities.h>

#include <LogWindow.h>
#include <SUIT_Desktop.h>
#include <SUIT_Study.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <pqServer.h>
#include <pqServerConnectReaction.h>
#include <pqActiveObjects.h>

/*!
  Constructor
*/
PVViewer_ViewManager::PVViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* desk, LogWindow * logWindow )
: SUIT_ViewManager( study, desk, new PVViewer_Viewer() ),
  desktop(desk)
{
  MESSAGE("PVViewer - view manager created ...")
  setTitle( tr( "PARAVIEW_VIEW_TITLE" ) );

  // Initialize minimal paraview stuff (if not already done)
  PVViewer_Core::ParaviewInitApp(desk, logWindow);

  connect( desk, SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
           this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );
}


PVServer_ServiceWrapper * PVViewer_ViewManager::GetService()
{
  return PVServer_ServiceWrapper::GetInstance();
}

QString PVViewer_ViewManager::GetPVConfigPath()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  return resMgr->stringValue("resources", "PVViewer", QString());
}

bool PVViewer_ViewManager::ConnectToExternalPVServer(QMainWindow* aDesktop)
{
  SUIT_ResourceMgr* aResourceMgr = SUIT_Session::session()->resourceMgr();
  bool noConnect = aResourceMgr->booleanValue( "PARAVIS", "no_ext_pv_server", false );
  if (noConnect)
    return true;

  pqServer* server = pqActiveObjects::instance().activeServer();
  if (server && server->isRemote())
    {
      // Already connected to an external server, do nothing
      MESSAGE("connectToExternalPVServer(): Already connected to an external PVServer, won't reconnect.");
      return false;
    }

  if (GetService()->GetGUIConnected())
    {
      // Should never be there as the above should already tell us that we are connected.
      std::stringstream msg2;
      msg2 << "Internal error while connecting to the pvserver.";
      msg2 << "ParaView doesn't see a connection, but PARAVIS engine tells us there is already one!" << std::endl;
      qWarning(msg2.str().c_str());  // will go to the ParaView console (see ParavisMessageOutput below)
      SUIT_MessageBox::warning( aDesktop,
                                      QString("Error connecting to PVServer"), QString(msg2.str().c_str()));
      return false;
    }

  std::stringstream msg;

  // Try to connect to the external PVServer - gives priority to an externally specified URL:
  QString serverUrlEnv = getenv("PARAVIEW_PVSERVER_URL");
  std::string serverUrl;
  if (!serverUrlEnv.isEmpty())
    serverUrl = serverUrlEnv.toStdString();
  else
    {
      // Get the URL from the engine (possibly starting the pvserver)
      serverUrl = GetService()->FindOrStartPVServer(0);  // take the first free port
    }

  msg << "connectToExternalPVServer(): Trying to connect to the external PVServer '" << serverUrl << "' ...";
  MESSAGE(msg.str());

  if (!pqServerConnectReaction::connectToServer(pqServerResource(serverUrl.c_str())))
    {
      std::stringstream msg2;
      msg2 << "Error while connecting to the requested pvserver '" << serverUrl;
      msg2 << "'. Might use default built-in connection instead!" << std::endl;
      qWarning(msg2.str().c_str());  // will go to the ParaView console (see ParavisMessageOutput below)
      SUIT_MessageBox::warning( aDesktop,
                                QString("Error connecting to PVServer"), QString(msg2.str().c_str()));
      return false;
    }
  else
    {
      MESSAGE("connectToExternalPVServer(): Connected!");
      GetService()->SetGUIConnected(true);
    }
  return true;
}


/*!Enable toolbars if view \a view is ParaView viewer and disable otherwise.
*/
void PVViewer_ViewManager::onWindowActivated(SUIT_ViewWindow* view)
{
  if (view)
    {
    PVViewer_ViewWindow* pvWindow = dynamic_cast<PVViewer_ViewWindow*>(view);
    PVViewer_GUIElements * guiElements = PVViewer_GUIElements::GetInstance(desktop);
    guiElements->setToolBarEnabled(pvWindow!=0);
    }
}
