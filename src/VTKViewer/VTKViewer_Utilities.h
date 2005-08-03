#ifndef VTKVIEWER_UTILITIES_H
#define VTKVIEWER_UTILITIES_H

#include <VTKViewer.h>

class vtkRenderer;

VTKVIEWER_EXPORT extern void  ResetCamera(vtkRenderer* theRenderer, int theUsingZeroFocalPoint = false);
VTKVIEWER_EXPORT extern int   ComputeVisiblePropBounds(vtkRenderer* theRenderer, float theBounds[6]);
VTKVIEWER_EXPORT extern void  ResetCameraClippingRange(vtkRenderer* theRenderer);
VTKVIEWER_EXPORT extern bool  ComputeTrihedronSize(vtkRenderer* theRenderer, double& theNewSize, 
				  const double theSize, const float theSizeInPercents);

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif
