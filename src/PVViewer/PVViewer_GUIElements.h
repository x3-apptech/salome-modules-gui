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

#include "PVViewer.h"

#include <QObject>
#include <QList>

class pqPropertiesPanel;
class pqPipelineBrowserWidget;
class SUIT_Desktop;
class QMenu;
class QToolBar;
class QAction;
class pqAnimationTimeToolbar;
class pqVCRToolbar;

/*!
 * Some GUI elements of ParaView need to be instanciated in a proper order. This class
 * holds all of them for the sake of clarity.
 */
class PVVIEWER_EXPORT PVViewer_GUIElements: public QObject
{
  Q_OBJECT

public:
  static PVViewer_GUIElements * GetInstance(SUIT_Desktop* desk);

  pqPropertiesPanel * getPropertiesPanel() { return propertiesPanel; }
  pqPipelineBrowserWidget * getPipelineBrowserWidget() { return pipelineBrowserWidget; }

  QMenu* getFiltersMenu() { return filtersMenu; }
  QMenu* getSourcesMenu() { return sourcesMenu; }
  QMenu* getMacrosMenu()  { return macrosMenu; }

  pqVCRToolbar* getVCRToolbar() { return vcrToolbar; }
  pqAnimationTimeToolbar* getTimeToolbar() { return timeToolbar; }

  void myBuildToolbars(SUIT_Desktop* desk);
  void addToolbars(SUIT_Desktop* desk);
  void setToolBarVisible(bool show);
  QList<QToolBar*> getToolbars();

public slots:
  void onEmulateApply();  // better use the slot from PVViewer_ViewManager if you want to trigger "Apply"

private:
  PVViewer_GUIElements(SUIT_Desktop* desk);
  virtual ~PVViewer_GUIElements() {}

  static PVViewer_GUIElements* theInstance;

  // Widgets
  pqPropertiesPanel* propertiesPanel;
  pqPipelineBrowserWidget* pipelineBrowserWidget;

  // Dummy QMenus receiving ParaView's reaction for automatic add when new sources are added
  QMenu* sourcesMenu;
  QMenu* filtersMenu;
  QMenu* macrosMenu;

  // Toolbars also need to be instanciated early:
  QToolBar* mainToolBar;
  pqVCRToolbar* vcrToolbar;
  pqAnimationTimeToolbar* timeToolbar;
  QToolBar* colorToolbar;
  QToolBar* reprToolbar;
  QToolBar* cameraToolbar;
  QToolBar* axesToolbar;
  QToolBar* macrosToolbar;
  QToolBar* commonToolbar;
  QToolBar* dataToolbar;

public:
  QAction* mainAction;
  QAction* vcrAction;
  QAction* timeAction;
  QAction* colorAction;
  QAction* reprAction;
  QAction* cameraAction;
  QAction* axesAction;
  QAction* macrosAction;
  QAction* commonAction;
  QAction* dataAction;
};

#endif /* PVVIEWERGUIELEMENTS_H_ */
