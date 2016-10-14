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

#include "PVViewer_Behaviors.h"

#include <QMainWindow>

#include <pqInterfaceTracker.h>
#include <pqApplicationCore.h>
#include <pqPluginManager.h>
#include <pqStandardPropertyWidgetInterface.h>
#include <pqStandardViewFrameActionsImplementation.h>
#include <pqPropertiesPanel.h>

#include <pqAlwaysConnectedBehavior.h>
#include <pqAutoLoadPluginXMLBehavior.h>
#include <pqCommandLineOptionsBehavior.h>
#include <pqCrashRecoveryBehavior.h>
#include <pqDataTimeStepBehavior.h>
#include <pqDefaultViewBehavior.h>
#include <pqObjectPickingBehavior.h>
#include <pqPersistentMainWindowStateBehavior.h>
#include <pqPipelineContextMenuBehavior.h>
#include <pqPluginActionGroupBehavior.h>
#include <pqPluginDockWidgetsBehavior.h>
#include <pqSpreadSheetVisibilityBehavior.h>
#include <pqUndoRedoBehavior.h>
#include <pqViewStreamingBehavior.h>
#include <pqCollaborationBehavior.h>
#include <pqVerifyRequiredPluginBehavior.h>
#include <pqPluginSettingsBehavior.h>
#include <pqFixPathsInStateFilesBehavior.h>
#include <pqApplyBehavior.h>

#include <pqPropertiesPanel.h>

int PVViewer_Behaviors::BehaviorLoadingLevel = 0;

PVViewer_Behaviors::PVViewer_Behaviors(QMainWindow * parent)
  : QObject(parent)
{
}

/**! Instanciate minimal ParaView behaviors needed when using an instance of PVViewer.
 * This method should be updated at each new version of ParaView with what is found in
 *    Qt/ApplicationComponents/pqParaViewBehaviors.cxx
 */
void PVViewer_Behaviors::instanciateMinimalBehaviors(QMainWindow * desk)
{
  if (BehaviorLoadingLevel < 1)
    {
      // Register ParaView interfaces.
      pqInterfaceTracker* pgm = pqApplicationCore::instance()->interfaceTracker();

      // Register standard types of property widgets.
      pgm->addInterface(new pqStandardPropertyWidgetInterface(pgm));
      // Register standard types of view-frame actions.
      pgm->addInterface(new pqStandardViewFrameActionsImplementation(pgm));

      // Load plugins distributed with application.
      pqApplicationCore::instance()->loadDistributedPlugins();

      new pqDefaultViewBehavior(this);  // shows a 3D view as soon as a server connection is made
      new pqAlwaysConnectedBehavior(this);  // client always connected to a server
      new pqVerifyRequiredPluginBehavior(this);
      new pqPluginSettingsBehavior(this);
      new pqFixPathsInStateFilesBehavior(this);
      new pqCrashRecoveryBehavior(this);
      new pqCommandLineOptionsBehavior(this);

      BehaviorLoadingLevel = 1;
    }
}

/**! Instanciate usual ParaView behaviors.
 * This method should be updated at each new version of ParaView with what is found in
 *    Qt/ApplicationComponents/pqParaViewBehaviors.cxx
 */
void PVViewer_Behaviors::instanciateAllBehaviors(QMainWindow * desk)
{
  //    "new pqParaViewBehaviors(anApp->desktop(), this);"
  // -> (which loads all standard ParaView behaviors at once) has to be replaced in order to
  // exclude using of pqQtMessageHandlerBehaviour

  // Define application behaviors.
  if (BehaviorLoadingLevel < 1)
    instanciateMinimalBehaviors(desk);

  if (BehaviorLoadingLevel < 2)
    {
      //new pqQtMessageHandlerBehavior(this);   // THIS ONE TO EXCLUDE !! see comment above
      new pqDataTimeStepBehavior(this);
      new pqSpreadSheetVisibilityBehavior(this);
      new pqPipelineContextMenuBehavior(this);
      new pqUndoRedoBehavior(this);
      new pqAutoLoadPluginXMLBehavior(this);  // auto load plugins GUI stuff
      new pqPluginDockWidgetsBehavior(desk);
      new pqPluginActionGroupBehavior(desk);
      new pqPersistentMainWindowStateBehavior(desk);
      new pqObjectPickingBehavior(desk);
      new pqCollaborationBehavior(this);
      new pqViewStreamingBehavior(this);

      // Move instantiation of the pqApplyBehavior to the PVViewer_GUIElements::buildPVWidgets(),
      // because without pqPropertiesPanel it doesn't make sense.      
      /*
      pqApplyBehavior* applyBehavior = new pqApplyBehavior(this);
      foreach (pqPropertiesPanel* ppanel, desk->findChildren<pqPropertiesPanel*>())
      {
        applyBehavior->registerPanel(ppanel);
      }
      */
      BehaviorLoadingLevel = 2;
    }
}
