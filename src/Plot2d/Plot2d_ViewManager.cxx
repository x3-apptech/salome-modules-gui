#include "Plot2d_ViewManager.h"
#include "Plot2d_ViewModel.h"
#include "Plot2d_ViewWindow.h"

int Plot2d_ViewManager::myMaxId = 0;

Plot2d_ViewManager::Plot2d_ViewManager( SUIT_Study* study, SUIT_Desktop* desk ) 
: SUIT_ViewManager( study, desk )
{
  myId = ++myMaxId;
  Plot2d_Viewer* v = new Plot2d_Viewer();
  setViewModel( v );
}

Plot2d_ViewManager::~Plot2d_ViewManager()
{
}

Plot2d_Viewer* Plot2d_ViewManager::getPlot2dModel() const
{
  return (Plot2d_Viewer*)myViewModel;
}

void Plot2d_ViewManager::setViewName( SUIT_ViewWindow* theView )
{
  int aPos = myViews.find(theView);
  theView->setCaption( QString( "Plot2d scene:%1 - viewer:%2" ).arg(myId).arg(aPos+1));
}

bool Plot2d_ViewManager::insertView( SUIT_ViewWindow* theView )
{
  bool res = SUIT_ViewManager::insertView( theView );
  if ( res )
  {
    Plot2d_ViewWindow* view = (Plot2d_ViewWindow*)theView;
    connect( view, SIGNAL( cloneView() ), this, SLOT( onCloneView() ) );
  }
  return res;
}

void Plot2d_ViewManager::createView()
{
  createViewWindow();
}

void Plot2d_ViewManager::onCloneView()
{
  SUIT_ViewWindow* vw = createViewWindow();

  Plot2d_ViewWindow  *newWnd = 0, *clonedWnd = 0;
  if( vw && vw->inherits( "Plot2d_ViewWindow" ) )
    newWnd = ( Plot2d_ViewWindow* )vw;
  if( sender() && sender()->inherits( "Plot2d_ViewWindow" ) )
    clonedWnd = ( Plot2d_ViewWindow* )sender();
  
  if( newWnd && clonedWnd )
    emit cloneView( clonedWnd->getViewFrame(), newWnd->getViewFrame() );
}
