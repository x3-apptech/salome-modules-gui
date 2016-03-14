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


#include <QStringList>
#include "TreeItem.hxx"

TreeItem::TreeItem(const QString &nameId,
                   const QVector<QVariant> &columnValues,
                   TreeItem *parent)
{
  this->initialize(nameId, columnValues, parent);
}

/*!
 * This initializes the tree item. Called by the constructors.
 */
void TreeItem::initialize(const QString &nameId,
                          const QVector<QVariant> &columnValues,
                          TreeItem *parent) {
  _itemNameId = nameId;
  _itemData   = columnValues;
  _parentItem = parent;

  // An item is associated to the model of its parent. It can't be
  //done automatically in the case where the item has no parent. Then
  //the function associatedToModel has to be called explicitely from
  //within the user code (for exemple at construction of the model,
  //when creating the root item).
  if ( parent != NULL ) {
    this->associateToModel(parent->associatedModel());
  }
}

TreeItem::~TreeItem()
{
  qDeleteAll(_childItems);
  qDeleteAll(_childItemsMapById);
  qDeleteAll(_childItemsMapByLabel);
}

/*!
 * This must be used to specified which model this item belongs to. It
 * is required because the item sometimes requests its model for
 * global informations about the data tree. In standard usage, this
 * function is automatically set when the item is instantiated by
 * requested the parent item. Then only the root item needs a manual
 * setting.
 */
void TreeItem::associateToModel(TreeModel * model) {
  _associatedModel = model;
}

TreeModel * TreeItem::associatedModel() {
  return _associatedModel;
}

/*!
 * This provide an identifier for this item
 */
QString TreeItem::nameId() {
  return _itemNameId;
}

/*!
 * This creates an item from the specified dataObject and put it in
 * the decendency of this item at the specified relativePath. The
 * created item could not be a direct child of this item in the case
 * where the relative path is not null. Note that no reference on the
 * dataObject is kept in this instance. Only the identifier is used to
 * set the item identifier, and the properties may be used to set the
 * values of the item (stored in columns).
 */
void TreeItem::appendChild(DataObject * dataObject,
                           const QStringList &relativePath) {
  // Definition of the nameId
  QString nameId = QString(dataObject->getNameId().c_str());

  // Definition of columns data values
  QVector<QVariant> columnValues;
  columnValues << QString(dataObject->getLabel().c_str());
  columnValues << "No value"; // We could use the dataObject properties

  // Append the item at the specified location with the specified values:
  this->appendChild(nameId, columnValues, relativePath);
}

void TreeItem::appendChild(const QString &nameId,
                           const QVector<QVariant> &columnValues,
                           const QStringList &relativePath) {

  if ( relativePath.isEmpty() ) {
    // It is a direct child => just create and append to this.
    TreeItem * child = new TreeItem(nameId, columnValues, this);
    this->appendChild(child);
    return;
  }

  // The child is embedded in a sub-folder (to be created if doesn't
  // exist).
  // We first check if the sub-folder already exist:
  TreeItem * folder = this->childByLabel(relativePath[0]);
  if ( folder == NULL ) {
    // The folder does not exist. It must be created before going any
    // further. By convention we choose the folder name as
    // identifier.
    QString folderNameId = relativePath[0];
    QVector<QVariant> folderColumnValues;
    folderColumnValues << relativePath[0] << "No value";
    folder = new TreeItem(folderNameId, folderColumnValues, this);
    this->appendChild(folder);
  }

  // We create the relative path of the next iteration (delete the
  // first folder path).
  QStringList nextRelativePath;
  for (int i = 1; i < relativePath.size(); ++i)
    nextRelativePath << relativePath[i];

  folder->appendChild(nameId, columnValues, nextRelativePath);
}

/*!
 * This appends the specified child to this item. This item is the
 * direct parent of the specified child.
 */
void TreeItem::appendChild(TreeItem *item)
{
  TreeModel * model = this->associatedModel();

  int position = this->childCount();
  model->beginInsertRows(this->modelIndex(), position, position);
  _childItems.append(item);
  _childItemsMapById[item->nameId()] = item;
  _childItemsMapByLabel[item->data(0).toString()] = item;
  model->endInsertRows();
}

/*!
 * This removes the specified child to this item. This item is the
 * direct parent of the specified child.
 */
void TreeItem::removeChild(TreeItem *item)
{
  TreeModel * model = this->associatedModel();

  int position = this->childCount();
  model->beginRemoveRows(this->modelIndex(), position, position);
  _childItems.removeOne(item);
  _childItemsMapById.remove(item->nameId());
  _childItemsMapByLabel.remove(item->data(0).toString());
  model->endRemoveRows();
}

void TreeItem::removeChild(DataObject * dataObject, const QStringList &relativePath) {
  if ( relativePath.isEmpty() ) {
    // It is a direct child => just remove it.
    QString nameId = QString(dataObject->getNameId().c_str());
    TreeItem * child = this->childById(nameId);
    if (child != NULL)
      this->removeChild(child);
    return;
  }

  // The child is embedded in a sub-folder.
  // We first check if the sub-folder already exist:
  TreeItem * folder = this->childByLabel(relativePath[0]);
  if ( folder == NULL )
    return;

  // Go down into subfolder, if any.
  QStringList nextRelativePath;
  for (int i = 1; i < relativePath.size(); ++i)
    nextRelativePath << relativePath[i];

  folder->removeChild(dataObject, nextRelativePath);

  if (folder->childCount() == 0)
    this->removeChild(folder);
}

/*!
 * The child() function returns the child that corresponds to the
 * specified row number in the item's list of child items.
 */
TreeItem *TreeItem::child(int row)
{
  return _childItems.value(row);
}

TreeItem *TreeItem::childById(const QString &nameId)
{
  QMap <QString, TreeItem*>::iterator it;
  it = _childItemsMapById.find ( nameId );

  if ( it != _childItemsMapById.end() ) {
    return it.value();
  }
  return NULL;
}

TreeItem *TreeItem::childByLabel(const QString &label)
{
  QMap <QString, TreeItem*>::iterator it;
  it = _childItemsMapByLabel.find ( label );

  if ( it != _childItemsMapByLabel.end() ) {
    return it.value();
  }
  return NULL;
}



int TreeItem::childCount() const
{
  return _childItems.count();
}

/*!
 * The rowIndex() function reports the item's location within its
 * parent's list of items.
 */
int TreeItem::rowIndex() const
{
  if (_parentItem)
    return _parentItem->_childItems.indexOf(const_cast<TreeItem*>(this));

  return 0;
}

int TreeItem::columnCount() const
{
  return _itemData.count();
}

QVariant TreeItem::data(int column) const
{
  return _itemData.value(column);
}

TreeItem *TreeItem::parent()
{
  return _parentItem;
}

bool TreeItem::setData(int column, const QVariant &value)
{
  if (column < 0 || column >= _itemData.size())
    return false;

  _itemData[column] = value;
  return true;
}

QModelIndex TreeItem::modelIndex(int column)
{
  TreeModel * model = this->associatedModel();
  if (_parentItem && (_parentItem != model->getRootItem()))
    return model->index(rowIndex(),
                        column,
                        _parentItem->modelIndex());
  else
    return model->index(rowIndex(),
                        column,
                        QModelIndex());
}
