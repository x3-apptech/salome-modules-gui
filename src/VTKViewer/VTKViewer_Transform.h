#ifndef VTKVIEWER_TRANSFORM_H
#define VTKVIEWER_TRANSFORM_H

#include "VTKViewer.h"

#include <vtkTransform.h>

class VTKVIEWER_EXPORT VTKViewer_Transform : public vtkTransform
{
public:
  static VTKViewer_Transform *New();
  vtkTypeMacro( VTKViewer_Transform, vtkTransform );

  int  IsIdentity();
  void SetScale( float theScaleX, float theScaleY, float theScaleZ );

protected:
  VTKViewer_Transform() {}
  VTKViewer_Transform(const VTKViewer_Transform&) {}
  ~VTKViewer_Transform() {}

  void operator=( const VTKViewer_Transform& ) {}
};

#endif
