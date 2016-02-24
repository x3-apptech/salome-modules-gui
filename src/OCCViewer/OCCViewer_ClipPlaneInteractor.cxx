// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "OCCViewer_ClipPlaneInteractor.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewModel.h"

#include <AIS_InteractiveContext.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <V3d_View.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_IntCS.hxx>
#include <gce_MakeDir.hxx>

#include <QMouseEvent>
#include <QKeyEvent>

/*!
  \brief Constructor.
  \param theVM [in] the view manager.
  \param theParent [in] the parent object.
*/
OCCViewer_ClipPlaneInteractor::OCCViewer_ClipPlaneInteractor( OCCViewer_ViewManager* theVM,
                                                              QObject* theParent )
: OCCViewer_ViewportInputFilter( theVM, theParent ),
  myPerformingOp( DragOperation_Undef ),
  myRotationCenter( gp::Origin() ),
  myIsDraggable( false ),
  myIsClickable( false )
{
}

/*!
  \brief Get sequence of planes to interact with.
  \return the sequence of accepted planes.
*/
const OCCViewer_ClipPlaneInteractor::SeqOfPlanes& OCCViewer_ClipPlaneInteractor::planes() const
{
  return myPlanes;
}

/*!
  \brief Sets sequence of planes allowed for interaction.
  \param thePlanes [in] the sequence of accepted planes.
*/
void OCCViewer_ClipPlaneInteractor::setPlanes( const SeqOfPlanes& thePlanes )
{
  myPlanes = thePlanes;
}

/*!
  \brief Sets center of rotation for the scene.
  \param theCenter [in] the center of rotation.
*/
void OCCViewer_ClipPlaneInteractor::setRotationCenter( const gp_Pnt& theCenter )
{
  myRotationCenter = theCenter;
}

/*!
  \brief Sets minimum and maximum bounding of the scene.
         The sliding movements are limited to minimum
         and maximum bounds.
  \param theMinMax [in] the minimum and maximum bounds.
*/
void OCCViewer_ClipPlaneInteractor::setMinMax( const Bnd_Box& theMinMax )
{
  myMinMax = theMinMax;
}

/*!
  \brief Enables or disables event processing within the viewer.
*/
void OCCViewer_ClipPlaneInteractor::setEnabled( const bool theIsEnabled )
{
  if ( !theIsEnabled )
  {
  }

  myMouseDragPln    = gp_Pln();
  myPerformingOp    = DragOperation_Undef;
  myPickPos         = QPoint();
  myDragPos         = QPoint();
  myInteractedPlane = NULL;
  myIsDraggable     = false;
  myIsClickable     = false;

  OCCViewer_ViewportInputFilter::setEnabled( theIsEnabled );
}

/*!
  \brief Checks whether the interactive operation is in progress.
  \return \c true if the interaction is performed on IO object meaning
          that no other operations in viewer should be processed.
*/
bool OCCViewer_ClipPlaneInteractor::isPerforming() const
{
  return !myInteractedPlane.IsNull();
}

/*!
  \brief Checks whether the interactive plane can be clicked.
  \return \c true if the click interaction is supported for the plane.
*/
bool OCCViewer_ClipPlaneInteractor::isClickable( const Handle(AIS_Plane)& thePlane )
{
  bool isFound = Standard_False;
  for ( int aPlaneIt = 0; aPlaneIt < myPlanes.size(); ++aPlaneIt )
  {
    if ( myPlanes[aPlaneIt] == thePlane )
    {
      isFound = true;
      break;
    }
  }

  if ( !isFound )
  {
    return false;
  }

  Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

  if ( anAISContext->IsSelected( Handle(AIS_InteractiveObject)::DownCast(thePlane) ) )
  {
    return false;
  }

  return true;
}

