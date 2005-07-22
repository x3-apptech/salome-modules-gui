#include "VTKViewer_ViewManager.h"
#include "VTKViewer_ViewModel.h"

int VTKViewer_ViewManager::_VTKViewMgr_Id = 0;

/*!Constructor.Initialize SIUT_ViewManager by \a study and \a theDesktop.
 * Create new instance of VTKViewer_Viewer and set view model by it.
 */
VTKViewer_ViewManager::VTKViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop ) 
: SUIT_ViewManager( study, theDesktop )
{
  myId = ++_VTKViewMgr_Id;
  setViewModel( new VTKViewer_Viewer() );
}

/*!Destructor.*/
VTKViewer_ViewManager::~VTKViewer_ViewManager()
{
  /*!Do nothing.*/
}

/*!Sets view window name*/
void VTKViewer_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption( tr( "VTK_VIEW_TITLE" ).arg( myId ).arg( aPos + 1 ) );
}
