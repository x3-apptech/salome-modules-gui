//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : SVTK_KeyFreeInteractorStyle.cxx
//  Author : Christophe ATTANASIO
//  Module : SALOME
//  $Header$
//
#include "SVTK_KeyFreeInteractorStyle.h"
#include "SVTK_Selector.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>


//----------------------------------------------------------------------------
vtkStandardNewMacro(SVTK_KeyFreeInteractorStyle);
//----------------------------------------------------------------------------

SVTK_KeyFreeInteractorStyle::SVTK_KeyFreeInteractorStyle():
  myIsMidButtonDown( false ),
  myIsLeftButtonDown( false )
{
}

//----------------------------------------------------------------------------
SVTK_KeyFreeInteractorStyle::~SVTK_KeyFreeInteractorStyle() 
{
}

//----------------------------------------------------------------------------
void SVTK_KeyFreeInteractorStyle::OnLeftButtonDown(int ctrl, int shift, 
						   int x, int y) 
{
  myIsLeftButtonDown = true;

  if (this->HasObserver(vtkCommand::LeftButtonPressEvent)) {
    this->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL);
    return;
  }
  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL) {
    return;
  }
  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  myOtherPoint = myPoint = QPoint(x, y);
  if (ForcedState != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    startOperation(ForcedState);
  } 
  else {
    if (!(ctrl||shift)){
      if (myIsMidButtonDown){
	startOperation(VTK_INTERACTOR_STYLE_CAMERA_ZOOM);
      }
      else{
	startOperation(VTK_INTERACTOR_STYLE_CAMERA_ROTATE);
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
void SVTK_KeyFreeInteractorStyle::OnMiddleButtonDown(int ctrl,
						     int shift, 
						     int x, int y) 
{
  myIsMidButtonDown = true;

  if (this->HasObserver(vtkCommand::MiddleButtonPressEvent))  {
    this->InvokeEvent(vtkCommand::MiddleButtonPressEvent,NULL);
    return;
  }
  this->FindPokedRenderer(x, y);
  if (this->CurrentRenderer == NULL)    {
    return;
  }
  myShiftState = shift;
  // finishing current viewer operation
  if (State != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    onFinishOperation();
    startOperation(VTK_INTERACTOR_STYLE_CAMERA_NONE);
  }
  myOtherPoint = myPoint = QPoint(x, y);
  if (ForcedState != VTK_INTERACTOR_STYLE_CAMERA_NONE) {
    startOperation(ForcedState);
  }
  else {
    if (!(ctrl||shift)){
      if ( myIsLeftButtonDown ){
	startOperation(VTK_INTERACTOR_STYLE_CAMERA_ZOOM);
      }
      else{
	startOperation(VTK_INTERACTOR_STYLE_CAMERA_PAN);
      }
    }
  }
}

//----------------------------------------------------------------------------
void SVTK_KeyFreeInteractorStyle::OnLeftButtonUp(int ctrl, int shift, int x, int y)
{
  myIsLeftButtonDown = false;
  SVTK_InteractorStyle::OnLeftButtonUp( ctrl, shift, x, y );

  if ( myIsMidButtonDown )
    OnMiddleButtonDown( ctrl, shift, x, y );
}

//----------------------------------------------------------------------------
void SVTK_KeyFreeInteractorStyle::OnMiddleButtonUp(int ctrl, int shift, int x, int y)
{
  myIsMidButtonDown = false;
  SVTK_InteractorStyle::OnMiddleButtonUp( ctrl, shift, x, y );

  if ( myIsLeftButtonDown )
    OnLeftButtonDown( ctrl, shift, x, y );
}

//----------------------------------------------------------------------------
void SVTK_KeyFreeInteractorStyle::OnChar()
{
  char key = GetInteractor()->GetKeyCode();
  switch (key) {
  case 's':
  case 'S':
    ActionPicking();
    EventCallbackCommand->AbortFlagOn();
    return;
  }
  SVTK_InteractorStyle::OnChar();
}
