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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SVTK_RectPicker.h
//  Author : 
//  Module : SALOME

#ifndef __SVTK_RectPicker_h
#define __SVTK_RectPicker_h

#include "SVTK.h"
#include "VTKViewer.h"

#include <map>
#include <vector>

#include <vtkAbstractPropPicker.h>

class vtkRenderer;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*! \class vtkAbstractPropPicker
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation
 */
/*! \class SVTK_RectPicker
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
  vtkSetMacro(Tolerance,vtkFloatingPointType);
  vtkGetMacro(Tolerance,vtkFloatingPointType);

  //! Use these methods to pick points or points and cells
  vtkSetMacro(PickPoints,int);
  vtkGetMacro(PickPoints,int);
  vtkBooleanMacro(PickPoints,int);

  virtual 
  int
  Pick(vtkFloatingPointType theSelectionX, 
       vtkFloatingPointType theSelectionY, 
       vtkFloatingPointType theSelectionZ, 
       vtkFloatingPointType theSelectionX2, 
       vtkFloatingPointType theSelectionY2, 
       vtkFloatingPointType theSelectionZ2,
       vtkRenderer *theRenderer);  

  int
  Pick(vtkFloatingPointType theSelection[3], 
       vtkFloatingPointType theSelection2[3], 
       vtkRenderer *theRenderer);

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
  vtkFloatingPointType Tolerance;

  //! use the following to control picking mode
  int PickPoints;

  //! second rectangle selection point in window (pixel) coordinates
  vtkFloatingPointType SelectionPoint2[3]; 

  //! second rectangle selection point in world coordinates
  vtkFloatingPointType PickPosition2[3]; 

  TVectorIdsMap myPointIdsMap;
  TVectorIdsMap myCellIdsMap;

 private:
  virtual 
  int
  Pick(vtkFloatingPointType, 
       vtkFloatingPointType, 
       vtkFloatingPointType, 
       vtkRenderer*);
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif


