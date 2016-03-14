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

#ifndef VTKViewer_DATASETMAPPER_H
#define VTKViewer_DATASETMAPPER_H

#include "VTKViewer.h"
#include "VTKViewer_MarkerDef.h"

#include <vtkDataSetMapper.h>

class VTKVIEWER_EXPORT VTKViewer_DataSetMapper : public vtkDataSetMapper 
{
public:
  static VTKViewer_DataSetMapper *New();
  vtkTypeMacro(VTKViewer_DataSetMapper,vtkDataSetMapper);

  //! Set point marker enabled
  void SetMarkerEnabled( bool );
  
  //! Set ball element enabled
  void SetBallEnabled( bool );
  
  //! Set ball element scale factor
  void SetBallScale( double );
  
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
  void Render(vtkRenderer *ren, vtkActor *act);

protected:
  VTKViewer_DataSetMapper();
  ~VTKViewer_DataSetMapper();

private:
  VTKViewer_DataSetMapper(const VTKViewer_DataSetMapper&);  // Not implemented.
  void operator=(const VTKViewer_DataSetMapper&);  // Not implemented.

private:
  bool               MarkerEnabled;
  bool               BallEnabled;
  double           BallScale;
  VTK::MarkerType    MarkerType;
  VTK::MarkerScale   MarkerScale;
  int                MarkerId;
  VTK::MarkerTexture MarkerTexture;
};

#endif
