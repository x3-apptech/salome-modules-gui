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

#ifndef TREEITEM_H
#define TREEITEM_H

#include "TreeData.hxx"

#include <QList>
#include <QVariant>
#include <QVector>
#include <QModelIndex>

#include "DataObject.hxx"
#include "TreeModel.hxx"

class TREEDATA_EXPORT TreeItem
{
 public:
  TreeItem(const QString &nameId, const QVector<QVariant> &columnValues, TreeItem *parent = 0);
  ~TreeItem();

  QString nameId();
  void associateToModel(TreeModel * model);
  TreeModel * associatedModel();
  QModelIndex modelIndex(int column=0);
  TreeItem *parent();

  void appendChild(TreeItem * child);
  void appendChild(DataObject * dataObject,
                   const QStringList &relativePath=QStringList());
  void appendChild(const QString &nameId,
                   const QVector<QVariant> &columnValues,
                   const QStringList &relativePath=QStringList());

  void removeChild(TreeItem * child);
  void removeChild(DataObject * dataObject,
                   const QStringList &relativePath=QStringList());

  TreeItem *child(int row);
  TreeItem *childById(const QString &nameId);
  TreeItem *childByLabel(const QString &label);
  int childCount() const;
  int columnCount() const;
  int rowIndex() const;
  QVariant data(int column) const;
  bool setData(int column, const QVariant &value);

 private:
  void initialize(const QString &nameId,
                  const QVector<QVariant> &columnValues,
                  TreeItem *parent);

  QList<TreeItem*> _childItems;
  QMap <QString, TreeItem *> _childItemsMapById;
  QMap <QString, TreeItem *> _childItemsMapByLabel;

  QString _itemNameId;
  QVector<QVariant> _itemData;
  TreeItem  * _parentItem;
  TreeModel * _associatedModel;

};

#endif
