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

#ifndef PVVIEWER_GUIELEMENTS_H
#define PVVIEWER_GUIELEMENTS_H

#include "PVViewer.h"

#include <QObject>
#include <QList>

class QAction;
class QMainWindow;
class QMenu;
class QToolBar;
class pqAnimationTimeToolbar;
class pqPipelineBrowserWidget;
class pqPipelineModel;
class pqPropertiesPanel;
class pqVCRToolbar;

/*!
 * Some GUI elements of ParaView need to be instanciated in a proper order. This class
 * holds all of them for the sake of clarity.
 */
class PVVIEWER_EXPORT PVViewer_GUIElements: public QObject
{
  Q_OBJECT

public:
  static PVViewer_GUIElements* GetInstance(QMainWindow*);

  pqPropertiesPanel* getPropertiesPanel();
  pqPipelineBrowserWidget* getPipelineBrowserWidget();

  QMenu* getFiltersMenu();
  QMenu* getSourcesMenu();
  QMenu* getMacrosMenu();
  QMenu* getCatalystMenu();

  pqVCRToolbar* getVCRToolbar();
  pqAnimationTimeToolbar* getTimeToolbar();

  void setToolBarVisible(bool);
  void setToolBarEnabled(bool);
  QList<QToolBar*> getToolbars();

  void setVCRTimeToolBarVisible(bool);

private:
  PVViewer_GUIElements(QMainWindow*);
  virtual ~PVViewer_GUIElements() {}

  static PVViewer_GUIElements* theInstance;

  void buildPVWidgets();

  void publishExistingSources();

  // Widgets
  pqPropertiesPanel* propertiesPanel;
  pqPipelineBrowserWidget* pipelineBrowserWidget;
  pqPipelineModel* pipelineModel;

  // Dummy QMenus receiving ParaView's reaction for automatic add when new sources are added
  QMenu* sourcesMenu;
  QMenu* filtersMenu;
  QMenu* macrosMenu;
  QMenu* catalystMenu;

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
  
  QMainWindow* myDesktop;
  bool myPVWidgetsFlag;

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

#endif // PVVIEWER_GUIELEMENTS_H
