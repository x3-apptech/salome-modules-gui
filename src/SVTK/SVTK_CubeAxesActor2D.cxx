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

//  SALOME OBJECT : kernel of SALOME component
//  File   : SVTK_CubeAxesActor2D.cxx
//  Author : Eugeny Nikolaev

#include "SVTK_CubeAxesActor2D.h"
#include "VTKViewer_Transform.h"

#include <vtkPolyDataMapper.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkActor.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkAxisActor2D.h>
#include <vtkCamera.h>
#include <vtkDataSet.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkTextProperty.h>
#include <vtkViewport.h>
#include <vtkFloatArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>

vtkStandardNewMacro(SVTK_CubeAxesActor2D);

// Instantiate this object.
SVTK_CubeAxesActor2D::SVTK_CubeAxesActor2D()
{
  this->wireActorXY = vtkActor::New();
  this->wireActorYZ = vtkActor::New();
  this->wireActorXZ = vtkActor::New();

  this->planeXY = vtkRectilinearGridGeometryFilter::New();
  this->planeYZ = vtkRectilinearGridGeometryFilter::New();
  this->planeXZ = vtkRectilinearGridGeometryFilter::New();

  this->rgridMapperXY = vtkPolyDataMapper::New();
  this->rgridMapperYZ = vtkPolyDataMapper::New();
  this->rgridMapperXZ = vtkPolyDataMapper::New();

  this->rgridMapperXY->SetInputConnection(this->planeXY->GetOutputPort());
  this->rgridMapperYZ->SetInputConnection(this->planeYZ->GetOutputPort());
  this->rgridMapperXZ->SetInputConnection(this->planeXZ->GetOutputPort());

  this->wireActorXY->SetMapper(rgridMapperXY);
  this->wireActorYZ->SetMapper(rgridMapperYZ);
  this->wireActorXZ->SetMapper(rgridMapperXZ);

  this->wireActorXY->GetProperty()->SetRepresentationToWireframe();
  this->wireActorYZ->GetProperty()->SetRepresentationToWireframe();
  this->wireActorXZ->GetProperty()->SetRepresentationToWireframe();

  // setting ambient to 1 (if no - incorrect reaction on light)
  this->wireActorXY->GetProperty()->SetAmbient(1);
  this->wireActorYZ->GetProperty()->SetAmbient(1);
  this->wireActorXZ->GetProperty()->SetAmbient(1);

  this->XAxis->SetTitle(this->XLabel);
  this->YAxis->SetTitle(this->YLabel);
  this->ZAxis->SetTitle(this->ZLabel);
  
  this->XAxis->SetNumberOfLabels(this->NumberOfLabels);
  this->YAxis->SetNumberOfLabels(this->NumberOfLabels);
  this->ZAxis->SetNumberOfLabels(this->NumberOfLabels);
 
  this->XAxis->SetLabelFormat(this->LabelFormat);
  this->YAxis->SetLabelFormat(this->LabelFormat);
  this->ZAxis->SetLabelFormat(this->LabelFormat);
  
  this->XAxis->SetFontFactor(this->FontFactor);
  this->YAxis->SetFontFactor(this->FontFactor);
  this->ZAxis->SetFontFactor(this->FontFactor);
  
  this->XAxis->SetProperty(this->GetProperty());
  this->YAxis->SetProperty(this->GetProperty());
  this->ZAxis->SetProperty(this->GetProperty());

  vtkTextProperty* aTLProp = vtkTextProperty::New();
  aTLProp->SetBold(0);
  aTLProp->SetItalic(0);
  aTLProp->SetShadow(0);
  aTLProp->SetFontFamilyToArial();
  aTLProp->SetColor(1,0,0);
  if (this->XAxis->GetLabelTextProperty())
    this->XAxis->GetLabelTextProperty()->ShallowCopy(aTLProp);
  aTLProp->SetColor(0,1,0);
  if (this->YAxis->GetLabelTextProperty())
    this->YAxis->GetLabelTextProperty()->ShallowCopy(aTLProp);
  aTLProp->SetColor(0,0,1);
  if (this->ZAxis->GetLabelTextProperty())
    this->ZAxis->GetLabelTextProperty()->ShallowCopy(aTLProp);;

  aTLProp->SetColor(1,0,0);
  if (this->XAxis->GetLabelTextProperty())
    this->XAxis->GetTitleTextProperty()->ShallowCopy(aTLProp);
  aTLProp->SetColor(0,1,0);
  if (this->YAxis->GetLabelTextProperty())
    this->YAxis->GetTitleTextProperty()->ShallowCopy(aTLProp);
  aTLProp->SetColor(0,0,1);
  if (this->ZAxis->GetLabelTextProperty())
    this->ZAxis->GetTitleTextProperty()->ShallowCopy(aTLProp);
  
  aTLProp->Delete();
  
}

