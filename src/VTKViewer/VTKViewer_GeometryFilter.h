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
  /*! \fn void SetStoreMapping(int theStoreMapping);
   * \brief Sets \a myStoreMapping flag and call this->Modified()
   * \param theStoreMapping - used for changing value of \a myStoreMapping variable.
   */
  void SetStoreMapping(int theStoreMapping);
  /*! \fn int GetStoreMapping()
   * \brief Return value of \a myStoreMapping
   * \retval myStoreMapping
   */
  int GetStoreMapping(){ return myStoreMapping;}
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
};

#endif
