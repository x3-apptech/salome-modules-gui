// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "VTKViewer_FramedTextActor.h"

#include <vtkCellArray.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTimeStamp.h>
#include <vtkViewport.h>
#include <vtkWindow.h>

#include <QStringList>

#define TEXT_MARGIN    4
#define OFFSET_SPACING 2

//VSR: uncomment below macro to support unicode text properly in SALOME
//     current commented out due to regressions
//#define PAL22528_UNICODE

namespace
{
  QString fromUtf8( const char* txt )
  {
#ifdef PAL22528_UNICODE
    return QString::fromUtf8( txt );
#else
    return QString( txt );
#endif
  }
  std::string toUtf8( const QString& txt )
  {
#ifdef PAL22528_UNICODE
    return txt.toUtf8().constData();
#else
    return txt.toLatin1().constData();
#endif
  }
}

//==================================================================
vtkStandardNewMacro(VTKViewer_FramedTextActor);

//==================================================================
// function : VTKViewer_FramedTextActor
// purpose  :
//==================================================================
VTKViewer_FramedTextActor::VTKViewer_FramedTextActor()
{
  PositionCoordinate->SetCoordinateSystemToNormalizedViewport();

  myTransparency=0.;
  myBar = vtkPolyData::New();
  myBarMapper = vtkPolyDataMapper2D::New();
  myBarMapper->SetInputData(myBar);
  myBarActor = vtkActor2D::New();
  myBarActor->SetMapper(myBarMapper);
  myBarActor->GetProperty()->SetOpacity(1.-myTransparency);  
  myBarActor->GetProperty()->SetColor(.5, .5, .5); 

  myTextProperty = vtkTextProperty::New();
  myTextProperty->SetFontSize(12);
  myTextProperty->SetBold(0);
  myTextProperty->SetItalic(0);
  myTextProperty->SetShadow(1);
  myTextProperty->SetFontFamilyToArial();
  
  myTextActor=vtkTextActor::New();
  myTextActor->SetTextProperty(myTextProperty);

  myBarActor->SetVisibility(1);
  myTextActor->SetVisibility(1);
  myBarActor->SetPickable(0);
  myTextActor->SetPickable(0);

  myModePosition = BelowPoint;
  myLayoutType = Vertical;

  for(int i=0; i<4; i++) {
    myWorldPoint[i] = 0.;
  }
  myDistance=10.;

  myTextMargin = TEXT_MARGIN;

  myHorizontalOffset = 0;
  myVerticalOffset = 0;

  myMoveFrameFlag = 0;
}

//==================================================================
// function : ~
// purpose  :
//==================================================================
VTKViewer_FramedTextActor::~VTKViewer_FramedTextActor()
{
  myTextActor->Delete();
  myTextProperty->Delete();
  myBarActor->Delete();
  myBarMapper->Delete();
  myBar->Delete();
}

//==================================================================
// function : SetVisibility
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetVisibility (int theVisibility)
{
  myBarActor->SetVisibility(theVisibility);
  myTextActor->SetVisibility(theVisibility);
}

//==================================================================
// function : GetVisibility
// purpose  :
//==================================================================
int VTKViewer_FramedTextActor::GetVisibility() 
{
  return myBarActor->GetVisibility();
}

//==================================================================
// function : SetPickable
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetPickable (int thePickability) 
{
  myBarActor->SetPickable(thePickability);
  myTextActor->SetPickable(thePickability);
}

//==================================================================
// function : GetPickable
// purpose  :
//==================================================================
int VTKViewer_FramedTextActor::GetPickable()
{
  return myBarActor->GetPickable();
}

//==================================================================
// function : GetSize
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::GetSize(vtkRenderer* vport, double theSize[2]) const
{
  myTextActor->GetSize(vport, theSize);
  theSize[0] = theSize[0] + 2 * GetTextMargin() + OFFSET_SPACING;
  theSize[1] = theSize[1] + 2 * GetTextMargin() + OFFSET_SPACING;
}

//==================================================================
// function : SetForegroundColor
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetForegroundColor(const double r,
                                                   const double g,
                                                   const double b)
{
  myTextProperty->SetColor(r, g, b);
  myTextActor->GetTextProperty()->ShallowCopy(myTextProperty);
  Modified();
}

//==================================================================
// function : GetForegroundColor
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::GetForegroundColor(double& r,
                                                   double& g,
                                                   double& b)
{
  double aColor[3];
  myTextProperty->GetColor(aColor);
  r = aColor[0];
  g = aColor[1];
  b = aColor[2];
}

