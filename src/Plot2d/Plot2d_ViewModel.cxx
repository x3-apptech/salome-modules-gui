// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// Plot2d_ViewModel.cxx: implementation of the Plot2d_ViewModel class.
//
#include "Plot2d_ViewModel.h"
#include "Plot2d_ViewWindow.h"
#include "Plot2d_ViewManager.h"
#include "Plot2d_ViewFrame.h"
#include "Plot2d_Prs.h"

#include <QMenu>
#include <QToolBar>
#include <QVector>

/*!
  Constructor
*/
Plot2d_Viewer::Plot2d_Viewer(bool theAutoDel)
:SUIT_ViewModel() 
{
  myPrs = 0;
  myAutoDel = theAutoDel;
  Plot2d_Object::setSelectionColor( QColor(80,80,80) );
  Plot2d_Object::setHighlightedLegendTextColor( QColor(255,255,255) );
}

/*!
  Destructor
*/
Plot2d_Viewer::~Plot2d_Viewer()
{
  if (myPrs)
    clearPrs();
}

/*!
  Create new instance of view window on desktop \a theDesktop.
  \retval SUIT_ViewWindow* - created view window pointer.
*/
SUIT_ViewWindow* Plot2d_Viewer::createView(SUIT_Desktop* theDesktop)
{
  Plot2d_ViewWindow* aView = new Plot2d_ViewWindow(theDesktop, this);
  aView->initLayout();
  if (myPrs)
    aView->getViewFrame()->Display(myPrs);
  return aView;
}

/*!
  Adds custom items to popup menu
  \param thePopup - popup menu
*/
void Plot2d_Viewer::contextMenuPopup(QMenu* thePopup)
{
  Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->contextMenuPopup(thePopup);

  if (!thePopup->isEmpty())
    thePopup->addSeparator();
  thePopup->addAction( tr( "MNU_DUMP_VIEW" ),                this, SLOT(onDumpView()));
  thePopup->addAction( tr( "MEN_PLOT2D_CHANGE_BACKGROUND" ), this, SLOT(onChangeBgColor()));

  if ( aView ) {
    if ( !aView->getToolBar()->isVisible() ) {
      if (!thePopup->isEmpty())
        thePopup->addSeparator();
      thePopup->addAction("Show toolbar", this, SLOT(onShowToolbar()));
    }
    aView->RefreshDumpImage();
  }
}

/*!
  Sets presentation of viewer
  \param thePrs - new presentation
*/
void Plot2d_Viewer::setPrs(Plot2d_Prs* thePrs) 
{
  if (myPrs)
    clearPrs();
  myPrs = thePrs;
  myPrs->setAutoDel(myAutoDel);
}

/*!
  Updates current viewer
*/
void Plot2d_Viewer::update()
{
  SUIT_ViewManager* aMgr = getViewManager();
  QVector<SUIT_ViewWindow*> aViews = aMgr->getViews();
  unsigned int aSize = aViews.size();
  for (uint i = 0; i < aSize; i++) {
    Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)aViews[i];
    if (myPrs && aView)
      aView->getViewFrame()->Display(myPrs);
  }
}

/*!
  Clear viewer presentation
*/
void Plot2d_Viewer::clearPrs()
{
  SUIT_ViewManager* aMgr = getViewManager();
  QVector<SUIT_ViewWindow*> aViews;
  if ( aMgr )
    aViews = aMgr->getViews();
  unsigned int aSize = aViews.size();
  for (uint i = 0; i < aSize; i++) {
    Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)aViews[i];
    if (myPrs && aView)
      aView->getViewFrame()->Erase(myPrs);
  }
  if (myAutoDel && myPrs) {
    delete myPrs;
  }
  myPrs = 0;
}

/*!
  Sets "auto delete" state of of presentation
  \param theDel - new state
*/
void Plot2d_Viewer::setAutoDel(bool theDel)
{
  myAutoDel = theDel;
  if (myPrs)
    myPrs->setAutoDel(theDel);
}

/*!
  SLOT: called when action "Change background" is activated
*/
void Plot2d_Viewer::onChangeBgColor()
{
  Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)(myViewManager->getActiveView());
  if( !aView )
    return;
  Plot2d_ViewFrame* aViewFrame = aView->getViewFrame();
  aViewFrame->onChangeBackground();
}

/*!
  SLOT: called when action "Show toolbar" is activated
*/
void Plot2d_Viewer::onShowToolbar() {
  Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
}

/*!
  SLOT: called when action "Dump view" is activated
*/
void Plot2d_Viewer::onDumpView()
{
  Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->onDumpView();    
}

/*!
  SLOT: called when action "Clone view" is activated
*/
void Plot2d_Viewer::onCloneView( Plot2d_ViewFrame* clonedVF, Plot2d_ViewFrame* newVF )
{
  if( !clonedVF || !newVF )
    return;

  // 1) Copy all properties of view

  newVF->copyPreferences( clonedVF );

  // 2) Display all curves displayed in cloned view

  curveList aCurves;
  clonedVF->getCurves( aCurves );
  curveList::const_iterator anIt = aCurves.begin(), aLast = aCurves.end();

  for( ; anIt!=aLast; anIt++ )
    if( clonedVF->isVisible( *anIt ) )
      newVF->displayCurve( *anIt, false );
  newVF->Repaint();
  
  if ( newVF )
  {
    // find view window corresponding to the frame 
    QWidget* p = newVF->parentWidget();
    while( p && !p->inherits( "SUIT_ViewWindow" ) )
      p = p->parentWidget();
    
    // emits signal
    if ( p && p->inherits( "SUIT_ViewWindow" ) )
      emit viewCloned( (SUIT_ViewWindow*)p );
  }
}

/*
  SLOT: called when clicked item in the legend from Plot2d_ViewManager
 */
void Plot2d_Viewer::onClicked(const QVariant& itemInfo, int index)
{
}

/*!
  Sets view manager
  \param mgr - new view manager
*/
void Plot2d_Viewer::setViewManager( SUIT_ViewManager* mgr )
{
  SUIT_ViewModel::setViewManager( mgr );
  if( mgr && mgr->inherits( "Plot2d_ViewManager" ) )
  {
    Plot2d_ViewManager* pmgr = ( Plot2d_ViewManager* )mgr;
    connect( pmgr, SIGNAL( cloneView( Plot2d_ViewFrame*, Plot2d_ViewFrame* ) ),
             this, SLOT( onCloneView( Plot2d_ViewFrame*, Plot2d_ViewFrame* ) ) );
  }
}
