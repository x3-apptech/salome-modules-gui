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


#ifndef TREEMODEL_H
#define TREEMODEL_H

#include "TreeData.hxx"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QStringList>

#include "DataObject.hxx"

class TreeItem;
class TreeView;

class TREEDATA_EXPORT TreeModel : public QAbstractItemModel
{
  Q_OBJECT

  // IMPORTANT NOTE:
  // In this implementation of QAbstractItemModel, a tree item is
  // associated to the tree model it belongs to (it can request its
  // model throw a pointer to this model). Then we declare the
  // TreeItem as a friend class so that it can request the protected
  // methods (for example beginInsertRows and endInsertRows, required
  // to manage correctly the addition of an item in the model. An
  // item can append a child to itself, so it needs to inform the
  // model when it begins and when it ends).
  friend class TreeItem;
  friend class TreeView;

public:
  TreeModel(const QStringList &headers, QObject *parent = 0);
  ~TreeModel();

  //
  // =================================================================
  // This part of the specification is the standard interface required
  // for providing an operational TreeModel. These methods are used by
  // the TreeView for display purpose. We just have to implement how
  // we want the items to be displayed in the view.
  // =================================================================
  //
  // MEM: note that these methods are not intended to be used
  // directly. That's the job of the viewer to know how to use
  // them. We just have to give the implementation for customizing the
  // appearance of the tree. The implementation generally requests the
  // items'data to set the appearance features.
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  Qt::ItemFlags flags(const QModelIndex &index) const;

  //
  // =================================================================
  // This part is a specific behavior to get a TreeModel that can
  // organize itself the tree hierarchy using data provided in a
  // filesystem-like format:
  //
  // data="a/b/c" ==> creation/filling of the hierarchy a->b->c
  // The "folder" categories are unique whereas the leaves may exists
  // in multiple instances.
  // =================================================================
  //
  bool addData(DataObject * dataObject);
  bool addData(DataObject * dataObject, const QStringList &path);

  // TODO: We should implement the delete and the update fucntions
  bool removeData(DataObject * dataObject);

  // This part contains helper functions for general purposes
  TreeItem * getRootItem();

private:
  TreeItem *getItem(const QModelIndex &index = QModelIndex()) const;
  TreeItem * _rootItem;
};

#endif // TREEMODEL_H
