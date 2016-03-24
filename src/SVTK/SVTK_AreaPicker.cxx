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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : SVTK_AreaPicker.cxx
//  Author : 
//  Module : SALOME
//
#include "SVTK_AreaPicker.h"

#include <set>

#include <vtkObjectFactory.h>
#include <vtkCommand.h>

#include <vtkAbstractMapper3D.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

#include <vtkAssemblyPath.h>
#include <vtkAssemblyNode.h>

#include <vtkRenderWindow.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>
#include <vtkPoints.h>
#include <vtkCamera.h>
#include <vtkCell.h>

namespace
{
  //----------------------------------------------------------------------------
  inline
  double GetZ( float* theZPtr, int theSelection[4], int theDX, int theDY )
  {
    return theZPtr[theDX - theSelection[0]
        + ( theDY - theSelection[1] )
            * ( theSelection[2] - theSelection[0] + 1 )];
  }

  //----------------------------------------------------------------------------
  inline
  int Check( float* theZPtr, int theSelection[4], double theTolerance,
      double theDZ, int theDX, int theDY )
  {
    int aRet = 0;
    double aZ = -1.0;
    if ( theDX >= theSelection[0] && theDX <= theSelection[2]
        && theDY >= theSelection[1] && theDY <= theSelection[3] ) {
      // Access the value from the captured zbuffer.  Note, we only
      // captured a portion of the zbuffer, so we need to offset dx by
      // the selection window.
      aZ = GetZ( theZPtr, theSelection, theDX, theDY );
      if ( aZ > theTolerance && aZ < 1.0 - theTolerance ) {
        aRet = fabs( aZ - theDZ ) <= theTolerance;
      }
    }
    return aRet;
  }

  //----------------------------------------------------------------------------
  inline
  void GetCenter( const double theBounds[6], double theCenter[3] )
  {
    theCenter[0] = ( theBounds[1] + theBounds[0] ) / 2.0;
    theCenter[1] = ( theBounds[3] + theBounds[2] ) / 2.0;
    theCenter[2] = ( theBounds[5] + theBounds[4] ) / 2.0;
  }

  //----------------------------------------------------------------------------
  void CalculatePickPosition( vtkRenderer *theRenderer, double theSelectionX,
      double theSelectionY, double theSelectionZ, double thePickPosition[3] )
  {
    // Convert the selection point into world coordinates.
    //
    theRenderer->SetDisplayPoint( theSelectionX, theSelectionY, theSelectionZ );
    theRenderer->DisplayToWorld();
    double* aWorldCoords = theRenderer->GetWorldPoint();
    if ( aWorldCoords[3] != 0.0 ) {
      for ( int i = 0; i < 3; i++ ) {
        thePickPosition[i] = aWorldCoords[i] / aWorldCoords[3];
      }
    }
  }
}

vtkStandardNewMacro( SVTK_AreaPicker )
;

SVTK_AreaPicker::SVTK_AreaPicker()
{
  this->Tolerance = 0.005;
  this->PickPoints = 1;
}

SVTK_AreaPicker::~SVTK_AreaPicker()
{
}

int SVTK_AreaPicker::Pick( double, double, double, vtkRenderer* )
{
  return 0;
}

int SVTK_AreaPicker::Pick( double theSelectionX, double theSelectionY,
    double theSelectionX2, double theSelectionY2, vtkRenderer *theRenderer,
    SelectionMode theMode )
{
  QVector< QPoint > aPoints;
  aPoints.append( QPoint( (int)theSelectionX, (int)theSelectionY ) );
  aPoints.append( QPoint( (int)theSelectionX2, (int)theSelectionY2 ) );
  return Pick( aPoints, theRenderer, theMode );
}

