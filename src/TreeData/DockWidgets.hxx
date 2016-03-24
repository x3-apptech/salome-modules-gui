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

#ifndef _DOCKWIDGETS_H_
#define _DOCKWIDGETS_H_

#include "TreeData.hxx"

// Qt includes
#include <QDockWidget>
#include <QTreeView>

// SALOME includes
#include <SalomeApp_Application.h>

class TREEDATA_EXPORT DockWidgets {
 public:
  DockWidgets(SalomeApp_Application* salomeApp,
        bool tabify=false,
        const char * title="Data Model");

  void tabify(bool tabify);
  void show(bool isVisible);
  void setDataView(QTreeView * dataView);
  void setPropertiesView(QTreeView * propertyView);

  QDockWidget * getDockWidget();

 private:
  SalomeApp_Application* _salomeApp;
  QDockWidget * _dwDataPanel;
  QTreeView * _tvDataView;
};

#endif // _DOCKWIDGETS_H_
