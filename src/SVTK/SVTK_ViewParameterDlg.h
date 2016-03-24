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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 

#ifndef SVTK_VIEWPARAMETERDLG_H
#define SVTK_VIEWPARAMETERDLG_H

#include "SVTK.h"

#include <ViewerTools_DialogBase.h>

#include <vtkSmartPointer.h>
#include <vtkTimeStamp.h>

class SVTK_ViewWindow;
class SVTK_RenderWindowInteractor;

class QtxAction;

class QLineEdit;
class QPushButton;
class QFrame;
class QCheckBox;
class QButtonGroup;

class vtkCallbackCommand;
class vtkObject;

class SVTK_EXPORT SVTK_ViewParameterDlg : public ViewerTools_DialogBase
{
  Q_OBJECT;

public:
  SVTK_ViewParameterDlg(QtxAction* theAction,
                        SVTK_ViewWindow* theParent,
                        const char* theName);

  ~SVTK_ViewParameterDlg();
  
  void addObserver();
  
protected:
  SVTK_ViewWindow *myMainWindow;
  SVTK_RenderWindowInteractor* myRWInteractor;
  bool myIsObserverAdded;
  bool myBusy;
  
  QButtonGroup* myProjectionMode;

  QPushButton*  myToBBCenter;
  QPushButton*  myToOrigin;
  QPushButton*  mySelectPoint;

  QFrame*       myFocalCoords;
  QLineEdit*    myFocalX;
  QLineEdit*    myFocalY;
  QLineEdit*    myFocalZ;
  
  QButtonGroup* myCameraPositionMode;

  QFrame*       myCameraCoords;
  QLineEdit*    myCameraX;
  QLineEdit*    myCameraY;
  QLineEdit*    myCameraZ;

  QFrame*       myProjDirection;
  QLineEdit*    myProjDirX;
  QLineEdit*    myProjDirY;
  QLineEdit*    myProjDirZ;

  QLineEdit*    myDistance;

  QFrame*       myViewDirection;
  QLineEdit*    myViewDirX;
  QLineEdit*    myViewDirY;
  QLineEdit*    myViewDirZ;

  QFrame*       myScaleBox;
  QLineEdit*    myScale;

  QFrame*       myViewAngleBox;
  QLineEdit*    myViewAngle;

  void setEnabled(QFrame* theWidget, const bool theState);
  bool computePoint(const double start[3], const double dir[3], 
                    const double dist, double result[3]);
  void updateProjection();
  void updateCoordinates();

  void updateData();

  //----------------------------------------------------------------------------
  // Priority at which events are processed
  double myPriority;

  // Used to process events
  vtkSmartPointer<vtkCallbackCommand> myEventCallbackCommand;

  // Used to update camera
  vtkTimeStamp myCameraMTime;

  // Description:
  // Main process event method
  static void ProcessEvents(vtkObject* object, 
                            unsigned long event,
                            void* clientdata, 
                            void* calldata);

protected slots:
  void onProjectionModeChanged(int);
  void onPositionModeChanged(int);
  
  void onToBBCenter();
  void onToOrigin();
  void onSelectPoint();

  void onFocalCoordChanged();
  void onCameraCoordChanged();
  void onDirectionChanged();
  void onDistanceChanged();
  void onViewDirectionChanged();

  void onZoomChanged();

  void onClickClose();

};

#endif // SVTK_VIEWPARAMETERDLG_H
