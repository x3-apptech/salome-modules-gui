// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author: Adrien Bruneton (CEA)

#include "PVViewer_GUIElements.h"
#include "PVServer_ServiceWrapper.h"
#include "PVViewer_Core.h"
#include "PVViewer_ViewManager.h"

#include <pqActiveObjects.h>
#include <pqAnimationManager.h>
#include <pqAnimationTimeToolbar.h>
#include <pqApplicationCore.h>
#include <pqApplyBehavior.h>
#include <pqAxesToolbar.h>
#include <pqCameraToolbar.h>
#include <pqColorToolbar.h>
#include <pqMainControlsToolbar.h>
#include <pqPVApplicationCore.h>
#include <pqParaViewMenuBuilders.h>
#include <pqPipelineBrowserWidget.h>
#include <pqPipelineModel.h>
#include <pqPropertiesPanel.h>
#include <pqPythonManager.h>
#include <pqRepresentationToolbar.h>
#include <pqServerConnectReaction.h>
#include <pqServerManagerModel.h>
#include <pqServerResource.h>
#include <pqSetName.h>
#include <pqVCRToolbar.h>
#include <pqPipelineSource.h>

#include <vtkSMSessionProxyManager.h>
#include <vtkSMProxyIterator.h>

#include <QAction>
#include <QCoreApplication>
#include <QLayout>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QToolBar>


PVViewer_GUIElements * PVViewer_GUIElements::theInstance = 0;

PVViewer_GUIElements::PVViewer_GUIElements(QMainWindow* desk) :
  myDesktop(desk),
  propertiesPanel(0),
  pipelineBrowserWidget(0),
  pipelineModel(0),
  sourcesMenu(0),
  filtersMenu(0),
  macrosMenu(0),
  myPVWidgetsFlag(false)
{
}

PVViewer_GUIElements * PVViewer_GUIElements::GetInstance(QMainWindow* desk)
{
  if (! theInstance)
    theInstance = new PVViewer_GUIElements(desk);
  return theInstance;
}

/**
 * See ParaView source code: pqParaViewMenuBuilders::buildToolbars()
 * to keep this function up to date:
 */
