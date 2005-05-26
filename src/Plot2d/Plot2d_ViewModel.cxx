// Plot2d_ViewModel.cxx: implementation of the Plot2d_ViewModel class.
//
//////////////////////////////////////////////////////////////////////

#include "Plot2d_ViewModel.h"
#include "Plot2d_ViewWindow.h"


Plot2d_Viewer::Plot2d_Viewer(bool theAutoDel)
:SUIT_ViewModel() 
{
  myPrs = 0;
  myAutoDel = theAutoDel;
}

Plot2d_Viewer::~Plot2d_Viewer()
{
  if (myPrs)
    clearPrs();
}

//*********************************************************************
SUIT_ViewWindow* Plot2d_Viewer::createView(SUIT_Desktop* theDesktop)
{
  Plot2d_ViewWindow* aPlot2dView = new Plot2d_ViewWindow(theDesktop, this);
  if (myPrs)
    aPlot2dView->getViewFrame()->Display(myPrs);
  return aPlot2dView;
}

//*********************************************************************
void Plot2d_Viewer::contextMenuPopup(QPopupMenu* thePopup)
{
  Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->contextMenuPopup(thePopup);

  if (thePopup->count() > 0) thePopup->insertSeparator();
  thePopup->insertItem("Change background...", this, SLOT(onChangeBgColor()));

  if ( aView ) {
    if ( !aView->getToolBar()->isVisible() ) {
      if (thePopup->count() > 0) thePopup->insertSeparator();
        thePopup->insertItem("Show toolbar", this, SLOT(onShowToolbar()));
    }
  }
}


//*********************************************************************
void Plot2d_Viewer::setPrs(Plot2d_Prs* thePrs) 
{
  if (myPrs)
    clearPrs();
  myPrs = thePrs;
  myPrs->setAutoDel(myAutoDel);
}

//*********************************************************************
void Plot2d_Viewer::update()
{
  SUIT_ViewManager* aMgr = getViewManager();
  QPtrVector<SUIT_ViewWindow> aViews = aMgr->getViews();
  unsigned int aSize = aViews.size();
  for (uint i = 0; i < aSize; i++) {
    Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)aViews[i];
    if (myPrs && aView)
      aView->getViewFrame()->Display(myPrs);
  }
}

//*********************************************************************
void Plot2d_Viewer::clearPrs()
{
  SUIT_ViewManager* aMgr = getViewManager();
  QPtrVector<SUIT_ViewWindow> aViews = aMgr->getViews();
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

//*********************************************************************
void Plot2d_Viewer::setAutoDel(bool theDel)
{
  myAutoDel = theDel;
  if (myPrs)
    myPrs->setAutoDel(theDel);
}

//*********************************************************************
void Plot2d_Viewer::onChangeBgColor()
{
  Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)(myViewManager->getActiveView());
  if( !aView )
    return;
  Plot2d_ViewFrame* aViewFrame = aView->getViewFrame();
  aViewFrame->onChangeBackground();
}

//*********************************************************************
void Plot2d_Viewer::onShowToolbar() {
  Plot2d_ViewWindow* aView = (Plot2d_ViewWindow*)(myViewManager->getActiveView());
  if ( aView )
    aView->getToolBar()->show();    
}
