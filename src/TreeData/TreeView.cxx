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


// include Qt
#include <QString>
#include <QMenu>
#include <QModelIndex>
#include <QAbstractItemView>

// include Xcad
#include "TreeView.hxx"
#include "TreeModel.hxx"
#include "TreeItem.hxx"
#include "QtHelper.hxx"

TreeView::TreeView(QWidget * parent)
  : QTreeView(parent)
{
  // We authorize the multiple selection of items
  this->setSelectionMode(QAbstractItemView::ExtendedSelection);

  _lastActionId = 0;

  // Default actions for tests
  int displayActionId = addAction(QObject::tr("Afficher"));
  int editActionId    = addAction(QObject::tr("Editer"));
}

TreeView::~TreeView() {
}

/*!
 * This function defines a menu item to add in the popup menu
 * associated to this TreeView, and return an integer that corresponds
 * to the unique identifier of this action (identifier used in the
 * signal emitted to notify observers that an action has been
 * selected). Then the caller of this function has to take care of
 * this return id (i.e. has to store it in its internal tables) to be
 * able to process the notifications from this TreeView.
 */
int TreeView::addAction(QString label) {
  QAction * action = new QAction(this);
  int actionId = _lastActionId;
  action->setObjectName(_idToName(actionId));
  action->setText(label);
  _listActions << action;
  _lastActionId++;

  return actionId;
}

/*!
 * This function removes all actions previously defined for the popup
 * menu of this TreeView.
 */
void TreeView::clearActions() {
  _listActions.clear();
}

/*!
 * You must use this function to create the name of an action object
 * from its id.
 */
QString TreeView::_idToName(int actionId) {
  return QString::number(actionId);
}
/*!
 * You must use this function to create the id of an action object
 * from its name (stored in objectName() attribute of the QAction).
 */
int TreeView::_nameToId(QString actionName) {
  return actionName.toInt();
}

void TreeView::contextMenuEvent(QContextMenuEvent *event) {
  if ( _listActions.size() == 0 ) {
    // Just return there is no actions defined for this popup menu
    return;
  }

  // _TODO_ display the QMenu only if the selected item is acceptable
  QMenu menu(this);
  for (int i = 0; i < _listActions.size(); ++i) {
    menu.addAction(_listActions.at(i));
  }
  connect(&menu, SIGNAL(triggered(QAction*)),
  	  this,  SLOT(processMenuAction(QAction*)));

  menu.exec(event->globalPos());
}

/*!
 * This SLOT is connected on the signal emited by the menu when an
 * action is selected.
 */
void TreeView::processMenuAction(QAction * actionSelected) {
  LOG("processMenuAction: START");

  // We first check than at least on item is selected
  QModelIndexList indexList = this->selectionModel()->selectedRows(0);
  if ( indexList.isEmpty() ) {
    LOG("No item selected");
    return;
  }

  // Then we can gather the list of model item ids associated the
  // selection.
  TreeModel *model = (TreeModel *)this->model();
  QListIterator<QModelIndex> it(indexList);
  QStringList nameIdList;
  while (it.hasNext()) {
    TreeItem * item = model->getItem(it.next());
    nameIdList << item->nameId();
  }

  // Finally, one can emit a signal to observers specifying the list of
  // id and the type of action (i.e. the action identifier)
  int actionId = _nameToId(actionSelected->objectName());
  LOG("TreeView::processMenuAction: signal emitted:\n"<<
      "item list: "<<nameIdList<<"\n"<<
      "action id: "<<actionId);
  emit itemListToProcess(nameIdList, actionId);

  LOG("processMenuAction: END");
}
