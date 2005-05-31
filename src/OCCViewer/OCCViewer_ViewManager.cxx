#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ViewWindow.h"
#include "SUIT_Desktop.h"

int OCCViewer_ViewManager::myMaxId = 0;

//***************************************************************/
OCCViewer_ViewManager::OCCViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop, bool DisplayTrihedron )
: SUIT_ViewManager( study, theDesktop )
{
  myId = ++myMaxId;
  setViewModel( new OCCViewer_Viewer( DisplayTrihedron ) );
}

//***************************************************************/
OCCViewer_ViewManager::~OCCViewer_ViewManager()
{
}

//***************************************************************/
void OCCViewer_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption( QString( "OCC scene:%1 - viewer:%2" ).arg(myId).arg(aPos+1));
}

//***************************************************************/
void OCCViewer_ViewManager::contextMenuPopup( QPopupMenu* popup )
{
  SUIT_ViewManager::contextMenuPopup( popup );
  // if it is necessary invoke method CreatePopup of ViewPort
  // be sure that existing QPopupMenu menu is used for that.
}
