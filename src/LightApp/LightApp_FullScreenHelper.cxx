// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:      LightApp_FullScreenHelper.h
// Created:   04/10/2011 18:44:25 PM
// Author:    OCC team
//
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>

#include <PyConsole_Console.h>

#include <STD_Application.h>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_DataBrowser.h>

#include "LightApp_FullScreenHelper.h"
#include "LightApp_Application.h"


/*!
 * Constructor
 */
LightApp_FullScreenHelper::LightApp_FullScreenHelper() 
{
  myStatusBarVisibility = false;
}

/*!
 * Destructor
 */
LightApp_FullScreenHelper::~LightApp_FullScreenHelper()
{
  
}


/*!
 * Switch application to the full screen mode.
 */
void LightApp_FullScreenHelper::switchToFullScreen() {
  
  SUIT_Session* session = SUIT_Session::session();
  if(!session)
    return;
  
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( session->activeApplication() );

  if(!app)
    return;
  
  SUIT_Desktop* desktop = app->desktop();

  if(!desktop)
    return;
  
  desktop->setWindowState(desktop->windowState() ^ Qt::WindowFullScreen);

  if(desktop->menuBar())
    desktop->menuBar()->hide();

  if(desktop->statusBar()) {
    myStatusBarVisibility = desktop->statusBar()->isVisible();
    desktop->statusBar()->hide();
    QAction *act = app->action(STD_Application::ViewStatusBarId);
    if(act)
      act->setEnabled(false);
  }


  QList<QDockWidget*> aDocWidgets = desktop->findChildren<QDockWidget*>();
  myDocWidgetMap.clear();

  QWidget* ob = app->objectBrowser();
  QObject* obParent = ob ? ob->parent() : 0;

  foreach(QDockWidget* aWidget, aDocWidgets) {
    if(aWidget && aWidget->parent() == desktop) {
      if( aWidget->isVisible() && aWidget != obParent ) {
	aWidget->hide();
	QAction* act = aWidget->toggleViewAction();
	bool isActionEnabled = false;
	if(act) {
	  isActionEnabled = act->isEnabled();
	  if( isActionEnabled ) {
	    act->setEnabled(false);
	  }
	}
	
	myDocWidgetMap.insert(aWidget, isActionEnabled);
      }
    }    
  }
  
  QList<QToolBar*> aToolBars = desktop->findChildren<QToolBar*>();
  myToolBarMap.clear();
  foreach(QToolBar* aWidget, aToolBars )  {
    if( aWidget && aWidget->parent() == desktop ) {
      if( aWidget->isVisible()) {
	aWidget->hide();
	QAction* act = aWidget->toggleViewAction();
	bool isActionEnabled = false;
	if(act) {
	  isActionEnabled = act->isEnabled();
	  if( isActionEnabled ) {
	    act->setEnabled(false);
	  }
	}	
	myToolBarMap.insert(aWidget, isActionEnabled);
      }
    }    
  }  
}

/*!
 * Switch application to the normal screen mode.
 */
void LightApp_FullScreenHelper::switchToNormalScreen() {

  SUIT_Session* session = SUIT_Session::session();
  if(!session)
    return;
  
  LightApp_Application* app = dynamic_cast<LightApp_Application*>( session->activeApplication() );

  if(!app)
    return;
  
  SUIT_Desktop* desktop = app->desktop();

  if(!desktop)
    return;
  
  desktop->setWindowState(desktop->windowState() ^ Qt::WindowFullScreen);


  DocWidgetMap::iterator it = myDocWidgetMap.begin();
  for( ;it != myDocWidgetMap.end() ; it++ ) {
    QDockWidget* aWidget = it.key();
    bool state = it.value();
    aWidget->show();
    QAction* act = aWidget->toggleViewAction();
    if(act && state)
      act->setEnabled(true);
  }

  ToolBarMap::iterator it1 = myToolBarMap.begin();
  for( ;it1 != myToolBarMap.end() ; it1++ ) {
    QToolBar* aWidget = it1.key();
    bool state = it1.value();
    aWidget->show();
    QAction* act = aWidget->toggleViewAction();
    if(act && state)
      act->setEnabled(true);
  }

  if(desktop->menuBar())
    desktop->menuBar()->show();

  if(desktop->statusBar() && myStatusBarVisibility) {
    desktop->statusBar()->show();
    QAction *act = app->action(STD_Application::ViewStatusBarId);
    if(act)
      act->setEnabled(true);
  }
  
}