int SVTK_AreaPicker::Pick( QVector< QPoint >& thePoints,
    vtkRenderer *theRenderer, SelectionMode theMode )
{
  //  Initialize picking process
  this->Initialize();
  myCellIdsMap.clear();
  myPointIdsMap.clear();
  this->Renderer = theRenderer;

  if ( theMode == RectangleMode ) {
    mySelection[0] = thePoints[0].x();
    mySelection[1] = thePoints[0].y();
    mySelection[2] = thePoints[1].x();
    mySelection[3] = thePoints[1].y();
  }
  else if( theMode == PolygonMode ) {
    int minX, minY, maxX, maxY;
    minX = maxX = thePoints[0].x();
    minY = maxY = thePoints[0].y();
    for ( int i=0; i < thePoints.size(); i++ ) {
      if ( thePoints[i].x() < minX )
      minX = thePoints[i].x();
      if ( thePoints[i].x() > maxX )
      maxX = thePoints[i].x();
      if ( thePoints[i].y() < minY )
      minY = thePoints[i].y();
      if ( thePoints[i].y() > maxY )
      maxY = thePoints[i].y();
    }
    mySelection[0] = minX;
    mySelection[1] = minY;
    mySelection[2] = maxX;
    mySelection[3] = maxY;
  }

  // Invoke start pick method if defined
  this->InvokeEvent( vtkCommand::StartPickEvent, NULL );

  vtkPropCollection *aProps;
  if ( this->PickFromList ) aProps = this->GetPickList();
  else
    aProps = theRenderer->GetViewProps();

  aProps->InitTraversal();
  while( vtkProp* aProp = aProps->GetNextProp() ) {
    aProp->InitPathTraversal();
    while( vtkAssemblyPath* aPath = aProp->GetNextPath() ) {
      vtkMapper *aMapper = NULL;
      bool anIsPickable = false;
      vtkActor* anActor = NULL;
      vtkProp *aPropCandidate = aPath->GetLastNode()->GetViewProp();
      if ( aPropCandidate->GetPickable() && aPropCandidate->GetVisibility() ) {
        anIsPickable = true;
        anActor = vtkActor::SafeDownCast( aPropCandidate );
        if ( anActor ) {
          aMapper = anActor->GetMapper();
          if ( anActor->GetProperty()->GetOpacity() <= 0.0 ) anIsPickable =
              false;
        }
      }
      if ( anIsPickable && aMapper && aMapper->GetInput() ) {
        if ( this->PickPoints ) {
          TVectorIds& aVisibleIds = myPointIdsMap[anActor];
          TVectorIds anInVisibleIds;
          SelectVisiblePoints( thePoints, theRenderer, aMapper->GetInput(),
              aVisibleIds, anInVisibleIds, this->Tolerance, theMode );
          if ( aVisibleIds.empty() ) {
            myPointIdsMap.erase( myPointIdsMap.find( anActor ) );
          }
        }
        else {
          TVectorIds& aVectorIds = myCellIdsMap[anActor];
          SelectVisibleCells( thePoints, theRenderer, aMapper->GetInput(),
              aVectorIds, this->Tolerance, theMode );
          if ( aVectorIds.empty() ) {
            myCellIdsMap.erase( myCellIdsMap.find( anActor ) );
          }
        }
      }
    }
  }

  // Invoke end pick method if defined
  this->InvokeEvent( vtkCommand::EndPickEvent, NULL );

  return myPointIdsMap.empty() || myCellIdsMap.empty();
}

