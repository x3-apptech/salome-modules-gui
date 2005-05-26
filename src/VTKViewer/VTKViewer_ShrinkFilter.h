#ifndef VTKVIEWER_SHRINKFILTER_H
#define VTKVIEWER_SHRINKFILTER_H

#include "VTKViewer.h"

#include <vtkShrinkFilter.h>

#include <vector>

class VTKVIEWER_EXPORT VTKViewer_ShrinkFilter : public vtkShrinkFilter 
{
public:
  static VTKViewer_ShrinkFilter *New();
  vtkTypeRevisionMacro(VTKViewer_ShrinkFilter, vtkShrinkFilter);

  void SetStoreMapping(int theStoreMapping);
  int GetStoreMapping(){ return myStoreMapping;}

  virtual vtkIdType GetNodeObjId(int theVtkID);
  virtual vtkIdType GetElemObjId(int theVtkID) { return theVtkID;}

protected:
  VTKViewer_ShrinkFilter();
  ~VTKViewer_ShrinkFilter();
  
  void Execute();
  void UnstructuredGridExecute();
    
private:
  int myStoreMapping;   
  typedef std::vector<vtkIdType> TVectorId;
  TVectorId myVTK2ObjIds;
};

#endif