SVTK_CubeAxesActor2D::~SVTK_CubeAxesActor2D()
{
  this->wireActorXY->Delete();
  this->wireActorYZ->Delete();
  this->wireActorXZ->Delete();

  this->planeXY->Delete();
  this->planeYZ->Delete();
  this->planeXZ->Delete();

  this->rgridMapperXY->Delete();
  this->rgridMapperYZ->Delete();
  this->rgridMapperXZ->Delete();
}

// Static variable describes connections in cube.
static int Conn[8][3] = {{1,2,4}, {0,3,5}, {3,0,6}, {2,1,7},
                         {5,6,0}, {4,7,1}, {7,4,2}, {6,5,3}};

// Project the bounding box and compute edges on the border of the bounding
// cube. Determine which parts of the edges are visible via intersection 
// with the boundary of the viewport (minus borders).
int SVTK_CubeAxesActor2D::RenderOverlay(vtkViewport *viewport)
{
  int renderedSomething=0;

  // Initialization
  if ( ! this->RenderSomething )
    {
    return 0;
    }
  
  //Render the axes
  if ( this->XAxisVisibility )
    {
    renderedSomething += this->XAxis->RenderOverlay(viewport);
    }
  if ( this->YAxisVisibility )
    {
    renderedSomething += this->YAxis->RenderOverlay(viewport);
    }
  if ( this->ZAxisVisibility )
    {
    renderedSomething += this->ZAxis->RenderOverlay(viewport);
    }
  
  bool RX=false,RY=false;
  if (this->XAxisVisibility){
    this->wireActorXY->RenderOverlay(viewport);
    this->wireActorXZ->RenderOverlay(viewport);
    RX = true;
  }
  if (this->YAxisVisibility){
    if(!RX) this->wireActorXY->RenderOverlay(viewport);
    this->wireActorYZ->RenderOverlay(viewport);
    RY = true;
  }
  if (this->ZAxisVisibility){
    if(!RX) this->wireActorXZ->RenderOverlay(viewport);
    if(!RY) this->wireActorYZ->RenderOverlay(viewport);
  }

  return renderedSomething;
}

static void ChangeValues(double* aArray1,
                         double* aArray2,
                         double *aRange1,
                         double* aRange2,
                         bool theY)
{
  double tmp=-1000;
  if (!theY){
    for (int i=0; i<4; i++){
      tmp = aArray1[i]; aArray1[i] = aArray2[i]; aArray2[i] = tmp;
    }
    for(int i=0;i<2; i++){
      tmp = aRange1[i]; aRange1[i] = aRange2[i]; aRange2[i] = tmp;
    }
  }
  else{
    tmp = aArray1[2]; aArray1[2] = aArray2[0]; aArray2[0] = tmp;
    tmp = aArray1[3]; aArray1[3] = aArray2[1]; aArray2[1] = tmp;
    tmp = aArray1[0]; aArray1[0] = aArray2[2]; aArray2[2] = tmp;
    tmp = aArray1[1]; aArray1[1] = aArray2[3]; aArray2[3] = tmp;

    tmp = aRange1[0]; aRange1[0] = aRange2[1]; aRange2[1] = tmp;
    tmp = aRange1[1]; aRange1[1] = aRange2[0]; aRange2[0] = tmp;
  }
}

static void ChangeArrays(double* xCoords,
                         double* yCoords,
                         double* zCoords,
                         double* xRange,
                         double* yRange,
                         double* zRange,
                         const int xAxes,
                         const int yAxes, 
                         const int zAxes)
{
  if ( xAxes == 0 && yAxes == 2 && zAxes == 1)
    ChangeValues(yCoords,zCoords,yRange,zRange,true);
  else if (xAxes == 1 && yAxes == 0 && zAxes == 2)
    ChangeValues(xCoords,yCoords,xRange,yRange,true);
  else if (xAxes == 1 && yAxes == 2 && zAxes == 0){
    ChangeValues(xCoords,zCoords,xRange,zRange,false);
    // xAxes == 0 && yAxes == 2 && zAxes == 1
    ChangeValues(yCoords,zCoords,yRange,zRange,true);
  } else if (xAxes == 2 && yAxes == 0 && zAxes == 1){
    ChangeValues(xCoords,yCoords,xRange,yRange,true);
    // xAxes == 0 && yAxes == 2 && zAxes == 1
    ChangeValues(zCoords,yCoords,zRange,yRange,true);
  } else if (xAxes == 2 && yAxes == 1 && zAxes == 0)
    ChangeValues(zCoords,xCoords,zRange,xRange,false);
}

