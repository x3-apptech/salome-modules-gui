#include "VTKViewer_ViewManager.h"
#include "VTKViewer_ViewModel.h"

//***************************************************************
VTKViewer_ViewManager::VTKViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop ) 
: SUIT_ViewManager( study, theDesktop )
{
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
  theView->setCaption( QString( "VTK viewer:%1" ).arg(aPos+1));
}