/*!
  \brief Checks whether the interactive plane can be dragged.
  \return \c true if the dragging interaction is supported for the plane.
*/
bool OCCViewer_ClipPlaneInteractor::isDraggable( const Handle(AIS_Plane)& thePlane )
{
  bool isFound = Standard_False;
  for ( int aPlaneIt = 0; aPlaneIt < myPlanes.size(); ++aPlaneIt )
  {
    if ( myPlanes[aPlaneIt] == thePlane )
    {
      isFound = true;
      break;
    }
  }

  if ( !isFound )
  {
    return false;
  }

  Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

  if ( !anAISContext->IsSelected( Handle(AIS_InteractiveObject)::DownCast(thePlane) ) ) 
  {
    return false;
  }

  return true;
}

/*!
  \brief Checks whether it is possible to start interaction with plane.
  \param thePickPos [in] the position of mouse picking.
  \param theDragPos [in] the position of initial mouse dragging.
  \param theDragOp [in] the drag operation to start.
  \param thePlane [in] the detected plane.
  \param theView [in] the view.
*/
bool OCCViewer_ClipPlaneInteractor::startDragging( const QPoint& thePickPos,
                                                   const QPoint& theDragPos,
                                                   const DragOperation theDragOp,
                                                   const Handle(AIS_Plane)& thePlane,
                                                   const Handle(V3d_View)& theView )
{
  // get point of view plane intersection with the plane
  Standard_Real P[3], D[3];
  theView->ConvertWithProj( thePickPos.x(), thePickPos.y(), P[0], P[1], P[2], D[0], D[1], D[2] );
  gp_Lin aPickProj = gp_Lin( gp_Pnt( P[0], P[1], P[2] ), gp_Dir( D[0], D[1], D[2] ) );
  gp_Pln aPlanePln = thePlane->Component()->Pln();

  Handle(Geom_Line)  aPickLine   = new Geom_Line( aPickProj );
  Handle(Geom_Plane) aCrossPlane = new Geom_Plane( aPlanePln );

  GeomAPI_IntCS aFindPick( aPickLine, aCrossPlane );
  if ( !aFindPick.IsDone() || aFindPick.NbPoints() == 0 )
  {
    return false;
  }

  // check plane geometry
  Standard_Real aSizeX = 0.0;
  Standard_Real aSizeY = 0.0;
  thePlane->Size(aSizeX, aSizeY);
  if ( aSizeX < Precision::Confusion() || aSizeY < Precision::Confusion() )
  {
    return false;
  }

  gp_Pnt aPickPoint = aFindPick.Point( 1 );

  const gp_Dir& aPlaneN      = aPlanePln.Axis().Direction();
  const gp_Dir& aPlaneX      = aPlanePln.XAxis().Direction();
  const gp_Dir& aPlaneY      = aPlanePln.YAxis().Direction();
  const gp_Pnt& aPlaneCenter = aPlanePln.Location();

  switch ( theDragOp )
  {
    // sliding operation is started
    case DragOperation_Slide :
    {
      if ( aPlaneN.IsParallel( aPickProj.Direction(), M_PI / 180.0 ) )
      {
        return false;
      }

      gp_Dir aMousePlnDir = ( aPickProj.Direction() ^ aPlaneN ) ^ aPlaneN;

      myMouseDragPln     = gp_Pln( aPickPoint, aMousePlnDir );
      myPlaneReferenceCS = gp_Ax3( aPlaneCenter, aPlaneN, aPlaneX );

      return true;
    }

    // rotation operation is requested
    case DragOperation_Rotate :
    {
      theView->ConvertWithProj( theDragPos.x(), theDragPos.y(), P[0], P[1], P[2], D[0], D[1], D[2] );
      gp_Lin aDragProj = gp_Lin( gp_Pnt( P[0], P[1], P[2] ), gp_Dir( D[0], D[1], D[2] ) );

      if ( aPickPoint.Distance( aDragProj.Location() ) < Precision::Confusion() )
      {
        return false;
      }

      // to determine whether we rotate around first or second axis, we
      // construct a virtual "arm" as vector of from center of rotation
      // to the picked point. Then we calculate dragging directions for both axes
      // depending on the difference of picking and dragging mouse coordinates.
      // The direction which is physically more easy to turn the "arm" is choosen
      // and the corresponding plane for dragging is selected.

      gp_Vec anArm = gp_Vec( myRotationCenter, aPickPoint );
      Standard_Real anArmLength = anArm.Magnitude();
      if ( anArmLength < Precision::Confusion() )
      {
        return false;
      }

      Handle(Geom_Line) aDragLine = new Geom_Line( aDragProj );
      Standard_Real aMomentArm1 = 0.0;
      Standard_Real aMomentArm2 = 0.0;
      Standard_Real anArmLength1 = Abs( anArm * gp_Vec( aPlaneN ^ aPlaneX ) );
      Standard_Real anArmLength2 = Abs( anArm * gp_Vec( aPlaneN ^ aPlaneY ) );

      // check virtual "arm" dragging moment for first axis of rotation
      if ( !aPlaneX.IsNormal( aDragProj.Direction(), M_PI / 180.0 ) && ( anArmLength1 / anArmLength > 0.3 ) )
      {
        Handle(Geom_Plane) aDragPln = new Geom_Plane( aPickPoint, aPlaneX );

        gp_Pnt aDragPnt = aPickPoint;
        gp_Vec aDragDir = gp_Vec( 0.0, 0.0, 0.0 );
        GeomAPI_IntCS aFindCross( aDragLine, aDragPln );
        if ( aFindCross.IsDone() && aFindCross.NbPoints() != 0 )
        {
          aDragPnt = aFindCross.Point( 1 );
        }

        if ( aDragPnt.Distance( aPickPoint ) > Precision::Confusion() )
        {
          aDragDir = gp_Vec( aPickPoint, aDragPnt );
        }

        aMomentArm1 = anArmLength1 * ( 1.0 - Abs( aDragDir.Normalized() * anArm.Normalized() ) );
      }

      // check virtual "arm" dragging moment for second axis of rotation
      if ( !aPlaneY.IsNormal( aDragProj.Direction(), M_PI / 180.0 )&& ( anArmLength2 / anArmLength > 0.3 ) )
      {
        Handle(Geom_Plane) aDragPln = new Geom_Plane( aPickPoint, aPlaneY );

        gp_Pnt aDragPnt = aPickPoint;
        gp_Vec aDragDir = gp_Vec( 0.0, 0.0, 0.0 );
        GeomAPI_IntCS aFindCross( aDragLine, aDragPln );
        if ( aFindCross.IsDone() && aFindCross.NbPoints() != 0 )
        {
          aDragPnt = aFindCross.Point( 1 );
        }

        if ( aDragPnt.Distance( aPickPoint ) > Precision::Confusion() )
        {
          aDragDir = gp_Vec( aPickPoint, aDragPnt );
        }

        aMomentArm2 = anArmLength2 * ( 1.0 - Abs( aDragDir.Normalized() * anArm.Normalized() ) );
      }

      // choose the best plane for dragging
      if ( aMomentArm1 >= aMomentArm2 )
      {
        gp_Vec aMousePlnN = gp_Vec( aPlaneX );

        myMouseDragPln = gp_Pln( aPickPoint, aMousePlnN );

        Standard_Real aDistance2Center = myMouseDragPln.Distance( aPlaneCenter );
        gp_Pnt aCenterOnMousePln = aMousePlnN * gp_Vec( aPickPoint, aPlaneCenter ) < 0.0
          ? aPlaneCenter.Translated( aMousePlnN *  aDistance2Center )
          : aPlaneCenter.Translated( aMousePlnN * -aDistance2Center );

        myRotationAxis = gp_Ax1( myRotationCenter, aMousePlnN );
      }
      else
      {
        gp_Vec aMousePlnN = gp_Vec( aPlaneY );

        myMouseDragPln = gp_Pln( aPickPoint, aMousePlnN );

        Standard_Real aDistance2Center = myMouseDragPln.Distance( aPlaneCenter );
        gp_Pnt aCenterOnMousePln = aMousePlnN * gp_Vec( aPickPoint, aPlaneCenter ) < 0.0
          ? aPlaneCenter.Translated( aMousePlnN *  aDistance2Center )
          : aPlaneCenter.Translated( aMousePlnN * -aDistance2Center );

        myRotationAxis = gp_Ax1( myRotationCenter, aMousePlnN );
      }

      myPlaneReferenceCS = gp_Ax3( aPlaneCenter, aPlaneN, aPlaneX );

      return true;
    }
  }

  return false;
}

