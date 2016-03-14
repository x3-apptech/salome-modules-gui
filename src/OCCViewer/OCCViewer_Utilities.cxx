// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// internal includes
#include "OCCViewer_Utilities.h"
#include "OCCViewer_ViewFrame.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewPort3d.h"

#include "SUIT_ViewManager.h"
#include "QtxActionToolMgr.h"
#include "QtxMultiAction.h"

// KERNEL includes
#include <Basics_OCCTVersion.hxx>

// OCC includes
#include <V3d_View.hxx>

// QT includes
#include <QImage>
#include <QAction>
#include <QDialog>

Handle(Image_PixMap) OCCViewer_Utilities::imageToPixmap( const QImage& anImage )
{
  Handle(Image_PixMap) aPixmap = new Image_PixMap();
  if ( !anImage.isNull() ) {
    aPixmap->InitTrash( Image_PixMap::ImgBGRA, anImage.width(), anImage.height() );
    aPixmap->SetTopDown( Standard_True );

    const uchar* aImageBytes = anImage.bits();
      
    for ( int aLine = anImage.height() - 1; aLine >= 0; --aLine ) {
#if OCC_VERSION_LARGE > 0x06070100
      // convert pixels from ARGB to renderer-compatible RGBA
      for ( int aByte = 0; aByte < anImage.width(); ++aByte ) {
	    Image_ColorBGRA& aPixmapBytes = aPixmap->ChangeValue<Image_ColorBGRA>(aLine, aByte);

	    aPixmapBytes.b() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes.g() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes.r() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes.a() = (Standard_Byte) *aImageBytes++;
	  }
#else
	  Image_ColorBGRA* aPixmapBytes = aPixmap->EditData<Image_ColorBGRA>().ChangeRow(aLine);
	
      // convert pixels from ARGB to renderer-compatible RGBA
      for ( int aByte = 0; aByte < anImage.width(); ++aByte ) {
	    aPixmapBytes->b() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes->g() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes->r() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes->a() = (Standard_Byte) *aImageBytes++;
	    aPixmapBytes++;
      }
#endif
    }
  }
  return aPixmap;
}

OCCViewer_ViewWindow::Mode2dType OCCViewer_Utilities::setViewer2DMode
                                         ( OCCViewer_Viewer* theViewer,
                                           const OCCViewer_ViewWindow::Mode2dType& theMode )
{
  OCCViewer_ViewWindow::Mode2dType anOldMode = OCCViewer_ViewWindow::No2dMode;
  OCCViewer_ViewFrame* aFrame = dynamic_cast<OCCViewer_ViewFrame*>
                                     ( theViewer->getViewManager()->getActiveView() );
  OCCViewer_ViewWindow* aView = aFrame ? aFrame->getView( OCCViewer_ViewFrame::MAIN_VIEW ) : 0;
  if ( !aView )
    return anOldMode;

  // set a view mode
  anOldMode = aView->get2dMode();
  aView->set2dMode( theMode );
  bool is2dMode = theMode != OCCViewer_ViewWindow::No2dMode;

  // enable/disable view actions
  QList<int> aNo2dActions;
  aNo2dActions << OCCViewer_ViewWindow::ChangeRotationPointId
               << OCCViewer_ViewWindow::RotationId
               << OCCViewer_ViewWindow::FrontId
               << OCCViewer_ViewWindow::BackId
               //<< OCCViewer_ViewWindow::TopId
               << OCCViewer_ViewWindow::BottomId
               << OCCViewer_ViewWindow::LeftId
               << OCCViewer_ViewWindow::RightId
               << OCCViewer_ViewWindow::AntiClockWiseId
               << OCCViewer_ViewWindow::ClockWiseId
               << OCCViewer_ViewWindow::OrthographicId
               << OCCViewer_ViewWindow::PerspectiveId
               << OCCViewer_ViewWindow::ResetId;

  QtxActionToolMgr* aToolMgr = aView->toolMgr();
  QAction* anAction;
  for ( int i = 0, aNb = aNo2dActions.size(); i < aNb; i++ ) {
    anAction = aToolMgr->action( aNo2dActions[i] );
    if ( anAction )
      anAction->setEnabled( !is2dMode );
  }
  QAction* aTop = aToolMgr->action( OCCViewer_ViewWindow::TopId );
  QtxMultiAction* aMulti = dynamic_cast<QtxMultiAction*>( aTop->parent() );
  aMulti->setActiveAction( aTop );

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

  return anOldMode;
}

bool OCCViewer_Utilities::isDialogOpened( OCCViewer_ViewWindow* theView, const QString& theName )
{
  bool isFound = false;
  OCCViewer_ViewFrame* aViewFrame = dynamic_cast<OCCViewer_ViewFrame*>( theView->parent()->parent() );
  QList<QDialog*> allDialogs = aViewFrame->findChildren<QDialog*>();
  foreach ( QDialog* d, allDialogs )
    if ( d->objectName() == theName )
      isFound = true;
  return isFound;
}
