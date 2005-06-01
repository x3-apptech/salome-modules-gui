#include "SVTK_ViewManager.h"
#include "SVTK_ViewModel.h"

#include <VTKViewer_ViewManager.h>

int SVTK_ViewManager::_SVTKViewMgr_Id = 0;

//***************************************************************
SVTK_ViewManager::SVTK_ViewManager( SUIT_Study* study, 
		    SUIT_Desktop* theDesktop ) 
: SUIT_ViewManager( study, theDesktop )
{
  myId = ++_SVTKViewMgr_Id;
  setViewModel( new SVTK_Viewer() );
}

//***************************************************************
SVTK_ViewManager::~SVTK_ViewManager()
{
}

//***************************************************************
void SVTK_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption( VTKViewer_ViewManager::tr( "VTK_VIEW_TITLE" ).arg( myId ).arg( aPos + 1 ) );
}