/*!
  \brief Performs dragging operation on the passed interactive plane.
  \param theDragPos [in] the position of mouse dragging.
  \param theDragOp [in] the drag operation to start.
  \param thePlane [in] the operated plane.
  \param theView [in] the view.
*/
void OCCViewer_ClipPlaneInteractor::performDragging( const QPoint& theDragPos,
                                                     const DragOperation theDragOp,
                                                     const Handle(AIS_Plane)& thePlane,
                                                     const Handle(V3d_View)& theView )
{
  Standard_Real P[3], D[3];
  theView->ConvertWithProj( theDragPos.x(), theDragPos.y(), P[0], P[1], P[2], D[0], D[1], D[2] );
  gp_Lin aProjection = gp_Lin( gp_Pnt( P[0], P[1], P[2] ), gp_Dir( D[0], D[1], D[2] ) );

  // get point on the plane
  Handle(Geom_Line)  aCrossLine  = new Geom_Line( aProjection );
  Handle(Geom_Plane) aCrossPlane = new Geom_Plane( myMouseDragPln );

  GeomAPI_IntCS aFindCross( aCrossLine, aCrossPlane );
  if ( !aFindCross.IsDone() || aFindCross.NbPoints() == 0 )
  {
    return;
  }

  gp_Pnt aDragPoint   = aFindCross.Point( 1 );
  gp_Pnt aPlaneCenter = myPlaneReferenceCS.Location();
  gp_Vec aPlaneN      = myPlaneReferenceCS.Direction();
  gp_Vec aPlaneX      = myPlaneReferenceCS.XDirection();
  gp_Pln aPlanePln    = gp_Pln( aPlaneCenter, aPlaneN );

  switch ( theDragOp )
  {
    // sliding the plane along its normal
    case DragOperation_Slide:
    {
      Standard_Real aTranslation = 
        gp_Vec( aPlaneCenter, aDragPoint ) * gp_Vec( aPlaneN ) > 0.0
          ?  aPlanePln.Distance( aDragPoint )
          : -aPlanePln.Distance( aDragPoint );

      gp_Pnt aNewCenter = aPlaneCenter.Translated( aPlaneN * aTranslation );

      myPlaneReferenceCS = gp_Ax3( aNewCenter, aPlaneN, aPlaneX );

      adjustBounds( myPlaneReferenceCS, myMinMax );

      thePlane->SetComponent( new Geom_Plane( myPlaneReferenceCS ) );
      thePlane->SetCenter( myPlaneReferenceCS.Location() );
      thePlane->SetToUpdate();
      thePlane->UpdateSelection();

      myViewer->getAISContext()->Update( thePlane );
    }
    break;

    case DragOperation_Rotate:
    {
      // project the dragging point on rotated plane
      gp_Dir aRotAxis = myRotationAxis.Direction();
      gp_Pln aDragAtCenterPln = gp_Pln( myRotationCenter, aRotAxis );
      gp_Pnt aDragAtCenterPnt = gp_Vec( myRotationCenter, aDragPoint ) * gp_Vec( aRotAxis ) < 0.0
        ? aDragPoint.Translated( gp_Vec( aRotAxis ) *  aDragAtCenterPln.Distance( aDragPoint ) )
        : aDragPoint.Translated( gp_Vec( aRotAxis ) * -aDragAtCenterPln.Distance( aDragPoint ) );

      gp_Pnt aDragOnPlanePnt = gp_Vec( aPlaneCenter, aDragAtCenterPnt ) * gp_Vec( aPlaneN ) < 0.0
        ? aDragAtCenterPnt.Translated( gp_Vec( aPlaneN ) *  aPlanePln.Distance( aDragAtCenterPnt ) )
        : aDragAtCenterPnt.Translated( gp_Vec( aPlaneN ) * -aPlanePln.Distance( aDragAtCenterPnt ) );

      gp_Vec aDragPointVector( myRotationCenter, aDragAtCenterPnt );
      gp_Vec aProjPointVector( myRotationCenter, aDragOnPlanePnt );

      // check for rotation tolerance
      if ( aDragPointVector.Magnitude() < 0.01 || aProjPointVector.Magnitude() < 0.01 )
      {
        return;
      }

      Standard_Real aTurnAngle = aProjPointVector.AngleWithRef( aDragPointVector, myRotationAxis.Direction() );

      gp_Trsf aRotationTrsf;
      aRotationTrsf.SetRotation( myRotationAxis, aTurnAngle );
      myPlaneReferenceCS.Transform( aRotationTrsf );

      adjustBounds( myPlaneReferenceCS, myMinMax );

      gp_Ax3 aPlaneCS( myPlaneReferenceCS.Location(), myPlaneReferenceCS.Direction() );

      thePlane->SetComponent( new Geom_Plane( aPlaneCS ) );
      thePlane->SetCenter( myPlaneReferenceCS.Location() );
      thePlane->SetToUpdate();
      thePlane->UpdateSelection();

      myViewer->getAISContext()->Update( thePlane );
    }
    break;
  }
}

