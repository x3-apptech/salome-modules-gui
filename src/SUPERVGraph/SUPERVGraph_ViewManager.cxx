#include "SUPERVGraph_ViewManager.h"

SUPERVGraph_ViewManager::SUPERVGraph_ViewManager( SUIT_Study* theStudy, 
						  SUIT_Desktop* theDesktop, 
						  SUIT_ViewModel* theViewModel )
  : SUIT_ViewManager( theStudy, theDesktop, theViewModel )
{
}

SUPERVGraph_ViewManager::~SUPERVGraph_ViewManager()
{
}

void SUPERVGraph_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption( QString( "SUPERVISION  - viewer:%1" ).arg(aPos+1));
}

void SUPERVGraph_ViewManager::contextMenuPopup( QPopupMenu* thePopup)
{
  SUIT_ViewManager::contextMenuPopup( thePopup );
  // to be implemented
}
