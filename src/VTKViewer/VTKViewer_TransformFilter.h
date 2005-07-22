#ifndef VTKVIEWER_TRANSFORMFILTER_H
#define VTKVIEWER_TRANSFORMFILTER_H

#include "VTKViewer.h"

#include <vtkTransformFilter.h>

/*!Transform points and associated normals and vectors
 *@see vtkTransformFilter
 */
class VTKVIEWER_EXPORT VTKViewer_TransformFilter : public vtkTransformFilter
{
public:
  /*!Create new instance of VTKViewer_TransformFilter.*/
  static VTKViewer_TransformFilter *New();
  vtkTypeMacro(VTKViewer_TransformFilter,vtkTransformFilter);

protected:
  /*!Constructor.*/
  VTKViewer_TransformFilter() {/*!Do nothing*/}
  /*!Destructor.*/
  ~VTKViewer_TransformFilter() {/*!Do nothing*/}
  /*!Copy constructor.*/
  VTKViewer_TransformFilter(const VTKViewer_TransformFilter&) {/*!Do nothing*/}
  /*!Operator = */
  void operator=(const VTKViewer_TransformFilter&) {/*!Do nothing*/}
  void Execute();
};

#endif
