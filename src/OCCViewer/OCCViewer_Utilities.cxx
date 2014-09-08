
#include "OCCViewer_Utilities.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewPort3d.h"

#include "SUIT_ViewManager.h"
#include "QtxActionToolMgr.h"

#include <V3d_View.hxx>

#include <QAction>

void OCCViewer_Utilities::setViewer2DMode( OCCViewer_Viewer* theViewer,
                                           const OCCViewer_ViewWindow::Mode2dType& theMode )
{
  OCCViewer_ViewFrame* aFrame = dynamic_cast<OCCViewer_ViewFrame*>
                                     ( theViewer->getViewManager()->getActiveView() );
  OCCViewer_ViewWindow* aView = aFrame ? aFrame->getView( OCCViewer_ViewFrame::MAIN_VIEW ) : 0;
  if ( !aView )
    return;

  // set a view mode
  aView->set2dMode( theMode );
  bool is2dMode = theMode != OCCViewer_ViewWindow::No2dMode;

  // enable/disable view actions
  QList<int> aNo2dActions;
  aNo2dActions << OCCViewer_ViewWindow::ChangeRotationPointId
               << OCCViewer_ViewWindow::RotationId
               << OCCViewer_ViewWindow::FrontId
               << OCCViewer_ViewWindow::BackId
               << OCCViewer_ViewWindow::TopId
               << OCCViewer_ViewWindow::BottomId
               << OCCViewer_ViewWindow::LeftId
               << OCCViewer_ViewWindow::RightId
               << OCCViewer_ViewWindow::AntiClockWiseId
               << OCCViewer_ViewWindow::ClockWiseId
               << OCCViewer_ViewWindow::ResetId;

  QtxActionToolMgr* aToolMgr = aView->toolMgr();
  QAction* anAction;
  for ( int i = 0, aNb = aNo2dActions.size(); i < aNb; i++ ) {
    anAction = aToolMgr->action( aNo2dActions[i] );
    if ( anAction )
      anAction->setEnabled( !is2dMode );
  }

  // change view position
  Handle(V3d_View) aView3d = aView->getViewPort()->getView();
  if ( !aView3d.IsNull() ) {
    switch ( theMode ) {
      case OCCViewer_ViewWindow::XYPlane:
        aView3d->SetProj (V3d_Zpos);
        break;
      case OCCViewer_ViewWindow::XZPlane:
        aView3d->SetProj (V3d_Yneg);
        break;
      case OCCViewer_ViewWindow::YZPlane:
        aView3d->SetProj (V3d_Xpos);
        break;
    }
  }
}
