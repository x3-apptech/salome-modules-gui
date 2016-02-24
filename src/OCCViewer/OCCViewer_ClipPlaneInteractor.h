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

#ifndef OCCVIEWER_CLIPPLANEINTERACTOR_H
#define OCCVIEWER_CLIPPLANEINTERACTOR_H

#include "OCCViewer_ViewportInputFilter.h"

#include <AIS_Plane.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>

#include <QPoint>

#include <vector>

#include <V3d_View.hxx>

/*!
  \class OCCViewer_ClipPlaneInteractor.
  \brief The class provides mouse dragging operations with AIS_Plane:
         1) Sliding plane by dragging it with point
         2) Rotating plane around one or both of the rotation axes by
            dragging its point with "ctrl" key pressed.
*/
class OCCViewer_ClipPlaneInteractor : public OCCViewer_ViewportInputFilter
{
  Q_OBJECT

public:
  typedef std::vector<Handle(AIS_Plane)> SeqOfPlanes;

  // Performing interactive operation.
  // - Slide: The picked point on plane is dragged along the normal of the plane.
  // - Rotate: The picked point on plane is dragged around one of the specified rotation
  //     axes ("x", "y" axes of the plane coordinate system, or two orthogonal arbitrary
  //     specified ones). When starting the rotation a corresponding axis is
  //     choosen depending on the direction of dragging. The axis is selected in
  //     the following way: a virtual "arm" is constructed from center of
  //     rotation to picking point. The dragging directions are constructed for
  //     both of the rotation axes, then the direction which is "easiest to
  //     turn" is selected (e.g. the "arm" projection is largest for that axis,
  //     the direction of dragging is closer to orthogonal one for the axis).
  enum DragOperation
  {
    DragOperation_Undef,
    DragOperation_Slide,
    DragOperation_Rotate
  };

public:
  OCCViewer_ClipPlaneInteractor( OCCViewer_ViewManager*, QObject* theParent );

  const                    SeqOfPlanes& planes() const;
  void                     setPlanes( const SeqOfPlanes& );
  void                     setRotationCenter( const gp_Pnt& );
  void                     setMinMax( const Bnd_Box& );
  virtual void             setEnabled( const bool );

signals:
  void                     planeClicked( const Handle_AIS_Plane& thePlane );
  void                     planeDragged( const Handle_AIS_Plane& thePlane );

protected:
  bool                     isClickable( const Handle(AIS_Plane)& thePlane );
  bool                     isDraggable( const Handle(AIS_Plane)& thePlane );
  bool                     isPerforming() const;
  bool                     startDragging( const QPoint& thePickPos,
                                          const QPoint& theDragPos,
                                          const DragOperation theOperation,
                                          const Handle(AIS_Plane)& thePlane,
                                          const Handle(V3d_View)& theView );
  void                     performDragging( const QPoint& theDragPos,
                                            const DragOperation theOperation,
                                            const Handle(AIS_Plane)& thePlane,
                                            const Handle(V3d_View)& theView );
  void                     adjustBounds( gp_Ax3&, const Bnd_Box& );

protected:
  virtual bool             mouseMove( QMouseEvent*, OCCViewer_ViewPort3d* );
  virtual bool             mousePress( QMouseEvent*, OCCViewer_ViewPort3d* );
  virtual bool             mouseRelease( QMouseEvent*, OCCViewer_ViewPort3d* );
  virtual bool             mouseDoubleClick( QMouseEvent*, OCCViewer_ViewPort3d* );
  virtual bool             keyPress( QKeyEvent*, OCCViewer_ViewPort3d* );
  virtual bool             keyRelease( QKeyEvent*, OCCViewer_ViewPort3d* );

protected:

  gp_Ax3            myPlaneReferenceCS;
  gp_Ax1            myRotationAxis;
  gp_Pln            myMouseDragPln;
  QPoint            myDragPos;
  QPoint            myPickPos;
  DragOperation     myPerformingOp;
  gp_Pnt            myRotationCenter;
  Bnd_Box           myMinMax;
  Handle(AIS_Plane) myInteractedPlane;
  SeqOfPlanes       myPlanes;
  bool              myIsDraggable;
  bool              myIsClickable;
};

#endif
