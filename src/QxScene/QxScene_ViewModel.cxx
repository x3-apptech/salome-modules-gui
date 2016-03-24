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

#include "QxScene_ViewModel.h"
#include "QxScene_ViewWindow.h"
#include "QxScene_ViewManager.h"
#include "QxScene_Def.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_Session.h"

#include <qcolordialog.h>
#include <QMenu>
#include <QToolBar>

//#define _DEVDEBUG_
#include "DebTrace.hxx"

/*!
  Constructor
*/
QxScene_Viewer::QxScene_Viewer()
  :SUIT_ViewModel()
{
  DEBTRACE("QxScene_Viewer::QxScene_Viewer");
}

/*!
  Destructor
*/
QxScene_Viewer::~QxScene_Viewer() 
{
  DEBTRACE("QxScene_Viewer::~QxScene_Viewer");
}

void QxScene_Viewer::setViewManager( SUIT_ViewManager* mgr)
{
  DEBTRACE("QxScene_Viewer::setViewManager");
   SUIT_ViewModel::setViewManager( mgr );
}

/*!
  Start initialization of view window
  \param view - view window to be initialized
*/
void QxScene_Viewer::initView( QxScene_ViewWindow* view )
{
  DEBTRACE("QxScene_Viewer::initView");
  if ( view )
  {
    view->initLayout();
  }
}

/*!
  Creates new view window
  \param theDesktop - main window of application
*/
SUIT_ViewWindow* QxScene_Viewer::createView(SUIT_Desktop* theDesktop)
{
  DEBTRACE("QxScene_Viewer::createView");
  QxScene_ViewWindow* aRes = new QxScene_ViewWindow( theDesktop, this );
  initView( aRes );
  return aRes;
}

/*!
  Builds popup for QxScene viewer
*/
void QxScene_Viewer::contextMenuPopup(QMenu* thePopup)
{
  DEBTRACE("QxScene_Viewer::contextMenuPopup");
}

/*!
  SLOT: called if background color is to be changed changed, passes new color to current canvas view
*/
void QxScene_Viewer::onChangeBgColor()
{
  QxScene_ViewWindow* aView = (QxScene_ViewWindow*)(myViewManager->getActiveView());
  if( !aView )
    return;
  QColor aColorActive = aView->backgroundColor();

  QColor selColor = QColorDialog::getColor( aColorActive, aView);
  if ( selColor.isValid() )
    aView->setBackgroundColor(selColor);
}

/*!
  SLOT: called when popup item "Show toolbar" is activated, shows toolbar of active view window
*/
void QxScene_Viewer::onShowToolbar()
{
  QxScene_ViewWindow* aView = (QxScene_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
}
