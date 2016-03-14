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
//  File   : 
//  Author : 

#ifndef SVTK_UPDATERATEDLG_H
#define SVTK_UPDATERATEDLG_H

#include <ViewerTools_DialogBase.h>

#include <vtkSmartPointer.h>

class SVTK_ViewWindow;
class SVTK_RenderWindowInteractor;

class QtxDoubleSpinBox;
class QtxAction;

class QGroupBox;
class QLineEdit;

class vtkCallbackCommand;
class vtkObject;


class SVTK_UpdateRateDlg : public ViewerTools_DialogBase
{
  Q_OBJECT;

public:
  SVTK_UpdateRateDlg(QtxAction* theAction,
                     SVTK_ViewWindow* theParent,
                     const char* theName);

  ~SVTK_UpdateRateDlg();

  void Update();

protected:
  SVTK_RenderWindowInteractor* myRWInteractor;
  QtxDoubleSpinBox* myDesiredUpdateRateSblSpinBox;
  QtxDoubleSpinBox* myStillUpdateRateSblSpinBox;
  QGroupBox* myIsEnableUpdateRateGroupBox;

  QLineEdit* myCurrentUpdateRateLineEdit;
  QLineEdit* myNumberOfCellsLineEdit;

  QtxAction* myAction;

  //----------------------------------------------------------------------------
  // Priority at which events are processed
  double myPriority;

  // Used to process events
  vtkSmartPointer<vtkCallbackCommand> myEventCallbackCommand;

  // Description:
  // Main process event method
  static void ProcessEvents(vtkObject* object, 
                            unsigned long event,
                            void* clientdata, 
                            void* calldata);
  
protected slots:
  void onClickOk();
  void onClickApply();
  void onClickClose();
};

#endif // SVTK_UPDATERATEDLG_H
