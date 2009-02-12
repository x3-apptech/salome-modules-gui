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
//  File   : SVTK_KeyFreeInteractorStyle.h
//  Author : Christophe ATTANASIO
//  Module : SALOME
//  $Header$
//
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

  //! Redefine SVTK_InteractorStyle::OnLeftButtonDown
  virtual void OnLeftButtonDown(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnMiddleButtonDown
  virtual void OnMiddleButtonDown(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnLeftButtonUp
  virtual void OnLeftButtonUp(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnMiddleButtonUp
  virtual void OnMiddleButtonUp(int ctrl, int shift, int x, int y);

  //! Redefine SVTK_InteractorStyle::OnChar
  virtual void OnChar();
  
  bool myIsMidButtonDown;
  bool myIsLeftButtonDown;
};

#endif
