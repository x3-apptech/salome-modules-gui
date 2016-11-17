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
// Author : Adrien Bruneton (CEA)

#include "PVViewer_ViewWindow.h"
#include "PVViewer_Core.h"
#include "PVViewer_ViewModel.h"
#include "PVViewer_GUIElements.h"
#include "PVViewer_ViewManager.h"

#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_Application.h>

#include <pqTabbedMultiViewWidget.h>
#include <pqApplicationCore.h>


/*!
  \class PVViewer_ViewWindow
  \brief PVGUI view window.
*/

/*!
  \brief Constructor.
  \param theDesktop parent desktop window
  \param theModel view model
*/
PVViewer_ViewWindow::PVViewer_ViewWindow( SUIT_Desktop* theDesktop, PVViewer_Viewer* theModel )
  : SUIT_ViewWindow( theDesktop ), myPVMgr( 0 )
{
  myDesktop = theDesktop;
  myModel = theModel;
  setViewManager(myModel->getViewManager());
  myPVMgr = qobject_cast<pqTabbedMultiViewWidget*>(pqApplicationCore::instance()->manager("MULTIVIEW_WIDGET"));
  if (myPVMgr) {
    SUIT_Application* app = SUIT_Session::session()->activeApplication();
    if ( app )
      app->addPostRoutine(&PVViewer_ViewWindow::removePVMgr);
    myPVMgr->setParent( this );
    // This is mandatory, see setParent() method in Qt 4 documentation
    myPVMgr->show();
    setCentralWidget( myPVMgr );

    // Finish ParaView set up: behaviors, connection and configurations.
    const QString configPath(PVViewer_ViewManager::GetPVConfigPath());
    PVViewer_Core::ParaviewInitBehaviors(true, theDesktop);
    PVViewer_ViewManager::ConnectToExternalPVServer(theDesktop);
    PVViewer_Core::ParaviewLoadConfigurations(configPath);

    // Hide toolbars
    PVViewer_GUIElements * pvge = PVViewer_GUIElements::GetInstance(myDesktop);
    pvge->setToolBarVisible(false);
  } else
    qDebug("No multiViewManager defined");
}

/*!
  \brief Destructor.
  As pqViewManager persists through the whole session,
  the destructor first removes it from the children of this PVViewer_ViewWindow
  to prevent its unexpected deletion.
*/
PVViewer_ViewWindow::~PVViewer_ViewWindow()
{
  if ( myPVMgr ) {
    // Hide toolbars
    PVViewer_GUIElements * pvge = PVViewer_GUIElements::GetInstance(myDesktop);
    pvge->setToolBarEnabled(false);
    myPVMgr->setParent( 0 );
    myPVMgr->hide();
    myPVMgr = 0;
    setCentralWidget( 0 );
  }
}

void PVViewer_ViewWindow::removePVMgr()
{
  pqTabbedMultiViewWidget* aPVMgr = qobject_cast<pqTabbedMultiViewWidget*>(pqApplicationCore::instance()->manager("MULTIVIEW_WIDGET"));
  delete aPVMgr;
}

/*!
  \brief Get the visual parameters of the view window.
  \return visual parameters of this view window formatted to the string
*/
QString PVViewer_ViewWindow::getVisualParameters()
{
  return SUIT_ViewWindow::getVisualParameters();
}

/*!
  \brief Restore visual parameters of the view window from the formated string
  \param parameters view window visual parameters
*/
void PVViewer_ViewWindow::setVisualParameters( const QString& parameters )
{
  SUIT_ViewWindow::setVisualParameters( parameters );
}


/*!
  \brief Returns the ParaView multi-view manager previously set with setMultiViewManager()
*/
pqTabbedMultiViewWidget* PVViewer_ViewWindow::getMultiViewManager() const
{
  return myPVMgr;
}

//void PVViewer_ViewWindow::onEmulateApply()
//{
//  emit this->applyRequest();
//}
