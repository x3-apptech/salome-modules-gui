#include "Plot2d_ViewManager.h"
#include "Plot2d_ViewModel.h"

int Plot2d_ViewManager::myMaxId = 0;

Plot2d_ViewManager::Plot2d_ViewManager( SUIT_Study* study, SUIT_Desktop* desk ) 
: SUIT_ViewManager( study, desk )
{
  myId = ++myMaxId;
  setViewModel( new Plot2d_Viewer() );
}

Plot2d_ViewManager::~Plot2d_ViewManager()
{
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
    connect( view, SIGNAL( cloneView() ), this, SLOT( createView() ) );
  }
  return res;
}

void Plot2d_ViewManager::createView()
{
  createViewWindow();
}
