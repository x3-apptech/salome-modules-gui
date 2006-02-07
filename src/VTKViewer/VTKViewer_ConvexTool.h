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

#ifndef _VTKViewer_ConvexTool_H
#define _VTKViewer_ConvexTool_H

#include <vtkUnstructuredGrid.h>
#include <vector>
#include <map>

typedef std::vector<vtkIdType> TCell; // ptsIds
typedef std::map<vtkIdType,TCell> TCellArray; // CellId, TCell

/*! This package \namespace CONVEX_TOOL used for: 
 *  calculation of VTK_POLYGON cell array from VTK_TRIANGLE (triangulation)
 *  of VTK_CONVEX_POINT_SET cell type.
 */
namespace CONVEX_TOOL
{
  /*! \fn void CONVEX_TOOL::GetPolygonalFaces(vtkUnstructuredGrid* theCell,int cellId,TCellArray &outputCellArray)
   *  \brief Main function.
   *  \param theCell - vtkUnstructuredGrid cell pointer
   *  \param cellId  - id of cell type VTK_CONVEX_POINT_SET
   *  \retval outputCellArray - output array with new cells types VTK_POLYGON
   */
  void
    WriteToFile(vtkUnstructuredGrid* theDataSet, const std::string& theFileName);
  void GetPolygonalFaces(vtkUnstructuredGrid* theCell,int cellId,TCellArray &outputCellArray);
}

#endif // _VTKViewer_ConvexTool_H
