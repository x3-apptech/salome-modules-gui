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
// Author : Adrien Bruneton (CEA)
//

#include "PVViewer_Core.h"
#include "PVViewer_LogWindowAdapter.h"
#include "PVViewer_GUIElements.h"
#include "PVViewer_Behaviors.h"
#include "PVViewer_Core.h"

#include <QApplication>
#include <QStringList>
#include <QDir>
#include <QMainWindow>

#include <string>

#include <pqOptions.h>
#include <pqSettings.h>
#include <pqPVApplicationCore.h>
#include <pqTabbedMultiViewWidget.h>
#include <pqParaViewMenuBuilders.h>
#include <pqActiveObjects.h>
#include <pqPipelineBrowserWidget.h>
#include <pqServerDisconnectReaction.h>


//---------- Static init -----------------
pqPVApplicationCore* PVViewer_Core::MyCoreApp = 0;
bool PVViewer_Core::ConfigLoaded = false;
PVViewer_Behaviors * PVViewer_Core::ParaviewBehaviors = NULL;

pqPVApplicationCore * PVViewer_Core::GetPVApplication()
{
  return MyCoreApp;
}

/*!
  \brief Static method, performs initialization of ParaView session.
  \param fullSetup whether to instanciate all behaviors or just the minimal ones.
  \return \c true if ParaView has been initialized successfully, otherwise false
*/
bool PVViewer_Core::ParaviewInitApp(QMainWindow * aDesktop, LogWindow * logWindow)
{
  if ( ! MyCoreApp) {
      // Obtain command-line arguments
      int argc = 0;
      char** argv = 0;
      QString aOptions = getenv("PARAVIEW_OPTIONS");
      QStringList aOptList = aOptions.split(":", QString::SkipEmptyParts);
      argv = new char*[aOptList.size() + 1];
      QStringList args = QApplication::arguments();
      argv[0] = (args.size() > 0)? strdup(args[0].toLatin1().constData()) : strdup("paravis");
      argc++;

      foreach (QString aStr, aOptList) {
        argv[argc] = strdup( aStr.toLatin1().constData() );
        argc++;
      }
      MyCoreApp = new pqPVApplicationCore (argc, argv);
      if (MyCoreApp->getOptions()->GetHelpSelected() ||
          MyCoreApp->getOptions()->GetUnknownArgument() ||
          MyCoreApp->getOptions()->GetErrorMessage() ||
          MyCoreApp->getOptions()->GetTellVersion()) {
          return false;
      }

      // Direct VTK log messages to our SALOME window - TODO: review this
      PVViewer_LogWindowAdapter * w = PVViewer_LogWindowAdapter::New();
      w->setLogWindow(logWindow);
      vtkOutputWindow::SetInstance(w);

      new pqTabbedMultiViewWidget(); // registers a "MULTIVIEW_WIDGET" on creation

      for (int i = 0; i < argc; i++)
        free(argv[i]);
      delete[] argv;
  }
   // Initialization of ParaView GUI widgets will be done when these widgets are
   // really needed.
   // PVViewer_GUIElements* inst = PVViewer_GUIElements::GetInstance(aDesktop);
   // inst->getPropertiesPanel();
   return true;
}

void PVViewer_Core::ParaviewInitBehaviors(bool fullSetup, QMainWindow* aDesktop)
{
  if (!ParaviewBehaviors)
      ParaviewBehaviors = new PVViewer_Behaviors(aDesktop);

  if(fullSetup)
    ParaviewBehaviors->instanciateAllBehaviors(aDesktop);
  else
    ParaviewBehaviors->instanciateMinimalBehaviors(aDesktop);
}

void PVViewer_Core::ParaviewLoadConfigurations(const QString & configPath, bool force)
{
  if (!ConfigLoaded || force)
    {
      if (!configPath.isNull()) {
          MyCoreApp->loadConfiguration(configPath + QDir::separator() + "ParaViewFilters.xml");
          MyCoreApp->loadConfiguration(configPath + QDir::separator() + "ParaViewSources.xml");
      }
      ConfigLoaded = true;
    }
}

void PVViewer_Core::ParaviewCleanup()
{
  // Disconnect from server
  pqServer* server = pqActiveObjects::instance().activeServer();
  if (server && server->isRemote())
    {
      pqServerDisconnectReaction::disconnectFromServer();
    }

  pqApplicationCore::instance()->settings()->sync();

  pqPVApplicationCore * app = GetPVApplication();
  // Schedule destruction of PVApplication singleton:
  if (app)
    app->deleteLater();
}

