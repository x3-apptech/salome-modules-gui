// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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
// Author : Anthony GEAY (EDF R&D)

#include "PVViewer_InitSingleton.h"
#include "PVViewer_Core.h"
#include "PVViewer_ViewManager.h"

bool PVViewer_InitSingleton::IS_INIT=false;

void PVViewer_InitSingleton::Init(QMainWindow* desktop)
{
  if(IS_INIT)
    return ;
  PVViewer_Core::ParaviewInitApp(desktop);
  // Finish ParaView set up: behaviors, connection and configurations.
  const QString configPath(PVViewer_ViewManager::GetPVConfigPath());
  PVViewer_Core::ParaviewInitBehaviors(true, desktop);
  PVViewer_ViewManager::ConnectToExternalPVServer(desktop);
  PVViewer_Core::ParaviewLoadConfigurations(configPath);
  IS_INIT=true;
}
