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

#ifndef VTKViewer_POLYDATAMAPPER_H
#define VTKViewer_POLYDATAMAPPER_H

#include "VTKViewer.h"
#include "VTKViewer_MarkerDef.h"

#ifdef WNT
#include <windows.h>
#endif

#include <map>

#include <GL/gl.h>

#include <vtkSmartPointer.h>

class vtkImageData;

#ifndef VTK_IMPLEMENT_MESA_CXX
#include <vtkOpenGLPolyDataMapper.h>
#define MAPPER_SUPERCLASS vtkOpenGLPolyDataMapper
#else
#include <vtkMesaPolyDataMapper.h>
#define MAPPER_SUPERCLASS vtkMesaPolyDataMapper
#endif

//----------------------------------------------------------------------------
//! OpenGL Point Sprites PolyData Mapper.
/*!
 * VTKViewer_PolyDataMapper is a class that maps polygonal data 
 * (i.e., vtkPolyData) to graphics primitives. It is performing the mapping
 * to the rendering/graphics hardware/software. It is now possible to set a 
 * memory limit for the pipeline in the mapper. If the total estimated memory 
 * usage of the pipeline is larger than this limit, the mapper will divide 
 * the data into pieces and render each in a for loop.
 */
class VTKVIEWER_EXPORT VTKViewer_PolyDataMapper : public MAPPER_SUPERCLASS
{
public:
  enum ExtensionsState { ES_None = 0, ES_Error, ES_Ok };

public:
  static VTKViewer_PolyDataMapper* New();
  vtkTypeRevisionMacro( VTKViewer_PolyDataMapper, MAPPER_SUPERCLASS );

  //! Set point marker enabled
  void SetMarkerEnabled( bool );

  //! Set standard point marker
  void SetMarkerStd( VTK::MarkerType, VTK::MarkerScale );

  //! Set custom point marker
  void SetMarkerTexture( int, VTK::MarkerTexture );

  //! Get type of the point marker
  VTK::MarkerType GetMarkerType();

  //! Get scale of the point marker
  VTK::MarkerScale GetMarkerScale();

  //! Get texture identifier of the point marker
  int GetMarkerTexture();

  //! Implement superclass render method.
  virtual void RenderPiece( vtkRenderer*, vtkActor* );

  //! Draw method for OpenGL.
  virtual int Draw( vtkRenderer*, vtkActor* );

protected:
  VTKViewer_PolyDataMapper();
  ~VTKViewer_PolyDataMapper();

  //! Initializing OpenGL extensions.
  int               InitExtensions();

  //! Activate Point Sprites.
  void              InitPointSprites();

  //! Deactivate Point Sprites.
  void              CleanupPointSprites();

  //! Initializing textures for Point Sprites.
  void              InitTextures();

private:
  int               ExtensionsInitialized;

  GLuint            PointSpriteTexture;

  vtkSmartPointer<vtkImageData> ImageData;

  bool              MarkerEnabled;
  VTK::MarkerType   MarkerType;
  VTK::MarkerScale  MarkerScale;
  int               MarkerId;

  typedef std::map< int, vtkSmartPointer<vtkImageData> > ImageDataMap;
  ImageDataMap      StandardTextures;
  ImageDataMap      CustomTextures;
};

#endif
