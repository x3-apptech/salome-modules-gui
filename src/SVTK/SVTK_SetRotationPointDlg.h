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

#ifndef SVTK_SETROTATIONPOINTDLG_H
#define SVTK_SETROTATIONPOINTDLG_H

#include "SVTK.h"
#include "SVTK_Selection.h"

#include <ViewerTools_DialogBase.h>

#include <vtkSmartPointer.h>

#include <QMap>
#include <QList>

class SVTK_ViewWindow;
class SVTK_RenderWindowInteractor;

class QtxAction;

class QLineEdit;
class QPushButton;
class QGroupBox;
class QCheckBox;
class QAction;

class vtkCallbackCommand;
class vtkInteractorStyle;
class vtkObject;

class SVTK_EXPORT SVTK_SetRotationPointDlg : public ViewerTools_DialogBase
{
  Q_OBJECT;

public:
  SVTK_SetRotationPointDlg(QtxAction* theAction,
                           SVTK_ViewWindow* theParent,
                           const char* theName);

  ~SVTK_SetRotationPointDlg();
  
  void addObserver();
  bool IsFirstShown();
  
protected:
  SVTK_ViewWindow *myMainWindow;
  SVTK_RenderWindowInteractor* myRWInteractor;
  QList<vtkInteractorStyle*> myInteractorStyleList;
  
  QMap<QAction*, Selection_Mode> mySelectActions;

  QCheckBox*    myIsBBCenter;

  QGroupBox   * myGroupBoxSel;
  QPushButton*  myToOrigin;
  QPushButton*  mySelectPoint;

  QGroupBox* myGroupBoxCoord;
  QLineEdit* myX;
  QLineEdit* myY;
  QLineEdit* myZ;

  void setEnabled(QGroupBox* theGrp, const bool theState);
  bool IsObserverAdded( vtkInteractorStyle* );

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
  void onBBCenterChecked();
  
  void onToOrigin();
  void onSelectMenu( QAction* );

  void onCoordChanged();

  void onClickClose();

};

#endif // SVTK_SETROTATIONPOINTDLG_H