//==================================================================
// function : SetBackgroundColor
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetBackgroundColor(const double r,
                                                   const double g,
                                                   const double b)
{
  myBarActor->GetProperty()->SetColor(r, g, b);
  Modified();
}

//==================================================================
// function : GetBackgroundColor
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::GetBackgroundColor(double& r,
                                                   double& g,
                                                   double& b)
{
  double aColor[3];
  myBarActor->GetProperty()->GetColor(aColor);
  r = aColor[0];
  g = aColor[1];
  b = aColor[2];
}

//==================================================================
// function : SetTransparency
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetTransparency(const double theTransparency)
{
  if (theTransparency>=0.  && theTransparency<=1.){
    myTransparency=theTransparency;
    myBarActor->GetProperty()->SetOpacity(1.-myTransparency);  
    Modified();
  }
}

//==================================================================
// function : GetTransparency
// purpose  :
//==================================================================
double VTKViewer_FramedTextActor::GetTransparency()const
{
  return myTransparency;
}

//==================================================================
// function : SetTextMargin
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetTextMargin(const int theMargin)
{
  if( theMargin >= 0 ) {
    myTextMargin = theMargin;
    Modified();
  }
}

//==================================================================
// function : GetTextMargin
// purpose  :
//==================================================================
int VTKViewer_FramedTextActor::GetTextMargin() const
{
  return myTextMargin;
}

//==================================================================
// function : SetOffset
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetOffset(const double theOffset[2])
{
  myHorizontalOffset = theOffset[0];
  myVerticalOffset = theOffset[1];
  Modified();
}

//==================================================================
// function : SetText
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetText(const char* theText)
{
  // remove whitespaces from from the start and the end
  // additionally, consider a case of multi-string text
  QString aString(fromUtf8(theText));

  QStringList aTrimmedStringList;
  QStringList aStringList = aString.split("\n");
  QStringListIterator anIter(aStringList);
  while(anIter.hasNext())
    aTrimmedStringList.append(anIter.next().trimmed());

  myTextActor->SetInput(toUtf8(aTrimmedStringList.join("\n")).c_str());
  Modified();
}

//==================================================================
// function : GetText
// purpose  :
//==================================================================
char* VTKViewer_FramedTextActor::GetText()
{
  return myTextActor->GetInput();
}

//==================================================================
// function : SetModePosition
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetModePosition(const int theMode)
{
  myModePosition = theMode;
  Modified();
}

//==================================================================
// function : GetModePosition
// purpose  :
//==================================================================
int VTKViewer_FramedTextActor::GetModePosition()const
{
  return myModePosition;
}

//==================================================================
// function : SetLayoutType
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetLayoutType(const int theType)
{
  myLayoutType = theType;
  Modified();
}

//==================================================================
// function : GetLayoutType
// purpose  :
//==================================================================
int VTKViewer_FramedTextActor::GetLayoutType() const
{
  return myLayoutType;
}

//==================================================================
// function : SetWorldPoint
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetWorldPoint(const double theWorldPoint[4])
{
  for(int i = 0; i<4; ++i) {
    myWorldPoint[i] = theWorldPoint[i];
  } 
  Modified();
}

//==================================================================
// function : GetWorldPoint
// purpose  :
//==================================================================
const double* VTKViewer_FramedTextActor::GetWorldPoint()const 
{
  return myWorldPoint;
}

//==================================================================
// function : SetDistance
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::SetDistance(const double theDistance)
{
  myDistance=theDistance;
}

//==================================================================
// function : GetDistance
// purpose  :
//==================================================================
double VTKViewer_FramedTextActor::GetDistance()const
{
  return myDistance;
}

//==================================================================
// function : SetMoveFrameFlag
// purpose  : If moveFrameFlag is true, then frame with text is moved
//            under world point
//==================================================================
void VTKViewer_FramedTextActor::SetMoveFrameFlag(const int theMoveFrameFlag)
{
  if(myMoveFrameFlag != theMoveFrameFlag) {
    myMoveFrameFlag = theMoveFrameFlag;
    Modified();
  }
}

//==================================================================
// function : GetDistance
// purpose  :
//==================================================================
int VTKViewer_FramedTextActor::GetMoveFrameFlag() const
{
  return myMoveFrameFlag;
}


//==================================================================
// function : ReleaseGraphicsResources
// purpose  :
//==================================================================
void VTKViewer_FramedTextActor::ReleaseGraphicsResources(vtkWindow *win)
{
  myTextActor->ReleaseGraphicsResources(win);
  myBarActor->ReleaseGraphicsResources(win);
}

