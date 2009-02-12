//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME SUPERVGraph : build Supervisor viewer into desktop
//  File   : SUPERVGraph_ViewFrame.cxx
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$
//
#include "SUPERVGraph_ViewFrame.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

//QT Include
#include <QVBoxLayout>
#include <QToolBar>

using namespace std;


/*!
  Constructor
*/
SUPERVGraph_View::SUPERVGraph_View( QWidget* theParent ): QWidget( theParent )
{
  init(theParent);
}

/*!
  Constructor
*/
SUPERVGraph_View::SUPERVGraph_View( SUPERVGraph_View* theParent ): QWidget( theParent )
{
  init(theParent);
}

/*!
  Builds popup for SUPERVGraph viewer
*/
void SUPERVGraph_View::contextMenuPopup( QMenu* )
{
  // to be implemented
}

/*!
  Initialization
*/
void SUPERVGraph_View::init( QWidget* theParent )
{
  if ( theParent->inherits( "QMainWindow" ) ) {
    ( ( QMainWindow* )theParent )->setCentralWidget( this );
  }
  else {
    QBoxLayout* layout = new QVBoxLayout( theParent );
    layout->addWidget( this );
  }

}

/*!
    Constructor
*/
SUPERVGraph_ViewFrame::SUPERVGraph_ViewFrame( SUIT_Desktop* theDesktop ) 
  : SUIT_ViewWindow( theDesktop )
{
  myView = 0;

  //myView = new SUPERVGraph_View(this); 
  // Set BackgroundColor
  /*
  int R = QAD_CONFIG->getSetting("SUPERVGraph:BackgroundColorRed").toInt();
  int G = QAD_CONFIG->getSetting("SUPERVGraph:BackgroundColorGreen").toInt();
  int B = QAD_CONFIG->getSetting("SUPERVGraph:BackgroundColorBlue").toInt();
  setBackgroundColor(QColor(R,G,B));*/

  myToolBar = new QToolBar(this);
  //myToolBar->setCloseMode(QDockWindow::Undocked);
  myToolBar->setWindowTitle(tr("LBL_TOOLBAR_LABEL"));
  createActions();
  createToolBar();
}

/*!
  Creates actions of SUPERVGraph view window
*/
void SUPERVGraph_ViewFrame::createActions()
{
  if (!myActionsMap.isEmpty()) return;
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QtxAction* aAction;

  // Panning
  aAction = new QtxAction(tr("MNU_PAN_VIEW"), aResMgr->loadPixmap( "SUPERVGraph", tr( "ICON_SUPERVGraph_PAN" ) ),
			  tr( "MNU_PAN_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_PAN_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewPan()));
  myActionsMap[ PanId ] = aAction;

  // Reset
  aAction = new QtxAction(tr("MNU_RESET_VIEW"), aResMgr->loadPixmap( "SUPERVGraph", tr( "ICON_SUPERVGraph_RESET" ) ),
			  tr( "MNU_RESET_VIEW" ), 0, this);
  aAction->setStatusTip(tr("DSC_RESET_VIEW"));
  connect(aAction, SIGNAL(activated()), this, SLOT(onViewReset()));
  myActionsMap[ ResetId ] = aAction;
}

/*!
  Creates toolbar of SUPERVGraph view window
*/
void SUPERVGraph_ViewFrame::createToolBar()
{
  myToolBar->addAction( myActionsMap[PanId] );
  myToolBar->addAction( myActionsMap[ResetId] );
}

/*!
  Destructor
*/
SUPERVGraph_ViewFrame::~SUPERVGraph_ViewFrame() {}

/*!
  Returns widget containing 3D-Viewer
*/
SUPERVGraph_View* SUPERVGraph_ViewFrame::getViewWidget()
{
  return myView;
}


/*!
  Sets new view widget
  \param theView - new view widget
*/
void SUPERVGraph_ViewFrame::setViewWidget( SUPERVGraph_View* theView )
{
  myView = theView;
}


/*!
  Display/hide Trihedron
*/
void SUPERVGraph_ViewFrame::onViewTrihedron()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewTrihedron" )
}

/*!
  Provides top projection of the active view
*/
void SUPERVGraph_ViewFrame::onViewTop()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewTop" )
}

