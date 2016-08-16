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

#ifndef VTKVIEWER_OPENGLRENDERER_H
#define VTKVIEWER_OPENGLRENDERER_H

#include "VTKViewer.h"
#include "vtkOpenGLRenderer.h"
#include "VTKViewer_OpenGLHelper.h"

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

#ifdef VTK_OPENGL2
  VTKViewer_OpenGLHelper OpenGLHelper;
  GLhandleARB            BackgroundProgram;
  GLhandleARB            BackgroundVertexShader;
  GLhandleARB            BackgroundFragmentShader;
  GLuint                 VertexArrayObject;

  struct Locations
  {
    static const GLint INVALID_LOCATION = -1;

    GLint UseTexture;
    GLint BackgroundTexture;

    Locations()
    : UseTexture        (INVALID_LOCATION),
      BackgroundTexture (INVALID_LOCATION)
    {
      //
    }

  } myLocations;
#endif


private:
  VTKViewer_OpenGLRenderer(const VTKViewer_OpenGLRenderer&);  // Not implemented.
  void operator=(const VTKViewer_OpenGLRenderer&);  // Not implemented.
};

#endif