/*!
  \brief Adjusts min-max bounds of the plane.
  \param thePlane [in/out] the plane.
  \param theMinMax [in] the min max bounds
*/
void OCCViewer_ClipPlaneInteractor::adjustBounds( gp_Ax3& thePlane, const Bnd_Box& theMinMax )
{
  gp_Trsf aRelativeTransform;
  aRelativeTransform.SetTransformation( gp_Ax3(), thePlane );
  Bnd_Box aRelativeBounds = theMinMax.Transformed( aRelativeTransform );

  Standard_Real aXmin, aXmax, aYmin, aYmax, aZmin, aZmax;
  aRelativeBounds.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  if ( aZmax < 0.0 ) // out in positive direction
  {
    thePlane.Translate( gp_Vec( thePlane.Direction() ) * aZmax );
  }
  else if ( aZmin > 0.0 ) // out in negative direction
  {
    thePlane.Translate( gp_Vec( thePlane.Direction() ) * aZmin );
  }
}

/*!
  \brief Handle mouse press events. Starts interaction with detected plane.
  \param theEvent [in] the user event.
  \param theViewPort [in] the viewport.
*/
bool OCCViewer_ClipPlaneInteractor::mousePress( QMouseEvent* theEvent,
                                                OCCViewer_ViewPort3d* theViewPort )
{
  if ( theEvent->button() != Qt::LeftButton )
  {
    return false;
  }

  Handle(V3d_View) aView3D = theViewPort->getView();

  Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

  // check detection of plane
  anAISContext->MoveTo( theEvent->x(), theEvent->y(), aView3D );

  if ( !anAISContext->HasDetected() )
  {
    return false;
  }

  // check that there is only one detected entity
  anAISContext->InitDetected();

  Handle(AIS_Plane) aPlane;
  Handle(SelectMgr_EntityOwner) aDetectedOwner = anAISContext->DetectedOwner();
  if ( !aDetectedOwner.IsNull() )
  {
    aPlane = Handle(AIS_Plane)::DownCast( aDetectedOwner->Selectable() );
  }

  if ( aPlane.IsNull() )
  {
    aPlane = Handle(AIS_Plane)::DownCast( anAISContext->DetectedInteractive() );
  }

  myIsClickable = isClickable( aPlane );

  // process mouse click on the object
  if ( myIsClickable )
  {
    myViewer->getAISContext()->SetSelected( Handle(AIS_InteractiveObject)::DownCast(aPlane) );
    emit planeClicked( aPlane );
  }

  myIsDraggable = isDraggable( aPlane );

  if ( !myIsClickable && !myIsDraggable )
  {
    return false;
  }

  myPickPos = theEvent->pos();
  myInteractedPlane = aPlane;

  return true;
}

/*!
  \brief Handle mouse move events. Performs dragging if interaction is in progress.
  \param theEvent [in] the user event.
  \param theViewPort [in] the viewport.
*/
bool OCCViewer_ClipPlaneInteractor::mouseMove( QMouseEvent* theEvent,
                                               OCCViewer_ViewPort3d* theViewPort )
{
  if ( !isPerforming() )
  {
    return false;
  }

  // no dragging operation can be performed...
  if ( !myIsDraggable )
  {
    return true;
  }

  Handle(V3d_View) aView3D = theViewPort->getView();

  myDragPos = theEvent->pos();

  // checking whether it is possible to start dragging operation
  if ( myPerformingOp == DragOperation_Undef )
  {
    int aDx = myDragPos.x() - myPickPos.x();
    int aDy = myDragPos.y() - myPickPos.y();
    if ( ( aDx * aDx + aDy * aDy ) < 16 )
    {
      return true;
    }

    DragOperation aDragOp =
      theEvent->modifiers().testFlag(Qt::ControlModifier)
        ? DragOperation_Rotate
        : DragOperation_Slide;

    myIsDraggable = startDragging( myPickPos, myDragPos, aDragOp, myInteractedPlane, aView3D );

    if ( !myIsDraggable )
    {
      return true;
    }

    myPerformingOp = aDragOp;
  }

  // performing dragging operation
  performDragging( myDragPos, myPerformingOp, myInteractedPlane, aView3D );

  emit planeDragged( myInteractedPlane );

  return true;
}