//----------------------------------------------------------------------------
void SVTK_AreaPicker::SelectVisiblePoints( QVector< QPoint >& thePoints,
    vtkRenderer *theRenderer, vtkDataSet *theInput,
    SVTK_AreaPicker::TVectorIds& theVisibleIds,
    SVTK_AreaPicker::TVectorIds& theInVisibleIds, double theTolerance,
    SelectionMode theMode )
{
  theVisibleIds.clear();
  theInVisibleIds.clear();

  vtkIdType aNumPts = theInput->GetNumberOfPoints();
  if ( aNumPts < 1 ) return;

  theVisibleIds.reserve( aNumPts / 2 + 1 );
  theInVisibleIds.reserve( aNumPts / 2 + 1 );

  // Grab the composite perspective transform.  This matrix is used to convert
  // each point to view coordinates.  vtkRenderer provides a WorldToView()
  // method but it computes the composite perspective transform each time
  // WorldToView() is called.  This is expensive, so we get the matrix once
  // and handle the transformation ourselves.
  vtkMatrix4x4 *aMatrix = vtkMatrix4x4::New();
  aMatrix->DeepCopy(
      theRenderer->GetActiveCamera()->GetCompositeProjectionTransformMatrix(
          theRenderer->GetTiledAspectRatio(), 0, 1 ) );

  // We grab the z-buffer for the selection region all at once and probe the resulting array.
  float *aZPtr = theRenderer->GetRenderWindow()->GetZbufferData( mySelection[0],
      mySelection[1], mySelection[2], mySelection[3] );

  for ( vtkIdType aPntId = 0; aPntId < aNumPts; aPntId++ ) {
    // perform conversion
    double aX[4] = { 1.0, 1.0, 1.0, 1.0 };
    theInput->GetPoint( aPntId, aX );

    double aView[4];
    aMatrix->MultiplyPoint( aX, aView );
    if ( aView[3] == 0.0 ) continue;
    theRenderer->SetViewPoint( aView[0] / aView[3], aView[1] / aView[3],
        aView[2] / aView[3] );
    theRenderer->ViewToDisplay();

    double aDX[3];
    theRenderer->GetDisplayPoint( aDX );

    bool isInSelection;
    if ( theMode == RectangleMode ) isInSelection = aDX[0] >= mySelection[0]
        && aDX[0] <= mySelection[2] && aDX[1] >= mySelection[1]
        && aDX[1] <= mySelection[3];
    else
      if ( theMode == PolygonMode ) isInSelection =
	isPointInPolygon( QPoint( (int)aDX[0], (int)aDX[1] ), thePoints );

    // check whether visible and in selection window
    if ( isInSelection ) {
      int aDX0 = int( aDX[0] );
      int aDX1 = int( aDX[1] );

      int aRet = Check( aZPtr, mySelection, theTolerance, aDX[2], aDX0, aDX1 );
      if ( aRet > 0 ) goto ADD_VISIBLE;
      if ( aRet < 0 ) goto ADD_INVISIBLE;

      static int aMaxRadius = 5;
      for ( int aRadius = 1; aRadius < aMaxRadius; aRadius++ ) {
        int aStartDX[2] = { aDX0 - aRadius, aDX1 - aRadius };
        for ( int i = 0; i <= aRadius; i++ ) {
          int aRet = Check( aZPtr, mySelection, theTolerance, aDX[2],
              aStartDX[0]++, aStartDX[1] );
          if ( aRet > 0 ) goto ADD_VISIBLE;
          if ( aRet < 0 ) goto ADD_INVISIBLE;
        }
        for ( int i = 0; i <= aRadius; i++ ) {
          int aRet = Check( aZPtr, mySelection, theTolerance, aDX[2],
              aStartDX[0], aStartDX[1]++ );
          if ( aRet > 0 ) goto ADD_VISIBLE;
          if ( aRet < 0 ) goto ADD_INVISIBLE;
        }
        for ( int i = 0; i <= aRadius; i++ ) {
          int aRet = Check( aZPtr, mySelection, theTolerance, aDX[2],
              aStartDX[0]--, aStartDX[1] );
          if ( aRet > 0 ) goto ADD_VISIBLE;
          if ( aRet < 0 ) goto ADD_INVISIBLE;
        }
        for ( int i = 0; i <= aRadius; i++ ) {
          int aRet = Check( aZPtr, mySelection, theTolerance, aDX[2],
              aStartDX[0], aStartDX[1]-- );
          if ( aRet > 0 ) goto ADD_VISIBLE;
          if ( aRet < 0 ) goto ADD_INVISIBLE;
        }
      }
      if ( false ) ADD_VISIBLE:theVisibleIds.push_back( aPntId );
      if ( false ) ADD_INVISIBLE:theInVisibleIds.push_back( aPntId );
    }
  }  //for all points

  aMatrix->Delete();

  if ( aZPtr ) delete[] aZPtr;
}

