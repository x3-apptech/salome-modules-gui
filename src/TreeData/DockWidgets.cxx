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

#include "DockWidgets.hxx"

// Qt includes
#include <QMainWindow>

// SALOME includes
#include <SUIT_Desktop.h>
#include <SUIT_DataBrowser.h>
#include <QtxTreeView.h>

namespace
{
  QString toObjectName( const QString& s )
  {
    QStringList words = s.split( QRegExp("\\s+") );
    QStringList result;
    if ( words.count() > 0 )
      result.append( words[0].left(1).toLower() + words[0].mid(1) );
    for ( int i = 1; i < words.count(); i++ )
      result.append( words[i].left(1).toUpper() + words[i].mid(1) );
    return result.join( "" );
  }
}

/*!
 * This create a gui container to hold widgets dedicated to the XCAD
 * data model. By default, the dock widgets are not visible. Use the
 * show() method to control the visibility (usefull when activating
 * and desactivating the module to show/hide the dock widgets).
 *
 * This class does not make any hypothesis on what will be embedded in
 * the dock widgets (only that it is QTreeView). The QTreeView is
 * defined elsewhere and is generaly rendering a tree model containing
 * tree items.
 */
DockWidgets::DockWidgets(SalomeApp_Application* salomeApp,
			 bool tabify,
			 const char * title) {
  _salomeApp = salomeApp;

  QMainWindow *parent = _salomeApp->desktop();
  _dwDataPanel = new QDockWidget(parent);
  _dwDataPanel->setVisible(false);
  _dwDataPanel->setWindowTitle(title);
  _dwDataPanel->setObjectName(toObjectName(title)+"Dock");
  parent->addDockWidget(Qt::LeftDockWidgetArea, _dwDataPanel);
  //
  // At this step, the _dwDataPanel is located side by side with the object
  // browser (or one over the other).
  //
  // It is possible to tabify the different dock widgets in one single
  // tabbed widget. See the above example:
  this->tabify(tabify);
}

void DockWidgets::tabify(bool tabify) {
  if ( tabify ) {    
    // We first get the object browser tree view, and then the
    // associated DockWidget. Note that the tree view is a SALOME
    // specific extention of the originate QTreeView and called
    // QtxTreeView.
    SUIT_DataBrowser* objectBrowser = _salomeApp->objectBrowser();
    QtxTreeView* treeView = objectBrowser->treeView();
    QWidget* pw = treeView->parentWidget();
    QDockWidget* dwObjectBrowser = 0;
    while ( pw && !dwObjectBrowser ) {
      dwObjectBrowser = ::qobject_cast<QDockWidget*>( pw );
      pw = pw->parentWidget();
    };
    QMainWindow *parent = _salomeApp->desktop();
    parent->tabifyDockWidget(_dwDataPanel, dwObjectBrowser);
    parent->setTabOrder(_dwDataPanel, dwObjectBrowser);
    parent->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
  }
  else {
    // TO BE IMPLEMENTED
  }
}

/*!
 * This function controls the visibility of the dock widgets.
 */
void DockWidgets::show(bool isVisible) {
  _dwDataPanel->setVisible(isVisible);
}

/*!
 * This function initializes the central part of the dock widget with
 * a tree view that can hold a hierarchical data model.
 */
void DockWidgets::setDataView(QTreeView * dataView) {
  _tvDataView = dataView;
  _tvDataView->setParent(_dwDataPanel);
  _tvDataView->setMinimumHeight(200);
  _dwDataPanel->setWidget(_tvDataView);
}

void DockWidgets::setPropertiesView(QTreeView * propertiesView) {
  // Not implemented yet
}

/*!
 * This function returns dock widget
 */
QDockWidget * DockWidgets::getDockWidget() {
  return _dwDataPanel;
}
