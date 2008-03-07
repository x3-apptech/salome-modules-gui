// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "QxGraph_ViewModel.h"
#include "QxGraph_ViewWindow.h"
#include "QxGraph_Canvas.h"
#include "QxGraph_CanvasView.h"
#include "QxGraph_Def.h"
#include "QxGraph_Prs.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_Session.h"

#include <qcolordialog.h>
#include <qpopupmenu.h>
#include <qcanvas.h> // for addRectangleItem(...) method (can be removed in the future)

/*!
  Constructor
*/
QxGraph_Viewer::QxGraph_Viewer()
  :SUIT_ViewModel(),
   myCanvas(0),
   myCurrentView(0)
{
  printf("Construct QxGraph_Viewer\n");
  //myCanvases.setAutoDelete(true);
  //myCanvasViews.setAutoDelete(true);
}

/*!
  Destructor
*/
QxGraph_Viewer::~QxGraph_Viewer() 
{
  if ( myCanvas ) delete myCanvas;
  //if ( !myCanvases.isEmpty() ) myCanvases.clear();
  if ( !myCanvasViews.isEmpty() ) myCanvasViews.clear();
  myCurrentView = 0;
}

/*!
  Start initialization of view window
  \param view - view window to be initialized
*/
void QxGraph_Viewer::initView( QxGraph_ViewWindow* view )
{
  if ( view )
  {
    view->initLayout();
    
    /*
    // test add items into the current canvas view
    QRect aRect(100,200,200,100);
    QCanvasItem* aRectItem = aPrs->addRectangleItem(aRect);
    //delete aRectItem;

    QPointArray aPA(6);
    aPA.putPoints(0, 6, 400,100, 500,70, 600,100, 600,200, 500,230, 400,200);
    QCanvasItem* aPolyItem = aPrs->addPolygonItem(aPA);
    //delete aPolyItem;

    QPoint aStart(500,300), aEnd(700,250);
    QCanvasItem* aLineItem = aPrs->addLineItem(aStart, aEnd);
    //delete aLineItem;

    QCanvasItem* aEllipseItem = aPrs->addEllipseItem(200, 100, 30*16, 120*16);
    aEllipseItem->setX(400);
    aEllipseItem->setY(400);
    //delete aEllipseItem;

    QCanvasItem* aTextItem = aPrs->addTextItem("This is a QCanvasText item");
    aTextItem->setX(100);
    aTextItem->setY(500);
    //delete aTextItem;
    */
  }
}

/*!
  Creates new view window
  \param theDesktop - main window of application
*/
SUIT_ViewWindow* QxGraph_Viewer::createView(SUIT_Desktop* theDesktop)
{
  QxGraph_ViewWindow* aRes = new QxGraph_ViewWindow( theDesktop, this );
  initView( aRes );
  return aRes;
}

/*!
  Set view with index theIndex from myCanvasViews as current view
  \param theIndex - the index of the view in the list
*/
void QxGraph_Viewer::setCurrentView(int theIndex)
{
  if ( theIndex >= 0 && theIndex < myCanvasViews.count() )
  {
    myCurrentView = myCanvasViews.at(theIndex);
    myViewManager->getActiveView()->setCentralWidget(myCurrentView);
  }
}

/*!
  Builds popup for QxGraph viewer
*/
void QxGraph_Viewer::contextMenuPopup(QPopupMenu* thePopup)
{
  printf("QxGraph_Viewer::contextMenuPopup\n");
  thePopup->insertItem( tr( "MEN_CHANGE_BACKGROUND" ), this, SLOT( onChangeBgColor() ) );

  thePopup->insertSeparator();

  QxGraph_ViewWindow* aView = (QxGraph_ViewWindow*)(myViewManager->getActiveView());
  if ( aView && !aView->getToolBar()->isVisible() )
    thePopup->insertItem( tr( "MEN_SHOW_TOOLBAR" ), this, SLOT( onShowToolbar() ) );
}

/*!
  SLOT: called if background color is to be changed changed, passes new color to current canvas view
*/
void QxGraph_Viewer::onChangeBgColor()
{
  QxGraph_ViewWindow* aView = (QxGraph_ViewWindow*)(myViewManager->getActiveView());
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
void QxGraph_Viewer::onShowToolbar() {
  QxGraph_ViewWindow* aView = (QxGraph_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
}
