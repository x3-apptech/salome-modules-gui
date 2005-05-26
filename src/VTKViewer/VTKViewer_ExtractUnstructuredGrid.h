#ifndef VTKVIEWER_EXTRACTUNSTRUCTUREDGRID_H
#define VTKVIEWER_EXTRACTUNSTRUCTUREDGRID_H

#include "VTKViewer.h"

#include <vtkUnstructuredGridToUnstructuredGridFilter.h>

#include <set>
#include <map>
#include <vector>

class VTKVIEWER_EXPORT VTKViewer_ExtractUnstructuredGrid : public vtkUnstructuredGridToUnstructuredGridFilter
{
public:
  vtkTypeMacro( VTKViewer_ExtractUnstructuredGrid, vtkUnstructuredGridToUnstructuredGridFilter );

  // Description:
  // Construct with all types of clipping turned off.
  static VTKViewer_ExtractUnstructuredGrid *New();

  enum EExtraction{ eCells, ePoints};
  void SetModeOfExtraction(EExtraction theExtractionMode){
    myExtractionMode = theExtractionMode; Modified();
  }
  EExtraction GetModeOfExtraction(){ return myExtractionMode;}

  enum EChanging{ ePassAll, eAdding, eRemoving};
  void SetModeOfChanging(EChanging theChangeMode){
    myChangeMode = theChangeMode; 
    Modified();
  }
  EChanging GetModeOfChanging(){ return myChangeMode;}

  // Remove the cell from the output
  void RegisterCell(vtkIdType theCellId);
  int IsCellsRegistered() { return !myCellIds.empty();}
  void ClearRegisteredCells() { 
    myCellIds.clear();
    Modified();
  }
  
  // Remove every cells with the type from the output
  void RegisterCellsWithType(vtkIdType theCellType);
  int IsCellsWithTypeRegistered() { return !myCellTypes.empty();}
  void ClearRegisteredCellsWithType() { 
    myCellTypes.clear();
    Modified();
  }

  // Do the filter do some real work
  int IsChanging() { return IsCellsRegistered() || IsCellsWithTypeRegistered();}

  // Do it keep the mapping between input's and output's UnstructuredGrid
  void SetStoreMapping(int theStoreMapping);
  int GetStoreMapping(){ return myStoreMapping;}

  vtkIdType GetInputId(int theOutId) const;
  vtkIdType GetOutputId(int theInId) const;

  typedef std::vector<vtkIdType> TVectorId;
  typedef std::map<vtkIdType,vtkIdType> TMapId;

protected:
  VTKViewer_ExtractUnstructuredGrid();
  ~VTKViewer_ExtractUnstructuredGrid();

  void Execute();

  EExtraction myExtractionMode;
  
  EChanging myChangeMode;
  typedef std::set<vtkIdType> TSetId;
  TSetId myCellIds;
  TSetId myCellTypes;

  bool myStoreMapping;
  TVectorId myOut2InId;
  TMapId myIn2OutId;

private:
  VTKViewer_ExtractUnstructuredGrid(const VTKViewer_ExtractUnstructuredGrid&);  // Not implemented.
  void operator=(const VTKViewer_ExtractUnstructuredGrid&);  // Not implemented.
};

#endif
