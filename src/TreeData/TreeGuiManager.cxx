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

#include "TreeGuiManager.hxx"

// SALOME includes
#include <QStringList>

// XCAD includes
#include "TreeView.hxx"
#include "QtHelper.hxx"

// TODO:
// IMP: The constructor should have a dockwidget as argument.
// The creation of the void dockwidget (without the data tree
// embedded) should be done previously by the StandardApp_Module.
// The constructor would fill the dockwidget with a data tree view

/*!
 * The construction of the gui manager setups a graphic framework that
 * consists in a set of dock widgets implanted in the SALOME GUI and
 * embedding a tree view rendering a data model (collection of data
 * objects) in a hierarchic graphical organisation.
 *
 * The data model is a straith list of data objects while the view is
 * a tree representation of this collection where folders corresponds
 * to specific properties of the objects.
 *
 * This represention is for the needs of navigation in a huge amount
 * of data and to ease the selection and processing of items.
 */
TreeGuiManager::TreeGuiManager(SalomeApp_Application * salomeApp, const char * title)
  : TreeObserver()
{
  _salomeApp = salomeApp;
  
  bool tabify = false;
  _dockWidgets = new DockWidgets(_salomeApp, tabify, title);

  // Create a TreeView to be associated to a TreeModel dedicated to
  // the Xcad data:
  _dataTreeView = new TreeView();
  QStringList headers;
  headers << tr("Name") << tr("Value");
  _dataTreeModel = new TreeModel(headers);
  _dataTreeView->setModel(_dataTreeModel);

  // Then plug the TreeView in the dock widget:
  _dockWidgets->setDataView(_dataTreeView);

  // We specify here the dataview to be observed
  this->observe(_dataTreeView);
}

/*!
 * This returns the SALOME application (SalomeApp_Application
 * instance) associated to this TreeGuiManager.
 */
SalomeApp_Application * TreeGuiManager::getSalomeApplication() {
  return _salomeApp;
}


/*!
 * This function set a layout of the different dock widgets in one
 * single tabbed widget.
 */
void TreeGuiManager::tabifyDockWidgets(bool tabify) {
  _dockWidgets->tabify(tabify);
}

/*!
 * This function switch on/off the dock widgets managed by this
 * gui manager.
 */
void TreeGuiManager::showDockWidgets(bool isVisible) {
  _dockWidgets->show(isVisible);
}

/*!
 * This returns the data tree model defined in this
 * TreeGuiManager. The data tree model is a tree representation of the
 * data model associated to this TreeGuiManager.
 */
TreeModel * TreeGuiManager::getDataTreeModel() {
  return _dataTreeModel;
}

/*!
 * This returns the data tree view defined in this
 * TreeGuiManager. The data tree view can be request to customize the
 * popup menu associated to the tree representation.
 */
TreeView * TreeGuiManager::getDataTreeView() {
  return _dataTreeView;
}

/*!
 * This returns the dock widgets manager
 */
DockWidgets * TreeGuiManager::getDockWidgets() {
  return _dockWidgets;
}

/*!
 * This function specifies the data model to be used by the
 * TreeGuiManager.
 */
void TreeGuiManager::setDataModel(DataModel * dataModel) {
  _dataModel = dataModel;
}

DataModel * TreeGuiManager::getDataModel() {
  return _dataModel;
}

/*!
 * This function processes the edit signals received from the
 * TreeView. This is a default implementation that only prints the
 * reception of the signal and some information about the dataObject
 * associated to the item whose id is specified. In practice, the data
 * model could be requested here to retrieve the data object to be
 * edited from the nameId.
 * TO BE IMPLEMENTED IN A DOMAIN SPECIFIC VERSION OF THIS CLASS
 */

void TreeGuiManager::processItemList(QStringList itemNameIdList,
				     int actionId)
{
  // WARN: THIS IS A DEFAULT IMPLEMENTATION GIVEN FOR DEMONSTRATION
  // OF WHAT TO DO WITH THE PARAMETERS

  QString itemNameId = itemNameIdList[0];
  LOG("TreeGuiManager: signal received : process item "<<itemNameId);
  DataObject * dataObject = _dataModel->getDataObject(QS2S(itemNameId));
  if ( dataObject != NULL ) {
    LOG("TreeGuiManager: dataObject = "<<dataObject->toString().c_str());
  } else {
    LOG("TreeGuiManager: no data object associated to this item");
  }
}
