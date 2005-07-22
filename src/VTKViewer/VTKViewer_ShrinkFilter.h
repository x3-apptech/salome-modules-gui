#ifndef VTKVIEWER_SHRINKFILTER_H
#define VTKVIEWER_SHRINKFILTER_H

#include "VTKViewer.h"

#include <vtkShrinkFilter.h>

#include <vector>

/*!Shrink cells composing an arbitrary data set.
 *\warning It is possible to turn cells inside out or cause self intersection in special cases.
 */
class VTKVIEWER_EXPORT VTKViewer_ShrinkFilter : public vtkShrinkFilter 
{
public:
  /*!Create new instance of VTKViewer_ShrinkFilter.*/
  static VTKViewer_ShrinkFilter *New();
  vtkTypeRevisionMacro(VTKViewer_ShrinkFilter, vtkShrinkFilter);

  void SetStoreMapping(int theStoreMapping);
  /*!Gets store mapping flag.*/
  int GetStoreMapping(){ return myStoreMapping;}

  virtual vtkIdType GetNodeObjId(int theVtkID);
  /*!Return element id by vtk id.*/
  virtual vtkIdType GetElemObjId(int theVtkID) { return theVtkID;}

protected:
  VTKViewer_ShrinkFilter();
  ~VTKViewer_ShrinkFilter();
  
  void Execute();
  /*!Not implemented.*/
  void UnstructuredGridExecute();
    
private:
  int myStoreMapping;   
  typedef std::vector<vtkIdType> TVectorId;
  TVectorId myVTK2ObjIds;
};

#endif
