// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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

#include "PVViewer_GUIElements.h"
#include "SUIT_Desktop.h"

#include <pqPropertiesPanel.h>
#include <pqPipelineBrowserWidget.h>
#include <pqParaViewMenuBuilders.h>

#include <QMenu>
#include <QList>
#include <QAction>

PVViewer_GUIElements * PVViewer_GUIElements::theInstance = 0;

PVViewer_GUIElements::PVViewer_GUIElements(SUIT_Desktop* desk) :
  propertiesPanel(0), pipelineBrowserWidget(0),
  sourcesMenu(0)
{
  propertiesPanel = new pqPropertiesPanel(desk);
  pipelineBrowserWidget  = new pqPipelineBrowserWidget(desk);

  sourcesMenu = new QMenu(desk);
  pqParaViewMenuBuilders::buildSourcesMenu(*sourcesMenu, desk);

  filtersMenu = new QMenu(desk);
  pqParaViewMenuBuilders::buildFiltersMenu(*filtersMenu, desk);

  macrosMenu = new QMenu(desk);
  pqParaViewMenuBuilders::buildMacrosMenu(*macrosMenu);
}

PVViewer_GUIElements * PVViewer_GUIElements::GetInstance(SUIT_Desktop* desk)
{
  if (! theInstance)
    theInstance = new PVViewer_GUIElements(desk);
  return theInstance;
}

void PVViewer_GUIElements::updateSourcesMenu(QMenu *menu)
{
  if (menu)
    {
      menu->clear();
      QList<QAction *> act_list = sourcesMenu->actions();
      foreach(QAction * a, act_list)
      {
        menu->addAction(a);
      }
    }
}

void PVViewer_GUIElements::updateFiltersMenu(QMenu *menu)
{
  if (menu)
    {
      menu->clear();
      QList<QAction *> act_list = filtersMenu->actions();
      foreach(QAction * a, act_list)
      {
        menu->addAction(a);
      }
    }
}

void PVViewer_GUIElements::updateMacrosMenu(QMenu *menu)
{
  if (menu)
    {
      menu->clear();
      QList<QAction *> act_list = macrosMenu->actions();
      foreach(QAction * a, act_list)
      {
        menu->addAction(a);
      }
    }
}


void PVViewer_GUIElements::onEmulateApply()
{
  if (propertiesPanel)
    propertiesPanel->apply();
}
