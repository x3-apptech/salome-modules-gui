#ifndef VTKVIEWER_TRANSFORMFILTER_H
#define VTKVIEWER_TRANSFORMFILTER_H

#include "VTKViewer.h"

#include <vtkTransformFilter.h>

class VTKVIEWER_EXPORT VTKViewer_TransformFilter : public vtkTransformFilter
{
public:
  static VTKViewer_TransformFilter *New();
  vtkTypeMacro(VTKViewer_TransformFilter,vtkTransformFilter);

protected:
  VTKViewer_TransformFilter() {}
  ~VTKViewer_TransformFilter() {}
  VTKViewer_TransformFilter(const VTKViewer_TransformFilter&) {}
  void operator=(const VTKViewer_TransformFilter&) {}
  void Execute();
};

#endif
