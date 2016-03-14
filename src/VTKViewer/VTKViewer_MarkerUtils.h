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

#ifndef VTKVIEWER_MARKERUTILS_H
#define VTKVIEWER_MARKERUTILS_H

#include "VTKViewer.h"
#include "VTKViewer_MarkerDef.h"

// VTK Includes
#include <vtkSmartPointer.h>

class vtkImageData;

class QImage;
class QString;

namespace VTK
{
  VTKVIEWER_EXPORT bool                          LoadTextureData( const QString& theFileName,
                                                                  VTK::MarkerScale theMarkerScale,
                                                                  VTK::MarkerTexture& theTextureData );

  VTKVIEWER_EXPORT vtkSmartPointer<vtkImageData> MakeVTKImage( const VTK::MarkerTexture& theTextureData,
                                                               bool theWhiteForeground = true );

  VTKVIEWER_EXPORT QImage                        ConvertToQImage( vtkImageData* theImageData );

  VTKVIEWER_EXPORT int                           GetUniqueId( const VTK::MarkerMap& theMarkerMap );
}

#endif
