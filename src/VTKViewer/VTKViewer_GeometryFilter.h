// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef VTKVIEWER_GEOMETRYFILTER_H
#define VTKVIEWER_GEOMETRYFILTER_H

#include "VTKViewer.h"

#include <vtkGeometryFilter.h>

#include <vector>
/*! \brief This class used same as vtkGeometryFilter. See documentation on VTK for more information.
 */
class VTKVIEWER_EXPORT VTKViewer_GeometryFilter : public vtkGeometryFilter 
{
public:
  /*! \fn static VTKViewer_GeometryFilter *New()
   */
  static VTKViewer_GeometryFilter *New();
  
  /*! \fn vtkTypeRevisionMacro(VTKViewer_GeometryFilter, vtkGeometryFilter)
   *  \brief VTK type revision macros.
   */
  vtkTypeRevisionMacro(VTKViewer_GeometryFilter, vtkGeometryFilter);
  /*! \fn void SetInside(int theShowInside)
   * \brief Sets \a myShowInside flag. \a myShowInside is changed, call this->Modified().
   * \param theShowInside - used for changing value of \a myShowInside variable.
   */
  void SetInside(int theShowInside);
  /*! \fn int GetInside()
   * \brief Return value of \a myShowInside
   * \retval myShowInside
   */
  int GetInside();
  /*! \fn void SetWireframeMode(int theIsWireframeMode)
   * \brief Sets \a myIsWireframeMode flag. \a myIsWireframeMode is changed, call this->Modified().
   * \param theIsWireframeMode - used for changing value of \a myIsWireframeMode variable.
   */
  void SetWireframeMode(int theIsWireframeMode);
  /*! \fn int GetWireframeMode()
   * \brief Return value of \a myIsWireframeMode
   * \retval myIsWireframeMode
   */
  int GetWireframeMode();
  /*! \fn void SetStoreMapping(int theStoreMapping);
   * \brief Sets \a myStoreMapping flag and call this->Modified()
   * \param theStoreMapping - used for changing value of \a myStoreMapping variable.
   */
  void SetStoreMapping(int theStoreMapping);
  /*! \fn int GetStoreMapping()
   * \brief Return value of \a myStoreMapping
   * \retval myStoreMapping
   */
  int GetStoreMapping();
  /*! \fn virtual vtkIdType GetNodeObjId(int theVtkID)
   * \brief Return input value theVtkID
   * \retval theVtkID
   */
  virtual vtkIdType GetNodeObjId(int theVtkID) { return theVtkID;}
  /*! \fn virtual vtkIdType GetElemObjId(int theVtkID);
   * \brief Return object ID by VTK ID cell 
   * \retval myVTK2ObjIds[theVtkID]
   */
  virtual vtkIdType GetElemObjId(int theVtkID);

protected:
  /*! \fn VTKViewer_GeometryFilter();
   * \brief Constructor which sets \a myShowInside = 0 and \a myStoreMapping = 0
   */
  VTKViewer_GeometryFilter();
  /*! \fn ~VTKViewer_GeometryFilter();
   * \brief Destructor.
   */
  ~VTKViewer_GeometryFilter();
  /*! \fn void Execute();
   * \brief Filter culculation method.
   */
  void Execute();
  /*! \fn void UnstructuredGridExecute();
   * \brief Filter culculation method for data object type is VTK_UNSTRUCTURED_GRID.
   */
  void UnstructuredGridExecute();
    
private:
  typedef std::vector<vtkIdType> TVectorId;

private:
  TVectorId myVTK2ObjIds;
  int       myShowInside;
  int       myStoreMapping;
  int       myIsWireframeMode;
};

#endif
