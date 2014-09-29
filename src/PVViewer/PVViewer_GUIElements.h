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

#ifndef PVVIEWERGUIELEMENTS_H_
#define PVVIEWERGUIELEMENTS_H_

#include <QObject>

class pqPropertiesPanel;
class pqPipelineBrowserWidget;
class SUIT_Desktop;
class QMenu;

/*!
 * Some GUI elements of ParaView need to be instanciated in a proper order. This class
 * holds all of them for the sake of clarity.
 * For example sources menu should be built *before* loading ParaView's configuration, so that the
 * list of sources gets properly populated.
 */
class PVViewer_GUIElements: public QObject
{
  Q_OBJECT

public:
  static PVViewer_GUIElements * GetInstance(SUIT_Desktop* desk);

  pqPropertiesPanel * getPropertiesPanel() { return propertiesPanel; }
  pqPipelineBrowserWidget * getPipelineBrowserWidget() { return pipelineBrowserWidget; }

  // Update the sources menu from what was built in private member 'sourcesMenu'
  void updateSourcesMenu(QMenu *);
  void updateFiltersMenu(QMenu *);
  void updateMacrosMenu(QMenu *);

public slots:
  void onEmulateApply();  // better use the slot from PVViewer_ViewManager if you want to trigger "Apply"

private:
  PVViewer_GUIElements(SUIT_Desktop* desk);
  virtual ~PVViewer_GUIElements() {}

  static PVViewer_GUIElements * theInstance;

  // Widgets
  pqPropertiesPanel * propertiesPanel;
  pqPipelineBrowserWidget * pipelineBrowserWidget;

  // Dummy QMenus receiving ParaView's reaction for automatic add when new sources are added
  QMenu * sourcesMenu;
  QMenu * filtersMenu;
  QMenu * macrosMenu;
};

#endif /* PVVIEWERGUIELEMENTS_H_ */
