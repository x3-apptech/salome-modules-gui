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
  //merge with V2_2_0_VISU_improvements:void SetScale( float theScaleX, float theScaleY, float theScaleZ );
  void SetMatrixScale(double theScaleX, double theScaleY, double theScaleZ);
  void GetMatrixScale(double theScale[3]);

protected:
  VTKViewer_Transform() {}
  VTKViewer_Transform(const VTKViewer_Transform&) {}
  ~VTKViewer_Transform() {}

  void operator=( const VTKViewer_Transform& ) {}
};

#endif