void SVTK_AreaPicker::SelectVisibleCells( QVector< QPoint >& thePoints,
    vtkRenderer *theRenderer, vtkDataSet *theInput,
    SVTK_AreaPicker::TVectorIds& theVectorIds, double theTolerance,
    SelectionMode theMode )
{
  theVectorIds.clear();

  vtkIdType aNumCells = theInput->GetNumberOfCells();
  if ( aNumCells < 1 ) return;

  theVectorIds.reserve( aNumCells / 2 + 1 );

  SVTK_AreaPicker::TVectorIds aVisiblePntIds;
  SVTK_AreaPicker::TVectorIds anInVisiblePntIds;
  SelectVisiblePoints( thePoints, theRenderer, theInput, aVisiblePntIds,
      anInVisiblePntIds, theTolerance, theMode );

  typedef std::set< vtkIdType > TIdsSet;
  TIdsSet aVisibleIds( aVisiblePntIds.begin(), aVisiblePntIds.end() );
  TIdsSet anInVisibleIds( anInVisiblePntIds.begin(), anInVisiblePntIds.end() );

  // Grab the composite perspective transform.  This matrix is used to convert
  // each point to view coordinates.  vtkRenderer provides a WorldToView()
  // method but it computes the composite perspective transform each time
  // WorldToView() is called.  This is expensive, so we get the matrix once
  // and handle the transformation ourselves.
  vtkMatrix4x4 *aMatrix = vtkMatrix4x4::New();
  aMatrix->DeepCopy(
      theRenderer->GetActiveCamera()->GetCompositeProjectionTransformMatrix(
          theRenderer->GetTiledAspectRatio(), 0, 1 ) );

  for ( vtkIdType aCellId = 0; aCellId < aNumCells; aCellId++ ) {
    vtkCell* aCell = theInput->GetCell( aCellId );

    double aBounds[6];
    aCell->GetBounds( aBounds );

    double aCenter[3];
    GetCenter( aBounds, aCenter );

    double aView[4];
    double aX[4] = { aCenter[0], aCenter[1], aCenter[2], 1.0 };
    aMatrix->MultiplyPoint( aX, aView );

    if ( aView[3] == 0.0 ) continue;

    theRenderer->SetViewPoint( aView[0] / aView[3], aView[1] / aView[3],
        aView[2] / aView[3] );
    theRenderer->ViewToDisplay();

    double aDX[3];
    theRenderer->GetDisplayPoint( aDX );

    bool isInSelection;
    if ( theMode == RectangleMode ) isInSelection = aDX[0] >= mySelection[0]
        && aDX[0] <= mySelection[2] && aDX[1] >= mySelection[1]
        && aDX[1] <= mySelection[3];
    else
      if ( theMode == PolygonMode ) isInSelection =
	isPointInPolygon( QPoint( (int)aDX[0], (int)aDX[1] ), thePoints );
    // check whether visible and in selection window
    if ( isInSelection ) {
      vtkIdType aNumPts = aCell->GetNumberOfPoints();
      bool anIsVisible = true;
      for ( vtkIdType anId = 0; anId < aNumPts; anId++ ) {
        vtkIdType aPntId = aCell->GetPointId( anId );
        anIsVisible = aVisibleIds.find( aPntId ) != aVisibleIds.end();
        if ( !anIsVisible ) break;
      }
      if ( anIsVisible ) theVectorIds.push_back( aCellId );
    }
  }  //for all parts
}

bool SVTK_AreaPicker::isPointInPolygon( const QPoint& thePoint, const QVector<QPoint>& thePolygon )
{
  double eps = 1.0;
  if ( thePolygon.size() < 3 ) return false;

  QVector< QPoint >::const_iterator end = thePolygon.end();
  QPoint last_pt = thePolygon.back();

  last_pt.setX( last_pt.x() - thePoint.x() );
  last_pt.setY( last_pt.y() - thePoint.y() );

  double sum = 0.0;

  for ( QVector< QPoint >::const_iterator iter = thePolygon.begin();
      iter != end; ++iter ) {
    QPoint cur_pt = *iter;
    cur_pt.setX( cur_pt.x() - thePoint.x() );
    cur_pt.setY( cur_pt.y() - thePoint.y() );

    double del = last_pt.x() * cur_pt.y() - cur_pt.x() * last_pt.y();
    double xy = cur_pt.x() * last_pt.x() + cur_pt.y() * last_pt.y();

    if ( del )
      sum +=
      ( atan( ( last_pt.x() * last_pt.x() + last_pt.y() * last_pt.y() - xy ) / del )
      + atan( ( cur_pt.x() * cur_pt.x() + cur_pt.y() * cur_pt.y() - xy ) / del ) );

    last_pt = cur_pt;
  }
  return fabs( sum ) > eps;
}

const SVTK_AreaPicker::TVectorIdsMap&
SVTK_AreaPicker::GetPointIdsMap() const
{
  return myPointIdsMap;
}

const SVTK_AreaPicker::TVectorIdsMap&
SVTK_AreaPicker::GetCellIdsMap() const
{
  return myCellIdsMap;
}
