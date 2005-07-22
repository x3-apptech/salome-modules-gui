#ifndef VTKVIEWER_TRANSFORM_H
#define VTKVIEWER_TRANSFORM_H

#include "VTKViewer.h"

#include <vtkTransform.h>

/*!\brief Describes linear transformations via a 4x4 matrix.
 *@see vtkTransform class
 */
class VTKVIEWER_EXPORT VTKViewer_Transform : public vtkTransform
{
public:
  /*!Create new instance of VTKViewer_Transform.*/
  static VTKViewer_Transform *New();
  vtkTypeMacro( VTKViewer_Transform, vtkTransform );

  int  IsIdentity();
  //merge with V2_2_0_VISU_improvements:void SetScale( float theScaleX, float theScaleY, float theScaleZ );
  void SetMatrixScale(double theScaleX, double theScaleY, double theScaleZ);
  void GetMatrixScale(double theScale[3]);

protected:
  /*!Constructor.*/
  VTKViewer_Transform() {/*!Do nothing*/}
  /*!Copy contructor.*/
  VTKViewer_Transform(const VTKViewer_Transform&) {/*!Do nothing*/}
  /*!Destructor.*/
  ~VTKViewer_Transform() {/*!Do nothing*/}

  /*!Operator = */
  void operator=( const VTKViewer_Transform& ) {/*!Do nothing*/}
};

#endif
