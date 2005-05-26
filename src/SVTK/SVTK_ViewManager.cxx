#include "SVTK_ViewManager.h"
#include "SVTK_ViewModel.h"

//***************************************************************
SVTK_ViewManager
::SVTK_ViewManager( SUIT_Study* study, 
		    SUIT_Desktop* theDesktop ) 
: SUIT_ViewManager( study, theDesktop )
{
  setViewModel( new SVTK_Viewer() );
}

//***************************************************************
SVTK_ViewManager
::~SVTK_ViewManager()
{
}

//***************************************************************
void 
SVTK_ViewManager
::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption( QString( "VTK viewer:%1" ).arg(aPos+1));
}