/*!
  \brief Handle mouse release events. Stops interaction.
  \param theEvent [in] the user event.
  \param theViewPort [in] the viewport.
*/
bool OCCViewer_ClipPlaneInteractor::mouseRelease( QMouseEvent* theEvent,
                                                  OCCViewer_ViewPort3d* theViewPort )
{
  if ( !isPerforming() )
  {
    return false;
  }

  myMouseDragPln    = gp_Pln();
  myPerformingOp    = DragOperation_Undef;
  myPickPos         = QPoint();
  myDragPos         = QPoint();
  myInteractedPlane = NULL;
  myIsDraggable     = false;
  myIsClickable     = false;
  return true;
}

/*!
  \brief Handle mouse double clicking events events. Stops the event propagation if
         interaction with plane is in progress.
  \param theEvent [in] the user event.
  \param theViewPort [in] the viewport.
*/
bool OCCViewer_ClipPlaneInteractor::mouseDoubleClick( QMouseEvent* theEvent,
                                                      OCCViewer_ViewPort3d* theViewPort )
{
  return isPerforming();
}

/*!
  \brief Handle key pressing events. Stops the event propagation if
         interaction with plane is in progress.
  \param theEvent [in] the user event.
  \param theViewPort [in] the viewport.
*/
bool OCCViewer_ClipPlaneInteractor::keyPress( QKeyEvent* theEvent,
                                              OCCViewer_ViewPort3d* theViewPort )
{
  // react to pressing & releasing ctrl key modifier
  if ( !isPerforming() )
  {
    return false;
  }

  DragOperation aDragOp =
    theEvent->modifiers().testFlag(Qt::ControlModifier)
      ? DragOperation_Rotate
      : DragOperation_Slide;

  if ( aDragOp != myPerformingOp )
  {
    myPerformingOp = DragOperation_Undef;
    myIsDraggable  = isDraggable( myInteractedPlane );
    myPickPos      = theViewPort->mapFromGlobal( QCursor::pos() );
  }

  return true;
}

/*!
  \brief Handle key releasing events. Stops the event propagation if
         interaction with plane is in progress.
  \param theEvent [in] the user event.
  \param theViewPort [in] the viewport.
*/
bool OCCViewer_ClipPlaneInteractor::keyRelease( QKeyEvent* theEvent,
                                                OCCViewer_ViewPort3d* theViewPort )
{
  // react to pressing & releasing ctrl key modifier
  if ( !isPerforming() )
  {
    return false;
  }

  DragOperation aDragOp =
    theEvent->modifiers().testFlag(Qt::ControlModifier)
      ? DragOperation_Rotate
      : DragOperation_Slide;

  if ( aDragOp != myPerformingOp )
  {
    myPerformingOp = DragOperation_Undef;
    myIsDraggable  = isDraggable( myInteractedPlane );
    myPickPos      = theViewPort->mapFromGlobal( QCursor::pos() );
  }

  return true;
}
