// Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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


#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "TreeData.hxx"

#include <QTreeView>
#include <QAction>
#include <QContextMenuEvent>
#include <QList>

class TREEDATA_EXPORT TreeView : public QTreeView
{
  Q_OBJECT

public:
  TreeView(QWidget * parent = 0 );
  ~TreeView();
  int  addAction(QString label);
  void clearActions();
  
protected:
  void contextMenuEvent(QContextMenuEvent *event);

private:
  int _lastActionId;
  QList<QAction*> _listActions;
  QString _idToName(int actionId);
  int _nameToId(QString actionName);
  
  // ---

private slots:
  void processMenuAction(QAction * actionSelected);
  
signals:
  void itemListToProcess(QStringList itemNameIdList, int actionId);
};

#endif // TREEVIEW_H

