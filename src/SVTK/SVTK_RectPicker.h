//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : SVTK_RectPicker.h
//  Author : 
//  Module : SALOME

#ifndef __SVTK_RectPicker_h
#define __SVTK_RectPicker_h

#include "SVTK.h"

#include <map>
#include <vector>

#include <vtkAbstractPropPicker.h>

class vtkRenderer;

/*! \class vtkAbstractPropPicker
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation
 */
/*! \class VTKViewer_RectPicker
 * \brief Rectangular picker class.
 */
class SVTK_EXPORT SVTK_RectPicker : public vtkAbstractPropPicker
{
 public:
  static
  SVTK_RectPicker *New();

  vtkTypeMacro(SVTK_RectPicker,vtkAbstractPropPicker);
  
  /*! 
    Specify tolerance for performing pick operation. Tolerance is specified
    as fraction of rendering window size. (Rendering window size is measured
    across diagonal.)
  */
  vtkSetMacro(Tolerance,float);
  vtkGetMacro(Tolerance,float);

  //! Use these methods to pick points or points and cells
  vtkSetMacro(PickPoints,int);
  vtkGetMacro(PickPoints,int);
  vtkBooleanMacro(PickPoints,int);

  virtual 
  int
  Pick(float theSelectionX, float theSelectionY, float theSelectionZ, 
       float theSelectionX2, float theSelectionY2, float theSelectionZ2,
       vtkRenderer *theRenderer);  

  int
  Pick(float theSelection[3], float theSelection2[3], vtkRenderer *theRenderer);

  typedef std::vector<vtkIdType> TVectorIds;
  typedef std::map<vtkActor*,TVectorIds> TVectorIdsMap;

  const TVectorIdsMap& 
  GetPointIdsMap() const;

  const TVectorIdsMap& 
  GetCellIdsMap() const;

 protected:
  SVTK_RectPicker();
  ~SVTK_RectPicker();

  //! tolerance for computation (% of window)
  float Tolerance;

  //! use the following to control picking mode
  int PickPoints;

  //! second rectangle selection point in window (pixel) coordinates
  float SelectionPoint2[3]; 

  //! second rectangle selection point in world coordinates
  float PickPosition2[3]; 

  TVectorIdsMap myPointIdsMap;
  TVectorIdsMap myCellIdsMap;

 private:
  virtual 
  int
  Pick(float, float, float, vtkRenderer*);
};

#endif


