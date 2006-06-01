// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef VTKVIEWER_UTILITIES_H
#define VTKVIEWER_UTILITIES_H

#include "VTKViewer.h"

class vtkRenderer;

VTKVIEWER_EXPORT 
extern
void
ResetCamera(vtkRenderer* theRenderer, 
	    int theUsingZeroFocalPoint = false);

VTKVIEWER_EXPORT
extern
int
ComputeVisiblePropBounds(vtkRenderer* theRenderer, 
			 vtkFloatingPointType theBounds[6]);

VTKVIEWER_EXPORT
extern
void
ResetCameraClippingRange(vtkRenderer* theRenderer);
VTKVIEWER_EXPORT
extern
bool
ComputeTrihedronSize(vtkRenderer* theRenderer, 
		     vtkFloatingPointType& theNewSize, 
		     const vtkFloatingPointType theSize, 
		     const vtkFloatingPointType theSizeInPercents);

#endif