void PVViewer_GUIElements::buildPVWidgets()
{
  if (!myPVWidgetsFlag) {

    //Pipeline Browser
    if ( !pipelineBrowserWidget ) {
        pqPipelineModel* pipelineModel = new pqPipelineModel(*pqApplicationCore::instance()->getServerManagerModel(), this);
        pipelineModel->setView(pqActiveObjects::instance().activeView());
        pipelineBrowserWidget  = new pqPipelineBrowserWidget(myDesktop);
        pipelineBrowserWidget->setModel(pipelineModel);
    }

    // Properties panel
    if ( !propertiesPanel ) {
      propertiesPanel = new pqPropertiesPanel(myDesktop);
    }
    
    // Sources Menu
    if (!sourcesMenu) {
      sourcesMenu = new QMenu(0);
      pqParaViewMenuBuilders::buildSourcesMenu(*sourcesMenu, myDesktop);
    }

    //Filer Menu
    if (!filtersMenu) {
      filtersMenu = new QMenu(0);
      pqParaViewMenuBuilders::buildFiltersMenu(*filtersMenu, myDesktop);
    }

    // Macros Menu
    if (!macrosMenu) {
      macrosMenu = new QMenu(0);
      pqParaViewMenuBuilders::buildMacrosMenu(*macrosMenu);
    }

    mainToolBar = new pqMainControlsToolbar(myDesktop)
      << pqSetName("MainControlsToolbar");
    mainToolBar->layout()->setSpacing(0);

    vcrToolbar = new pqVCRToolbar(myDesktop)
      << pqSetName("VCRToolbar");
    vcrToolbar->layout()->setSpacing(0);

    timeToolbar = new pqAnimationTimeToolbar(myDesktop)
      << pqSetName("currentTimeToolbar");
    timeToolbar->layout()->setSpacing(0);

    colorToolbar = new pqColorToolbar(myDesktop)
      << pqSetName("variableToolbar");
    colorToolbar->layout()->setSpacing(0);

    reprToolbar = new pqRepresentationToolbar(myDesktop)
      << pqSetName("representationToolbar");
    reprToolbar->layout()->setSpacing(0);

    cameraToolbar = new pqCameraToolbar(myDesktop)
      << pqSetName("cameraToolbar");
    cameraToolbar->layout()->setSpacing(0);
    
    axesToolbar = new pqAxesToolbar(myDesktop)
      << pqSetName("axesToolbar");
    axesToolbar->layout()->setSpacing(0);
    
    // Give the macros menu to the pqPythonMacroSupervisor
    pqPythonManager* manager = qobject_cast<pqPythonManager*>(
    pqApplicationCore::instance()->manager("PYTHON_MANAGER"));

    macrosToolbar = new QToolBar("Macros Toolbars", myDesktop)
      << pqSetName("MacrosToolbar");
    manager->addWidgetForRunMacros(macrosToolbar);
    
    commonToolbar = new QToolBar("Common", myDesktop) << pqSetName("Common");
    commonToolbar->layout()->setSpacing(0);
    
    dataToolbar = new QToolBar("DataAnalysis", myDesktop) << pqSetName("DataAnalysis");
    dataToolbar->layout()->setSpacing(0);
    
    // add Toolbars    
    myDesktop->addToolBar(Qt::TopToolBarArea, mainToolBar);
    myDesktop->addToolBar(Qt::TopToolBarArea, vcrToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, timeToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, colorToolbar);
    myDesktop->insertToolBarBreak(colorToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, reprToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, cameraToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, axesToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, macrosToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, commonToolbar);
    myDesktop->addToolBar(Qt::TopToolBarArea, dataToolbar);
    
    mainAction = mainToolBar->toggleViewAction();
    vcrAction = vcrToolbar->toggleViewAction();
    timeAction = timeToolbar->toggleViewAction();
    colorAction = colorToolbar->toggleViewAction();
    reprAction = reprToolbar->toggleViewAction();
    cameraAction = cameraToolbar->toggleViewAction();
    axesAction = axesToolbar->toggleViewAction();
    macrosAction = macrosToolbar->toggleViewAction();
    commonAction = commonToolbar->toggleViewAction();
    dataAction = dataToolbar->toggleViewAction();

    // The piece od the code below is neccessary to correct update "Pipeline Browser",
    // "Properties Panel", toolbars and menus
    
    // Initilaize pqApplyBehavior here because witout pqPropertiesPanel instance it doesn't make sence
    pqApplyBehavior* applyBehavior = new pqApplyBehavior(this);
    foreach (pqPropertiesPanel* ppanel, myDesktop->findChildren<pqPropertiesPanel*>())
      {
        applyBehavior->registerPanel(ppanel);
      }
    
    QMetaObject::invokeMethod( &pqActiveObjects::instance(),
			       "portChanged",
			       Qt::AutoConnection,
			       Q_ARG( pqOutputPort*, pqActiveObjects::instance().activePort() ) );
    
    QMetaObject::invokeMethod( &pqActiveObjects::instance(),
			       "viewChanged",
			       Qt::AutoConnection,
			       Q_ARG( pqView*, pqActiveObjects::instance().activeView() ) ); 

    pqServerManagerModel *smModel = pqApplicationCore::instance()->getServerManagerModel();
    pqServer* serv = pqActiveObjects::instance().activeServer();    

    if (serv) {
      QMetaObject::invokeMethod( smModel,
				 "serverAdded",
				 Qt::AutoConnection,
				 Q_ARG( pqServer*, serv ) );
      
      QMetaObject::invokeMethod( serv,
				 "nameChanged",
				 Qt::AutoConnection,
				 Q_ARG( pqServerManagerModelItem* , NULL ) );
    }
    
    publishExistingSources();
    myPVWidgetsFlag = true;
  }
}