//==================================================================
// function : RenderOverlay
// purpose  :
//==================================================================
int VTKViewer_FramedTextActor::RenderOverlay(vtkViewport *viewport)
{
  int renderedSomething = 0;
  renderedSomething +=myTextActor->RenderOverlay(viewport);
  renderedSomething +=myBarActor->RenderOverlay(viewport);
  return renderedSomething;
}

//==================================================================
// function : RenderOpaqueGeometry
// purpose  :
//==================================================================
int 
VTKViewer_FramedTextActor
::RenderOpaqueGeometry(vtkViewport *theViewport)
{
  int anIsRenderedSomething = 0;

  int* aViewportSize = theViewport->GetSize();
  int aViewPortWidth = aViewportSize[0];
  int aViewPortHeight = aViewportSize[1];
  if(aViewPortWidth == 1 || aViewPortHeight == 1)
    return anIsRenderedSomething;

  if(!myTextActor->GetInput())
    return anIsRenderedSomething;

  myBar->Initialize();

  int aNbPoints = 4;
  vtkPoints *aPoints = vtkPoints::New();
  aPoints->SetNumberOfPoints(aNbPoints);
  myBar->SetPoints(aPoints);
  aPoints->Delete();

  vtkCellArray *aPolys = vtkCellArray::New();
  aPolys->Allocate(aPolys->EstimateSize(1,4));
  vtkIdType aPointsIds[4] = {0, 1, 3, 2};
  aPolys->InsertNextCell(4,aPointsIds);
  myBar->SetPolys(aPolys);
  aPolys->Delete(); 

  double aTextSize[2]; 
  myTextActor->GetSize(theViewport, aTextSize);
  int aBarWidth = aTextSize[0];
  int aBarHeight = aTextSize[1];

  int aTextMargin = GetTextMargin();

  double xMin = 0.0;
  double xMax = 0.0;
  double yMin = -aBarHeight/2 - aTextMargin;
  double yMax =  aBarHeight/2 + aTextMargin;

  int aHorizontalOffset = GetLayoutType() == Horizontal ? myHorizontalOffset : 0;
  int aVerticalOffset = GetLayoutType() == Vertical ? myVerticalOffset : 0;

  if( myModePosition == BelowPoint )
  {
    theViewport->SetWorldPoint(myWorldPoint);
    theViewport->WorldToDisplay();

    double aSelectionPoint[3];
    theViewport->GetDisplayPoint(aSelectionPoint);
    double u = aSelectionPoint[0];
    double v = aSelectionPoint[1] - myDistance;
    if(myMoveFrameFlag)
      v -= aBarHeight/2.;
    theViewport->ViewportToNormalizedViewport(u, v);
    PositionCoordinate->SetValue(u, v);

    myTextProperty->SetJustificationToCentered();

    xMin = -aBarWidth/2 - aTextMargin;
    xMax =  aBarWidth/2 + aTextMargin;
  }
  else // except BelowPoint, only TopLeft and TopRight modes are supported at this moment
  {
    double x = 0, xOffset = aHorizontalOffset + aTextMargin + OFFSET_SPACING;
    double y = 0, yOffset = aVerticalOffset + aTextMargin + OFFSET_SPACING;

    if( myModePosition == TopLeft )
    {
      x = xOffset;
      y = aViewPortHeight - yOffset - aBarHeight/2;
      myTextProperty->SetJustificationToLeft();

      xMin =            - aTextMargin;
      xMax =  aBarWidth + aTextMargin;
    }
    else if( myModePosition == TopRight )
    {
      x = aViewPortWidth - xOffset;
      y = aViewPortHeight - yOffset - aBarHeight/2;
      myTextProperty->SetJustificationToRight();

      xMin = -aBarWidth - aTextMargin;
      xMax =              aTextMargin;
    }

    PositionCoordinate->SetValue(x / (double)aViewPortWidth,
                                 y / (double)aViewPortHeight);
  }


  aPoints->SetPoint(0, xMin, yMax, 0.0);
  aPoints->SetPoint(1, xMin, yMin, 0.0);
  aPoints->SetPoint(2, xMax, yMax, 0.0);
  aPoints->SetPoint(3, xMax, yMin, 0.0);

  myTextProperty->SetVerticalJustificationToCentered();

  myBarActor ->GetPositionCoordinate()->SetReferenceCoordinate(PositionCoordinate);
  myTextActor->GetPositionCoordinate()->SetReferenceCoordinate(PositionCoordinate);

  myBuildTime.Modified();

  return anIsRenderedSomething;
}
