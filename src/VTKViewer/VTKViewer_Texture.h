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

#ifndef VTKVIEWER_TEXTURE_H
#define VTKVIEWER_TEXTURE_H

#include "VTKViewer.h"
#include <vtkOpenGLTexture.h>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class vtkRenderer;

class VTKVIEWER_EXPORT VTKViewer_Texture : public vtkOpenGLTexture
{
public:
  enum { Stretched = 0, Centered, Tiled };

public:
  static VTKViewer_Texture *New();
  vtkTypeMacro(VTKViewer_Texture,vtkOpenGLTexture);

  virtual void Load(vtkRenderer *ren);

  int GetWidth() const;
  int GetHeight() const;

  void SetPosition(int type);
  int GetPosition() const;

protected:
  VTKViewer_Texture();
  ~VTKViewer_Texture();

  int myWidth;
  int myHeight;
  int myPosition;

private:
  virtual void Initialize(vtkRenderer * ren);
};

#endif
