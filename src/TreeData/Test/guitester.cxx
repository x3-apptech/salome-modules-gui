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

#include <QApplication>
#include <QMainWindow>
#include <QDockWidget>
#include <QTreeView>

#include <Basics_Utils.hxx>

//
// =================================================================
// Generic helper functions
// =================================================================
//
/*!
 * This functions displays a main window that embeds the specified
 * widget. A dockwidget is used to create a context similar to as the
 * SALOME target context.
 */
void showWidget(QWidget * widget) {

  QMainWindow * window = new QMainWindow();

  // Prepare a gui framework for testing the widget. We use a
  // dockwidget, just to be in a context similar to as the SALOME
  // target context.
  QDockWidget * dwDataPanel = new QDockWidget(window);
  dwDataPanel->setVisible(true);
  dwDataPanel->setWindowTitle("XCAD data model");
  window->addDockWidget(Qt::LeftDockWidgetArea, dwDataPanel);

  // Then plug the widget in the dock widget framework:
  widget->setParent(dwDataPanel);
  widget->setMinimumHeight(300);
  dwDataPanel->setWidget(widget);

  window->show();
}

//
// =================================================================
// Tests functions for TreeModel
// =================================================================
//
#include "TreeModel.hxx"
#include "MyDataModel.hxx"
#include "testhelper.hxx"


/*!
 * This function fills the specified tree with data that show
 * different levels of path in the tree.
 */
void _TEST_treemodel_addData_01(TreeModel * dataTreeModel) {
  // We can first add categories (for example to set categories
  // properties)
  QStringList path;
  DataObject * folder;

  path << "folder_1";
  folder = TESTHELPER_dummyObject("folder_1.1");
  dataTreeModel->addData(folder, path);
  folder = TESTHELPER_dummyObject("folder_1.2");
  dataTreeModel->addData(folder, path);

  path.clear();
  path << "folder_2";
  folder = TESTHELPER_dummyObject("folder_2.1");
  dataTreeModel->addData(folder, path);
  
  // Then we can add data
  DataObject * data;
  path.clear();
  path << "folder_1" << "folder_1.1";
  data = TESTHELPER_dummyObject("data_1.1.1");
  dataTreeModel->addData(data, path);
  data = TESTHELPER_dummyObject("data_1.1.2");
  dataTreeModel->addData(data, path);
  // You can notice that there is no conceptual difference between a
  // folder and an item, as in the QTreeModel.
  
  // No limit to the depth
  path.clear();
  path << "xcad" << "data1" << "x" << "y";
  data = TESTHELPER_dummyObject("z");
  dataTreeModel->addData(data,path);
}

#define LOOPSIZE 15
/*!
 * This function fills the specified tree with a huge amount of data
 */
void _TEST_treemodel_addData_02(TreeModel * dataTreeModel) {
  QStringList path;
  DataObject * data;
  
  START_TIMING(treemodel);
  for (int i=0; i<LOOPSIZE; i++) {
    END_TIMING(treemodel,1);
    for (int j=0; j<LOOPSIZE; j++) {
      for (int k=0; k<LOOPSIZE; k++) {
        // The data list corresponds to the path of the item in the tree
        path << QString("folder_%0").arg(i)
             << QString("subfolder_%0_%1").arg(i).arg(j);
        data = TESTHELPER_dummyObject(QString("item_%0_%1_%2").arg(i).arg(j).arg(k));
        dataTreeModel->addData(data,path);
        path.clear();
      } 
    }
  }
  END_TIMING(treemodel,1);
}

void _TEST_treemodel_addData_03(TreeModel * dataTreeModel) {
  MyDataObject * dataObject = new MyDataObject();
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_TYPE,    "Tuyauterie");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_CIRCUIT, "RCP");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_REPFONC, "RF1");
  dataTreeModel->addData(dataObject);

  dataObject = new MyDataObject();
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_TYPE,    "Tuyauterie");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_CIRCUIT, "RCP");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_REPFONC, "RF1");
  dataTreeModel->addData(dataObject);

  dataObject = new MyDataObject();
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_TYPE,    "Tuyauterie");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_CIRCUIT, "RCP");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_REPFONC, "RF2");
  dataTreeModel->addData(dataObject);

  dataObject = new MyDataObject();
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_TYPE,    "Tuyauterie");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_CIRCUIT, "RRA");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_REPFONC, "RF1");
  dataTreeModel->addData(dataObject);

  dataObject = new MyDataObject();
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_TYPE,    "Génie civil");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_CIRCUIT, "RRA");
  dataObject->setProperty(MyDataObject::PROPERTY_KEY_REPFONC, "RF1");
  dataTreeModel->addData(dataObject);
}

/*!
 * This test function shows how it's possible to load data from a file
 * to populate the tree model.
 */
void _TEST_treemodel_loadDataFromFile(TreeModel * dataTreeModel, const QString &filename) {
  TESTHELPER_loadDataFromFile(dataTreeModel, filename);
}

/*!
 * Main test function for the tree model demo.
 */
#include "TreeModel.hxx"
#include "TreeView.hxx"
void TEST_treemodel() {

  START_TIMING(treemodel);

  // We first prepare a data view embedding a tree model
  TreeView * dataView = new TreeView();
  QStringList headers;
  headers << QObject::tr("Name") << QObject::tr("Value");
  TreeModel * dataTreeModel = new TreeModel(headers);
  dataView->setModel(dataTreeModel);
  END_TIMING(treemodel,1);

  // Then we can fill the tree model with data. Can proceed with
  // different ways (comment/uncomment which you want to test):
  _TEST_treemodel_loadDataFromFile(dataTreeModel, TESTHELPER_testfilename(DATAFILENAME));
  //_TEST_treemodel_addData_01(dataTreeModel);
  //_TEST_treemodel_addData_02(dataTreeModel);
  //_TEST_treemodel_addData_03(dataTreeModel);
  // Finally, show the widget in a main window
  END_TIMING(treemodel,1);

  showWidget(dataView);
  END_TIMING(treemodel,1);
}

//
// =================================================================
// Tests functions for TreeModel with interactive changes
// =================================================================
//
#include "mainwindow.hxx"
void TEST_treemodel_interactif() {
  MainWindow * window = new MainWindow();
  window->show();
}

//
// =================================================================
//
int main(int argc, char * argv[ ])
{
  QApplication app(argc, argv);
  TEST_treemodel();
  //TST_treemodel_interactif();
  return app.exec();
}
