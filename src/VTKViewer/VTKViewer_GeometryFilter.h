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

#ifndef VTKVIEWER_GEOMETRYFILTER_H
#define VTKVIEWER_GEOMETRYFILTER_H

#include "VTKViewer.h"

#include <vtkGeometryFilter.h>

#include <map>
#include <vector>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class vtkUnstructuredGrid;

/*! \brief This class used same as vtkGeometryFilter. See documentation on VTK for more information.
 */
class VTKVIEWER_EXPORT VTKViewer_GeometryFilter : public vtkGeometryFilter 
{
public:
  /*! \fn static VTKViewer_GeometryFilter *New()
   */
  static VTKViewer_GeometryFilter *New();
  
  /*! \fn vtkTypeMacro(VTKViewer_GeometryFilter, vtkGeometryFilter)
   *  \brief VTK type revision macros.
   */
  vtkTypeMacro(VTKViewer_GeometryFilter, vtkGeometryFilter);
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
  /*! \fn int GetAppendCoincident3D()
   * \brief Return value of \a myAppendCoincident3D
   * \retval myAppendCoincident3D
   */
  int GetAppendCoincident3D() const;
  /*! \fn void SetAppendCoincident3D()
   *  \brief Sets \a myAppendCoincident3D flag. If this flag is true, filter append to the 
      result data set coincident 3D elements.
   */
  void SetAppendCoincident3D(int theFlag);
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

  virtual void SetQuadraticArcMode(bool theFlag);
  virtual bool GetQuadraticArcMode() const;

  virtual void   SetQuadraticArcAngle(double theMaxAngle);
  virtual double GetQuadraticArcAngle() const;

  typedef std::vector<vtkIdType> TVectorId;
  //typedef std::map<vtkIdType, TVectorId> TMapOfVectorId;
  typedef std::vector<TVectorId> TMapOfVectorId;

  static void InsertId( const vtkIdType theCellId,
                        const vtkIdType theCellType,
                        TVectorId& theVTK2ObjIds,
                        TMapOfVectorId& theDimension2VTK2ObjIds );

protected:
  /*! \fn VTKViewer_GeometryFilter();
   * \brief Constructor which sets \a myShowInside = 0 and \a myStoreMapping = 0
   */
  VTKViewer_GeometryFilter();
  /*! \fn ~VTKViewer_GeometryFilter();
   * \brief Destructor.
   */
  ~VTKViewer_GeometryFilter();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  //special cases for performance
  
  /*! \fn void UnstructuredGridExecute();
   * \brief Filter culculation method for data object type is VTK_UNSTRUCTURED_GRID.
   */
  int UnstructuredGridExecute (vtkDataSet *, vtkPolyData *, vtkInformation *);


  void BuildArcedPolygon(vtkIdType cellId,
                         vtkUnstructuredGrid* input,
                         vtkPolyData *output,
                         TMapOfVectorId& theDimension2VTK2ObjIds,
                         bool triangulate = false);
    
private:
  TVectorId myVTK2ObjIds;
  int       myShowInside;
  int       myStoreMapping;
  int       myIsWireframeMode;
  int       myAppendCoincident3D;

  double    myMaxArcAngle;   // define max angle for mesh 2D quadratic element in the degrees
  bool      myIsBuildArc;     // flag for representation 2D quadratic element as arked polygon
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
