// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef VTKVIEWER_FRAMEDTEXTACTOR_H
#define VTKVIEWER_FRAMEDTEXTACTOR_H

#include "VTKViewer.h"

#include <vtkActor2D.h>

class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkRenderer;
class vtkTimeStamp;
class vtkTextMapper;
class vtkTextProperty;
class vtkViewport;
class vtkWindow;

class VTKVIEWER_EXPORT VTKViewer_FramedTextActor : public vtkActor2D
{
public:
  enum ModePosition { BelowPoint = 0, TopLeft, TopRight, BottomLeft, BottomRight };
  enum LayoutType { Vertical = 0, Horizontal };

public:
  vtkTypeRevisionMacro(VTKViewer_FramedTextActor,vtkActor2D);
  static VTKViewer_FramedTextActor *New();

  virtual int                 RenderOpaqueGeometry(vtkViewport*);
  virtual int                 RenderTranslucentGeometry(vtkViewport*) { return 0; }
  virtual int                 RenderOverlay(vtkViewport*);
  virtual void                ReleaseGraphicsResources(vtkWindow*);

  virtual void                SetVisibility(int);
  virtual int                 GetVisibility();
  virtual void                SetPickable(int);
  virtual int                 GetPickable();

  virtual void                GetSize(vtkRenderer* theRenderer, int theSize[2]) const;

  void                        SetText(const char* theText);
  char*                       GetText();

  void                        SetModePosition(const int theMode);
  int                         GetModePosition() const;

  void                        SetLayoutType(const int theType);
  int                         GetLayoutType() const;

  void                        SetWorldPoint(const vtkFloatingPointType theWorldPoint[4]);
  const vtkFloatingPointType* GetWorldPoint() const;

  void                        SetDistance(const vtkFloatingPointType theDistance);
  vtkFloatingPointType        GetDistance() const;

  void                        SetMoveFrameFlag(const int flag);
  int                         GetMoveFrameFlag() const;

  void                        SetForegroundColor(const vtkFloatingPointType r,
                                                 const vtkFloatingPointType g,
                                                 const vtkFloatingPointType b);
  void                        GetForegroundColor(vtkFloatingPointType& r,
                                                 vtkFloatingPointType& g,
                                                 vtkFloatingPointType& b);

  void                        SetBackgroundColor(const vtkFloatingPointType r,
                                                 const vtkFloatingPointType g,
                                                 const vtkFloatingPointType b);
  void                        GetBackgroundColor(vtkFloatingPointType& r,
                                                 vtkFloatingPointType& g,
                                                 vtkFloatingPointType& b);

  void                        SetTransparency(const vtkFloatingPointType theTransparency);
  vtkFloatingPointType        GetTransparency() const;

  void                        SetTextMargin(const int theMargin);
  int                         GetTextMargin() const;

  void                        SetOffset(const int theOffset[2]);

protected:
  VTKViewer_FramedTextActor();
  ~VTKViewer_FramedTextActor();

protected:
  vtkPolyData*                myBar;
  vtkPolyDataMapper2D*        myBarMapper;
  vtkActor2D*                 myBarActor;

  vtkTextProperty*            myTextProperty;
  vtkTextMapper*              myTextMapper;
  vtkActor2D*                 myTextActor;

  vtkTimeStamp                myBuildTime;

  int                         myModePosition;
  int                         myLayoutType;
  int                         myMoveFrameFlag;

  vtkFloatingPointType        myWorldPoint[4];
  vtkFloatingPointType        myDistance;
  vtkFloatingPointType        myTransparency;

  int                         myTextMargin;

  int                         myHorizontalOffset;
  int                         myVerticalOffset;

private:
  VTKViewer_FramedTextActor(const VTKViewer_FramedTextActor&);  // Not implemented.
  void operator=(const VTKViewer_FramedTextActor&);  // Not implemented.
};

#endif
