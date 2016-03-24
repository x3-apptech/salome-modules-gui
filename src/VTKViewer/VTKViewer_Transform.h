// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
