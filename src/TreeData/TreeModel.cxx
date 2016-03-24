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


#include "TreeItem.hxx"
#include "TreeModel.hxx"

TreeModel::TreeModel(const QStringList &headers, QObject *parent)
  : QAbstractItemModel(parent)
{
  QVector<QVariant> rootData;
  foreach (QString header, headers)
    rootData << header;

  // _MEM_ We have to specify a string identifier for each item so
  // that it could be easily retrieved by its parent. In the case of
  // the root item, the value of the identifier doesn't matter => we
  // choose an arbitrary value
  //QString rootNameId = "rootItem";
  _rootItem = new TreeItem("rootItem", rootData);
  _rootItem->associateToModel(this);
}

TreeModel::~TreeModel()
{
  delete _rootItem;
}

TreeItem * TreeModel::getRootItem() {
  return _rootItem;
}

//
// =================================================================
// This part of the implementation is the standard interface required
// for providing an operational TreeModel. These methods are used by
// the TreeView for display purpose. We just have to implement how we
// want the items to be displayed in the view.
// =================================================================
//
int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
  return _rootItem->columnCount();
}

/*!
 * This function is used by the tree model to inform the tree view of
 * what data used for rendering of the item in the different possible
 * role (edition, ...).
 */
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();

  TreeItem *item = getItem(index);

  return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/*!
 * This retrieves the item asociated to the specified index.
 */
TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
  // The item associated to an index can be retrieved using the
  // internalPointer() function on the index object.
  if (index.isValid()) {
    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    if (item) return item;
  }
  return _rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return _rootItem->data(section);

  return QVariant();
}

/*!
 * This retrieves the index of the item located at (row,column) place
 * relative to the parent specified by its index.
 */
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  TreeItem *parentItem = getItem(parent);

  TreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  TreeItem *childItem = getItem(index);
  TreeItem *parentItem = childItem->parent();

  if (parentItem == _rootItem)
    return QModelIndex();

  return createIndex(parentItem->rowIndex(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
  TreeItem *parentItem = getItem(parent);

  return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value,
                        int role)
{
  if (role != Qt::EditRole)
    return false;

  TreeItem *item = getItem(index);
  bool result = item->setData(index.column(), value);

  if (result)
    emit dataChanged(index, index);

  return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
  if (role != Qt::EditRole || orientation != Qt::Horizontal)
    return false;

  bool result = _rootItem->setData(section, value);

  if (result)
    emit headerDataChanged(orientation, section, section);

  return result;
}


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

/*
 * The addData functions run a recurcive filling of the tree model, starting
 * form the rootItem and descending in the tree using the recurcive
 * function TreeItem::addData.
 */

bool TreeModel::addData(DataObject * dataObject) {
  QStringList path = QString(dataObject->getPath().c_str()).split(DataObject::pathsep.c_str());
  return addData(dataObject, path);
}
bool TreeModel::addData(DataObject * dataObject, const QStringList &path) {
  TreeItem * rootItem = this->getItem();
  rootItem->appendChild(dataObject, path);
  return true;
}

bool TreeModel::removeData(DataObject * dataObject) {
  QStringList path = QString(dataObject->getPath().c_str()).split(DataObject::pathsep.c_str());
  TreeItem * rootItem = this->getItem();
  rootItem->removeChild(dataObject, path);
  return true;
}
