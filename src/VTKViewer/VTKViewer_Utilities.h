#ifndef VTKVIEWER_UTILITIES_H
#define VTKVIEWER_UTILITIES_H

class vtkRenderer;

extern void  ResetCamera(vtkRenderer* theRenderer, int theUsingZeroFocalPoint = false);
extern int   ComputeVisiblePropBounds(vtkRenderer* theRenderer, float theBounds[6]);
extern void  ResetCameraClippingRange(vtkRenderer* theRenderer);

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif
