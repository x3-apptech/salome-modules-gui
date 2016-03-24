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
//  File   : SVTK_KeyFreeInteractorStyle.h
//  Author : Christophe ATTANASIO

#ifndef __SVTK_KeyFreeInteractorStyle_h
#define __SVTK_KeyFreeInteractorStyle_h

#include "SVTK.h"

#include "SVTK_InteractorStyle.h"

//! Introduce new style of interaction (keyboard free)
class SVTK_EXPORT SVTK_KeyFreeInteractorStyle : public SVTK_InteractorStyle
{
 public:
  static SVTK_KeyFreeInteractorStyle *New();
  vtkTypeMacro(SVTK_KeyFreeInteractorStyle,SVTK_InteractorStyle);
  
 protected:
  SVTK_KeyFreeInteractorStyle();
  ~SVTK_KeyFreeInteractorStyle();

  SVTK_KeyFreeInteractorStyle(const SVTK_KeyFreeInteractorStyle&); // Not implemented
  void operator=(const SVTK_KeyFreeInteractorStyle&); // Not implemented

  // Generic event bindings must be overridden in subclasses

  //! Redefine SVTK_InteractorStyle::OnMouseMove
  virtual void OnMouseMove(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnLeftButtonDown
  virtual void OnLeftButtonDown(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnMiddleButtonDown
  virtual void OnMiddleButtonDown(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnRightButtonDown
  virtual void OnRightButtonDown(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnLeftButtonUp
  virtual void OnLeftButtonUp(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnMiddleButtonUp
  virtual void OnMiddleButtonUp(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnRightButtonUp
  virtual void OnRightButtonUp(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnChar
  virtual void OnChar();

  bool myIsLeftButtonDown;
  bool myIsRightButtonDown;
};

#endif