// Project the bounding box and compute edges on the border of the bounding
// cube. Determine which parts of the edges are visible via intersection 
// with the boundary of the viewport (minus borders).
int SVTK_CubeAxesActor2D::RenderOpaqueGeometry(vtkViewport *viewport)
{
  double bounds[6], slope = 0.0, minSlope, num, den;
  double pts[8][3], d2, d2Min, min;
  int i, idx = 0;
  int xIdx, yIdx = 0, zIdx = 0, zIdx2, renderedSomething=0;
  int xAxes = 0, yAxes, zAxes;

  // Initialization
  if ( !this->Camera )
    {
    vtkErrorMacro(<<"No camera!");
    this->RenderSomething = 0;
    return 0;
    }
  
  this->RenderSomething = 1;

  // determine the bounds to use
  this->GetBounds(bounds);

  // Build the axes (almost always needed so we don't check mtime)
  // Transform all points into display coordinates
  this->TransformBounds(viewport, bounds, pts);

  // Find the portion of the bounding box that fits within the viewport,
  if ( this->ClipBounds(viewport, pts, bounds) == 0 )
    {
    this->RenderSomething = 0;
    return 0;
    }

  // Take into account the inertia. Process only so often.
  if ( this->RenderCount++ == 0 || !(this->RenderCount % this->Inertia) )
    {
    // Okay, we have a bounding box, maybe clipped and scaled, that is visible.
    // We setup the axes depending on the fly mode.
    if ( this->FlyMode == VTK_FLY_CLOSEST_TRIAD )
      {
      // Loop over points and find the closest point to the camera
      min = VTK_FLOAT_MAX;
      for (i=0; i < 8; i++)
        {
        if ( pts[i][2] < min )
          {
          idx = i;
          min = pts[i][2];
          }
        }

      // Setup the three axes to be drawn
      xAxes = 0;
      xIdx = Conn[idx][0];
      yAxes = 1;
      yIdx = Conn[idx][1];
      zAxes = 2;
      zIdx = idx;
      zIdx2 = Conn[idx][2];
      }
    else
      {
      double e1[2], e2[2], e3[2];

      // Find distance to origin
      d2Min = VTK_FLOAT_MAX;
      for (i=0; i < 8; i++)
        {
        d2 = pts[i][0]*pts[i][0] + pts[i][1]*pts[i][1];
        if ( d2 < d2Min )
          {
          d2Min = d2;
          idx = i;
          }
        }

      // find minimum slope point connected to closest point and on 
      // right side (in projected coordinates). This is the first edge.
      minSlope = VTK_FLOAT_MAX;
      for (xIdx=0, i=0; i<3; i++)
        {
        num = (pts[Conn[idx][i]][1] - pts[idx][1]);
        den = (pts[Conn[idx][i]][0] - pts[idx][0]);
        if ( den != 0.0 )
          {
          slope = num / den;
          }
        if ( slope < minSlope && den > 0 )
          {
          xIdx = Conn[idx][i];
          yIdx = Conn[idx][(i+1)%3];
          zIdx = Conn[idx][(i+2)%3];
          xAxes = i;
          minSlope = slope;
          }
        }

      // find edge (connected to closest point) on opposite side
      for ( i=0; i<2; i++)
        {
        e1[i] = (pts[xIdx][i] - pts[idx][i]);
        e2[i] = (pts[yIdx][i] - pts[idx][i]);
        e3[i] = (pts[zIdx][i] - pts[idx][i]);
        }
      vtkMath::Normalize2D(e1);
      vtkMath::Normalize2D(e2);
      vtkMath::Normalize2D(e3);

      if ( vtkMath::Dot2D(e1,e2) < vtkMath::Dot2D(e1,e3) )
        {
        yAxes = (xAxes + 1) % 3;
        }
      else
        {
        yIdx = zIdx;
        yAxes = (xAxes + 2) % 3;
        }

      // Find the final point by determining which global x-y-z axes have not 
      // been represented, and then determine the point closest to the viewer.
      zAxes = (xAxes != 0 && yAxes != 0 ? 0 :
              (xAxes != 1 && yAxes != 1 ? 1 : 2));
      if ( pts[Conn[xIdx][zAxes]][2] < pts[Conn[yIdx][zAxes]][2] )
        {
        zIdx = xIdx;
        zIdx2 = Conn[xIdx][zAxes];
        }
      else
        {
        zIdx = yIdx;
        zIdx2 = Conn[yIdx][zAxes];
        }
      }//else boundary edges fly mode
    this->InertiaAxes[0] = idx;
    this->InertiaAxes[1] = xIdx;
    this->InertiaAxes[2] = yIdx;
    this->InertiaAxes[3] = zIdx;
    this->InertiaAxes[4] = zIdx2;
    this->InertiaAxes[5] = xAxes;
    this->InertiaAxes[6] = yAxes;
    this->InertiaAxes[7] = zAxes;
    } //inertia
  else
    {
    idx = this->InertiaAxes[0];
    xIdx = this->InertiaAxes[1];
    yIdx = this->InertiaAxes[2];
    zIdx = this->InertiaAxes[3];
    zIdx2 = this->InertiaAxes[4];
    xAxes = this->InertiaAxes[5];
    yAxes = this->InertiaAxes[6];
    zAxes = this->InertiaAxes[7];
    }

  // Setup the axes for plotting
  double xCoords[4], yCoords[4], zCoords[4], xRange[2], yRange[2], zRange[2];
  this->AdjustAxes(pts, bounds, idx, xIdx, yIdx, zIdx, zIdx2, 
                   xAxes, yAxes, zAxes, 
                   xCoords, yCoords, zCoords, xRange, yRange, zRange);

  // Upate axes
  this->Labels[0] = this->XLabel;
  this->Labels[1] = this->YLabel;
  this->Labels[2] = this->ZLabel;

  // correct XAxis, YAxis, ZAxis, which must be 
  // parallel OX, OY, OZ system coordinates
  // if xAxes=0 yAxes=1 zAxes=2 - good situation
  if (!(xAxes == 0 && yAxes == 1 && zAxes == 2))
    ChangeArrays(xCoords,yCoords,zCoords,
                 xRange,yRange,zRange,
                 xAxes,yAxes,zAxes);

  double aTScale[3];
  if(m_Transform.GetPointer() != NULL)
    m_Transform->GetMatrixScale(aTScale);

  this->XAxis->GetPositionCoordinate()->SetValue(xCoords[0], xCoords[1]);
  this->XAxis->GetPosition2Coordinate()->SetValue(xCoords[2], xCoords[3]);
  if(m_Transform.GetPointer() != NULL) this->XAxis->SetRange(xRange[0]/aTScale[0], xRange[1]/aTScale[0]);
  else this->XAxis->SetRange(xRange[0], xRange[1]);

  this->YAxis->GetPositionCoordinate()->SetValue(yCoords[2], yCoords[3]);
  this->YAxis->GetPosition2Coordinate()->SetValue(yCoords[0], yCoords[1]);
  if(m_Transform.GetPointer() != NULL) this->YAxis->SetRange(yRange[1]/aTScale[1], yRange[0]/aTScale[1]);
  else this->YAxis->SetRange(yRange[1], yRange[0]);

  this->ZAxis->GetPositionCoordinate()->SetValue(zCoords[0], zCoords[1]);
  this->ZAxis->GetPosition2Coordinate()->SetValue(zCoords[2], zCoords[3]);
  if(m_Transform.GetPointer() != NULL) this->ZAxis->SetRange(zRange[0]/aTScale[2], zRange[1]/aTScale[2]);
  else this->ZAxis->SetRange(zRange[0], zRange[1]);

  int numOfLabelsX = this->XAxis->GetNumberOfLabels();
  int numOfLabelsY = this->YAxis->GetNumberOfLabels();
  int numOfLabelsZ = this->ZAxis->GetNumberOfLabels();

  // XCoords coordinates for X grid
  vtkFloatArray *XCoords = vtkFloatArray::New();
  for(int i=0;i<numOfLabelsX;i++){
    double val = bounds[0]+i*(bounds[1]-bounds[0])/(numOfLabelsX-1);
    XCoords->InsertNextValue(val);
  }
  // YCoords coordinates for Y grid
  vtkFloatArray *YCoords = vtkFloatArray::New();
  for(int i=0;i<numOfLabelsX;i++){
    double val = bounds[2]+i*(bounds[3]-bounds[2])/(numOfLabelsY-1);
    YCoords->InsertNextValue(val);
  }
  // ZCoords coordinates for Z grid
  vtkFloatArray *ZCoords = vtkFloatArray::New();
  for(int i=0;i<numOfLabelsZ;i++){
    double val = bounds[4]+i*(bounds[5]-bounds[4])/(numOfLabelsZ-1);
    ZCoords->InsertNextValue(val);
  }

  vtkRectilinearGrid *rgrid = vtkRectilinearGrid::New();
  rgrid->SetDimensions(numOfLabelsX,numOfLabelsY,numOfLabelsZ);
  rgrid->SetXCoordinates(XCoords);
  rgrid->SetYCoordinates(YCoords);
  rgrid->SetZCoordinates(ZCoords);

  this->planeXY->SetInputData(rgrid);
  this->planeYZ->SetInputData(rgrid);
  this->planeXZ->SetInputData(rgrid);

  rgrid->Delete();

  double aCPosition[3];
  double aCDirection[3];
  this->Camera->GetPosition(aCPosition);
  this->Camera->GetDirectionOfProjection(aCDirection);

  // culculate placement of XY
  bool replaceXY=false;
  bool replaceYZ=false;
  bool replaceXZ=false;
  double p[6][3]; // centers of planes
  double vecs[6][3]; // 6 vectors from camera position to centers

  double aMiddleX = (XCoords->GetValue(0) + XCoords->GetValue(numOfLabelsX-1))/2;
  double aMiddleY = (YCoords->GetValue(0) + YCoords->GetValue(numOfLabelsY-1))/2;
  double aMiddleZ = (ZCoords->GetValue(0) + ZCoords->GetValue(numOfLabelsZ-1))/2;

  // plane XY
  p[0][0] = aMiddleX; // plane X=0.5 Y=0.5 Z=0
  p[0][1] = aMiddleY;
  p[0][2] = ZCoords->GetValue(0);

  p[1][0] = aMiddleX; // plane X=0.5 Y=0.5 Z=1
  p[1][1] = aMiddleY;
  p[1][2] = ZCoords->GetValue(numOfLabelsZ-1);

  // plane YZ
  p[2][0] = XCoords->GetValue(0); // plane X=0 Y=0.5 Z=0.5
  p[2][1] = aMiddleY;
  p[2][2] = aMiddleZ;

  p[3][0] = XCoords->GetValue(numOfLabelsX-1);
  p[3][1] = aMiddleY;
  p[3][2] = aMiddleZ;

  // plane XZ
  p[4][0] = aMiddleX; // plane X=0.5 Y=0 Z=0.5
  p[4][1] = YCoords->GetValue(0);
  p[4][2] = aMiddleZ;

  p[5][0] = aMiddleX; // plane X=0.5 Y=1 Z=0.5
  p[5][1] = YCoords->GetValue(numOfLabelsY-1);
  p[5][2] = aMiddleZ;

  for(int i=0;i<3;i++) 
    for(int j=0;j<6;j++) vecs[j][i] = p[j][i] - aCPosition[i];

  if ( vtkMath::Dot(vecs[0],aCDirection) < vtkMath::Dot(vecs[1],aCDirection))
    replaceXY = true;
  if ( vtkMath::Dot(vecs[2],aCDirection) < vtkMath::Dot(vecs[3],aCDirection))
    replaceYZ = true;
  if ( vtkMath::Dot(vecs[4],aCDirection) < vtkMath::Dot(vecs[5],aCDirection))
    replaceXZ = true;

  if(replaceXY) this->planeXY->SetExtent(0,numOfLabelsX, 0,numOfLabelsY, numOfLabelsZ,numOfLabelsZ);
  else this->planeXY->SetExtent(0,numOfLabelsX, 0,numOfLabelsY, 0,0);

  if(replaceYZ) this->planeYZ->SetExtent(numOfLabelsX,numOfLabelsX, 0,numOfLabelsY, 0,numOfLabelsZ);
  else this->planeYZ->SetExtent(0,0, 0,numOfLabelsY, 0,numOfLabelsZ);

  if(replaceXZ) this->planeXZ->SetExtent(0,numOfLabelsX, numOfLabelsY,numOfLabelsY, 0,numOfLabelsZ);
  else this->planeXZ->SetExtent(0,numOfLabelsX, 0,0, 0,numOfLabelsZ);

  XCoords->Delete();
  YCoords->Delete();
  ZCoords->Delete();

  double color[3];

  this->GetProperty()->GetColor(color);
  this->wireActorXY->GetProperty()->SetColor(color);
  this->wireActorYZ->GetProperty()->SetColor(color);
  this->wireActorXZ->GetProperty()->SetColor(color);

  /*
  // Rebuid text props
  // Perform shallow copy here since each individual axis can be
  // accessed through the class API (i.e. each individual axis text prop
  // can be changed). Therefore, we can not just assign pointers otherwise
  // each individual axis text prop would point to the same text prop.

  if (this->AxisLabelTextProperty &&
      this->AxisLabelTextProperty->GetMTime() > this->BuildTime)
    {
    if (this->XAxis->GetLabelTextProperty())
      {
      this->XAxis->GetLabelTextProperty()->ShallowCopy(
        this->AxisLabelTextProperty);
      }
    if (this->YAxis->GetLabelTextProperty())
      {
      this->YAxis->GetLabelTextProperty()->ShallowCopy(
        this->AxisLabelTextProperty);
      }
    if (this->ZAxis->GetLabelTextProperty())
      {
      this->ZAxis->GetLabelTextProperty()->ShallowCopy(
        this->AxisLabelTextProperty);
      }
    }

  if (this->AxisTitleTextProperty &&
      this->AxisTitleTextProperty->GetMTime() > this->BuildTime)
    {
    if (this->XAxis->GetLabelTextProperty())
      {
      this->XAxis->GetTitleTextProperty()->ShallowCopy(
        this->AxisTitleTextProperty);
      }
    if (this->YAxis->GetLabelTextProperty())
      {
      this->YAxis->GetTitleTextProperty()->ShallowCopy(
        this->AxisTitleTextProperty);
      }
    if (this->ZAxis->GetLabelTextProperty())
      {
      this->ZAxis->GetTitleTextProperty()->ShallowCopy(
        this->AxisTitleTextProperty);
      }
    }
  */  
  this->BuildTime.Modified();

  //Render the axes
  if ( this->XAxisVisibility )
  {
    renderedSomething += this->XAxis->RenderOpaqueGeometry(viewport);
  }
  if ( this->YAxisVisibility )
  {
    renderedSomething += this->YAxis->RenderOpaqueGeometry(viewport);
  }
  if ( this->ZAxisVisibility )
  {
    renderedSomething += this->ZAxis->RenderOpaqueGeometry(viewport);
  }

  bool RX=false,RY=false;
  if (this->XAxisVisibility){
    this->wireActorXY->RenderOpaqueGeometry(viewport);
    this->wireActorXZ->RenderOpaqueGeometry(viewport);
    RX = true;
  }
  if (this->YAxisVisibility){
    if(!RX) this->wireActorXY->RenderOpaqueGeometry(viewport);
    this->wireActorYZ->RenderOpaqueGeometry(viewport);
    RY = true;
  }
  if (this->ZAxisVisibility){
    if(!RX) this->wireActorXZ->RenderOpaqueGeometry(viewport);
    if(!RY) this->wireActorYZ->RenderOpaqueGeometry(viewport);
  }

  return renderedSomething;
}

// Release any graphics resources that are being consumed by this actor.
// The parameter window could be used to determine which graphic
// resources to release.
void SVTK_CubeAxesActor2D::ReleaseGraphicsResources(vtkWindow *win)
{
  this->XAxis->ReleaseGraphicsResources(win);
  this->YAxis->ReleaseGraphicsResources(win);
  this->ZAxis->ReleaseGraphicsResources(win);

  this->wireActorXY->ReleaseGraphicsResources(win);
  this->wireActorYZ->ReleaseGraphicsResources(win);
  this->wireActorXZ->ReleaseGraphicsResources(win);
}

void SVTK_CubeAxesActor2D::SetTransform(VTKViewer_Transform* theTransform){
  this->m_Transform = theTransform;
}

VTKViewer_Transform* SVTK_CubeAxesActor2D::GetTransform(){
  return (this->m_Transform.GetPointer());
}