/*!
  Provides bottom projection of the active view
*/
void SUPERVGraph_ViewFrame::onViewBottom()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewBottom" )
}

/*!
  Provides left projection of the active view
*/
void SUPERVGraph_ViewFrame::onViewLeft()    
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewLeft" )
}

/*!
  Provides right projection of the active view
*/
void SUPERVGraph_ViewFrame::onViewRight()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewRight" )
}

/*!
  Provides back projection of the active view
*/
void SUPERVGraph_ViewFrame::onViewBack()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewBack" )
}

/*!
  Provides front projection of the active view
*/
void SUPERVGraph_ViewFrame::onViewFront()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewFront" )
}

/*!
  Reset the active view
*/
void SUPERVGraph_ViewFrame::onViewReset()    
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewReset" )
    if (myView)
      myView->ResetView();
}

/*!
  Rotates the active view
*/
void SUPERVGraph_ViewFrame::onViewRotate()
{
//   MESSAGE ( "SUPERVGraph_ViewFrame::onViewRotate" )
}

/*!
  Sets a new center of the active view
*/
void SUPERVGraph_ViewFrame::onViewGlobalPan()
{
//   MESSAGE ( "SUPERVGraph_ViewFrame::onViewGlobalPan" )
}

/*!
  Zooms the active view
*/
void SUPERVGraph_ViewFrame::onViewZoom()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewZoom" )
}

/*!
  Moves the active view
*/
void SUPERVGraph_ViewFrame::onViewPan()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewPan" )
    if (myView != NULL)
      myView->ActivatePanning();
}

/*!
  Fits all obejcts within a rectangular area of the active view
*/
void SUPERVGraph_ViewFrame::onViewFitArea()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewFitArea" )
}

/*!
  Fits all objects in the active view
*/
void SUPERVGraph_ViewFrame::onViewFitAll()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onViewFitAll" )
}

/*!
    Set background of the viewport
*/
void SUPERVGraph_ViewFrame::setBackgroundColor( const QColor& color )
{
  if (myView) {
    QPalette palette;
    palette.setColor(myView->backgroundRole(), color);
    myView->setPalette(palette);
  }
}

/*!
    Returns background of the viewport
*/
QColor SUPERVGraph_ViewFrame::backgroundColor() const
{
  if (myView)
    return myView->palette().color( myView->backgroundRole() );
  return palette().color( backgroundRole() );
}

/*!
*/
void SUPERVGraph_ViewFrame::onAdjustTrihedron()
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::onAdjustTrihedron" )  
}

/*!
  Changes name of object
  \param obj - object to be renamed
  \param name - new name
*/
void SUPERVGraph_ViewFrame::rename( const Handle(SALOME_InteractiveObject)& IObject, 
				    QString newName )
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::rename" )
}

/*!
  Unhilights all object in viewer
  \param updateviewer - update current viewer
*/
void SUPERVGraph_ViewFrame::unHighlightAll() 
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::unHighlightAll" )
}

/*!
  Hilights/unhilights object in viewer
  \param obj - object to be updated
  \param hilight - if it is true, object will be hilighted, otherwise it will be unhilighted
  \param update - update current viewer
*/
void SUPERVGraph_ViewFrame::highlight( const Handle(SALOME_InteractiveObject)& IObject, 
				       bool highlight, bool immediatly ) 
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::highlight" )
}

/*!
  \return true if object is in viewer or in collector
  \param obj - object to be checked
  \param onlyInViewer - search object only in viewer (so object must be displayed)
*/
bool SUPERVGraph_ViewFrame::isInViewer( const Handle(SALOME_InteractiveObject)& IObject ) 
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::isInViewer" )
  return false;
}

/*!
  \return true if object is displayed in viewer
  \param obj - object to be checked
*/
bool SUPERVGraph_ViewFrame::isVisible( const Handle(SALOME_InteractiveObject)& IObject ) 
{
//  MESSAGE ( "SUPERVGraph_ViewFrame::isVisible" )
  return false;
}

/*!
  Custom resize event handler
*/
void SUPERVGraph_ViewFrame::resizeEvent( QResizeEvent* theEvent )
{
  QMainWindow::resizeEvent( theEvent );
  if ( myView ) myView->resizeView( theEvent );
}
