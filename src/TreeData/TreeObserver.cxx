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

#include "TreeObserver.hxx"
#include "QtHelper.hxx"

TreeObserver::TreeObserver() : QObject() {
}

/*!
 * This declares the TreeView to be observed by this tree events
 * listener.
 */
void TreeObserver::observe(TreeView * treeView) {
  // We just connect the signals emitted from the treeview to
  // corresponding slots of this observer.
  connect(treeView, SIGNAL(itemListToProcess(QStringList,int)),
          this, SLOT(processItemList(QStringList,int)));
}

/*!
 * This slot should be implemented in a specialized version of
 * TreeObserver to process the signal emitted from the TreeView. The
 * parameters are:
 * - itemNameIdList: the list of name identifiers of model objects
 *   associated to the selected qt items in the TreeView (ids for
 *   requesting directly the data model).
 * - actionId: the identifier of the action selected in the popup menu
 *   that triggered this signal.
 */
void TreeObserver::processItemList(QStringList itemNameIdList, int actionId) {
  LOG("TreeObserver::processItemList: signal received:\n"<<
      "item list: "<<itemNameIdList<<"\n"<<
      "action id: "<<actionId);
}