void PVViewer_GUIElements::setToolBarVisible(bool show)
{  
  QCoreApplication::processEvents();
  if (!myPVWidgetsFlag)
    return;
  
  mainAction->setChecked(!show);
  mainAction->setVisible(show);
  mainAction->trigger();

  setVCRTimeToolBarVisible(show);

  colorAction->setChecked(!show);
  colorAction->setVisible(show);
  colorAction->trigger();
  reprAction->setChecked(!show);
  reprAction->setVisible(show);
  reprAction->trigger();
  cameraAction->setChecked(!show);
  cameraAction->setVisible(show);
  cameraAction->trigger();
  axesAction->setChecked(!show);
  axesAction->setVisible(show);
  axesAction->trigger();
  macrosAction->setChecked(!show);
  macrosAction->setVisible(show);
  macrosAction->trigger();
  commonAction->setChecked(!show);
  commonAction->setVisible(show);
  commonAction->trigger();
  dataAction->setChecked(!show);
  dataAction->setVisible(show);
  dataAction->trigger();
}

void PVViewer_GUIElements::setVCRTimeToolBarVisible(bool show)
{
  vcrAction->setChecked(!show);
  vcrAction->setVisible(show);
  vcrAction->trigger();
  timeAction->setChecked(!show);
  timeAction->setVisible(show);
  timeAction->trigger();
}

QList<QToolBar*> PVViewer_GUIElements::getToolbars()
{
  buildPVWidgets();
  QList<QToolBar*> l;
  l << mainToolBar << vcrToolbar << timeToolbar << colorToolbar
    << reprToolbar << cameraToolbar << axesToolbar << macrosToolbar
    << commonToolbar << dataToolbar;
  return l;
}

void PVViewer_GUIElements::setToolBarEnabled(bool enabled)
{
  if (!myPVWidgetsFlag)
    return;
  
  mainToolBar  ->setEnabled(enabled);
  vcrToolbar   ->setEnabled(enabled);
  timeToolbar  ->setEnabled(enabled);
  colorToolbar ->setEnabled(enabled);
  reprToolbar  ->setEnabled(enabled);
  cameraToolbar->setEnabled(enabled);
  axesToolbar  ->setEnabled(enabled);
  macrosToolbar->setEnabled(enabled);
  commonToolbar->setEnabled(enabled);
  dataToolbar  ->setEnabled(enabled);
}


pqPropertiesPanel* PVViewer_GUIElements::getPropertiesPanel()
{
  buildPVWidgets();
  return propertiesPanel;
}

pqPipelineBrowserWidget* PVViewer_GUIElements::getPipelineBrowserWidget()
{
  buildPVWidgets();
  return pipelineBrowserWidget;   
}


pqVCRToolbar* PVViewer_GUIElements::getVCRToolbar()
{
  buildPVWidgets();
  return vcrToolbar;
}

pqAnimationTimeToolbar* PVViewer_GUIElements::getTimeToolbar()
{
  buildPVWidgets();
  return timeToolbar;
}

QMenu* PVViewer_GUIElements::getFiltersMenu() {
  buildPVWidgets();
  return filtersMenu;
}
QMenu* PVViewer_GUIElements::getSourcesMenu() {
  buildPVWidgets();
  return sourcesMenu;
}

QMenu* PVViewer_GUIElements::getMacrosMenu()  {
  buildPVWidgets();
  return macrosMenu;
}

void PVViewer_GUIElements::publishExistingSources() {
  vtkSMSessionProxyManager* pxm = pqActiveObjects::instance().proxyManager();
  pqServerManagerModel* smmodel = pqApplicationCore::instance()->getServerManagerModel();
  if( !pxm || !smmodel )
    return;
  vtkSMProxyIterator* iter = vtkSMProxyIterator::New();
  iter->SetModeToOneGroup();
  iter->SetSessionProxyManager( pxm );
  for ( iter->Begin( "sources" ); !iter->IsAtEnd(); iter->Next() ) {
    if ( pqProxy* item = smmodel->findItem<pqProxy*>( iter->GetProxy() ) ) {
      pqPipelineSource* source = qobject_cast<pqPipelineSource*>( item );
      QMetaObject::invokeMethod( smmodel,
				 "sourceAdded",
				 Qt::AutoConnection,
				 Q_ARG( pqPipelineSource* , source ) );
    }
  }
}
