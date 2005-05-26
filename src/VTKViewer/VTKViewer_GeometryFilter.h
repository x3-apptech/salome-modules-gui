#ifndef VTKVIEWER_GEOMETRYFILTER_H
#define VTKVIEWER_GEOMETRYFILTER_H

#include "VTKViewer.h"

#include <vtkGeometryFilter.h>

#include <vector>

class VTKVIEWER_EXPORT VTKViewer_GeometryFilter : public vtkGeometryFilter 
{
public:
  static VTKViewer_GeometryFilter *New();
  vtkTypeRevisionMacro(VTKViewer_GeometryFilter, vtkGeometryFilter);
  void SetInside(int theShowInside);
  int GetInside();

  void SetStoreMapping(int theStoreMapping);
  int GetStoreMapping(){ return myStoreMapping;}

  virtual vtkIdType GetNodeObjId(int theVtkID) { return theVtkID;}
  virtual vtkIdType GetElemObjId(int theVtkID);

protected:
  VTKViewer_GeometryFilter();
  ~VTKViewer_GeometryFilter();
  
  void Execute();
  void UnstructuredGridExecute();
    
private:
  typedef std::vector<vtkIdType> TVectorId;

private:
  TVectorId myVTK2ObjIds;
  int       myShowInside;
  int       myStoreMapping;
};

#endif
