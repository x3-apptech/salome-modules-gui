// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   :
//  Author :
//  Module :
//  $Header$
//

#ifndef VTKVIEWER_OPENGLRENDERER_H
#define VTKVIEWER_OPENGLRENDERER_H

#include "VTKViewer.h"
#include "vtkOpenGLRenderer.h"

class VTKVIEWER_EXPORT VTKViewer_OpenGLRenderer : public vtkOpenGLRenderer
{
public:
  enum {
    HorizontalGradient,
    VerticalGradient,
    FirstDiagonalGradient,
    SecondDiagonalGradient,
    FirstCornerGradient,
    SecondCornerGradient,
    ThirdCornerGradient,
    FourthCornerGradient
  };

public:
  static VTKViewer_OpenGLRenderer *New();
  vtkTypeMacro(VTKViewer_OpenGLRenderer,vtkOpenGLRenderer);

  virtual void SetGradientType( const int );

  virtual void Clear(void);

protected:
  VTKViewer_OpenGLRenderer();
  ~VTKViewer_OpenGLRenderer();

private:
  int GradientType;

private:
  VTKViewer_OpenGLRenderer(const VTKViewer_OpenGLRenderer&);  // Not implemented.
  void operator=(const VTKViewer_OpenGLRenderer&);  // Not implemented.
};

#endif
