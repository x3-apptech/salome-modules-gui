#include "VTKViewer_ViewManager.h"
#include "VTKViewer_ViewModel.h"

int VTKViewer_ViewManager::_VTKViewMgr_Id = 0;

//***************************************************************
VTKViewer_ViewManager::VTKViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop ) 
: SUIT_ViewManager( study, theDesktop )
{
  myId = ++_VTKViewMgr_Id;
  setViewModel( new VTKViewer_Viewer() );
}

//***************************************************************
VTKViewer_ViewManager::~VTKViewer_ViewManager()
{
}

//***************************************************************
void VTKViewer_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption( tr( "VTK_VIEW_TITLE" ).arg( myId ).arg( aPos + 1 ) );
}
