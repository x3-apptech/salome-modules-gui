#include "SUPERVGraph_ViewModel.h"
#include "SUPERVGraph_ViewFrame.h"
#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"

SUPERVGraph_Viewer::SUPERVGraph_Viewer()
:SUIT_ViewModel() 
{
}

SUPERVGraph_Viewer::~SUPERVGraph_Viewer() 
{
}

SUIT_ViewWindow* SUPERVGraph_Viewer::createView(SUIT_Desktop* theDesktop)
{
  SUPERVGraph_ViewFrame* aRes = new SUPERVGraph_ViewFrame( theDesktop );
  return aRes;
}
