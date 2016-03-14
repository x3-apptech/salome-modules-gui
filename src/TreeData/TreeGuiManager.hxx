// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// Author: Guillaume Boulant (EDF/R&D)

#ifndef _TREEGUIMANAGER_H_
#define _TREEGUIMANAGER_H_

#include "TreeData.hxx"

// SALOME includes
#include <SalomeApp_Application.h>

// TREEDATA includes
#include "DockWidgets.hxx"
#include "TreeModel.hxx"
#include "DataModel.hxx"
#include "TreeView.hxx"
#include "TreeObserver.hxx"

class TREEDATA_EXPORT TreeGuiManager : public TreeObserver {
  
public:
  TreeGuiManager(SalomeApp_Application * salomeApp, const char * title="Data Model");
  void tabifyDockWidgets(bool tabify);
  void showDockWidgets(bool isVisible);
  SalomeApp_Application * getSalomeApplication();
  
  TreeModel * getDataTreeModel();
  TreeView * getDataTreeView();
  DockWidgets * getDockWidgets();
  
  void setDataModel(DataModel * dataModel);
  DataModel * getDataModel();
  
private:
  SalomeApp_Application * _salomeApp;
  
  DockWidgets * _dockWidgets;
  TreeView * _dataTreeView;
  
  TreeModel * _dataTreeModel;
  DataModel * _dataModel;
  
public slots:
  virtual void processItemList(QStringList itemNameIdList, int actionId);
};

#endif /* _TREEGUIMANAGER_H_ */
